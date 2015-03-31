#include "reach_system_priv.h"
#include "reach_ipcbit_priv.h"

#include <osa_que.h>
#include <osa_mutex.h>
#include <osa_thr.h>
#include <osa_sem.h>




#define REACH_IPCBITS_MAX_PENDING_RECV_SEM_COUNT		(10)

#define REACH_IPC_BITS_MAX_NUM_CHANNELS					(16)

#define REACH_IPCBITS_MAX_NUM_FREE_BUFS_PER_CHANNEL		(6)
#define REACH_IPCBITS_FREE_QUE_MAX_LEN					(REACH_IPC_BITS_MAX_NUM_CHANNELS * \
                                                         REACH_IPCBITS_MAX_NUM_FREE_BUFS_PER_CHANNEL)
#define REACH_IPCBITS_FULL_QUE_MAX_LEN					(REACH_IPCBITS_FREE_QUE_MAX_LEN)


#define REACH_IPCBITS_USERFXN_TSK_PRI                   (99)
#define REACH_IPCBITS_USERFXN_TSK_STACK_SIZE            (0)


#define REACH_IPCBITS_SENDFXN_TSK_PRI                   (99)
#define REACH_IPCBITS_RECVFXN_TSK_PRI                   (99)

#define REACH_IPCBITS_SENDFXN_TSK_STACK_SIZE            (0) /* 0 means system default will be used */
#define REACH_IPCBITS_RECVFXN_TSK_STACK_SIZE            (0)

#define REACH_SEND_FXN_WAIT_USECOND						(8)
#define REACH_RECV_FXN_WAIT_USECOND						(8)


typedef struct _ipcbit_process_st_ {
	UInt32	ipcBitsInLinkId;
	UInt32	ipcBitsOutLinkId;

	Bool	bitsout_flag;
	Bool	bitsin_flag;
	
	bits_callback_fxn	bitsout_user_fxn;
	Void				*bitsout_user_param;
	bits_callback_fxn	bitsin_user_fxn;
	Void				*bitsin_user_param;

	OSA_ThrHndl bitsin_user_thr;
    OSA_ThrHndl bitsout_user_thr;

	OSA_ThrHndl bitsin_thr;
    OSA_ThrHndl bitsout_thr;

	OSA_QueHndl	empty_que;
	OSA_QueHndl	full_que;
	
	bits_user_param	bitsin_quehndl;
	bits_user_param	bitsout_quehndl;
	
	OSA_SemHndl bitsin_sem;

	Uint32		run_status;
}ipcbit_process_st;



static Void bits_copy_bitbufinfo(Bitstream_Buf *dst, const Bitstream_Buf *src)
{
    dst->channelNum = src->channelNum;
    dst->codingType = src->codingType;
    dst->fillLength = src->fillLength;
    dst->isKeyFrame = src->isKeyFrame;
    dst->timeStamp  = src->timeStamp;
 //   dst->mvDataFilledSize = src->mvDataFilledSize;

	dst->frameWidth = src->frameWidth;
	dst->frameHeight = src->frameHeight;
#if 0	
	dst->startOffset = src->startOffset;
	dst->mvDataOffset = src->mvDataOffset;

	dst->upperTimeStamp = src->upperTimeStamp;
	dst->lowerTimeStamp = src->lowerTimeStamp;
	dst->encodeTimeStamp = src->encodeTimeStamp;

	dst->doNotDisplay = src->doNotDisplay;
	dst->allocPoolID = src->allocPoolID;
	dst->appData = src->appData;
	dst->bottomFieldBitBufSize = src->bottomFieldBitBufSize;

	dst->bufSize = src->bufSize;
	dst->reserved[0] = src->reserved[0];
	dst->reserved[1] = src->reserved[1];
#endif
}

static Void bits_copy_bitbufdata(Bitstream_Buf *dst, Bitstream_Buf *src)
{
    OSA_assert(src->fillLength < dst->bufSize);
    memcpy(dst->addr,src->addr,src->fillLength);
}



