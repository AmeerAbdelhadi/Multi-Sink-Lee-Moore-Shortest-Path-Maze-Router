/***********************************************************************************
** Copyright (c) 2011, University of British Columbia (UBC); All rights reserved. **
**                                                                                **
** Redistribution  and  use  in  source   and  binary  forms,   with  or  without **
** modification,  are permitted  provided that  the following conditions are met: **
**   * Redistributions   of  source   code  must  retain   the   above  copyright **
**     notice,  this   list   of   conditions   and   the  following  disclaimer. **
**   * Redistributions  in  binary  form  must  reproduce  the  above   copyright **
**     notice, this  list  of  conditions  and the  following  disclaimer in  the **
**     documentation and/or  other  materials  provided  with  the  distribution. **
**   * Neither the name of the University of British Columbia (UBC) nor the names **
**     of   its   contributors  may  be  used  to  endorse  or   promote products **
**     derived from  this  software without  specific  prior  written permission. **
**                                                                                **
** THIS  SOFTWARE IS  PROVIDED  BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" **
** AND  ANY EXPRESS  OR IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,  THE **
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE **
** DISCLAIMED.  IN NO  EVENT SHALL University of British Columbia (UBC) BE LIABLE **
** FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL **
** DAMAGES  (INCLUDING,  BUT NOT LIMITED TO,  PROCUREMENT OF  SUBSTITUTE GOODS OR **
** SERVICES;  LOSS OF USE,  DATA,  OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER **
** CAUSED AND ON ANY THEORY OF LIABILITY,  WHETHER IN CONTRACT, STRICT LIABILITY, **
** OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE **
** OF  THIS SOFTWARE,  EVEN  IF  ADVISED  OF  THE  POSSIBILITY  OF  SUCH  DAMAGE. **
***********************************************************************************/

/***********************************************************************************
** array.c: unsigned integer array module                                         **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Feb. 2011                 **
***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include "array.h"
#include "common.h"

/* returns index of minimum number */
unsigned int minInd(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int min=arr[0];
		unsigned int minind=0;
		for (i=0;i<arrSize;i++) {
			if (arr[i]<min) { /* new minimum */
				min=arr[i];
				minind=i;
			}
		}
		return minind;
}

/* returns index of maximum number */
unsigned int maxInd(unsigned int *arr, unsigned int arrSize) {
		unsigned int i;
		unsigned int max=arr[0];
		unsigned int maxind=0;
		for (i=0;i<arrSize;i++) {
			if (arr[i]>max) { /* new maximum */
				max=arr[i];
				maxind=i;
			}
		}
		return maxind;
}

/* returns the index of the only positive value (when all others are zero), else return UINT_MAX */
unsigned int onlyPositiveInd(unsigned int *arr, unsigned int arrSize) {
	unsigned int i;
	unsigned int foundInd;
	unsigned int found=0;
	for(i=0;i<arrSize;i++) {
		if ((arr[i])>0) {
			if (found)	return UINT_MAX;	/* another positive found */
			else {							/* first positive found */
				found=1;
				foundInd=i;
			}
		}
	} /* for i */
	if (!found)	return UINT_MAX; /* no positive numbers */
	else		return foundInd; /* return index of the only positive number */
}

/* add values in first array to second array */
void addArr(unsigned int *arr, unsigned int *acc,unsigned int arrSize) {
	int i;
	for (i=0;i<arrSize;i++) acc[i]+=arr[i]; /* add arr to acc element by element */
}

/* initialize all array elements to initVal value */
void initArr(unsigned int *arr, unsigned int initVal, unsigned int arrSize) {
	int i;
	for (i=0;i<arrSize;i++) arr[i]=initVal; /* init all arr elements to initVal */
}
