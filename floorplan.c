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
** floorplan.c:floorplan module for multi-sink Lee-Moore shortest path maze router**
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
#include "floorplan.h"
#include "graphics.h"
#include "array.h"
#include "common.h"


/* returns a new grid floorplan data structure, size colXrow */
fp *fpCreateGrid(unsigned int col, unsigned int row) {
	int i;
	int fpSize = col*row;
	fp *f = (fp*)safeMalloc(sizeof(fp));
	f->col = col;
	f->row = row;
	f->cellValue = (unsigned int*)safeMalloc(sizeof(unsigned int)*fpSize);
	f->waveValue = (unsigned int*)safeMalloc(sizeof(unsigned int)*fpSize);
	f->cellType = (char*        )safeMalloc(sizeof(unsigned int)*fpSize);
	f->waveType = (char*        )safeMalloc(sizeof(unsigned int)*fpSize);
	for (i=0;i<fpSize;i++) {
		f->cellValue[i] = UINT_MAX;
		f->waveValue[i] = UINT_MAX;
		f->cellType[i] = 'E';
		f->waveType[i] = 'U';
	}
	return f;
}

/* creates a new wires data structure inside a floorplan structure */
void fpCreateWires(fp *f, unsigned int wireNum) {
	int i;
	f->wireNum = wireNum;
	f->srcX = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->srcY = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->bbox = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->dstNum = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->routeOrder = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->ripCnt = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	f->ripCntAcc = (unsigned int*)safeMalloc(sizeof(unsigned int)*wireNum);
	for (i=0;i<wireNum;i++) {
		f->srcX[i] = 0;
		f->srcY[i] = 0;
		f->bbox[i] = 0;
		f->dstNum[i] = 0;
		f->routeOrder[i] = i;
		f->ripCnt[i] = 0;
		f->ripCntAcc[i] = 0;
	}
}

/* free a floorplan data structure */
void fpDelete(fp *f){
	safeFree(f->cellValue);
	safeFree(f->cellType);
	safeFree(f->waveValue );
	safeFree(f->waveType);
	safeFree(f->srcX);
	safeFree(f->srcY);
	safeFree(f->bbox);
	safeFree(f->dstNum);
	safeFree(f->routeOrder);
	safeFree(f->ripCnt);
	safeFree(f->ripCntAcc);
	safeFree(f);
}

/* set floorplan grid cell value at (iCol,iRow) floorplan grid place*/
inline void fpSetCellValue(fp *f, unsigned int iCol, unsigned int iRow, unsigned int cellValue){
    f->cellValue[iRow*f->col+iCol]=cellValue;
}

/* set expansion wave value at (iCol,iRow) floorplan grid place */
inline void fpSetWaveValue(fp *f, unsigned int iCol, unsigned int iRow, unsigned int waveValue){
    f->waveValue[iRow*f->col+iCol]=waveValue;
}

/* set floorplan grid cell type at (iCol,iRow) floorplan grid place*/
inline void fpSetCellType(fp *f, unsigned int iCol, unsigned int iRow, char cellType){
    f->cellType[iRow*f->col+iCol]=cellType;
}

/* set expansion wave type at (iCol,iRow) floorplan grid place*/
inline void fpSetWaveType(fp *f, unsigned int iCol, unsigned int iRow, char waveType){
    f->waveType[iRow*f->col+iCol]=waveType;
}

/* get floorplan grid cell value from (iCol,iRow) floorplan grid place*/
inline unsigned int fpGetCellValue(fp *f, unsigned int iCol, unsigned int iRow){
    return(f->cellValue[iRow*f->col+iCol]);
}

/* get expansion wave value from (iCol,iRow) floorplan grid place */
inline unsigned int fpGetWaveValue(fp *f, unsigned int iCol, unsigned int iRow){
    return(f->waveValue[iRow*f->col+iCol]);
}

/* get floorplan grid cell type from (iCol,iRow) floorplan grid place*/
inline char fpGetCellType(fp *f, unsigned int iCol, unsigned int iRow){
    return(f->cellType[iRow*f->col+iCol]);
}

/* get expansion wave type from (iCol,iRow) floorplan grid place*/
inline char fpGetWaveType(fp *f, unsigned int iCol, unsigned int iRow){
    return(f->waveType[iRow*f->col+iCol]);
}

/* set location (srcX,srcY) and number of destinations (dstNum) and bounding box area (bbox) of wire #iWire */
void fpSetWire(fp *f, unsigned int iWire, unsigned int srcX, unsigned int srcY, unsigned int dstNum, unsigned int bbox) {
	f->srcX[iWire]=srcX;
	f->srcY[iWire]=srcY;
	f->dstNum[iWire]=dstNum;
	f->bbox[iWire]=bbox;
}