static Void bits_que_empty_bitbufs(Void * prm)
{	
	ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;
	
    Bitstream_BufList emptyBufList;
    Bitstream_Buf *pBuf;
    IpcBitsOutLinkHLOS_BitstreamBufReqInfo reqInfo;
    Int index;
    Int status;
    UInt32 bitBufSize;

	OSA_QueHndl	*psempty_que;

	if(pipcbit->bitsout_user_fxn != NULL) {
		psempty_que = &(pipcbit->bitsout_quehndl.bits_empty_que);
	}
	else {
		psempty_que = &(pipcbit->empty_que);
	}


    bitBufSize = 1920*1080;
	
    emptyBufList.numBufs = 0;
    reqInfo.numBufs = VIDBITSTREAM_MAX_BITSTREAM_BUFS;
    for(index = 0; index < VIDBITSTREAM_MAX_BITSTREAM_BUFS; index++) {
        reqInfo.u[index].minBufSize = bitBufSize;
		reqInfo.u[index].chNum = 0;
		reqInfo.reqType = IPC_BITSOUTHLOS_BITBUFREQTYPE_BUFSIZE;
    }

    IpcBitsOutLink_getEmptyVideoBitStreamBufs(pipcbit->ipcBitsOutLinkId, &emptyBufList, &reqInfo);

    for(index = 0; index < emptyBufList.numBufs; index++) {
        pBuf = emptyBufList.bufs[index];
        OSA_assert(pBuf->bufSize >= bitBufSize);
        status = OSA_quePut(psempty_que, (Int32)pBuf,OSA_TIMEOUT_NONE);
        OSA_assert(status == 0);
    }
}


static Void bits_send_full_bitbufs(Void * prm)
{
	ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;
	
    Bitstream_BufList fullBufList;
    Bitstream_Buf *pBuf;
    Int status;

	OSA_QueHndl	*psend_que = NULL;

	if(pipcbit->bitsin_flag && (NULL == pipcbit->bitsout_user_fxn)) {
		psend_que = &(pipcbit->full_que);	//输入的数据传递给输出
	}
	else if(pipcbit->bitsout_user_fxn != NULL){//输入的数据不传递给输出，由out cbfxn传递给输出
		psend_que = &(pipcbit->bitsout_quehndl.bits_full_que);
	}
	
    fullBufList.numBufs = 0;
    while((status = OSA_queGet(psend_que, (Int32 *)(&pBuf),OSA_TIMEOUT_NONE)) == 0) {
        OSA_assert(fullBufList.numBufs < VIDBITSTREAM_MAX_BITSTREAM_BUFS);

        fullBufList.bufs[fullBufList.numBufs] = pBuf;
        fullBufList.numBufs++;
        if(fullBufList.numBufs == VIDBITSTREAM_MAX_BITSTREAM_BUFS) {
            break;
        }
    }
	
    if(fullBufList.numBufs) {
        IpcBitsOutLink_putFullVideoBitStreamBufs(pipcbit->ipcBitsOutLinkId, &fullBufList);
    }
}


static Void *bits_send_fxn(Void * prm)
{
    ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;

    while(TRUE == pipcbit->run_status) {
        OSA_waitMsecs(REACH_SEND_FXN_WAIT_USECOND);
		bits_que_empty_bitbufs(pipcbit);
		bits_send_full_bitbufs(pipcbit);
    }
	pthread_detach(pthread_self());
	pthread_exit(NULL);
    return NULL;
}

