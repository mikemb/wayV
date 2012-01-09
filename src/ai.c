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
 *	Carry out Point Density Analysis (PDA) and gesture matching
*/

#include <string.h>
#include "defines.h"
#include "ai.h"
#include "aimath.h"
#include "gesture.h"

/*
 * Purpose:
 *	Find a gesture if one exists
 *
 * Parameters:
 *	1st - internal representation of the configuration
 *		  file and data structures
 *	2nd - Gesture matrix to find
 *  3rd - Vector to find
 *	4th - AI rule to use to do matching
 *
 * Returns:
 * 	WGESTURE * to gesture that matches
 *	NULL on no match
 *
 * History:
 *	19/06/2000 (Mike Bennett): Added PDA_REFUSE
 *	02/08/2000 (Simon P Hart): Added vector check
 *	08/08/2000 (Mike Bennett): Added a new rule for Simon's vectoring, it could
 *  				   share a lot of the same code but each rule should
 *  				   be completely separate (there is going to be a 
 *  				   fair number of them)
*/
WGESTURE *findGesture(WSETUP *wayv, MATRIX *find, VECTOR *vector, char *rule) {
	WGESTURE *action = NULL;
	int n, pdaTemp, pda = 100;

	/* Walk through each brain testing for a result pattern */
	if(strcmp(rule, AIRULE_PDA) == 0) {
		/* Walk through all the gestures */
		for(n = 0; wayv->gestures[n] != NULL; n++) {
			pdaTemp = pointDensityAnalysis(find, wayv->gestures[n]->wshape);

			/* Type of bubble sort */
			if (pdaTemp < pda) {
				pda = pdaTemp;
				action = wayv->gestures[n];
			}
		}
	} else if(strcmp(rule, AIRULE_PDA_VECTOR) == 0) {
		/* Walk through all the gestures */
		for(n = 0; wayv->gestures[n] != NULL; n++) {
			pdaTemp = pointDensityAnalysis(find, wayv->gestures[n]->wshape);

			if(vectCmp(vector,wayv->gestures[n]->wvector) == TRUE)
				pdaTemp /= 2;

			/* Type of bubble sort */
			if (pdaTemp < pda) {
				pda = pdaTemp;
				action = wayv->gestures[n];
			}
		}
	}

	if(pda > PDA_REFUSE)
		return NULL;

	return action;
}


/*
 * Purpose:
 * 	Perform a technique I've called point density anaylsis - I'll
 * 	writeup something on it later - for now I'm focused on the
 * 	implementation (must do, must do, cannot stop coding, help... ;)
 *
 * Parameters:
 * 	1st - Matrix we want to find
 * 	2nd - Matrix we think may be a match
 *
 * Returns:
 * 	Percentage difference between test and the original
 *
 * History:
 *	15/06/2000 (Mike Bennett): Created function
*/
int pointDensityAnalysis(MATRIX *find, MATRIX *test) {
	MATRIX *temp;
	float original, after, check;
	int ans;

	check = (float)pdaMat(test, 1);
	original = (float)pdaMat(find, 1);

	if(original > check)
		temp = denormMat(subMat(find, test));
	else
		temp = denormMat(subMat(test, find));

	after = (float)pdaMat(temp, 1);

	ans = float2Int((after / original) * 100);

	freeMat(temp);

	return(ans);
}

/* 
 * Purpose:
 *   Compares two vectors 
 * 
 * Parameters:
 *   Two VECTORS
 *
 * Returns:
 *   True if identical, False otherwise
 *
 * History:
 *   02/08/2000 (Simon P Hart): Created function
*/
int vectCmp(VECTOR *vec1, VECTOR *vec2) {
	VECTOR *v1 = vec1, *v2 = vec2;

	while(*v1 != DIR_NONE && *v2 != DIR_NONE) {
		if(*v1 != *v2) break;
		v1++; v2++;
	}
	
	if(*v1 == DIR_NONE && *v2 == DIR_NONE) return TRUE;

	return FALSE;
}  
