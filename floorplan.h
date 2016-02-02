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

#ifndef __FLOORPLAN_HEADER__
#define __FLOORPLAN_HEADER__

/* floorplan structure */
struct fpStr	{
	
	/* floorplan Grid */
	unsigned int  col;	     /* number of columns in the floorplan (X dimension)                         */
	unsigned int  row;	     /* number of rows in the floorplan (Y dimension)                            */
	unsigned int *cellValue; /* a value assigned to floorplan tiles                                      */
	unsigned int *waveValue; /* a wave expansion value assigned to floorplan tiles                       */
	char	     *cellType;  /* Grid cell type:-      E:Empty, B:Blockage, W:Wire, S:Source,             */ 
	                         /*                       T:Target s:routed source t:routed target           */
	char	     *waveType;	 /* Expansion wave type:- U:Unvisited, V:Visited,                            */
                             /*                       F:Forehead, f:new forehead                         */

	/* floorplan wires*/
	unsigned int  wireNum;	  /* number of wires                                  */
	unsigned int *srcX;	      /* source x dimension for each source               */
	unsigned int *srcY;	      /* source y dimension for each source               */
	unsigned int *dstNum;	  /* number of destination for each source            */
	unsigned int *bbox;	      /* bbox area of each net                            */
	unsigned int *routeOrder; /* routing order                                    */
	unsigned int *ripCnt;     /* Counts how much a net tackles the wave expansion */
	unsigned int *ripCntAcc;  /* Accumulative Counter                             */
};

typedef struct fpStr fp;

extern fp *gfp;         /* global floorplan database - declaired in maze.c */
extern int verbose;         /* verbose status            - declaired in maze.c */
extern char steppingMode;   /* stepping mode, see maze.c - declaired in maze.c */
extern int psEnable;
extern char tracebackMode;
extern char glabel[128]; /* global text message       - declaired in maze.c */
extern void drawScreen ();

/* returns a new grid floorplan data structure, size colXrow */
fp	        *fpCreateGrid (unsigned int col, unsigned int row);

/* creates a new wires data structure inside a floorplan structure */
void	     fpCreateWires(fp *f, unsigned int wireNum);

/* free a floorplan data structure */
void	     fpDelete(fp *f);

/* set floorplan grid cell value at (iCol,iRow) floorplan grid place*/
inline void	        fpSetCellValue(fp *f, unsigned int iCol, unsigned int iRow, unsigned int CellValue);

/* set expansion wave value at (iCol,iRow) floorplan grid place */
inline void	        fpSetWaveValue(fp *f, unsigned int iCol, unsigned int iRow, unsigned int WaveValue);

/* set floorplan grid cell type at (iCol,iRow) floorplan grid place*/
inline void	        fpSetCellType (fp *f, unsigned int iCol, unsigned int iRow, char cellType);

/* set expansion wave type at (iCol,iRow) floorplan grid place*/
inline void	        fpSetWaveType (fp *f, unsigned int iCol, unsigned int iRow, char waveType);

/* get floorplan grid cell value from (iCol,iRow) floorplan grid place*/
inline unsigned int fpGetCellValue(fp *f, unsigned int iCol, unsigned int iRow);

/* get expansion wave value from (iCol,iRow) floorplan grid place */
inline unsigned int fpGetWaveValue(fp *f, unsigned int iCol, unsigned int iRow);

/* get floorplan grid cell type from (iCol,iRow) floorplan grid place*/
inline char	        fpGetCellType (fp *f, unsigned int iCol, unsigned int iRow);

/* get expansion wave type from (iCol,iRow) floorplan grid place*/
inline char	        fpGetWaveType (fp *f, unsigned int iCol, unsigned int iRow);

/* set location (srcX,srcY) and number of destinations (dstNum) and bounding box area (bbox) of wire #iWire */
void 	            fpSetWire(fp *f, unsigned int iWire, unsigned int srcX, unsigned int srcY,
                                                                      unsigned int dstNum, unsigned int bbox);

/* draw floorplan using EasyGl graphics module. Wold size is (xDim X yDim) */
void	            fpDraw(fp *f, float xDim, float yDim);

/* parse floorplan infile format into a new floorplan structue, return a new allocated structure */
fp	               *fpInfileParse(const char* filename);

/**************************************************************
* Lee-Moore multi-destination maze routing related algorithms *
***************************************************************/

/* expand wave related to wire #wireNum from targer (targetX,targetY)        *
 * returns:- 0: wave is not expandable, 1: wave is expanded, 2: target found */
int                 fpWaveExpand(fp *f,unsigned int wireNum, unsigned int *targetX, unsigned int *targetY);

/* route wire #wireNum to any avaliable unrouted target            *
 *	returns:- 0: Source could not be routed to any relevant target *
 *	          1: Source has been routed to a relevant target       */
int                 fpRouteDestination(fp *f, unsigned int wireNum);

/* traceback wire #wireNum from target (tarX,tarY) towards source (srcX,srcY) *
 * trace modes:- 'T': minimum turns, 'C': minimum contour                     */
void                fpTraceback(fp *f, unsigned int tarX, unsigned int tarY,
                                       unsigned int srcX, unsigned int srcY,
                                       unsigned int wireNum, char traceMode);

/* route wire wireNum to all destinations *
 * returns 1 if routeable, 0 other wise   */
int                 fpRouteWire(fp *f, unsigned int wireNum);

/* ripup wire #wireNum */
void                fpRipupWire(fp *f, unsigned int wireNum);

/* route all wires in floorplan, returns the amount of successfully routed wires*/
int fpRouteAll(fp  *f);

/* order floorplan wires by their bbox area, f->routerOrder holds the ordered indexes */
void                fpOrderWireByBBox(fp *f);

/* for each wire, count how many it tackles a wave expansion form (expX,expY) *
 *  results for each wire are located in f->ripCnt                            */
void                fpCountBlockage(fp *f, unsigned int expX, unsigned int expY);

/* find the maximum blocking wire to wire #wireNum                          *
 * try to expand from the source and each target of wire #wireNum           *
 * if one wire tackles the expansion, it's blocking and should be ripped-up *
 * if more than one wire tackles the expansion, choose the maximum blocking */
unsigned int        fpFindBlockingWire(fp *f, unsigned int wireNum);

#endif /* __FLOORPLAN_HEADER__ */