static Void bits_handle_fullbufs(Void * prm)
{
	ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;
	
    Bitstream_BufList fullBufList;
    Bitstream_Buf *pFullBuf;
    Bitstream_Buf *pEmptyBuf;
    Int index,status;

    IpcBitsInLink_getFullVideoBitStreamBufs(pipcbit->ipcBitsInLinkId, &fullBufList);

    for(index = 0; index < fullBufList.numBufs; index++) {
		pFullBuf = fullBufList.bufs[index];
		
		if(pipcbit->bitsout_flag && (NULL == pipcbit->bitsout_user_fxn)) {
			//输入的数据传递给输出
			status = OSA_queGet(&(pipcbit->empty_que), (Int32 *)(&pEmptyBuf), OSA_TIMEOUT_FOREVER);
			OSA_assert(status == 0);

			bits_copy_bitbufinfo(pEmptyBuf,pFullBuf);
			bits_copy_bitbufdata(pEmptyBuf,pFullBuf);
			status = OSA_quePut(&(pipcbit->full_que), (Int32)pEmptyBuf,OSA_TIMEOUT_NONE);
			OSA_assert(status == 0);
		}

		if(pipcbit->bitsin_user_fxn) {
			status = OSA_quePut(&(pipcbit->bitsin_quehndl.bits_full_que), (Int32)pFullBuf,OSA_TIMEOUT_NONE);
			OSA_assert(status == 0);
		}
    }

	if(pipcbit->bitsin_user_fxn) {
		for(index = 0; index < fullBufList.numBufs; index++) {
			status = OSA_queGet(&(pipcbit->bitsin_quehndl.bits_empty_que), (Int32 *)(&pFullBuf), OSA_TIMEOUT_FOREVER);
			OSA_assert(status == 0);
			fullBufList.bufs[index] = pFullBuf;
		}
	}

    IpcBitsInLink_putEmptyVideoBitStreamBufs(pipcbit->ipcBitsInLinkId, &fullBufList);
}

static Void *bits_recv_fxn(Void * prm)
{
	ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;

    while(TRUE == pipcbit->run_status) {
//        OSA_semWait(&(pipcbit->bitsin_sem), OSA_TIMEOUT_FOREVER);//待修改
		OSA_waitMsecs(REACH_RECV_FXN_WAIT_USECOND);
        bits_handle_fullbufs(pipcbit);
    }
	pthread_detach(pthread_self());
	pthread_exit(NULL);
    return NULL;
}

static Void bits_process_init(Void * prm)
{
	ipcbit_process_st *pipcbit = (ipcbit_process_st *)prm;

	Int32 status = -1;
	
    OSA_semCreate(&(pipcbit->bitsin_sem), REACH_IPCBITS_MAX_PENDING_RECV_SEM_COUNT, 0);
    pipcbit->run_status = TRUE;
	
    OSA_queCreate(&(pipcbit->empty_que), REACH_IPCBITS_FREE_QUE_MAX_LEN);
    OSA_queCreate(&(pipcbit->full_que), REACH_IPCBITS_FULL_QUE_MAX_LEN);

	if(pipcbit->bitsin_user_fxn != NULL) {
		pipcbit->bitsin_quehndl.run_status = TRUE;
		pipcbit->bitsin_quehndl.appdata = pipcbit->bitsin_user_param;
		OSA_queCreate(&(pipcbit->bitsin_quehndl.bits_full_que), REACH_IPCBITS_FULL_QUE_MAX_LEN);
		OSA_queCreate(&(pipcbit->bitsin_quehndl.bits_empty_que), REACH_IPCBITS_FULL_QUE_MAX_LEN);

	//		printf("(pipcbit->bitsin_user_fxn) = %p\n",(pipcbit->bitsin_user_fxn));
		printf_pthread_create(__FILE__, ("IpcBitsInLink_periodicTaskFxn"));
		status = OSA_thrCreate(&(pipcbit->bitsin_user_thr),
	                  (OSA_ThrEntryFunc)(pipcbit->bitsin_user_fxn),
	                  REACH_IPCBITS_USERFXN_TSK_PRI,
	                  REACH_IPCBITS_USERFXN_TSK_STACK_SIZE,
	                  &(pipcbit->bitsin_quehndl));
		OSA_assert(status == 0);
	}

	if(pipcbit->bitsout_user_fxn != NULL) {
		pipcbit->bitsout_quehndl.run_status = TRUE;
		pipcbit->bitsout_quehndl.appdata = pipcbit->bitsout_user_param;
		OSA_queCreate(&(pipcbit->bitsout_quehndl.bits_full_que), REACH_IPCBITS_FULL_QUE_MAX_LEN);
		OSA_queCreate(&(pipcbit->bitsout_quehndl.bits_empty_que), REACH_IPCBITS_FULL_QUE_MAX_LEN);

//		printf("(pipcbit->bitsout_user_fxn) = %p\n",(pipcbit->bitsout_user_fxn));
		printf_pthread_create(__FILE__, ("IpcBitsInLink_periodicTaskFxn"));
		status = OSA_thrCreate(&(pipcbit->bitsout_user_thr),
	                  (OSA_ThrEntryFunc)(pipcbit->bitsout_user_fxn),
	                  REACH_IPCBITS_USERFXN_TSK_PRI,
	                  REACH_IPCBITS_USERFXN_TSK_STACK_SIZE,
	                  &(pipcbit->bitsout_quehndl));
		OSA_assert(status == 0);
	}

	if(TRUE == pipcbit->bitsout_flag) {
//		printf("( bits_send_fxn) = %p\n",(bits_send_fxn));
		printf_pthread_create(__FILE__, ("bits_send_fxn"));
	    status = OSA_thrCreate(&(pipcbit->bitsout_thr),
	                  (OSA_ThrEntryFunc)bits_send_fxn,
	                  REACH_IPCBITS_SENDFXN_TSK_PRI,
	                  REACH_IPCBITS_SENDFXN_TSK_STACK_SIZE,
	                  pipcbit);
		OSA_assert(status == 0);
	}

	if(TRUE == pipcbit->bitsin_flag) {
//				printf("( bits_recv_fxn) = %p\n",(bits_recv_fxn));
	printf_pthread_create(__FILE__, ("bits_recv_fxn"));
	    status = OSA_thrCreate(&(pipcbit->bitsin_thr),
	                  (OSA_ThrEntryFunc)bits_recv_fxn,
	                  REACH_IPCBITS_RECVFXN_TSK_PRI,
	                  REACH_IPCBITS_RECVFXN_TSK_STACK_SIZE,
	                  pipcbit);
		OSA_assert(status == 0);
	}
}



