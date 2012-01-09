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
 *	for AI (at this stage neural net)
*/

#include<stdlib.h>
#include "defines.h"
#include "aimath.h"

/*
 * Purpose:
 *	Convert to +/-1 (calling it normalising for now) and return as
 *	a new matrix
 *
 * Parameters:
 * 	Pointer to matrix to normalise
 * 		
 * Returns:
 *	Pointer to normalised matrix
 *				
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	02/05/2000 (Mike Bennett): Updated to return a new matrix
 * 							   & changed to convert less than 1
 * 							   to -1
*/
MATRIX *normMat(MATRIX *matrix) {
	MATRIX *ans;

	int m, n;

	ans = createMat(matrix->m, matrix->n);

	for(m = 0; m < matrix->m; m++)
		for(n = 0; n < matrix->n; n++)
			if(matrix->me[m][n] > 0)
				ans->me[m][n] = 1;
			else
				ans->me[m][n] = -1;

	return(ans);
}


/*
 * Purpose:
 * 	Set all of a matrices rows and columns to the same value
 *
 * Parameters:
 * 	1st - Pointer to matrix to set
 * 	2nd - value to set all the rows and columns to
 * 		
 * Returns:
 *	NONE
 *				
 * History:
 * 	09/06/2000 (Mike Bennett): Created function
*/
void setMat(MATRIX *matrix, int value) {
	int m, n;

	for(m = 0; m < matrix->m; m++)
		for(n = 0; n < matrix->n; n++)
			matrix->me[m][n] = value;
}


/*
 * Purpose:
 * 	Count of number of X int's found in a matrix. Part
 * 	of the point density analysis technique.
 *
 * Parameters:
 * 	1st - Pointer to matrix to count values in
 * 	2nd - Value (X) to find numerous times
 * 		
 * Returns:
 *	Int with number of X int's found
 *				
 * History:
 * 	15/06/2000 (Mike Bennett): Created function
*/
int pdaMat(MATRIX *matrix, int value) {
	int m, n, ans;

	for(ans = 0, m = 0; m < matrix->m; m++)
		for(n = 0; n < matrix->n; n++)
			if(matrix->me[m][n] == value)
				ans++;

	return(ans);
}


/*
 * Purpose:
 * 	Convert a normalized matrix from +1/-1 to +1/0
 *
 * Parameters:
 * 	Pointer to matrix to normalise
 * 		
 * Returns:
 *	Pointer to normalized matrix
 *	
 * NOTE:
 * 	This doesn't create a new matrix and return that (unlike normMat())
 * 	instead it just converts an existing one and returns a pointer to that
 *				
 * History:
 * 	07/05/2000 (Mike Bennett): Created function
 * 	15/06/2000 (Mike Bennett): Changed to return pointer to updated matrix
*/
MATRIX *denormMat(MATRIX *matrix) {
	int m, n;

	for(m = 0; m < matrix->m; m++)
		for(n = 0; n < matrix->n; n++)
			if(matrix->me[m][n] > 0)
				matrix->me[m][n] = 1;
			else
				matrix->me[m][n] = 0;

	return(matrix);
}


/*
 * Purpose:
 *	Create a matrix
 *
 * Parameters:
 * 	1st - number of units matrix across
 * 	2nd - number of units matrix down
 * 
 * Returns:
 *	Pointer to matrix
 *
 * NOTE:
 *	Access element of matrix by MATRIX->me[m][n]
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	07/06/2000 (Mike Bennett): Updated to allocate memory for the MATRIX
 * 	02/08/2000 (Simon Hart): Added memory checking code
*/
MATRIX *createMat(int m, int n) {
	MATRIX *ans;
	int i;

	if((ans = (MATRIX *)malloc(sizeof(MATRIX))) == NULL) {
	  fputs("No mem for matrix\n",stderr);
	  exit(1);
	}
	
	ans->m = m;
	ans->n = n;
	
	if((ans->me = (int **)calloc(m, sizeof(int *))) == NULL) {
	  fputs("No mem for matrix\n",stderr);
	  exit(1);
	}

	/* Now allocate across */
	for(i = 0; i < m; i++) {
	  if((ans->me[i] = (int *)calloc(n, sizeof(int))) == NULL) {
	    fputs("No mem for matrix\n",stderr);
	    exit(1);
	  }
	}

	setMat(ans, 0);

	return(ans);
}