/* draw floorplan using EasyGl graphics module. Wold size is (xDim X yDim) */
void fpDraw(fp *f, float xDim, float yDim) {
	unsigned int iRow=0, iCol=0;                /* counters         */
	unsigned int row=f->row, col=f->col;        /* dimention        */
	float curRow, curCol;                       /* drawing location */
	float step=MIN(xDim/(col+7),yDim/(row+5));  /* grid pitch       */
	char  label[16];                            /* general   label  */
	char slabel[16];                            /* source    label  */
	char tlabel[16];                            /* target    label  */
	char wlabel[16];                            /* wire      label  */
	char elabel[16];                            /* expansion label  */
	char	waveType;
	char	cellType;
	float epsilon=0.001;
	
	/**** draw header message ****/
	setcolor(LIGHTGREY);
	fillrect(0,0,(col+7)*step,step);
	setcolor(BLACK);
	drawtext((col/2+3.5)*step,step/2,"Lee-Moore Shortest Path Maze Router",(col+7)*step);

	/**** draw global message ****/
	setcolor(LIGHTGREY);
	fillrect(0,(row+4)*step,(col+7)*step,(row+5)*step);
	setcolor(BLACK);
	drawtext((col/2+3.5)*step,(row+4.5)*step,glabel,(col+7)*step);


	/**** draw legend ****/
	 
	/* legend tag */
	setcolor(LIGHTGREY);
	fillrect((col+2)*step,2*step,(col+7)*step,(col+3)*step);
	setcolor(BLACK);
	drawtext((col+4.5)*step,3.5*step,"Legend",5*step);

	/* blockage tag */
	fillrect((col+2.5)*step,5*step,(col+3.5)*step,6*step);
	setcolor(WHITE);
	drawtext((col+3)*step,5.5*step,"B",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,5.5*step,"Blockage",4.5*step);

	/* unrouted source tag */
	setcolor(RED);
	fillrect((col+2.5)*step,7*step,(col+3.5)*step,8*step);
	setcolor(WHITE);
	drawtext((col+3)*step,7.5*step,"Ri",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,7.25*step,"Unrouted",4.5*step);
	drawtext((col+5.25)*step,7.75*step,"source",4.5*step);

	/* routed source tag */
	setcolor(RED);
	fillrect((col+2.5)*step,9*step,(col+3.5)*step,10*step);
	setcolor(WHITE);
	drawtext((col+3)*step,9.5*step,"Ri",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,9.25*step,"Routed",4.5*step);
	drawtext((col+5.25)*step,9.75*step,"source",4.5*step);
	drawline((col+2.5)*step,9*step,(col+3.5)*step,10*step);
	drawline((col+2.5)*step,10*step,(col+3.5)*step,9*step);	

	/* unrouted target tag */
	setcolor(DARKGREEN);
	fillrect((col+2.5)*step,11*step,(col+3.5)*step,12*step);
	setcolor(WHITE);
	drawtext((col+3)*step,11.5*step,"Ti",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,11.25*step,"Unrouted",4.5*step);
	drawtext((col+5.25)*step,11.75*step,"target",4.5*step);

	/* routed target tag */
	setcolor(DARKGREEN);
	fillrect((col+2.5)*step,13*step,(col+3.5)*step,14*step);
	setcolor(WHITE);
	drawtext((col+3)*step,13.5*step,"Ti",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,13.25*step,"Routed",4.5*step);
	drawtext((col+5.25)*step,13.75*step,"target",4.5*step);
	drawline((col+2.5)*step,13*step,(col+3.5)*step,14*step);
	drawline((col+2.5)*step,14*step,(col+3.5)*step,13*step);

	/* partially routed wire tag */
	setcolor(CYAN);
	fillrect((col+2.5)*step,15*step,(col+3.5)*step,16*step);
	setcolor(BLACK);
	drawtext((col+3)*step,15.5*step,"Wi",step);
	drawtext((col+5.25)*step,15.25*step,"Partial",4.5*step);
	drawtext((col+5.25)*step,15.75*step,"wire",4.5*step);

	/* fully routed wire tag */
	setcolor(BLUE);
	fillrect((col+2.5)*step,17*step,(col+3.5)*step,18*step);
	setcolor(WHITE);
	drawtext((col+3)*step,17.5*step,"Wi",step);
	setcolor(BLACK);
	drawtext((col+5.25)*step,17.25*step,"Full",4.5*step);
	drawtext((col+5.25)*step,17.75*step,"wire",4.5*step);

	/* wave expansion forehead tag */
	setcolor(YELLOW);
	fillrect((col+2.5)*step,19*step,(col+3.5)*step,20*step);
	setcolor(BLACK);
	drawtext((col+3)*step,19.5*step,"i",step);
	drawtext((col+5.25)*step,19.25*step,"Wave",4.5*step);
	drawtext((col+5.25)*step,19.75*step,"forehead",4.5*step);

	/* wave expansion history tag */
	drawrect((col+2.5)*step,21*step,(col+3.5)*step,22*step);
	drawtext((col+3)*step,21.5*step,"i",step);
	drawtext((col+5.25)*step,21.25*step,"Wave",4.5*step);
	drawtext((col+5.25)*step,21.75*step,"history",4.5*step);

	/**** draw vertical ruler ****/
	iRow=0;
	for (curRow=3*step;curRow<=((2+row)*step+epsilon);curRow+=step) {
		sprintf(label,"%u",iRow);
		setcolor(BLACK);
		drawtext(step/2,curRow+step/2,label,step);
		drawline(step*0.8,curRow,step,curRow);
		iRow++;
	}

	/**** draw horizontal ruler ****/
	iCol=0;
	for (curCol=step;curCol<=(col*step+epsilon);curCol+=step) {
		sprintf(label,"%u",iCol);
		setcolor(BLACK);
		drawtext(curCol+step/2,2.5*step,label,step);
		drawline(curCol,step*2.8,curCol,3*step);
		iCol++;
	}
	
	/**** draw floorplan grid ****/
	iRow=0; iCol=0;
	for (curRow=3*step;curRow<((2+row)*step+epsilon);curRow+=step) {
		for (curCol=step;curCol<(col*step+epsilon);curCol+=step) {
			
			sprintf(label,"%u",fpGetCellValue(f,iCol,iRow));
			sprintf(elabel,"%u",fpGetWaveValue(f,iCol,iRow));			
			waveType = fpGetWaveType(f,iCol,iRow);
			setcolor(BLACK);

			cellType = fpGetCellType(f,iCol,iRow);
			switch(toupper(cellType)) {
				case 'E':	/* empty cell */
					if (waveType == 'V') {	/* visited while expansion*/
						setcolor(LIGHTGREY);
						fillrect(curCol,curRow,curCol+step,curRow+step);
						setcolor(BLACK);
						drawtext(curCol+step/2,curRow+step/2,elabel,step);
					} else if (toupper(waveType) == 'F') {	/* wave expansion forehead */
						setcolor(YELLOW);
						fillrect(curCol,curRow,curCol+step,curRow+step);
						setcolor(BLACK);
						drawtext(curCol+step/2,curRow+step/2,elabel,step);
					}
					break;
				case 'B':	/* blockage cell */
					setcolor(BLACK);
					fillrect(curCol,curRow,curCol+step,curRow+step);
					setcolor(WHITE);
					drawtext(curCol+step/2,curRow+step/2,(char*)"B",step);
					break;
				case 'W':	/* wire cell */
					if (cellType == 'w') {setcolor(CYAN);} else {setcolor(BLUE);}	/* fully routed     */
					fillrect(curCol,curRow,curCol+step,curRow+step);
					if (cellType == 'w') {setcolor(BLACK);} else {setcolor(WHITE);}	/* partially routed */
					strcpy(wlabel,"w");
					strcat(wlabel,label);
					drawtext(curCol+step/2,curRow+step/2,wlabel,step);
					setcolor(BLACK);
					break;
				case 'S':	/* source cell */
					setcolor(RED);
					fillrect(curCol,curRow,curCol+step,curRow+step);
					setcolor(WHITE);
					strcpy(slabel,"S");
					strcat(slabel,label);
					drawtext(curCol+step/2,curRow+step/2,slabel,step);
					if (cellType == 's') { /* routed source */
						setcolor(BLACK);
						drawline(curCol,curRow,curCol+step,curRow+step);
						drawline(curCol,curRow+step,curCol+step,curRow);
					}
					setcolor(BLACK);
					break;
				case 'T':	/* target cell */
					setcolor(DARKGREEN);
					fillrect(curCol,curRow,curCol+step,curRow+step);
					setcolor(WHITE);
					strcpy(tlabel,"T");
					strcat(tlabel,label);
					drawtext(curCol+step/2,curRow+step/2,tlabel,step);
					if (cellType == 't') { /* routed target */
						setcolor(BLACK);
						drawline(curCol,curRow,curCol+step,curRow+step);
						drawline(curCol,curRow+step,curCol+step,curRow);
					}
					setcolor(BLACK);
			}
			drawrect(curCol,curRow,curCol+step,curRow+step);
			iCol++;
		}
		iCol=0;
		iRow++;
	}
}

