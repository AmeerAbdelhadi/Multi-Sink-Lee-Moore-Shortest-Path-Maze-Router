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
** array.c: multi-sink Lee-Moore shortest path maze router - main                 **
**                                                                                **
**    Author: Ameer M. Abdelhadi (ameer@ece.ubc.ca, ameer.abdelhadi@gmail.com)    **
**                University of British Columbia (UBC), Feb. 2011                 **
***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include "graphics.h"
#include "common.h"
#include "floorplan.h"
#include "array.h"

/* global variables declaration                                                                       */
fp *gfp;             /* global floorplan database                                                     */
int ps=0;            /* PostScript status                                                             */
char glabel[128]=""; /* global text message                                                           */

/* global variables for argument                                                                      */
int verbose=0;          /* verbose status                                                             */
int psEnable=0;		    /* enable PostScript creation                                                 */
char tracebackMode='M'; /* tracebback mode:- M: Minturn, D: Direct                                    */
char steppingMode='W';  /* Routing steps:- W: Wave, D: Destination, N: Net, R: Route all mode         */

/* functions associated with buttons                                                                  */
inline void waveModeFunc(void (*drawScreen_ptr) (void)) {steppingMode='W';} /* show wave expansion    */
inline void destModeFunc(void (*drawScreen_ptr) (void)) {steppingMode='D';} /* route each destination */
inline void wireModeFunc(void (*drawScreen_ptr) (void)) {steppingMode='N';} /* route each net         */
inline void routeAllFunc(void (*drawScreen_ptr) (void)) {steppingMode='R';} /* route all at once      */

/* redrawing routine for still pictures. Redraw if user changes the window                            */
void drawScreen () { clearscreen(); fpDraw(gfp,1000,1000); } /* clear and redraw                      */

/* called whenever event_loop gets a button press in the graphics area.                               */
void buttonPress (float x, float y, int flags) { }

/* receives the current mouse position in the current world as in init_world                          */
void mouseMove (float x, float y) { }

/* function to handle keyboard press event, the ASCII character is returned	                          */
void keyPress (int i) { }

/* show global message, wait until 'Proceed' pressed then draw screen                                 */
void waitLoop () {
	event_loop(buttonPress,mouseMove,keyPress,drawScreen);
	drawScreen();
	update_message(glabel);
}


/***********************************************  main  ***********************************************
 * arguments: infile             : contains florplan details                                          *
 *           -help       (or -h) : print command usage help                                           *
 *           -verbose    (or -v) : enables verbose logging mode                                       *
 *           -postscript (or -p) : enables PostScript plotting                                        *
 *           -stepping   (or -s) : choose stepping mode, should be followed by one of the following:  *
 *                                 wave        (or w) : shows wave expansion (default)                *
 *                                 destination (or d) : shows routing of each destination             *
 *                                 net         (or n) : shows routing of each net                     *
 *                                 routeall    (or r) : shows final routing result                    *
 ******************************************************************************************************/
