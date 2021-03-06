/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*

$Log$
Revision 1.15  2004/06/26 03:50:14  markster
Merge source cleanups (bug #1911)

Revision 1.14  2003/02/12 13:59:14  matteo
mer feb 12 14:56:57 CET 2003

Revision 1.1.1.1  2003/02/12 13:59:14  matteo
mer feb 12 14:56:57 CET 2003

Revision 1.2  2000/01/05 08:20:39  markster
Some OSS fixes and a few lpc changes to make it actually work

 * Revision 1.1  1996/08/19  22:32:31  jaf
 * Initial revision
 *

*/

#include <math.h>
#include <stdlib.h>

#include "difmag.h"
#include "lpc10.h"
#include "lpc10tools.h"

/* ********************************************************************** */

/* 	DIFMAG Version 49 */

/* $Log$
 * Revision 1.15  2004/06/26 03:50:14  markster
 * Merge source cleanups (bug #1911)
 *
 * Revision 1.14  2003/02/12 13:59:14  matteo
 * mer feb 12 14:56:57 CET 2003
 *
 * Revision 1.1.1.1  2003/02/12 13:59:14  matteo
 * mer feb 12 14:56:57 CET 2003
 *
 * Revision 1.2  2000/01/05 08:20:39  markster
 * Some OSS fixes and a few lpc changes to make it actually work
 *
 * Revision 1.1  1996/08/19  22:32:31  jaf
 * Initial revision
 * */
/* Revision 1.3  1996/03/15  23:09:39  jaf */
/* Just added a few comments about which array indices of the arguments */
/* are used, and mentioning that this subroutine has no local state. */

/* Revision 1.2  1996/03/13  14:41:31  jaf */
/* Comments added explaining that none of the local variables of this */
/* subroutine need to be saved from one invocation to the next. */

/* Revision 1.1  1996/02/07 14:45:04  jaf */
/* Initial revision */

/* ********************************************************************* */

/*  Compute Average Magnitude Difference Function */

/* Inputs: */
/*  SPEECH - Low pass filtered speech */
/*           Indices MIN_N1 through MAX_N1+LPITA-1 are read, where */
/*      MIN_N1 = (MAXLAG - MAX_TAU)/2+1  MAX_TAU = max of TAU(I) for I=1,LTAU
*/
/*      MAX_N1 = (MAXLAG - MIN_TAU)/2+1  MIN_TAU = min of TAU(I) for I=1,LTAU
*/
/*  LPITA  - Length of speech buffer */
/*  TAU    - Table of lags */
/*           Indices 1 through LTAU read. */
/*  LTAU   - Number of lag values to compute */
/*  MAXLAG - Maximum possible lag value */
/* Outputs: */
/*  (All of these outputs are also read, but only after being written.) */
/*  AMDF   - Average Magnitude Difference for each lag in TAU */
/*           Indices 1 through LTAU written */
/*  MINPTR - Index of minimum AMDF value */
/*  MAXPTR - Index of maximum AMDF value */

/* This subroutine has no local state. */

int lpc10_difmag(float *speech, int32_t * lpita, int32_t * tau,
		 int32_t * ltau, int32_t * maxlag, float *amdf,
		 int32_t * minptr, int32_t * maxptr)
{
	/* System generated locals */
	int32_t i__1, i__2;
	float r__1;

	/* Local variables */
	int32_t i__, j, n1, n2;
	float sum;

/*       Arguments */
/*       Local variables that need not be saved */
/*       Local state */
/*       None */
	/* Parameter adjustments */
	--amdf;
	--tau;
	--speech;

	/* Function Body */
	*minptr = 1;
	*maxptr = 1;
	i__1 = *ltau;
	for (i__ = 1; i__ <= i__1; ++i__) {
		n1 = (*maxlag - tau[i__]) / 2 + 1;
		n2 = n1 + *lpita - 1;
		sum = 0.f;
		i__2 = n2;
		for (j = n1; j <= i__2; j += 4) {
			sum += (r__1 =
				speech[j] - speech[j + tau[i__]], fabsf(r__1));
		}
		amdf[i__] = sum;
		if (amdf[i__] < amdf[*minptr]) {
			*minptr = i__;
		}
		if (amdf[i__] > amdf[*maxptr]) {
			*maxptr = i__;
		}
	}
	return 0;
}				/* lpc10_difmag */