/* parse floorplan infile format into a new floorplan structue, return a new allocated structure */
fp *fpInfileParse(const char* fileName) {
	FILE *infile;
	fp *f;
	char line[128];
	unsigned int buf[128];
	int i=0,bufInd=-1;
	char delims[] = " \t\n"; /* delimiters: space, tap, and new line */
	char *res = NULL;
	unsigned long val;
	unsigned int lineNum=1;
	unsigned int blockCnt=0, wireCnt=0;
	int state=1;
	unsigned int gridCols=0, gridRows=0, blockNum=0, wireNum=0;
	unsigned int maxX,maxY,minX,minY;

	infile=fopen(fileName,"r");
	if(infile==NULL) { /* open failed */
		printf("-E- File open error %s! Exiting...\n", fileName); exit(-1);
	}

	while (fgets(line,sizeof line,infile) != NULL ) {	/* read a line */

		bufInd=-1;
		res = strtok( line, delims );     /* read token from line               */
		while(res != NULL) {              /* read all tokens                    */
			val = strtoul(res, NULL, 10); /* convert string to unsigned int     */
			if (errno != 0) {             /* conversion failed (EINVAL, ERANGE) */
				printf("-E- Parsing error in file %s, line %u: value is not positive integer! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
        		bufInd++;		/* holds number of tokens */
			buf[bufInd]=val;	/* hold integer tokens */

			
			res = strtok(NULL,delims); /* next token */
		}
		if (bufInd<0) continue; /* empty line */

		switch(state) {

		 /*read gird size*/
		 case 1:if (bufInd != 1) {
				printf("-E- Parsing error in file %s, line %u: Should include two integer numbers! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
			gridCols=buf[0];
			gridRows=buf[1];
			f=fpCreateGrid(gridCols,gridRows);
			state=2;
			break;

		 /* read number of blockages */
		 case 2:if (bufInd != 0) {
				printf("-E- Parsing error in file %s, line %u: Should include one integer numbers",fileName,lineNum);
				exit(-1);
			}
			blockNum=buf[0];
			state=3;
			break;

		 /* read blockages coordinates*/
		 case 3:if (blockCnt==blockNum-1) state=4;
			if (bufInd != 1) {
				printf("-E- Parsing error in file %s, line %u: Should include two integer numbers! Exiting...\n",fileName,lineNum);
				exit(-1);
			}	
			fpSetCellType(f,buf[0],buf[1],'B');
			blockCnt++;
			break;

		 /* read number of wires */
		 case 4:if (bufInd != 0) { 
				printf("-E- Parsing error in file %s, line %u: Should include one integer numbers! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
			wireNum=buf[0];
			fpCreateWires(f,wireNum);
			state=5;
			break;

		 /* read a wire */
		 case 5:if (wireCnt==wireNum-1) state=6; /* if finished check extra lines */
			if (bufInd < 0) { 
				printf("-E- Parsing error in file %s, line %u: Should include at least one integer numbers! Exiting...\n",fileName,lineNum);
				exit(-1);
			}
			if (bufInd != (2*buf[0])) { /* read a wire */
				printf("-E- Parsing error in file %s, line %u: Should include %u integer numbers! Exiting...\n",fileName,lineNum, 1+2*buf[0]);
				exit(-1);
			}
			
			/* set wire source */
			fpSetCellType(f,buf[1],buf[2],'S');
			fpSetCellValue(f,buf[1],buf[2],wireCnt);
			
			/* find wire bbox */
			maxX=buf[1];
			minX=buf[1];
			maxY=buf[2];
			minY=buf[2];
			for (i=3;i<=bufInd;i+=2) {
				fpSetCellType(f,buf[i],buf[i+1],'T');
				fpSetCellValue(f,buf[i],buf[i+1],wireCnt);
				maxX=MAX(maxX,buf[i]);
				minX=MIN(minX,buf[i]);
				maxY=MAX(maxY,buf[i+1]);
				minY=MIN(minY,buf[i+1]);
			}

			/* set wire attributes */
			fpSetWire(f,wireCnt,buf[1],buf[2],(bufInd-1)/2,(maxX-minX+1)*(maxY-minY+1));
			
			wireCnt++; /* next wire */
			break;

		 /* check extra lines */
		 case 6: if (bufInd != -1) { 
				printf("-E- Parsing error in file %s, line %u: Extra line! Exiting...\n",fileName,lineNum);
				exit(-1);
			}

		} /* switch */		
	} /* while fgets */

	fclose(infile);
	return f;
}

/**************************************************************
* Lee-Moore multi-destination maze routing related algorithms *
***************************************************************/

/* expand wave related to wire #wireNum from targer (targetX,targetY)        *
 * returns:- 0: wave is not expandable, 1: wave is expanded, 2: target found */
int fpWaveExpand(fp  *f,unsigned int wireNum, unsigned int *targetX, unsigned int *targetY) {
	unsigned int srcX=f->srcX[wireNum];
	unsigned int srcY=f->srcY[wireNum];
	unsigned int iRow, iCol, rows=f->row, cols=f->col;
	char	     waveType;
	char	     cellType;
	unsigned int val,expVal;
	int expandable = 0;
	int found = 0;
	
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			waveType = fpGetWaveType(f,iCol,iRow);
			if (waveType == 'F') {	/* if wave forehead -> explore */
				expandable = 1;
				expVal  = fpGetWaveValue(f,iCol,iRow);
				if (iCol < cols-1) { /*Avoid boundary*/
					cellType = fpGetCellType(f,iCol+1,iRow);
					waveType = fpGetWaveType(f,iCol+1,iRow);
					val	= fpGetCellValue(f,iCol+1,iRow);
					if ( (cellType == 'E') && (waveType == 'U') ) {
						fpSetWaveType(f,iCol+1,iRow,'f');
						fpSetWaveValue(f,iCol+1,iRow,expVal+1);
					} else if ( (cellType == 'T') && (val == wireNum) ) {
						*targetX = iCol+1;
						*targetY = iRow;
						found = 1;
					}
				}
				if (iCol > 0) { /*Avoid boundary*/
					cellType = fpGetCellType(f,iCol-1,iRow);
					waveType = fpGetWaveType(f,iCol-1,iRow);
					val	= fpGetCellValue(f,iCol-1,iRow);
					if ( (cellType == 'E') && (waveType == 'U') ) {
						fpSetWaveType(f,iCol-1,iRow,'f');
						fpSetWaveValue(f,iCol-1,iRow,expVal+1);
					} else if ( (cellType == 'T') && (val == wireNum) ) {
						*targetX = iCol-1;
						*targetY = iRow;
						found = 1;
					}
				}
				if (iRow < rows-1) { /*Avoid boundary*/
					cellType = fpGetCellType(f,iCol,iRow+1);
					waveType = fpGetWaveType(f,iCol,iRow+1);
					val	= fpGetCellValue(f,iCol,iRow+1);
					if ( (cellType == 'E') && (waveType == 'U') ) {
						fpSetWaveType(f,iCol,iRow+1,'f');
						fpSetWaveValue(f,iCol,iRow+1,expVal+1);
					} else if ( (cellType == 'T') && (val == wireNum) ) {
						*targetX = iCol;
						*targetY = iRow+1;
						found = 1;
					}
				}
				if (iRow > 0) { /*Avoid boundary*/
					cellType = fpGetCellType(f,iCol,iRow-1);
					waveType = fpGetWaveType(f,iCol,iRow-1);
					val	= fpGetCellValue(f,iCol,iRow-1);
					if ( (cellType == 'E') && (waveType == 'U') ) {
						fpSetWaveType(f,iCol,iRow-1,'f');
						fpSetWaveValue(f,iCol,iRow-1,expVal+1);
					} else if ( (cellType == 'T') && (val == wireNum) ) {
						*targetX = iCol;
						*targetY = iRow-1;
						found = 1;
					}
				}
				fpSetWaveType(f,iCol,iRow,'V');
				if (found) {
					fpSetWaveValue(f,*targetX,*targetY,expVal+1);
					if (verbose) {printf("-I- Destination found at (%u,%u)\n",*targetX,*targetY);}
					if (steppingMode=='W') {
						sprintf(glabel,"Target found for source #%u",wireNum);
						waitLoop();
						if (psEnable) postscript(drawScreen);
					}
				}
			} /* if 'F' */
		if (found) break;
		} /* for columns */
	if (found) break;
	} /* for raws */

	/* Move newly wave forehead tile to the next expansion iteration */
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
				if (fpGetWaveType(f,iCol,iRow) == 'f') {
					fpSetWaveType(f,iCol,iRow,'F');
				}
		}
	}

	/* if wave expansion mode and not found, print wave expansion message */
	if ((steppingMode=='W') && (!found)) {	
		sprintf(glabel,"Wave expansion #%u from source #%u",expVal+1,wireNum);
		waitLoop();
		if (psEnable) postscript(drawScreen);
	}

	if (found == 1) return 2;
	return expandable;
	
}

/* route wire #wireNum to any avaliable unrouted target            *
 *	returns:- 0: Source could not be routed to any relevant target *
 *	          1: Source has been routed to a relevant target       */
int fpRouteDestination(fp  *f, unsigned int wireNum) {
	unsigned int srcX=f->srcX[wireNum];
	unsigned int srcY=f->srcY[wireNum];
	unsigned int iRow, iCol, rows=f->row, cols=f->col;
	char cellType, waveType;
	unsigned int val;
	unsigned int targetX,targetY;
	unsigned int i;
	int expRes = 1;
	int ret=0;

	/* initialize before route */
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			val = fpGetCellValue(f,iCol,iRow);
			cellType = fpGetCellType(f,iCol,iRow);
			waveType = fpGetWaveType(f,iCol,iRow); 
			if ( ((cellType == 'S') && (val == wireNum)) ||     /* if relevant source        */
				 ((cellType == 'w') && (val == wireNum)) ||     /* or relevant wire          */
                 ((cellType == 't') && (val == wireNum))    ) { /* or relevant routed target */
				fpSetWaveType(f,iCol,iRow,'F');           		/* set as wave forehead      */
				fpSetWaveValue(f,iCol,iRow,0);		            /* reset expansion value     */

			} else {	/* set to unvisited */
				fpSetWaveType(f,iCol,iRow,'U');
				fpSetWaveValue(f,iCol,iRow,UINT_MAX); 
			}			
		}
	}
	for(i=0;i<(f->wireNum);i++) {f->ripCnt[i] = 0;} /* reset rip-up counter*/

	/* while expandable and target not found, expand */
	while (expRes == 1) {
		expRes = fpWaveExpand(f,wireNum,&targetX,&targetY);
	}

	/* target found, traceback */
	if (expRes ==2) {
		if (verbose) {printf("-I- Target found at (%u,%u) \n",targetX,targetY);}
		fpTraceback(f,targetX,targetY,srcX,srcY,wireNum,tracebackMode);
		ret=1;
	}

	/* reset floorplan */
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			fpSetWaveType(f,iCol,iRow,'U');
			fpSetWaveValue(f,iCol,iRow,UINT_MAX);
		}
	}

	/* print message if wave or destination mode */
	if ( ((steppingMode=='W') || (steppingMode=='D')) && ret) {	
		sprintf(glabel,"A target for wire #%u has been routed",wireNum);
		waitLoop();
		if (psEnable) postscript(drawScreen);
	}

	return ret;
}

