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

 WebRtcIlbcfix_HpOutput.c

******************************************************************/

#include "defines.h"

/*----------------------------------------------------------------*
 *  high-pass filter of output and *2 with saturation
 *---------------------------------------------------------------*/

void WebRtcIlbcfix_HpOutput(WebRtc_Word16 * signal,	/* (i/o) signal vector */
			    WebRtc_Word16 * ba,	/* (i)   B- and A-coefficients (2:nd order)
						   {b[0] b[1] b[2] -a[1] -a[2]} a[0]
						   is assumed to be 1.0 */
			    WebRtc_Word16 * y,	/* (i/o) Filter state yhi[n-1] ylow[n-1]
						   yhi[n-2] ylow[n-2] */
			    WebRtc_Word16 * x,	/* (i/o) Filter state x[n-1] x[n-2] */
			    WebRtc_Word16 len)
{				/* (i)   Number of samples to filter */
	int i;
	WebRtc_Word32 tmpW32;
	WebRtc_Word32 tmpW32b;

	for (i = 0; i < len; i++) {

		/*
		   y[i] = b[0]*x[i] + b[1]*x[i-1] + b[2]*x[i-2]
		   + (-a[1])*y[i-1] + (-a[2])*y[i-2];
		 */

		tmpW32 = WEBRTC_SPL_MUL_16_16(y[1], ba[3]);	/* (-a[1])*y[i-1] (low part) */
		tmpW32 += WEBRTC_SPL_MUL_16_16(y[3], ba[4]);	/* (-a[2])*y[i-2] (low part) */
		tmpW32 = (tmpW32 >> 15);
		tmpW32 += WEBRTC_SPL_MUL_16_16(y[0], ba[3]);	/* (-a[1])*y[i-1] (high part) */
		tmpW32 += WEBRTC_SPL_MUL_16_16(y[2], ba[4]);	/* (-a[2])*y[i-2] (high part) */
		tmpW32 = (tmpW32 << 1);

		tmpW32 += WEBRTC_SPL_MUL_16_16(signal[i], ba[0]);	/* b[0]*x[0] */
		tmpW32 += WEBRTC_SPL_MUL_16_16(x[0], ba[1]);	/* b[1]*x[i-1] */
		tmpW32 += WEBRTC_SPL_MUL_16_16(x[1], ba[2]);	/* b[2]*x[i-2] */

		/* Update state (input part) */
		x[1] = x[0];
		x[0] = signal[i];

		/* Rounding in Q(12-1), i.e. add 2^10 */
		tmpW32b = tmpW32 + 1024;

		/* Saturate (to 2^26) so that the HP filtered signal does not overflow */
		tmpW32b =
		    WEBRTC_SPL_SAT((WebRtc_Word32) 67108863, tmpW32b,
				   (WebRtc_Word32) - 67108864);

		/* Convert back to Q0 and multiply with 2 */
		signal[i] = (WebRtc_Word16) WEBRTC_SPL_RSHIFT_W32(tmpW32b, 11);

		/* Update state (filtered part) */
		y[2] = y[0];
		y[3] = y[1];

		/* upshift tmpW32 by 3 with saturation */
		if (tmpW32 > 268435455) {
			tmpW32 = WEBRTC_SPL_WORD32_MAX;
		} else if (tmpW32 < -268435456) {
			tmpW32 = WEBRTC_SPL_WORD32_MIN;
		} else {
			tmpW32 = WEBRTC_SPL_LSHIFT_W32(tmpW32, 3);
		}

		y[0] = (WebRtc_Word16) (tmpW32 >> 16);
		y[1] =
		    (WebRtc_Word16) ((tmpW32 -
				      WEBRTC_SPL_LSHIFT_W32((WebRtc_Word32)
							    y[0], 16)) >> 1);

	}

	return;
}
