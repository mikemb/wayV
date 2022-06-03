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
 *	Perform actions and manage signals
*/

#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include "defines.h"
#include "aimath.h"
#include "setup.h"
#include "display.h"
#include "gesture.h"
#include "process.h"

int child;

/*
 * Purpose:
 *	Carry out a list of actions in a gesture struct
 *
 * Parameters:
 * 	1st - General wayV configuration structure, wanted for action list
 *	2nd - Gesture structure
 *	3rd - Display to close in child
 *	4th - X & Y points travelled by the mouse, used by the WARP sub-action
 *
 * Returns:
 * 	Child process ID or 0 if none or an error
 *
 * History:
 *	14/04/2001 (Mike Bennett): Got keypress' properly working
 *	16/04/2001 (Mike Bennett): Composite keypress', etc working properly
 *	17/04/2001 (Mike Bennett): Removed signal handling setup
*/
void performAction(WSETUP *wayv, WGESTURE *gesture, GDISPLAY *video, GPOINT *gpoints) {
	int i, n, a, b;
	char **temp, *single, *comp;

	/* Action names have to be at least one character in length */
	if(!strlen(gesture->action))
		return;

	/* Try and find an action to perform */
	for(i = 0; wayv->actions[i] != NULL
		&& strcmp(wayv->actions[i]->name, gesture->action) != 0; i++);

	if(wayv->actions[i] == NULL)
		return;

	/* Carry out sub-actions */
	for(n = 0; wayv->actions[i]->subs[n] != NULL; n++) {
		switch(wayv->actions[i]->subs[n]->perform) {
			case WA_EXEC:
				temp = createCmdLine(wayv->actions[i]->subs[n]->tparam, video, 
					 gpoints);

				if((child = fork()) == 0) {
					/* Child process to tidy up */
					close(ConnectionNumber(video->display));
					setsid();
					execvp(temp[0], temp);
					exit(BAD);	/* Shouldn't get to this point */
				} else
					printf("Child ID: %d\n", child);

				/* Free up the parsed commandline */
				for(a = 0; temp[a] != NULL; a++)
					free(temp[a]);

				free(temp);
			break;

			case WA_WARP:
				XWarpPointer(video->display, None, video->window, 0, 0, 0, 0,
					gpoints[0].x, gpoints[0].y);
			break;

			case WA_DELAY:
				sleep(wayv->actions[i]->subs[n]->nparam);
			break;

			case WA_KILL:
				if(child != 0)
					kill(child, SIGKILL);
			break;

			case WA_SENDKEY:
				/* This shouldn't be doing parsing here - but I'll leave it for now... */
				for(a = 0; (single = getDelimText(wayv->actions[i]->subs[n]->tparam, &a, ' ')) != NULL;) {
					if(strchr(single, '+')) {
						for(b = 0; (comp = getDelimText(single, &b, '+')) != NULL;) {
							sendKey(wayv, video, comp, PRESS);
							free(comp);
						}

						for(b = 0; (comp = getDelimText(single, &b, '+')) != NULL;) {
							sendKey(wayv, video, comp, RELEASE);
							free(comp);
						}
					} else {
						sendKey(wayv, video, single, PRESS);
						sendKey(wayv, video, single, RELEASE);
					}

					free(single);
				}

			break;
		}
	}

	return;
}


/*
 * Purpose:
 *	Setup the commandline as required by execvp, and also replace 
 *	certain values, i.e. WAYV_MOUSE_START
 *
 * Parameters:
 * 	1st - text version of the command line
 *
 * Returns:
 *	Command line broken into parts using space as the separator
 *
 * History:
 *	21/08/2000 (Mike Bennett): Created function
 *	01/03/2001 (Mike Bennett): Updated to support WAYV_MOUSE_END
 *	15/03/2001 (Mike Bennett): Memory over run bug fix
*/
char **createCmdLine(char *cmdline, GDISPLAY *video, GPOINT *gpoints) {
	int a, b, i;
	char *temp, **cmdunits;

	/* Work out the number of arguments and array length */
	for(a = 1, b = 0; (temp = getDelimText(cmdline, &b, ' ')) != NULL; a++)
		free(temp);

	/* Create memory for the argument list */
	cmdunits = (char **)malloc(sizeof(char *) * (a + 1));

	/* Parse the command line and replace keys */
	for(a = 0, b = 0; (cmdunits[a] = getDelimText(cmdline, &b, ' ')) != NULL; a++) {
		if(!strcmp(cmdunits[a], "WAYV_MOUSE_START")) {
			free(cmdunits[a]);
			cmdunits[a] = (char *)malloc(20);
			snprintf(cmdunits[a], 20, "+%d+%d", (int)gpoints[0].x, (int)gpoints[0].y);
		} else if(!strcmp(cmdunits[a], "WAYV_MOUSE_END")) {
			free(cmdunits[a]);
			cmdunits[a] = (char *)malloc(20);

			for(i = 0; gpoints[i].x != GEND; i++);

			i--;

			snprintf(cmdunits[a], 20, "+%d+%d", (int)gpoints[i].x, (int)gpoints[i].y);
		}
	}

	cmdunits[a + 1] = NULL;

	return cmdunits;
}


/*
 * Purpose:
 *	Setup signal handlers for wayV - only manage SIGCHLD at this
 *	stage.
 *
 * Parameters:
 * 	NONE
 *
 * Returns:
 * 	NONE
 *
 * History:
 *	17/04/2001 (Mike Bennett): Created function
*/
void setupSignals() {
	signal(SIGCHLD, destroyZombies);
}


/*
 * Purpose:
 *	Signal handler when a child process dies, basically don't leave
 *	it around as a zombie process. *hehe* funky function name - its a bit
 *	sad that there's more lines of comments than code :)
 *
 * Parameters:
 * 	1st - Signal id
 *
 * Returns:
 * 	NONE
 *
 * History:
 *	13/06/2000 (Mike Bennett): Created function
*/
void destroyZombies(int sigNum) {
	wait(NULL);
}