/* traceback wire #wireNum from target (tarX,tarY) towards source (srcX,srcY) *
 * trace modes:- 'T': minimum turns, 'C': minimum contour                     */
void fpTraceback(fp  *f, unsigned int tarX, unsigned int tarY,
                         unsigned int srcX, unsigned int srcY,
                         unsigned int wireNum, char traceMode) {

	unsigned int curX=tarX;
	unsigned int curY=tarY;
	unsigned int rows=f->row;
	unsigned int cols=f->col;
	unsigned int valUp=UINT_MAX;
	unsigned int valDown=UINT_MAX;
	unsigned int valRight=UINT_MAX;
	unsigned int valLeft=UINT_MAX;
	unsigned int waveCnt=fpGetWaveValue(f,curX,curY);
	char *traceOrder="RDLU";	/* R:Right, D:Down, L:Left, U:Up */
	char prvDir='R';			/* previous direction */
	int i;

	for (waveCnt=fpGetWaveValue(f,tarX,tarY);waveCnt>0;waveCnt--) { /* traceback from target */

		/* get neighbors' expansion values */
		valUp=UINT_MAX;
		valDown=UINT_MAX;
		valRight=UINT_MAX;
		valLeft=UINT_MAX;
		if ( curX+1 < cols ) { /* avoid boundary */
			if ((fpGetCellType(f,curX+1,curY)=='E')&&(fpGetWaveType(f,curX+1,curY)=='V')) {
				valRight = fpGetWaveValue(f,curX+1,curY);
			}
		}
		if ( curX > 0 ) { /* avoid boundary */
			if ((fpGetCellType(f,curX-1,curY)=='E')&&(fpGetWaveType(f,curX-1,curY)=='V')) {
				valLeft = fpGetWaveValue(f,curX-1,curY);
			}
		}
		if ( curY+1 < rows ) { /* avoid boundary */
			if ((fpGetCellType(f,curX,curY+1)=='E')&&(fpGetWaveType(f,curX,curY+1)=='V')) {
				valDown = fpGetWaveValue(f,curX,curY+1);
			}
		}
		if ( curY > 0 ) { /* avoid boundary */
			if ((fpGetCellType(f,curX,curY-1)=='E')&&(fpGetWaveType(f,curX,curY-1)=='V')) {
				valUp = fpGetWaveValue(f,curX,curY-1);
			}
		}

		if (traceMode == 'D') { /* direct mode */
			/* try to choose paths that goes directly towards the source */
			if ( (srcX>curX ) && (srcY>=curY) ) traceOrder="RDLU";
			if ( (srcX>curX ) && (srcY<=curY) ) traceOrder="RULD";
			if ( (srcX<curX ) && (srcY>=curY) ) traceOrder="LDRU";
			if ( (srcX<curX ) && (srcY<=curY) ) traceOrder="LURD";
			if ( (srcX>=curX) && (srcY>curY ) ) traceOrder="DRLU";
			if ( (srcX>=curX) && (srcY<curY ) ) traceOrder="URLD";
			if ( (srcX<=curX) && (srcY>curY ) ) traceOrder="DLRU";
			if ( (srcX<=curX) && (srcY<curY ) ) traceOrder="ULRD";
		}
		else if (traceMode == 'M') { /* min turns mode */
			/* consider previous direction first */	
			if (prvDir=='R') traceOrder="RDLU";
			if (prvDir=='D') traceOrder="DLUR";
			if (prvDir=='L') traceOrder="LURD";
			if (prvDir=='U') traceOrder="URDL";
		}

		for(i=0;i<4;i++) { /* check four neighbors */
			if ( (traceOrder[i]=='R') && (valRight==waveCnt) ) {curX++; prvDir='R'; break;}
			if ( (traceOrder[i]=='L') && (valLeft ==waveCnt) ) {curX--; prvDir='L'; break;}
			if ( (traceOrder[i]=='D') && (valDown ==waveCnt) ) {curY++; prvDir='D'; break;}
			if ( (traceOrder[i]=='U') && (valUp   ==waveCnt) ) {curY--; prvDir='U'; break;}
		}

		if (fpGetCellType(f,curX,curY)=='E') {	    /* if empty, mark as wire */
			fpSetCellType(f,curX,curY,'w');
			fpSetCellValue(f,curX,curY,wireNum);
			if (steppingMode=='W') {					/* print message */
				sprintf(glabel,"Trace back from target #%u",wireNum);
				waitLoop();
				if (psEnable) postscript(drawScreen);
			}
		}

	}
	fpSetCellType(f,tarX,tarY,'t'); /* mark as routed target */

}
/* route wire wireNum to all destinations *
 * returns 1 if routeable, 0 other wise   */
