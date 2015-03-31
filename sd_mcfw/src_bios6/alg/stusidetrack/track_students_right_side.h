/* 
 * Copyright (c) 2009, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
/*
 *  ======== track.h ========
 */
#ifndef codecs_track_TRACK_STUDENTS_RIGHT_SIDE_TI_H_
#define codecs_track_TRACK_STUDENTS_RIGHT_SIDE_TI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define DM8168

#ifdef DM8168
#include <mcfw/src_bios6/links_c6xdsp/alg_link/stusidetrack/stusidetrackLink_priv.h>
#endif


#ifdef DM6467
#include <track_students_right_side_priv.h>
#endif

#ifdef __cplusplus
}
#endif

extern int32_t	track_students_right_side_process(StuSideTrackHand *inst,int8_t *input_buf, int8_t *output_buf, StuSideITRACK_OutArgs *output_param);

extern void track_students_right_side_area_check(StuSideTrackHand *inst);

extern int32_t set_check_max_rect(StuSideTrackHand *inst);
extern int32_t get_max_rect(StuSideITRACK_Params *pParams, T_Rect *prect);

#endif
/*
 *  @(#) ti.sdo.ce.examples.codecs.track_students_right_side; 1, 0, 0,204; 7-25-2009 21:30:30; /db/atree/library/trees/ce/ce-n11x/src/
 */

