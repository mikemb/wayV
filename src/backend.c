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
 *	Backend for wayV
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "gesture.h"
#include "display.h"
#include "setup.h"
#include "process.h"
#include "ai.h"
#include "misc.h"
#include "../config.h"

int main(int argc, char **argv) {
	GDISPLAY *video, *feedback;
	GPOINT *gPoints;
	XEvent event;
	WSETUP *wayv;
	WGESTURE *gaction;
	MATRIX *gesture;
	VECTOR *vector;
	int gPosition = 0;
	char *file;

	printf("wayV version %s\n", VERSION);

	/* Decide what configuration file to open */
	if(argc != 2)
		file = strdup(SETUPFILE);
	else 
		file = strdup(argv[1]);

	if((wayv = readSetup(file)) == NULL)
		exit(BAD);

//	writeSetup(wayv, SETUPOUT);

	/* Setup the display */
	if(!(video = setupDrawable(wayv->pretty, wayv->pretty->window, argc, argv))) {
		printf("Cannot connected to X server\n");
		return BAD;
	}

	/* Setup the mouse/pointer as an input device */
	setupPointer(wayv, video);

	/* Allocate memory to store the gesture points */
	gPoints = setupGPoints(wayv->universe->maxgpoints);

	if(wayv->pretty->display[0] == 'i')
		handleDrawableWindow(video, MAP);

	setupSignals();

	/* Process the events */
	while(1) {
		XNextEvent(video->display, &event);

		switch(event.type) {
			case MotionNotify:
				if(gPosition < wayv->universe->maxgpoints) {
					gPoints[gPosition].x = event.xmotion.x;
					gPoints[gPosition].y = event.xmotion.y;
					
					if(wayv->pretty->display[0] == 'y')
						writePixels(video, (int)gPoints[gPosition-1].x - GDEF_BORDER, 
							(int)gPoints[gPosition-1].y - GDEF_BORDER, 
							(int)gPoints[gPosition].x - GDEF_BORDER, 
							(int)gPoints[gPosition].y - GDEF_BORDER);
					else if(wayv->pretty->display[0] == 'i')
						writePixels(video, (int)gPoints[gPosition-1].x, 
							(int)gPoints[gPosition-1].y, 
							(int)gPoints[gPosition].x, 
							(int)gPoints[gPosition].y);

					gPosition++;
				}
			break;

			case ButtonPress:
				if(wayv->pretty->display[0] == 'y')
					handleDrawableWindow(video, MAP);

				gPosition = 0;
				gPoints[gPosition].x = event.xbutton.x;
				gPoints[gPosition].y = event.xbutton.y;
				gPosition++;
			break;

			case ButtonRelease:
				if(wayv->pretty->display[0] == 'y')
					handleDrawableWindow(video, UNMAP);

				gPoints[gPosition].x = GEND;
				gPoints[gPosition].y = GEND;

				gesture = gridGPoints(gPoints, wayv->universe->xgrid,
						wayv->universe->ygrid);
				vector = vectGPoints(gPoints,  wayv->universe->xgrid,
						wayv->universe->ygrid, wayv->universe->maxvectors);
						     
				printf("\n\n=======Gesture=======");
				
				printMat(gesture);
				printVec(vector);
				
				if(wayv->pretty->display[0] != 'n')
					handleDrawableWindow(video, CLEAR);

				if((gaction = findGesture(wayv, gesture, vector, wayv->think))) {
					printf("Is Action : %s\n", gaction->name);

					performAction(wayv, gaction, video, gPoints);

					/* Fork off program to handle feedback window */
					if(wayv->pretty->feedback[0] == 'y' && fork() == 0) {
						feedback = writeText(gaction->action);
						waitMilli(wayv->pretty->wait);
						handleDrawableWindow(feedback, DESTROY);
						close(ConnectionNumber(video->display));
						setsid();
						exit(BAD);
					}
				}

				freeMat(gesture);
				free(vector);
			break;

			case ClientMessage:
				if(event.xclient.data.l[0] == video->wmdelete) {
					closeDisplay(video);
					exit(GOOD);
				}
			break;
		}
	}
}