int fpRouteWire(fp  *f, unsigned int wireNum) {
	unsigned int iRow, iCol, rows=f->row, cols=f->col;
	unsigned int dstNum=f->dstNum[wireNum];
	unsigned int i=0;
	for (i=0;i<dstNum;i++) { 						/* route all destinations */
		if (fpRouteDestination(f,wireNum) == 0) {	/* if failed, ripup and sned message */
			fpRipupWire(f,wireNum);
			if ((steppingMode=='W') || (steppingMode=='D') || (steppingMode=='N')) {				
				sprintf(glabel,"Wire #%u is not routable!",wireNum);
				waitLoop();
				if (psEnable) postscript(drawScreen);
			}
			return 0;
		}
	}
	fpSetCellType(f,f->srcX[wireNum],f->srcY[wireNum],'s'); /* mark as routed source */

	/* mark partially routed wires as fully routed */
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			if (fpGetCellType(f,iCol,iRow)=='w') {fpSetCellType(f,iCol,iRow,'W');}
		}
	}
	if ((steppingMode=='W') || (steppingMode=='D') || (steppingMode=='N')) { /* message */
		sprintf(glabel,"Wire #%u has been routed successfully!",wireNum);
		waitLoop();
		if (psEnable) postscript(drawScreen);
	}
	return 1;
}

