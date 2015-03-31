#include "reach_system_priv.h"
#include "reach_tiler_priv.h"


#if(SYSTEM_TILER_ALLOCATOR_PROC_ID == SYSTEM_PROC_DSP)
#    define  SYSTEM_TILER_LINK_ID                           (SYSTEM_LINK_ID_DSP)
#else
#    if(SYSTEM_TILER_ALLOCATOR_PROC_ID == SYSTEM_PROC_M3VIDEO)
#        define  SYSTEM_TILER_LINK_ID                   (SYSTEM_LINK_ID_M3VIDEO)
#    else
#        if (SYSTEM_TILER_ALLOCATOR_PROC_ID == SYSTEM_PROC_M3VPSS)
#            define  SYSTEM_TILER_LINK_ID                (SYSTEM_LINK_ID_M3VPSS)
#        else
#            if (SYSTEM_TILER_ALLOCATOR_PROC_ID == SYSTEM_PROC_HOSTA8)
#                define  SYSTEM_TILER_LINK_ID               (SYSTEM_PROC_HOSTA8)
#            else
#                error "Unknown PROC ID:"##SYSTEM_TILER_ALLOCATOR_PROC_ID
#            endif
#        endif
#    endif
#endif

UInt32 tiler_isallocator_disabled(void)
{
    SystemCommon_TilerIsDisabled tilerAllocatorStatus;
    Int32 status;
    UInt32 isDisabled = TRUE;


    status = System_linkControl(SYSTEM_TILER_LINK_ID,
                                SYSTEM_COMMON_CMD_TILER_IS_ALLOCATOR_DISABLED,
                                &tilerAllocatorStatus,
                                sizeof(tilerAllocatorStatus), TRUE);

    if (!OSA_ISERROR(status))
    {
        isDisabled = tilerAllocatorStatus.isAllocatorDisabled;
    }
    return isDisabled;
}


Int32 tiler_disable_allocator(void)
{
    Int32 status;

    status = System_linkControl(SYSTEM_TILER_LINK_ID,
                                SYSTEM_COMMON_CMD_TILER_DISABLE_ALLOCATOR,
                                NULL, 0, TRUE);

    return status;
}


Int32 tiler_enable_allocator(void)
{
    Int32 status;

    status = System_linkControl(SYSTEM_TILER_LINK_ID,
                                SYSTEM_COMMON_CMD_TILER_ENABLE_ALLOCATOR,
                                NULL, 0, TRUE);

    return status;
}