int main(int argc, char *argv[]) {

	/* arguments parsing */
	int argi; /* arguments index */
	int fileNameArgInd=-1; /* file name argument index */
	char psm,sm,tbm;
	for(argi=1;argi<argc;argi++) { /* check all argument */
		if (argv[argi][0]=='-') { /* switch argument */
			switch (tolower(argv[argi][1])) { /* consider first letter */

				/* help */
				case 'h':	printf("Lee-Moore Shortest Path Maze Router\n");
							printf("Usage:\n");
							printf("\tmaze [OPTIONS] INFILE\n");
							printf("Options:\n");
							printf("\t-help       (also -h): print this message\n");
							printf("\t-verbose    (also -v): verbose logging\n");
							printf("\t-postscript (also -p): generate PostScript every step\n");
							printf("\t-traceback  (also -t): traceback mode, followed by one of the following\n");
							printf("\t\tminturn     (also m): avoid turns, try to keep same direction (default)\n");
							printf("\t\tdirect      (also d): use direct paths toward source\n");
							printf("\t-stepping   (also -s): stepping mode, followed by one of the following\n");
							printf("\t\twave        (also w): wave expansion (default)\n");
							printf("\t\tdestination (also d): route one destination at once\n");
							printf("\t\tnet         (also n): route one net at once\n");
							printf("\t\trouteall    (also n): route all nets at once\n");
							printf("\tinfile syntax:\n");
							printf("\t\t<FLOORPLAN COORDINATES>\n");
							printf("\t\t<BLOCKAGES#>\n");
							printf("\t\t<BLOCKAGE 1 COORDINATE>\n");
							printf("\t\t<BLOCKAGE 2 COORDINATE>\n");
							printf("\t\t...\n");
							printf("\t\t<BLOCKAGE b COORDINATE>\n");
							printf("\t\t<NETS#>\n");
							printf("\t\t<NET 1 TARGETS#> <DEST. 1 COORD.> ... <DEST. t1 COORD.>\n");
							printf("\t\t...\n");
							printf("\t\t<NET 2 TARGETS#> <DEST. 1 COORD.> ... <DEST. t2 COORD.>\n");
							printf("\t\t<NET n TARGETS#> <DEST. 1 COORD.> ... <DEST. tm COORD.>\n");
							printf("Examples:\n");
							printf("\tmaze a.infile (using default options)\n");
							printf("\tmaze a.infile -verbose -postscript -stepping destination\n");
							printf("\tmaze a.infile -v -p -s d (same as above)\n");
							printf("Report bugs to <ameer.abdelhadi@gmail.com>\n");
							return(1);

				/* verbose */
				case 'v':	verbose=1;			/* set verbose */
							break;

				/* PostScript mode */
				case 'p':	psEnable=1;			/* enable ps creation */
							break;

				/* stepping mode */
				case 's':	argi++;												/* next argument */
							if (argi>=argc) sm = 'X';							/* if index is out of range, exit */
								else sm = toupper(argv[argi][0]);
							if ((sm!='W')&&(sm!='D')&&(sm!='N')&&(sm!='R')) {	/* consider first letter */
								printf("-E- stepping modes are: wave, destination, net, routeall! Exiting...\n");
								exit(-1);
							} else steppingMode=sm;
							break;

				/* traceback mode */
				case 't':	argi++;												/* next argument */
							if (argi>=argc) tbm = 'X';							/* if index is out of range, exit */
								else tbm = toupper(argv[argi][0]);
							if ((tbm!='M')&&(tbm!='D')) {						/* consider first letter */
								printf("-E- traceback modes are: minturn, direct! Exiting...\n");
								exit(-1);
							} else tracebackMode=tbm;
							break;

				/* unknown argument */
				default :	printf("-E- unknown argument %s\n",argv[argi]);
							exit(-1);
			} /* switch */
		} else fileNameArgInd = argi; /* file name argument index */
	}

	/* check if infile is supplied */
	if (fileNameArgInd<0) {printf(" -E- infile should be supplied\n"); exit(-1);}

	/* parse input file into floorplan database */
	gfp = fpInfileParse(argv[fileNameArgInd]);

	/* initialize display with WHITE 1000x1000 background */
	init_graphics((char*)"Lee-Moore Shortest Path Maze Router", WHITE, NULL);
	init_world (0.,0.,1000.,1000.);

	/* Create new buttons */
	create_button ((char*)"Window"    , (char*)"---1"      , NULL        ); /* Separator              */
	create_button ((char*)"---1"      , (char*)"Wave Mode" , waveModeFunc); /* Show wave expansion    */
	create_button ((char*)"Wave Mode" , (char*)"Dest. Mode", destModeFunc); /* route each destination */
	create_button ((char*)"Dest. Mode", (char*)"Wire Mode" , wireModeFunc); /* route each wire        */
	create_button ((char*)"Wire Mode" , (char*)"Route All" , routeAllFunc); /* route all at once      */
 
	/* update global message and wait for 'Proceed' to proceed */
	strcpy(glabel,"Initial floorplan. Choose stepping mode then press 'Proceed' to make one step.");
	waitLoop();
	if (psEnable) postscript(drawScreen);

	/* route all pins in floorplan */
	fpRouteAll(gfp);

	/* finished! wait still until 'Exit" is pressed */
	while(1) waitLoop();

	/* free database */
	fpDelete(gfp);

}