/* ripup wire #wireNum */
void fpRipupWire(fp  *f, unsigned int wireNum) {
	unsigned int iRow, iCol, rows=f->row, cols=f->col;
	unsigned int val;
	char cellType;
	int fullyRouted=0;
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			cellType=fpGetCellType(f,iCol,iRow);
			val=fpGetCellValue(f,iCol,iRow);
			if ( (toupper(cellType) == 'W') && (val == wireNum) ) { /* if wire, make empty */
				fpSetCellType(f,iCol,iRow,'E');
				fpSetCellValue(f,iCol,iRow,UINT_MAX);
			} else if ( (cellType == 's') && (val == wireNum) ) { /* if routed source, make unrouted */
				fpSetCellType(f,iCol,iRow,'S');
				fullyRouted=1;
			} else if ( (cellType == 't') && (val == wireNum) ) { /* if routed target, make unrouted */
				fpSetCellType(f,iCol,iRow,'T');
			}
			fpSetWaveType(f,iCol,iRow,'U');			/* mark unvisited */
			fpSetWaveValue(f,iCol,iRow,UINT_MAX);
		}
	}
	if ( ((steppingMode=='W') || (steppingMode=='D') || (steppingMode=='N')) && fullyRouted) {
		sprintf(glabel,"Wire #%u has been ripped-up!",wireNum);
		waitLoop();
		if (psEnable) postscript(drawScreen);
	}
}

