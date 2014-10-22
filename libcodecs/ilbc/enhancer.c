/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 WebRtcIlbcfix_Enhancer.c

******************************************************************/

#include "defines.h"
#include "constants.h"
#include "get_sync_seq.h"
#include "smooth.h"

/*----------------------------------------------------------------*
 * perform enhancement on idata+centerStartPos through
 * idata+centerStartPos+ENH_BLOCKL-1
 *---------------------------------------------------------------*/

void WebRtcIlbcfix_Enhancer(WebRtc_Word16 * odata,	/* (o) smoothed block, dimension blockl */
			    WebRtc_Word16 * idata,	/* (i) data buffer used for enhancing */
			    WebRtc_Word16 idatal,	/* (i) dimension idata */
			    WebRtc_Word16 centerStartPos,	/* (i) first sample current block within idata */
			    WebRtc_Word16 * period,	/* (i) pitch period array (pitch bward-in time) */
			    WebRtc_Word16 * plocs,	/* (i) locations where period array values valid */
			    WebRtc_Word16 periodl	/* (i) dimension of period and plocs */
    )
{
	/* Stack based */
	WebRtc_Word16 surround[ENH_BLOCKL];

	WebRtcSpl_MemSetW16(surround, 0, ENH_BLOCKL);

	/* get said second sequence of segments */

	WebRtcIlbcfix_GetSyncSeq(idata, idatal, centerStartPos, period, plocs,
				 periodl, ENH_HL, surround);

	/* compute the smoothed output from said second sequence */

	WebRtcIlbcfix_Smooth(odata, idata + centerStartPos, surround);
}