/*
* 功能说明:  创建一个在A8端对比特流处理的一个实例
* 参数说明:  ---- ppipcbit_handle 描述此实例的一个指针的指针
*			---- ipcBitInLinkId  有此linkId表明有比特流输入，
*								此时pbitsin_cbfxn参数才有效
*								不需要输入时则使用 SYSTEM_LINK_ID_INVALID
*
*			---- ipcBitOutLinkId 有此linkId表明有比特流输出，
*								此时pbitsout_cbfxn参数才有效,
*								不需要输出时则使用 SYSTEM_LINK_ID_INVALID
*
*			---- pbitsin_cbfxn   获取比特流的回调函数
*			---- pbitsout_cbfxn   输出比特流的回调函数
*
* 回调函数格式: void *fun_name(OSA_QueHndl *full_que, OSA_QueHndl *empty_que)
*			---- full_que 此队列句柄指针用于获取满的一帧数据(对于pbitsin_cbfxn)，
*						 或者用于获取空的buffer(对于pbitsout_cbfxn)
*			---- empty_que 此队列句柄指针用于返还空的buffer(对于pbitsin_cbfxn),
*						  或者用于返回装满数据的buffer(对于pbitsout_cbfxn)
*			注意:如果回调函数不为空，则此两个队列句柄必须被调用，以使数据能够正常流动
*
* 返回说明:  成功返回0，失败返回非0
*/
Int32 ipcbit_create_bitsprocess_inst(Void **ppipcbit_handle,
											  Uint32 ipcBitInLinkId,
											  Uint32 ipcBitOutLinkId,
											  bits_callback_fxn pbitsin_cbfxn,
											  Void *bitsin_param,
											  bits_callback_fxn pbitsout_cbfxn,
											  Void *bitsout_param)
{
	if(NULL == ppipcbit_handle) {
		OSA_printf("ipcbits: handle is NULL !!\n");
		return -1;
	}
		
	ipcbit_process_st *ppipcbit = (ipcbit_process_st *)malloc(sizeof(ipcbit_process_st));
	if(NULL == ppipcbit) {
		OSA_printf("ipcbits: malloc handle fail !!\n");
		return -1;
	}

	memset(ppipcbit, 0, sizeof(ipcbit_process_st));
	
	if(pbitsin_cbfxn != NULL) {
		ppipcbit->bitsin_user_fxn = (bits_callback_fxn)pbitsin_cbfxn;
		ppipcbit->bitsin_user_param = bitsin_param;
	}
	else {
		ppipcbit->bitsin_user_fxn = NULL;
	}
	
	if(pbitsout_cbfxn != NULL) {
		ppipcbit->bitsout_user_fxn = (bits_callback_fxn)pbitsout_cbfxn;
		ppipcbit->bitsout_user_param = bitsout_param;
	}
	else {
		ppipcbit->bitsout_user_fxn = NULL;
	}

	if(ipcBitInLinkId != SYSTEM_LINK_ID_INVALID) {
		ppipcbit->ipcBitsInLinkId = ipcBitInLinkId;
		ppipcbit->bitsin_flag = TRUE;
	}
	else {
		ppipcbit->bitsin_flag = FALSE;
		ppipcbit->bitsin_user_fxn = NULL;
	}

	if(ipcBitOutLinkId != SYSTEM_LINK_ID_INVALID) {
		ppipcbit->ipcBitsOutLinkId = ipcBitOutLinkId;
		ppipcbit->bitsout_flag = TRUE;
	}
	else {
		ppipcbit->bitsout_flag = FALSE;
		ppipcbit->bitsout_user_fxn = NULL;
	}

	if((FALSE == ppipcbit->bitsin_flag) && (TRUE == ppipcbit->bitsout_flag) 
					&& (NULL == ppipcbit->bitsout_user_fxn))
	{
		OSA_printf("ipcbit: no bits in, and no data for out, no anything, how going ????\n");
		return -1;
	}

	if((FALSE == ppipcbit->bitsin_flag) && (FALSE == ppipcbit->bitsout_flag))
	{
		OSA_printf("ipcbit: no bits in, and no bits out, how going ????\n");
		return -1;
	}

	bits_process_init(ppipcbit);

	*ppipcbit_handle = ppipcbit;

	return 0;
}