/* route all wires in floorplan, returns the amount of successfully routed wires*/
int fpRouteAll(fp  *f) {
	unsigned int i,ripup,routed=0;
	fpOrderWireByBBox(f);
	for(i=0;i<(f->wireNum);i++) {					/* route all wires */
		if (fpRouteWire(f,(f->routeOrder[i]))) {	/* by order */
			routed ++;								/* if routed */
			if (verbose) {printf("-I- Wire# %u routed successfully\n",f->routeOrder[i]);}
		}
		else { /* unrouteable */
			if (verbose) {printf("-I- failed to route wire# %u\n",f->routeOrder[i]);}
			ripup=fpFindBlockingWire(f,f->routeOrder[i]);			/* choose wire to ripup */
			fpRipupWire(f,ripup); routed--;							/* ripup */
			if (verbose) {printf("-I- ripup wire# %u\n",ripup);}
			if (fpRouteWire(f,(f->routeOrder[i]))) {				/* reroute */
				routed++;
				if (verbose) {printf("-I- Wire# %u routed successfully\n",f->routeOrder[i]);}
			}
			if (fpRouteWire(f,ripup)) {								/* reroute ripped-up */
				routed++;
				if (verbose) {printf("-I- Wire# %u routed successfully\n",ripup);}
			}
		}
	}

	sprintf(glabel,"Routing finished! %u out of %u wires have been routed successfully!",routed,f->wireNum);
	waitLoop();
	if (psEnable) postscript(drawScreen);

	return routed;
}

