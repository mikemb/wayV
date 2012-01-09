/* wayV - NGUI (Next Generation User Interfaces)
 *
 * Copyright (C) 2000-2003 Mike Bennett (smoog at stressbunny dot com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * 
 * Purpose:
 *		Prototypes for ai.c
*/

#ifndef __AI_H__
#define __AI_H__

#include "setup.h"
#include "aimath.h"

/* Names for the different AI rules */
#define AIRULE_PDA			"Pda"			/* Point Density Analysis */
#define AIRULE_PDA_VECTOR	"Pda & Vector"	/* Point Density Analysis with direction */

WGESTURE *findGesture(WSETUP *, MATRIX *, VECTOR *, char *);
int pointDensityAnalysis(MATRIX *, MATRIX *);
int vectCmp(VECTOR *, VECTOR *);

#endif /* __AI_H__ */
