/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/******************************************************************

 iLBC Speech Coder ANSI-C Source Code

 iLBCInterface.c

******************************************************************/

#include "ilbc.h"
#include "defines.h"
#include "init_encode.h"
#include "encode.h"
#include "init_decode.h"
#include "decode.h"
#include <stdlib.h>

int16_t WebRtcIlbcfix_EncoderCreate(iLBC_Enc_Inst_t ** iLBC_encinst)
{
	*iLBC_encinst = (iLBC_Enc_Inst_t *) malloc(sizeof(iLBC_Enc_Inst_t));
	if (*iLBC_encinst != NULL) {
		return (0);
	} else {
		return (-1);
	}
}

int16_t WebRtcIlbcfix_DecoderCreate(iLBC_Dec_Inst_t ** iLBC_decinst)
{
	*iLBC_decinst = (iLBC_Dec_Inst_t *) malloc(sizeof(iLBC_Dec_Inst_t));
	if (*iLBC_decinst != NULL) {
		return (0);
	} else {
		return (-1);
	}
}

int16_t WebRtcIlbcfix_EncoderFree(iLBC_Enc_Inst_t * iLBC_encinst)
{
	free(iLBC_encinst);
	return (0);
}

int16_t WebRtcIlbcfix_DecoderFree(iLBC_Dec_Inst_t * iLBC_decinst)
{
	free(iLBC_decinst);
	return (0);
}

int16_t WebRtcIlbcfix_EncoderInit(iLBC_Enc_Inst_t * iLBCenc_inst,
					int16_t mode)
{
	if ((mode == 20) || (mode == 30)) {
		WebRtcIlbcfix_InitEncode((iLBC_Enc_Inst_t *) iLBCenc_inst,
					 mode);
		return (0);
	} else {
		return (-1);
	}
}

int16_t WebRtcIlbcfix_Encode(iLBC_Enc_Inst_t * iLBCenc_inst,
				   const int16_t * speechIn,
				   int16_t len, int16_t * encoded)
{

	int16_t pos = 0;
	int16_t encpos = 0;

	if ((len != ((iLBC_Enc_Inst_t *) iLBCenc_inst)->blockl) &&
#ifdef SPLIT_10MS
	    (len != 80) &&
#endif
	    (len != 2 * ((iLBC_Enc_Inst_t *) iLBCenc_inst)->blockl) &&
	    (len != 3 * ((iLBC_Enc_Inst_t *) iLBCenc_inst)->blockl)) {
		/* A maximum of 3 frames/packet is allowed */
		return (-1);
	} else {

		/* call encoder */
		while (pos < len) {
			WebRtcIlbcfix_EncodeImpl((uint16_t *) &
						 encoded[encpos],
						 &speechIn[pos],
						 (iLBC_Enc_Inst_t *)
						 iLBCenc_inst);
#ifdef SPLIT_10MS
			pos += 80;
			if (((iLBC_Enc_Inst_t *) iLBCenc_inst)->section == 0)
#else
			pos += ((iLBC_Enc_Inst_t *) iLBCenc_inst)->blockl;
#endif
			encpos +=
			    ((iLBC_Enc_Inst_t *) iLBCenc_inst)->no_of_words;
		}
		return (encpos * 2);
	}
}

int16_t WebRtcIlbcfix_DecoderInit(iLBC_Dec_Inst_t * iLBCdec_inst,
					int16_t mode)
{
	if ((mode == 20) || (mode == 30)) {
		WebRtcIlbcfix_InitDecode((iLBC_Dec_Inst_t *) iLBCdec_inst, mode,
					 1);
		return (0);
	} else {
		return (-1);
	}
}

int16_t WebRtcIlbcfix_Decode(iLBC_Dec_Inst_t * iLBCdec_inst,
				   const int16_t * encoded,
				   int16_t len,
				   int16_t * decoded,
				   int16_t * speechType)
{
	int i = 0;
	/* Allow for automatic switching between the frame sizes
	   (although you do get some discontinuity) */
	if ((len == ((iLBC_Dec_Inst_t *) iLBCdec_inst)->no_of_bytes) ||
	    (len == 2 * ((iLBC_Dec_Inst_t *) iLBCdec_inst)->no_of_bytes) ||
	    (len == 3 * ((iLBC_Dec_Inst_t *) iLBCdec_inst)->no_of_bytes)) {
		/* ok, do nothing */
	} else {
		/* Test if the mode has changed */
		if (((iLBC_Dec_Inst_t *) iLBCdec_inst)->mode == 20) {
			if ((len == NO_OF_BYTES_30MS) ||
			    (len == 2 * NO_OF_BYTES_30MS) ||
			    (len == 3 * NO_OF_BYTES_30MS)) {
				WebRtcIlbcfix_InitDecode(((iLBC_Dec_Inst_t *)
							  iLBCdec_inst), 30,
							 ((iLBC_Dec_Inst_t *)
							  iLBCdec_inst)->
							 use_enhancer);
			} else {
				/* Unsupported frame length */
				return (-1);
			}
		} else {
			if ((len == NO_OF_BYTES_20MS) ||
			    (len == 2 * NO_OF_BYTES_20MS) ||
			    (len == 3 * NO_OF_BYTES_20MS)) {
				WebRtcIlbcfix_InitDecode(((iLBC_Dec_Inst_t *)
							  iLBCdec_inst), 20,
							 ((iLBC_Dec_Inst_t *)
							  iLBCdec_inst)->
							 use_enhancer);
			} else {
				/* Unsupported frame length */
				return (-1);
			}
		}
	}

	while ((i * ((iLBC_Dec_Inst_t *) iLBCdec_inst)->no_of_bytes) < len) {
		WebRtcIlbcfix_DecodeImpl(&decoded
					 [i *
					  ((iLBC_Dec_Inst_t *) iLBCdec_inst)->
					  blockl],
					 (const uint16_t *)&encoded[i *
									  ((iLBC_Dec_Inst_t *) iLBCdec_inst)->no_of_words], (iLBC_Dec_Inst_t *) iLBCdec_inst, 1);
		i++;
	}
	/* iLBC does not support VAD/CNG yet */
	*speechType = 1;
	return (i * ((iLBC_Dec_Inst_t *) iLBCdec_inst)->blockl);
}