/* order floorplan wires by their bbox area, f->routerOrder holds the ordered indexes */
void fpOrderWireByBBox(fp  *f) {
	unsigned int  wnum=f->wireNum;
	unsigned int *bbox=f->bbox;
	unsigned int *rord=f->routeOrder;
	unsigned int minind;
	unsigned int i,j;

	/* make a copy of bbox array */
	unsigned int *bboxc = (unsigned int*)malloc(sizeof(unsigned int)*wnum);
	for (i=0;i<wnum;i++) {bboxc[i]=bbox[i];}

	/* sort */
	for (i=0;i<wnum;i++) {
		minind=minInd(bboxc,wnum);
		bboxc[minind]=UINT_MAX;
		rord[i]=minind;
	}
	if (bbox != NULL) {free(bboxc);}
}

/* for each wire, count how many it tackles a wave expansion form (expX,expY) *
 *  results for each wire are located in f->ripCnt                            */
void fpCountBlockage(fp  *f, unsigned int expX, unsigned int expY) {
	unsigned int  wnum=f->wireNum;
	unsigned int i, j, iRow, iCol, rows=f->row, cols=f->col;
	unsigned int curX, curY;
	unsigned int val;
	int expandable=1;
	int add[]={-1,0,+1};			/* direction */
	char cellType,waveType;
	fpSetWaveType(f,expX,expY,'F');

	/* reset ripCnt */
	for (i=0;i<wnum;i++) (f->ripCnt[i])=0;

	/* expand from point to check blocking wires */
	while (expandable) {
		expandable=0;
		for (iRow=0;iRow<rows;iRow++){
			for (iCol=0;iCol<cols;iCol++){

				if (fpGetWaveType(f,iCol,iRow)=='F') {
					expandable=1;
					for (i=0;i<3;i++) {
						for (j=0;j<3;j++) {		/* for all directions */
							if ( (i==0) && (j==0) ) continue;	/* but not the same point */
							curX=iCol+add[i];
							curY=iRow+add[j];
							if ( (curX<0) || (curX>=cols) || (curY<0) || (curY>=rows) ) continue; /* avoid boundary	*/
							cellType = fpGetCellType(f,curX,curY);
							val = fpGetCellValue(f,curX,curY);
							waveType = fpGetWaveType(f,curX,curY);						
							if ( (cellType == 'E') && (waveType == 'U') ) {
								fpSetWaveType(f,curX,curY,'F'); /* expand */
							} else if ((cellType=='W') && (val!=wnum)) (f->ripCnt[val])++;	/* other wire blocks expansion*/								
						} /* for j */
					} /* for i */
				fpSetWaveType(f,iCol,iRow,'V');
				}	/* if 'F' */
			}	/* for iCol */
		}	/* for iRow*/
	} /* while expandable */

	/* reset state */
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			fpSetWaveType(f,iCol,iRow,'U');
		}
	}

}

/* find the maximum blocking wire to wire #wireNum                          *
 * try to expand from the source and each target of wire #wireNum           *
 * if one wire tackles the expansion, it's blocking and should be ripped-up *
 * if more than one wire tackles the expansion, choose the maximum blocking */
unsigned int fpFindBlockingWire(fp  *f, unsigned int wireNum) {
	unsigned int  wnum=f->wireNum;
	unsigned int iRow, iCol, rows=f->row, cols=f->col;
	unsigned int blockingWire;
	unsigned int val,i;
	char cellType;
	for (iRow=0;iRow<rows;iRow++){
		for (iCol=0;iCol<cols;iCol++){
			cellType = fpGetCellType(f,iCol,iRow);
			val = fpGetCellValue(f,iCol,iRow);
			if ( (cellType == 'S') && (val == wireNum) ||		/* if relevant unrouted source			*/
				 (cellType == 'T') && (val == wireNum)	 ) {	/* or relevant unrouted target			*/
				fpCountBlockage(f,iCol,iRow);					/* count wire blockages from that point	*/
				blockingWire=onlyPositiveInd(f->ripCnt,wnum);	/* if only one wire is blocking			*/
				if (blockingWire<UINT_MAX) return blockingWire; /* then it's the problematic wire!		*/
				addArr(f->ripCnt,f->ripCntAcc,wnum);			/* or add to accumulative counter		*/
			}
		} /* for iCol */
	} /* for iRow */
	blockingWire=maxInd(f->ripCntAcc,wnum);	/* maximum blocking		*/
	initArr(f->ripCntAcc,0,wnum);			/* init for next turn	*/
	initArr(f->ripCnt,0,wnum);				/* init for next turn	*/
	return blockingWire;
}