/*
 * Purpose:
 *	Copy from one matrix to another
 *
 * Parameters:
 * 	1st - matrix to copy to
 *  2nd - matrix to copy from
 *
 * Returns:
 *	Matrix with copy
 *	NULL on failure
 *
 * History:
 * 	01/05/2000 (Mike Bennett): Created function
*/
MATRIX *copyMat(MATRIX *copy, MATRIX *original) {
	int m, n;

	/* Make sure they're the same size */
	if(copy->m != original->m || copy->n != original->n)
		return NULL;

	for(n = 0; n < original->n; n++)
		for(m = 0; m < original->m; m++)
			copy->me[m][n] = original->me[m][n];

	return(copy);
}

 
/*
 * Purpose:
 *	Create an exact copy of a matrix
 *
 * Parameters:
 * 	Matrix to create a copy of
 * 
 * Returns:
 *	New copy of the original matrix
 *	NULL on failure
 *
 * History:
 * 	01/05/2000 (Mike Bennett): Created function
*/
MATRIX *cloneMat(MATRIX *original) {
	MATRIX *ans;

	ans = createMat(original->m, original->n);
	
	return copyMat(ans, original);
}


/*
 * Purpose:
 *	Add two matrix's together
 *
 * Parameters:
 * 	1st - first matrix to add
 *  2nd - second matrix to add
 *
 * Returns:
 *	New matrix with the result of the add
 *	NULL on failure
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	09/06/2000 (Mike Bennett): Rewrote to get rid of Meschach
*/
MATRIX *addMat(MATRIX *first, MATRIX *second) {
	MATRIX *ans;
	int m, n;

	if(first->m != second->m || first->n != second->n)
		return NULL;

	ans = createMat(first->m, first->n);

	for(m = 0; m < first->m; m++)
		for(n = 0; n < first->n; n++)
			ans->me[m][n] = first->me[m][n] + second->me[m][n];

	return(ans);
}


/*
 * Purpose:
 *	Subtract two matrix's
 *
 * Parameters:
 * 	1st - matrix to be subtracted from
 *  2nd - matrix to subtract
 *
 * Returns:
 *	New matrix with the result of the subtract
 *	NULL on failure
 *
 * History:
 * 	15/06/2000 (Mike Bennett): Created function
*/
MATRIX *subMat(MATRIX *first, MATRIX *second) {
	MATRIX *ans;
	int m, n;

	if(first->m != second->m || first->n != second->n)
		return NULL;

	ans = createMat(first->m, first->n);

	for(m = 0; m < first->m; m++)
		for(n = 0; n < first->n; n++)
			ans->me[m][n] = first->me[m][n] - second->me[m][n];

	return(ans);
}


/*
 * Purpose:
 *	Multiply two matrix's together
 *
 * Parameters:
 * 	1st - first matrix to multiply
 *  2nd - second matrix to multiply
 *
 * Returns:
 *	New matrix with the result of the multiply
 *	NULL on failure
 *	
 * NOTE:
 * 	The first matrix multiplies the second
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	10/06/2000 (Mike Bennett): Rewrote to get rid of Meschach
*/
MATRIX *mulMat(MATRIX *first, MATRIX *second) {
	MATRIX *ans;
	int m, n, i;

	if(first->n != second->m)
		return NULL;

	ans = createMat(first->m, second->n);

	/* Loop the loop multiply like magic - this algo is loosely
	 * based on Meschach's */
	for(m = 0; m < ans->m; m++)
		for(n = 0; n < ans->n; n++)
			for(i = 0; i < second->m; i++)
				ans->me[m][n] += first->me[m][i] * second->me[i][n];

	return(ans);
}


/*
 * Purpose:
 *	Create the transpose of a matrix
 *
 * Parameters:
 * 	Matrix to get the transpose of
 *
 * Returns:
 *	New matrix with the transpose
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	10/06/2000 (Mike Bennett): Rewrote to get rid of Meschach
*/
MATRIX *transMat(MATRIX *matrix) {
	MATRIX *ans;
	int m, n;

	ans = createMat(matrix->n, matrix->m);	

	/* Simplicity embodied - easy to do and elegant*/
	for(m = 0; m < matrix->m; m++)
		for(n = 0; n < matrix->n; n++)
			ans->me[n][m] = matrix->me[m][n];

	return(ans);
}


/*
 * Purpose:
 *	Resize of an existing matrix
 *
 * Parameters:
 * 	1st - Matrix to resize
 * 	2nd - New size of units across
 * 	3rd - New size of units down
 *
 * Returns:
 *	Resized matrix
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	10/06/2000 (Mike Bennett): Rewrote to get rid of Meschach
*/
MATRIX *resizeMat(MATRIX *matrix, int m, int n) {
	MATRIX *ans;

	ans = createMat(m, n);

	for(n = 0; n < matrix->n && n < ans->n; n++)
		for(m = 0; m < matrix->m && m < ans->m; m++)
			ans->me[m][n] = matrix->me[m][n];

	freeMat(matrix);
	matrix = ans;

	return(matrix);
}


