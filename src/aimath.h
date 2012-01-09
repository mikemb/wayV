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
 *	Present generic interface to maths required
 *	for ai (at this stage neural net)
*/

#ifndef __AIMATH_H__
#define __AIMATH_H__

/* Loosely based on the MAT in Meschach */
typedef struct {
	int m;
	int n;

	int **me;
} MATRIX;

typedef char VECTOR;

MATRIX *normMat(MATRIX *);
void setMat(MATRIX *, int);
int pdaMat(MATRIX *, int);
MATRIX *denormMat(MATRIX *);
MATRIX *createMat(int, int);
MATRIX *copyMat(MATRIX *, MATRIX *);
MATRIX *cloneMat(MATRIX *);
MATRIX *addMat(MATRIX *, MATRIX *);
MATRIX *subMat(MATRIX *, MATRIX *);
MATRIX *mulMat(MATRIX *, MATRIX *);
MATRIX *transMat(MATRIX *);
MATRIX *resizeMat(MATRIX *, int, int);
MATRIX *rescaleMat(MATRIX *, int, int);
int cmpMat(MATRIX *, MATRIX *);
MATRIX *joinMat(MATRIX *, MATRIX *);
void freeMat(MATRIX *);
void printMat(MATRIX *);
void printVec(VECTOR *);

#endif /* __AIMATH_H__ */