/*
* 功能说明:  删除一个比特流处理的实例
* 参数说明:	---- handle 描述一个实例的指针
* 返回说明:  成功返回0，失败返回非0
*/

Int32 ipcbit_delete_bitsprocess_inst(Void *handle)
{
	ipcbit_process_st *pipcbit = handle;
		
	if(NULL == pipcbit) {
		OSA_printf("ipcbit: handle is NULL !!!");
		return -1;
	}

	pipcbit->run_status = FALSE;
	pipcbit->bitsin_quehndl.run_status = FALSE;
	pipcbit->bitsout_quehndl.run_status = FALSE;
	OSA_waitMsecs(100);
		
	OSA_semDelete(&(pipcbit->bitsin_sem));
    
    OSA_queDelete(&(pipcbit->empty_que));
    OSA_queDelete(&(pipcbit->full_que));

	if(pipcbit->bitsin_user_fxn) {
		OSA_queDelete(&(pipcbit->bitsin_quehndl.bits_full_que));
		OSA_queDelete(&(pipcbit->bitsin_quehndl.bits_empty_que));
		pipcbit->bitsin_user_fxn = NULL; 
	}

	if(pipcbit->bitsout_user_fxn) {
		OSA_queDelete(&(pipcbit->bitsout_quehndl.bits_full_que));
		OSA_queDelete(&(pipcbit->bitsout_quehndl.bits_empty_que));
		pipcbit->bitsout_user_fxn = NULL;
	}

	//ADD free thread 
	//if(pipcbit->bitsin_user_thr == 0)
		  OSA_thrDelete(&(pipcbit->bitsin_user_thr));
	//if(pipcbit->bitsout_user_thr == 0)
		OSA_thrDelete(&(pipcbit->bitsout_user_thr));
	//if(pipcbit->bitsin_thr == 0)
		OSA_thrDelete(&(pipcbit->bitsin_thr));	
	//if(pipcbit->bitsout_thr == 0)
		OSA_thrDelete(&(pipcbit->bitsout_thr));

	free(pipcbit);

	return 0;
}





