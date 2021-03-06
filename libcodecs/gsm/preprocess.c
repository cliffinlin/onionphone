/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*
 * Copyright 1992 by Jutta Degener and Carsten Bormann, Technische
 * Universitaet Berlin.  See the accompanying file "COPYRIGHT" for
 * details.  THERE IS ABSOLUTELY NO WARRANTY FOR THIS SOFTWARE.
 */

/* $Header: /cvsroot/speak-freely-u/speak_freely/gsm/src/preprocess.c,v 1.1.1.1 2002/11/09 12:41:02 johnwalker Exp $ */

#include	<stdio.h>
#include	<assert.h>

#include "private.h"

#include	"gsm.h"

/*	4.2.0 .. 4.2.3	PREPROCESSING SECTION
 *  
 *  	After A-law to linear conversion (or directly from the
 *   	Ato D converter) the following scaling is assumed for
 * 	input to the RPE-LTP algorithm:
 *
 *      in:  0.1.....................12
 *	     S.v.v.v.v.v.v.v.v.v.v.v.v.*.*.*
 *
 *	Where S is the sign bit, v a valid bit, and * a "don't care" bit.
 * 	The original signal is called sop[..]
 *
 *      out:   0.1................... 12 
 *	     S.S.v.v.v.v.v.v.v.v.v.v.v.v.0.0
 */

void Gsm_Preprocess(struct gsm_state *S, int16_t * s, int16_t * so)
{				/* [0..159]       IN/OUT  */

	int16_t z1 = S->z1;
	int32_t L_z2 = S->L_z2;
	int16_t mp = S->mp;

	int16_t s1;
	int32_t L_s2;

	int32_t L_temp;

	int16_t msp, lsp;
	int16_t SO;

	volatile int32_t ltmp = 0;		/* for   ADD */
	volatile uint32_t utmp = 0;		/* for L_ADD */

	register int k = 160;

	while (k--) {

		/*  4.2.1   Downscaling of the input signal
		 */
		SO = SASR(*s, 3) << 2;
		s++;

		assert(SO >= -0x4000);	/* downscaled by     */
		assert(SO <= 0x3FFC);	/* previous routine. */

		/*  4.2.2   Offset compensation
		 * 
		 *  This part implements a high-pass filter and requires extended
		 *  arithmetic precision for the recursive part of this filter.
		 *  The input of this procedure is the array so[0...159] and the
		 *  output the array sof[ 0...159 ].
		 */
		/*   Compute the non-recursive part
		 */

		s1 = SO - z1;	/* s1 = gsm_sub( *so, z1 ); */
		z1 = SO;

		assert(s1 != MIN_WORD);

		/*   Compute the recursive part
		 */
		L_s2 = s1;
		L_s2 <<= 15;

		/*   Execution of a 31 bv 16 bits multiplication
		 */

		msp = SASR(L_z2, 15);
		lsp = L_z2 - ((int32_t) msp << 15);	/* gsm_L_sub(L_z2,(msp<<15)); */

		L_s2 += GSM_MULT_R(lsp, 32735);
		L_temp = (int32_t) msp *32735;	/* GSM_L_MULT(msp,32735) >> 1; */
		L_z2 = GSM_L_ADD(L_temp, L_s2);

		/*    Compute sof[k] with rounding
		 */
		L_temp = GSM_L_ADD(L_z2, 16384);

		/*   4.2.3  Preemphasis
		 */

		msp = GSM_MULT_R(mp, -28180);
		mp = SASR(L_temp, 15);
		*so++ = GSM_ADD(mp, msp);
	}

	S->z1 = z1;
	S->L_z2 = L_z2;
	S->mp = mp;

	(void)ltmp;
	(void)utmp;
}
