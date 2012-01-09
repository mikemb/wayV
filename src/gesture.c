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
 *	Code that handles the gesture based aspects
 *	including converting from one form to another
*/

#include<stdlib.h>
#include "defines.h"
#include "setup.h"
#include "gesture.h"
#include "aimath.h"

/*
 * Purpose:
 *      Allocate the memory required to store a GPOINT
 *
 * Parameters:
 *      NONE
 *
 * Returns:
 *      Pointer to memory allocated to hold a GPOINT 
 *
 * History:
 *      12/04/2000 (Mike Bennett): Created function
 *      08/08/2000 (Simon Hart): Add memory check code
*/
GPOINT *allocGPoint() {
	GPOINT *gPoint;

	if ((gPoint = (GPOINT *)malloc(sizeof(GPOINT)))==NULL) {
	  fputs("No memory for gpoint",stderr);
	  exit(1);
	}

	gPoint->x = GEND;
	gPoint->y = GEND;

    return gPoint;
}


/*
 * Purpose:
 * 	Allocate the memory required to store the gestures
 *
 * Parameters:
 * 	NONE
 * 		
 * Returns:
 *	Pointer to memory allocated to hold up to MAXGPOINTS
 *	of GPOINTs
 *				
 * History:
 * 	07/04/2000 (Mike Bennett): Created function
*/
GPOINT *setupGPoints(int maxpoints) {
	GPOINT *gPoints;
	int i;

	gPoints = (GPOINT *)malloc(sizeof(GPOINT) * (maxpoints + 1));

	for(i = 0; i < maxpoints; i++) {
		gPoints[i].x = GEND;
		gPoints[i].y = GEND;
	}

	return gPoints;
}


/*
 * Purpose:
 * 	Turn a list of 2D GPOINTs into a 2D grid (matrix)
 *
 * Parameters:
 * 	1st - Array of 2D GPOINTs
 * 	2nd - number of grid units in x
 * 	3rd - number of grid units in y
 * 		
 * Returns:
 * 	Matrix with filled in gesture
 * 	NULL on failure
 *				
 * History:
 * 	07/05/2000 (Mike Bennett): Update to return a MATRIX
 * 	15/06/2000 (Mike Bennett): Fixed bug in rounding of floats, means
 * 				   gesture matrices are more accurate
 *	02/08/2000 (Simon Hart): Added code to allow straight liness
*/
MATRIX *gridGPoints(GPOINT *gPoints, int xGrid, int yGrid) {
	MATRIX *gMat;
	int i;
	float xmax = -1, xmin = 100000000, ymax = -1, ymin = 100000000;
	float xunit, yunit;

	gMat = createMat(xGrid, yGrid);

	/* Work out the x and y max and mins */
	for(i = 0; gPoints[i].x != GEND; i++) {
		if(gPoints[i].x > xmax) xmax = gPoints[i].x;
		if(gPoints[i].x < xmin) xmin = gPoints[i].x;		
		if(gPoints[i].y > ymax) ymax = gPoints[i].y;
		if(gPoints[i].y < ymin) ymin = gPoints[i].y;		
	}
	
	xunit = (xmax - xmin)/(xGrid - 1);
	yunit = (ymax - ymin)/(yGrid - 1);

	/* Need to keep some minimum grid else anything which is not a purely
	   straight line appears to be at 45 degrees.*/
	if(xunit > 3 * yunit) yunit = xunit;
	if(yunit > 3 * xunit) xunit = yunit;
	   
	/* Scale rule */
	/*
	if(xunit > yunit)
		yunit = xunit;
	else
		xunit = yunit;
	*/

	/* Turn x and y into grid positions */
	for(i = 0; gPoints[i].x != GEND; i++) {
		if((gPoints[i].x - xmin) != 0 && xunit != 0)
			xGrid = float2Int((gPoints[i].x - xmin) / xunit);
		else
			xGrid = 0;

		if((gPoints[i].y - ymin) != 0 && yunit != 0)
			yGrid = float2Int((gPoints[i].y - ymin) / yunit);
		else
			yGrid = 0;

		gMat->me[xGrid][yGrid] = 1;
	}

	return gMat;
}


