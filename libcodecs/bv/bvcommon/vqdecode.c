/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:noexpandtab */

/*****************************************************************************/
/* BroadVoice(R)16 (BV16) Fixed-Point ANSI-C Source Code                     */
/* Revision Date: November 13, 2009                                          */
/* Version 1.1                                                               */
/*****************************************************************************/

/*****************************************************************************/
/* Copyright 2000-2009 Broadcom Corporation                                  */
/*                                                                           */
/* This software is provided under the GNU Lesser General Public License,    */
/* version 2.1, as published by the Free Software Foundation ("LGPL").       */
/* This program is distributed in the hope that it will be useful, but       */
/* WITHOUT ANY SUPPORT OR WARRANTY; without even the implied warranty of     */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the LGPL for     */
/* more details.  A copy of the LGPL is available at                         */
/* http://www.broadcom.com/licenses/LGPLv2.1.php,                            */
/* or by writing to the Free Software Foundation, Inc.,                      */
/* 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.                 */
/*****************************************************************************/

/*****************************************************************************
  vqdecode.c : Common Fixed-Point Library: 

  $Log$
******************************************************************************/

#include <stdint.h>

void vqdec(int16_t * xq,		/* VQ output vector (quantized version of input vector) */
	   int16_t idx,		/* VQ codebook index for the nearest neighbor */
	   int16_t * cb,		/* VQ codebook */
	   int16_t vdim)
{				/* vector dimension */

	int16_t j, k;
	j = idx * vdim;
	for (k = 0; k < vdim; k++)
		xq[k] = cb[j + k];
}