/*
 * Purpose:
 *	Rescale an existing matrix. Note this differs from
 *	resizeMat() because it changes the shape of an 
 *	existing matrix while keeping the sames values
 *
 * Parameters:
 * 	1st - Matrix to rescale
 * 	2nd - New size of units across
 * 	3rd - New size of units down
 *
 * Returns:
 *	Rescaled matrix
 *
 * History:
 * 	08/05/2000 (Mike Bennett): Created function
*/
MATRIX *rescaleMat(MATRIX *matrix, int m, int n) {
	MATRIX *ans;
	int a, b, x, y;

	ans = createMat(m, n);

	/* Walk across then down */
	for(a = 0, b = 0, x = 0; x < matrix->m; x++) {
		for(y = 0; y < matrix->n; y++, b++) {
			/* do we move to next line to transfer to */
			if(b == ans->m) {
				b = 0;
				a++;
			}
			ans->me[a][b] = matrix->me[x][y];
		}
	}

	return ans;
}


/*
 * Purpose:
 * 	Compare two matrices
 *
 * Parameters:
 * 	1st - Matrix to do compare against
 * 	2nd - Matrix to compare with
 *
 * Returns:
 *	GOOD on match
 *	BAD on failure
 *
 * History:
 * 	08/05/2000 (Mike Bennett): Created function
*/
int cmpMat(MATRIX *first, MATRIX *second) {
	int m, n;

	if(first->m != second->m || first->n != second->n)
		return BAD;

	for(m = 0; m < first->m; m++)	
		for(n = 0; n < first->n; n++)
			if(first->me[m][n] != second->me[m][n])
				return BAD;

	return GOOD;
}


/*
 * Purpose:
 *	Join the second matrix onto the first
 *
 * Parameters:
 * 	1st - matrix to get entended
 * 	2nd - matrix to join onto the first one
 *
 * Returns:
 * 	Pointer to the first matrix with the second added
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	16/06/2000 (Mike Bennett): Forgot to return matrix
*/
MATRIX *joinMat(MATRIX *first, MATRIX *second) {
	int m, n, a, b;

	if(first->m != second->m)
		return NULL;

	n = first->n;
	
	resizeMat(first, first->m, first->n + second->n);

	for(m = 0; m < first->m; m++)
		for(a = n, b = 0; a < first->n; a++, b++)
				first->me[m][a] = second->me[m][b];

	return(first);
}


/*
 * Purpose:
 *	Clear memory used by matrix
 *
 * Parameters:
 * 	Matrix to free the memory of
 *
 * Returns:
 * 	NONE
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	07/06/2000 (Mike Bennett): Free memory in the MATRIX struct
*/
void freeMat(MATRIX *matrix) {
	int i;

	for(i = 0; i < matrix->m; i++)
		free(matrix->me[i]);

	free(matrix->me);
	free(matrix);
}



/*
 * Purpose:
 *	Print the matrix to stdout
 *
 * Parameters:
 * 	Matrix to print
 *
 * Returns:
 * 	NONE
 *
 * History:
 * 	15/04/2000 (Mike Bennett): Created function
 * 	10/06/2000 (Mike Bennett): Got rid of printing floats
 *  16/05/2000 (Mike Bennett): Made print out comma's
*/
void printMat(MATRIX *matrix) {
	int m, n;

	printf("\n\nMatrix:\n");

	for(n = 0; n < matrix->n; n++) {
		for(m = 0; m < matrix->m; m++)
			if(n == 0 && m == 0)
				printf(" %d", matrix->me[m][n]);
			else
				printf(",%d", matrix->me[m][n]);

		printf("\n");
	}
}


/*
 * Purpose:
 *	Print a VECTOR to stdout
 *
 * Parameters:
 * 	Vector to print
 *
 * Returns:
 * 	NONE
 * 	
 * Note:
 * 	As Simon rightly pointed this code doesn't belong in this file,
 * 	but in honesty it doesn't have a better home at the moment so
 * 	we'll let it get cosy here :) (Mike Bennet)
 *
 * History:
 * 	02/08/2000 (Simon Hart): Created function
*/
void printVec(VECTOR *v) {
	int i;
	
	printf("\n\nVector:\n");
	
	for(i = 0; v[i] != DIR_NONE; i++) {
		switch(v[i]) {
			case DIR_N: printf("N "); break;
			case DIR_NE: printf("NE "); break;
			case DIR_E: printf("E "); break;
			case DIR_SE: printf("SE "); break;
			case DIR_S: printf("S "); break;
			case DIR_SW: printf("SW "); break;
			case DIR_W: printf("W "); break;
			case DIR_NW: printf("NW "); break;
			default: printf("\nWarning: Invalid vector %d\n",v[i]);
		}
	}
	
	printf("\n");
}