/* 
 * Purpose:
 * 	Turn a list of points into a set of vectors
 *
 * Parameters:
 * 	1st - Array of 2d GPOINTs
 * 	2nd - Number grid units in x
 * 	3rd - Number grid units in y
 * 	4th - Max number of vectors to read
 *
 * Returns:
 * 	VECTOR, whose array is filled with values of DIR_*
 *
 * History:
 * 	03/08/2000 (Simon P Hart): Created function
*/
VECTOR *vectGPoints(GPOINT *gPoints, int xGrid, int yGrid, int maxvectors) {
	VECTOR *v;
	float xmax = -1, xmin = 100000000, ymax = -1, ymin = 100000000;
	float xunit, yunit;
	int pxGrid = 0, pyGrid = 0, vec = 0, i;
	int ndir, pndir=0;

	if((v=(VECTOR *)malloc(sizeof(VECTOR)*maxvectors))==NULL) {
		fputs("No mem for vector\n",stderr);
		exit(1);
	}
  
	/* Work out the x and y max and mins  CODE: again*/
	for(i = 0; gPoints[i].x != GEND; i++) {
		if(gPoints[i].x > xmax) xmax = gPoints[i].x;
		if(gPoints[i].x < xmin) xmin = gPoints[i].x;		
		if(gPoints[i].y > ymax) ymax = gPoints[i].y;
		if(gPoints[i].y < ymin) ymin = gPoints[i].y;		
	}

	xunit = (xmax - xmin)/(xGrid - 1);
	yunit = (ymax - ymin)/(yGrid - 1);
	
	if(xunit > 3 * yunit) yunit = xunit;
	if(yunit > 3 * xunit) xunit = yunit;
  
	for(i = 0; gPoints[i].x != GEND; i++) {
		if((gPoints[i].x - xmin) != 0 && xunit != 0)
			xGrid = float2Int((gPoints[i].x - xmin) / xunit);
		else
			xGrid = 0;
    
		if((gPoints[i].y - ymin) != 0 && yunit != 0)
			yGrid = float2Int((gPoints[i].y - ymin) / yunit);
		else
			yGrid = 0;
		
		if(i == 0) {
			pxGrid = xGrid;
			pyGrid = yGrid;
		} else {
			ndir = 0;
			
			if(pxGrid > xGrid) ndir += DIR_W;
			if(pxGrid < xGrid) ndir += DIR_E;
			if(pyGrid > yGrid) ndir += DIR_N;
			if(pyGrid < yGrid) ndir += DIR_S;
			
			if(ndir != 0) {
				pxGrid = xGrid;
				pyGrid = yGrid;
				
				if(vec > 0 && pndir != ndir) {
					pndir = ndir;
					continue;
				}
				
				if(vec > 0 && v[vec-1] == ndir) continue;
				
				v[vec++] = ndir;
				pndir = ndir;
				
				if(vec == (maxvectors-1)) {
					fputs ("Warning: too many vectors. Increase universe.maxvectors\n",stderr);
					break;
				}
			}
		}   
	}
	
	v[vec] = DIR_NONE;
	
	return v;
}


/*
 * Purpose:
 * 	Turn a float into an integer after rounding it. Note that
 * 	this only works with positive floats.
 *
 * Parameters:
 * 	Float to round
 *
 * Returns:
 *	Rounded integer of the float
 *
 * History:
 * 	15/06/2000 (Mike Bennett): Created function
*/
int float2Int(register float convert) {
	register int ans;

	/* Righto, when we cast a float to an int we loose everything after
	 * the decimal point - going to take advantage of that...oh clever ;) */
	ans = (int)convert;

	convert -= ans;

	if(convert >= 0.5)
		ans++;

	return ans;
}
