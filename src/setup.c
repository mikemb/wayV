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
 *	Code that handles the setup and configuration
 *
 * Query: (SPH) Instead of reading vectors from the ini file, they could be 
 *  created from the shapes. But it's more flexible to specify them.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "defines.h"
#include "setup.h"
#include "gesture.h"
#include "display.h"
#include "../config.h"


/*
 * Purpose:
 * 	Single function for taking care of reading the setup file, 
 * 	including parsing it and setting up various data structures
 *
 * Parameters:
 * 	Char * with the filename and path
 * 		
 * Returns:
 *	Pointer to filled in WSETUP struct
 *	NULL on failure to open the setup file
 *				
 * History:
 * 	16/06/2000 (Mike Bennett): Got rid of the brain malloc
 *	14/09/2001 (Mike Bennett): Updated for keymap
 *	21/09/2001 (Mike Bennett): Set version to NULL so its able to crudely
 *				   detect whether read in a wayv config file
*/
WSETUP *readSetup(char *fileName) {
	WSETUP *wayv;

	/* Setup up the structs and arrays, i.e. the linked lists */
	wayv = (WSETUP *)malloc(sizeof(WSETUP));
	wayv->version = NULL;
	wayv->gestures = (WGESTURE **)malloc(sizeof(WGESTURE *) * (MAXSUBS + 1));
	wayv->orbits = (WORBIT **)malloc(sizeof(WORBIT *) * (MAXSUBS + 1));
	wayv->actions = (WACTION **)malloc(sizeof(WACTION *) * (MAXSUBS + 1));
	wayv->keymap = (WKEYMAP **)malloc(sizeof(WKEYMAP *) * (MAXSUBS + 1));

	/* Do various things with the configuration file */
	if(parseSetup(wayv, fileName) == NULL)
		return NULL;

	processSetup(wayv);

	return wayv;
}


/*
 * Purpose:
 *	Write out the setup to a file
 *
 * Parameters:
 * 	WSETUP * with filled in data
 * 	Char * with the filename and path
 * 		
 * Returns:
 * 	GOOD or BAD
 *				
 * History:
 *	16/06/2000 (Mike Bennett): No longer writing out brains
 *	02/08/2000 (Simon Hart): Added support for vectors
 *	28/02/2001 (Mike Bennett): Updated for Pretty feedback
*/
int writeSetup(WSETUP *wayv, char *fileName) {
	FILE *setupFile;
	int i;

	/* Create and open the output file */
	if((setupFile = fopen(fileName,"w")) == NULL)
		return BAD;

	/* Write out the default header */
	fprintf(setupFile, SF_HEADER);
	fprintf(setupFile, SF_VERSION, VERSION);
	fprintf(setupFile, SF_THINK, wayv->think);

	/* Write out the launch information */
	if(wayv->launch->pbutton != PBUTTONNONE) {
		fprintf(setupFile, SF_LAUNCH, wayv->launch->pbutton);
	} else
		fprintf(setupFile, SF_LAUNCH, 0);	

	/* Write out the pretty/display sub-section */
	fprintf(setupFile, SF_PRETTY, wayv->pretty->display, 
		wayv->pretty->color, wayv->pretty->size,
		wayv->pretty->feedback, wayv->pretty->wait);

	/* Write out the universe sub-section */
	fprintf(setupFile, SF_UNIVERSE, wayv->universe->griddle,
		wayv->universe->xgrid, wayv->universe->ygrid, 
		wayv->universe->maxgpoints, wayv->universe->maxvectors);

	/* Write out the gestures sub-sections */
	for(i = 0; wayv->gestures[i] != NULL; i++) {
		/* Make sure the Matrix and ascii representation are in sync */
		wayv->gestures[i]->shape = parseMSpace(wayv->gestures[i]->wshape);
		wayv->gestures[i]->vector = parseVSpace(wayv->gestures[i]->wvector);
		
		fprintf(setupFile, SF_GESTURE, wayv->gestures[i]->name,
			wayv->gestures[i]->desc, wayv->gestures[i]->shape,
			wayv->gestures[i]->action, wayv->gestures[i]->vector);
	}

	/* Write out the orbits sub-sections */
	for(i = 0; wayv->orbits[i] != NULL; i++)
		fprintf(setupFile, SF_ORBIT, wayv->orbits[i]->name,
			wayv->orbits[i]->path);

	/* Write out the actions sub-setions */
	for(i = 0; wayv->actions[i] != NULL; i++)
		fprintf(setupFile, SF_ACTION, wayv->actions[i]->name,
			wayv->actions[i]->process, wayv->actions[i]->match);

	fclose(setupFile);

	return GOOD;
}

/*
 * Purpose:
 * 	Takes in a setup structure and convert various fields to specific
 * 	data structures, i.e. list of x and y points to GPOINTs list, text
 * 	matrix to matrix data structure
 *
 * Parameters:
 * 	Partially completed setup structure
 * 		
 * Returns:
 *	Pointer to filled in WSETUP struct
 *				
 * History:
 * 	20/05/2000 (Mike Bennett): Extended to parse process lists
 * 	16/05/2000 (Mike Bennett): Got rid of brain code
 *	03/08/2000 (Simon P Hart): Extended to parse vectors
*/
WSETUP *processSetup(WSETUP *wayv) {
	int i;

	/* Convert ascii gesture matrices to data matrices */
	for(i = 0; wayv->gestures[i] != NULL; i++) {
		wayv->gestures[i]->wshape = parseGSpace(wayv->gestures[i]->shape,
						wayv->universe->xgrid, wayv->universe->ygrid);
		wayv->gestures[i]->wvector = parseVector(wayv->gestures[i]->vector,
						wayv->universe->maxvectors);
	}

	/* Convert orbits to GPoints list */
	for(i = 0; wayv->orbits[i] != NULL; i++)
		wayv->orbits[i]->wpath = parseOrbitPath(wayv->orbits[i]->path);

	/* Convert action processes into component sub-actions */
	for(i = 0; wayv->actions[i] != NULL; i++)
		wayv->actions[i]->subs = parseProcess(wayv->actions[i]->process);

	return wayv;
}


/*
 * Purpose:
 * 	Read in the configuration file and parse.
 *
 * Parameters:
 * 	WSETUP structure to fill in with configuration values
 * 	Filename to parse
 * 		
 * Returns:
 *	Pointer to filled in WSETUP struct
 *				
 * History:
 *	04/03/2001 (Mike Bennett): Ammended Pretty.display for input box
 *	12/03/2001 (Mike Bennett): Support for turning on whether the input
 *				   window is managed by the window manager
 *	19/04/2001 (Mike Bennett): Report an error if cannot find the 
 *				   configuration file and updated to look in
 *				   the right place for the keymap
 *	08/04/2003 (Nick Rusnov): Allow any mouse button to be used to activate
 *
 * Notes:
 *	When the parser is rewritten make sure and setup everything with
 *	default values
*/
WSETUP *parseSetup(WSETUP *wayv, char *fileName) {
	char *setup, *temp, *nullstr, *type, *file;
	int i, n, a, pGesture = 0, pAction = 0, pOrbit = 0;
	FILE *setupFile;

	if((setupFile = fopen(fileName,"r")) == NULL) {
		printf("Error: Cannot open the configuration file '%s'\n", fileName);
		return NULL;
	}

	nullstr=(char *)malloc(1);
	*nullstr='\0'; /* never explictly freed */
	temp = (char *)malloc(LINEMAX);
	setup = (char *)malloc(SETUPSIZE);
	*setup = '\0';

	/* Read the config file into memory and strip out
	 * the excess stuff */
	while(fgets(temp, LINEMAX-1, setupFile) != NULL) {
		if((strlen(temp) + strlen(setup)) < SETUPSIZE)
			strncat(setup, temp, strlen(temp));
	}

	fclose(setupFile);
	free(temp);	
	temp = stripPadding(setup);
	free(setup);
	setup = temp;

	/* Now do the parsing */
	for(i = 0; i < strlen(setup); i++) {
		switch(setup[i]) {
			case 'G':
				temp = getKeyText(setup, &i, SKT_NONE);
				wayv->gestures[pGesture] = (WGESTURE *)malloc(sizeof(WGESTURE));
				wayv->gestures[pGesture]->name = nullstr;
				wayv->gestures[pGesture]->desc = nullstr;
				wayv->gestures[pGesture]->shape = nullstr;
				wayv->gestures[pGesture]->vector = nullstr;
				wayv->gestures[pGesture]->action = nullstr;

				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 'n':
							wayv->gestures[pGesture]->name = getKeyText(temp, &n, SKT_QUOTES);
						break;

						case 'd':
							wayv->gestures[pGesture]->desc = getKeyText(temp, &n, SKT_QUOTES);
						break;
						
						case 's':
							wayv->gestures[pGesture]->shape = getKeyText(temp, &n, SKT_NONE);
						break;
						
						case 'a':
							wayv->gestures[pGesture]->action = getKeyText(temp, &n, SKT_QUOTES);
						break;

					        case 'v':
							wayv->gestures[pGesture]->vector = getKeyText(temp, &n, SKT_NONE);
						break;
					}
				}
				
				pGesture++;
				free(temp);
			break;

			case 'O':
				temp = getKeyText(setup, &i, SKT_NONE);
				wayv->orbits[pOrbit] = (WORBIT *)malloc(sizeof(WORBIT));

				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 'n':
							wayv->orbits[pOrbit]->name = getKeyText(temp, &n, SKT_QUOTES);
						break;

						case 'p':
							wayv->orbits[pOrbit]->path = getKeyText(temp, &n, SKT_NONE);
						break;
					}
				}

				pOrbit++;
				free(temp);
			break;

			case 'A':
				temp = getKeyText(setup, &i, SKT_NONE);
				wayv->actions[pAction] = (WACTION *)malloc(sizeof(WACTION));
				wayv->actions[pAction]->name = nullstr;
				wayv->actions[pAction]->process = nullstr;
				wayv->actions[pAction]->match = nullstr;
				
				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 'n':
							wayv->actions[pAction]->name = getKeyText(temp, &n, SKT_QUOTES);
						break;

						case 'p':
							wayv->actions[pAction]->process = getKeyText(temp, &n, SKT_NONE);
						break;

						case 'm':
							wayv->actions[pAction]->match = getKeyText(temp, &n, SKT_QUOTES);
						break;
					}
				}

				pAction++;
				free(temp);
			break;

			case 'V':
				wayv->version = getKeyText(setup, &i, SKT_NONE);
			break;

			case 'L':
				wayv->launch = (WLAUNCH *)malloc(sizeof(WLAUNCH));
				wayv->launch->method = nullstr;
				wayv->launch->pbutton = PBUTTONNONE;
				wayv->launch->kmodifier = KMODNONE;

				wayv->launch->method = getKeyText(setup, &i, SKT_QUOTES);

				for(n = 0; (temp = getCommaText(wayv->launch->method, &n)) != NULL;) {
					if((strlen(temp) >= 2) && (temp[0] == 'M') && (isdigit(temp[1])))
				       		wayv->launch->pbutton = atoi(&temp[1]);
					else if(strcmp(temp, "SHIFT") == 0)
						wayv->launch->kmodifier |= KMODSHIFT;
					else if(strcmp(temp, "LOCK") == 0)
						wayv->launch->kmodifier |= KMODLOCK;
					else if(strcmp(temp, "CONTROL") == 0)
						wayv->launch->kmodifier |= KMODCONTROL;
					else if(strcmp(temp, "MOD1") == 0)
						wayv->launch->kmodifier |= KMOD1;
					else if(strcmp(temp, "MOD2") == 0)
						wayv->launch->kmodifier |= KMOD2;
					else if(strcmp(temp, "MOD3") == 0)
						wayv->launch->kmodifier |= KMOD3;
					else if(strcmp(temp, "MOD4") == 0)
						wayv->launch->kmodifier |= KMOD4;
					else if(strcmp(temp, "MOD5") == 0)
						wayv->launch->kmodifier |= KMOD5;

					free(temp);
				}
			break;

			case 'T':
				wayv->think = getKeyText(setup, &i, SKT_QUOTES);
			break;

			case 'P':
				temp = getKeyText(setup, &i, SKT_NONE);
				wayv->pretty = (WPRETTY *)malloc(sizeof(WPRETTY));
				wayv->pretty->display = nullstr;
				wayv->pretty->managed = nullstr;
				wayv->pretty->color = nullstr;
				wayv->pretty->size = DEF_PRETTY_SIZE;
				wayv->pretty->feedback = nullstr;
				wayv->pretty->wait = DEF_PRETTY_WAIT;

				for(n = 0; n < 4; n++)
					wayv->pretty->window[n] = UNSETINT;

				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 'd':
							wayv->pretty->display = getKeyText(temp, &n, SKT_NONE);
						break;

						case 'm':
							wayv->pretty->managed = getKeyText(temp, &n, SKT_NONE);
						break;
						
						case 'c':
							wayv->pretty->color = getKeyText(temp, &n, SKT_NONE);
						break;
						
						case 's':
							wayv->pretty->size = getKeyNum(temp, &n);
						break;

						case 'f':
							wayv->pretty->feedback = getKeyText(temp, &n, SKT_NONE);
						break;

						case 'w':
							wayv->pretty->wait = getKeyNum(temp, &n);
						break;
					}
				}

				/* Are we using an input box? */
				if(wayv->pretty->display != nullstr && wayv->pretty->display[0] != 'n'
					&& wayv->pretty->display[0] != 'y') {

					for(n = 0, a = 0; a < 4; a++)
						wayv->pretty->window[a] = atoi(getCommaText(wayv->pretty->display, &n));

					wayv->pretty->display = strdup("input");
				}

				free(temp);
			break;
			
			case 'U':
				temp = getKeyText(setup, &i, SKT_NONE);
				wayv->universe = (WUNIVERSE *)malloc(sizeof(WUNIVERSE));
				wayv->universe->griddle = nullstr;
				wayv->universe->xgrid = DEF_UNI_XGRID;
				wayv->universe->ygrid = DEF_UNI_YGRID;
				wayv->universe->maxgpoints = DEF_UNI_MAXGPOINTS;
				wayv->universe->maxvectors = DEF_UNI_MAXVECTORS;

				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 'g':
							wayv->universe->griddle = getKeyText(temp, &n, SKT_QUOTES);
						break;

						case 'x':
							wayv->universe->xgrid = getKeyNum(temp, &n);
						break;

						case 'y':
							wayv->universe->ygrid = getKeyNum(temp, &n);
						break;

						case 'm':
							wayv->universe->maxgpoints = getKeyNum(temp, &n);
						break;

						case 'v':
							wayv->universe->maxvectors = getKeyNum(temp, &n);
						break;
					}
				}
				
				free(temp);
			break;

			case 'I':
				temp = getKeyText(setup, &i, SKT_NONE);
				type = file = NULL;

				for(n = 0; n < strlen(temp); n++) {
					switch(temp[n]) {
						case 't':
							type = getKeyText(temp, &n, SKT_QUOTES);
						break;

						case 'f':
							file = getKeyText(temp, &n, SKT_QUOTES);
						break;
					}
				}

				free(temp);
		
				if(type != NULL && file != NULL) {
					/* Figure out the path to the keymap. NOTE: This is nasty code - it'll
					   be gone in the next version */
					if(!strrchr(file, '/') && (temp = strrchr(fileName, '/'))) {
						*temp = '\0';
						temp = malloc(sizeof(char) * (strlen(fileName) + strlen(file) + 1));
						strcpy(temp, fileName);
						strcat(temp, "/");
						strcat(temp, file);

						free(file);
						file = temp;
					}
						
					parseInclude(wayv, type, file);
					free(type);
					free(file);
				}
			break;
		}
	}

	/* Terminate the array with NULL */
	wayv->gestures[pGesture] = NULL;
	wayv->orbits[pOrbit] = NULL;
	wayv->actions[pAction] = NULL;

	free(setup);

	if(wayv->version == NULL || strlen(wayv->version) > 10) {
		printf("Error: '%s' is not a recognisable wayV configuration file\n", fileName);
		return NULL;
	}

	return wayv;
}


/*
 * Purpose:
 * 	Strip out the excess characters in a line, includes \t, \r
 * 	and space (don't strip space when between "'s)
 *
 * Parameters:
 * 	Line to strip
 * 		
 * Returns:
 *	Pointer to stripped line
 *				
 * History:
 * 	08/04/2000 (Mike Bennett): Created function
*/
char *stripPadding(char *line) {
	int i, n, length, ignoreSpace = 1;
	char *clean;

	clean = (char *)malloc(strlen(line));

	for(i = 0, n = 0, length = strlen(line); i < length; i++) {
		switch(line[i]) {
			case '\t': case '\r': case ' ': case '\n':
				if(!ignoreSpace)
					clean[n++] = line[i];
			break;

			case '#':
				if(!ignoreSpace)
					clean[n++] = line[i];			
				else
					for(; i < length && line[i] != '\n'; i++);
			break;

			case '\\':
				switch(line[i+1]) {
					case '"': case '\\':
						clean[n++] = line[i++];
						clean[n++] = line[i];
					break;
				}
			break;

			case '"':
				if(!ignoreSpace)
					ignoreSpace = 1;
				else
					ignoreSpace = 0;

			default:
				clean[n++] = line[i];
			break;
		}
	}

	clean[n] = '\0';

	return clean;
}



/*
 * Purpose:
 * 	Return the key value as text - btw the line should already
 * 	have been fed through stripPadding()
 *
 * Parameters:
 * 	1st - String to get key in
 * 	2nd - Position to start finding key
 * 	3rd - Are first and last quotes(") to be stripped out and
 * 		  only get the text between them?
 *
 * Returns:
 *	Pointer to the key
 *				
 * History:
 * 	08/04/2000 (Mike Bennett): Created function
 * 	23/04/2000 (Mike Bennett): Renamed
 * 	05/06/2000 (Mike Bennett): Added optional stripping of quotes
*/
char *getKeyText(char *setup, int *position, int strip) {
	int n, length, i = *position;
	char *value;

	/* Find the start of the key data */
	for(length = strlen(setup); i < length && setup[i] != '=' 
		&& setup[i] != '{'; i++);

	/* Are we dealing with a subsection? */
	if(setup[i] == '{')
		for(n = ++i; n < length && setup[n] != '}'; n++);
	else
		for(n = ++i; n < length && setup[n] != ';'; n++);	

	*position = n;

	/* Want to get rid of the first and last quote and so only
	 * get the text between them */
	if(strip == SKT_QUOTES) {
		if(setup[i] == '"')
			i++;
		else
			for(; i < length && setup[i] != '"'; i++);

		if(setup[n] == '"')
			n--;
		else		
			for(; n > 0 && setup[n] != '"'; n--);
	}

	value = (char *)malloc(n - i + 1);
	strncpy(value, setup + i, n - i);
	*(value + n - i) = '\0';

	return value;
}


/*
 * Purpose:
 * 	Return the key value as a number. Btw the line should
 * 	already have been fed through stripPadding()
 *
 * Parameters:
 * 	String to get key in
 * 	Position to begin the search
 * 		
 * Returns:
 *	Int with key value or default value if not present
 *				
 * History:
 * 	23/04/2000 (Mike Bennett): Created function
*/
int getKeyNum(char *setup, int *position) {
	int value;
	char *text;

	text = getKeyText(setup, position, SKT_NONE);
	value = atoi(text);
	free(text);

	return value;
}


/*
 * Purpose:
 *	Get an item from a list where ',' is the value
 *	used to separate tokens in a string 
 *
 * Parameters:
 *	String to get value in
 *	Position to begin the search
 *
 * Returns:
 *	Pointer to the string
 *
 * History:
 *	13/04/2000 (Mike Bennett): Created function
 *	03/08/2000 (Simon Hart): Memory check
*/
char *getCommaText(char *setup, int *position) {
	int length, n = *position, i = *position;
	char *value;

	/* Find the start of the key data */
	for(length = strlen(setup); n < length && setup[n] != ','
		&& setup[n] != ';'; n++);

	if((n - i) <= 0)
		return NULL;

	if((value = (char *)malloc(sizeof(char) * (n - i + 1))) == NULL) {
		fputs("No mem",stderr);
		exit(1);
	}

	strncpy(value, setup + i, n - i);
	*(value + n - i) = '\0';

	*position = n + 1;
	return value;
}


/*
 * Purpose:
 *	Get an item from a list where ' ' (space) is the value
 *	used to separate tokens in a string 
 *
 * Parameters:
 *	String to get value in
 *	Position to begin the search
 *
 * Returns:
 *	Pointer to the string
 *
 * History:
 *	31/07/2000 (Mike Bennett): Created function
 *	16/04/2001 (Mike Bennett): Renamed to getDelimText from
 *				   getSpaceText and made a little
 *				   more generic
*/
char *getDelimText(char *setup, int *position, char delimiter) {
	int length, n = *position, i = *position;
	char *value;

	/* Find the start of the key data */
	for(length = strlen(setup); n < length && setup[n] != delimiter; n++);

	if((n - i) <= 0)
		return NULL;

	value = (char *)malloc(sizeof(char) * (n - i + 1));
	strncpy(value, setup + i, n - i);
	*(value + n - i) = '\0';

	*position = n + 1;
	return value;
}


/*
 * Purpose:
 * 	Get the text between 2 quotes, the quotes used are the first and
 * 	last ones found
 *
 * Parameters:
 * 	1st - String to quoted text from
 *
 * Returns:
 *	Pointer to the text
 *	NULL on failure
 *				
 * History:
 * 	20/05/2000 (Mike Bennett): Created function
*/
char *getQuoteText(char *text) {
	int i, n;
	char *value;

	for(i = 0; i < strlen(text) && text[i] != '"'; i++);
	for(n = strlen(text); n > 0 && text[i] != '"'; n--);

	/* Dump the quote symbols */
	i++;
	n--;

	if((n - i) <= 0)
		return NULL;

	value = (char *)malloc(n - i + 1);
	strncpy(value, text + i, n - i);
	*(value + n - i) = '\0';

	return value;
}


/*
 * Purpose:
 *	Turn a gesture x,y path in the setup file into a
 *	list of GPOINTs
 *
 * Parameters:
 *	List of x and y points in ascii separated by commas
 *
 * Returns:
 *	Pointer to the key
 *
 * History:
 *	13/04/2000 (Mike Bennett): Created function
 *	23/04/2000 (Mike Bennett): Renamed
*/
GPOINT *parseOrbitPath(char *values) {
	GPOINT *gPoints;
	int i, n;
	char *xValue, *yValue;

	/* Work out number of points */
	for(i = 0, n = 0; i < strlen(values); i++)
		if(values[i] == ',')
			n++;

	n = (n >> 1) + 1;

	gPoints = setupGPoints(n);

	/* Walk through the comma delimiter list */
	for(i = 0, n = 0; (xValue = getCommaText(values, &i)) != NULL
	    && (yValue = getCommaText(values, &i)) != NULL; n++) {
		gPoints[n].x = atoi(xValue);
		gPoints[n].y = atoi(yValue);

		free(xValue);
		free(yValue);
	}
	
	gPoints[n].x = GEND;
	gPoints[n].y = GEND;

    return gPoints;
}

/*
 * Purpose:
 *	Turn a vector string from the setup file into an array
 *
 * Parameters:
 *	1st - String of comma seperated values
 *	2nd - Maximum number of vectors to read
 *
 * Returns:
 *	array of VECTORs
 *
 * History:
 *	03/08/2000 (Simon P Hart): Created Function
 *	15/03/2001 (Mike Bennett): Bug fix, wasn't counting the number
 *				   of input vectors correctly causing a
 *				   memory over run
*/
VECTOR *parseVector(char *values,int maxvectors) {
	VECTOR *v;
	int i, n;
	char *val;

	/* Work out number of vectors */
	for(i = 0, n = 1; i < strlen(values); i++)
		if(values[i] == ',')
			n++;

	n = (n >> 1) + 1;

	if((v = (VECTOR *)malloc(sizeof(VECTOR) * (n + 1))) == NULL) {
		fputs("Not enough mem for vectors from setup file\n", stderr);
		exit(1);
	}

	/* Walk through the comma delimiter list. CODE: Not too efficiently. */
	for(i = 0, n = 0; (val = getCommaText(values, &i)) != NULL; n++) {
		if(n == maxvectors) {
			printf("Error: Increase Universe.vectors to match the maximum number of vectors in Gesture.vector\n");
			exit(1);
		}

		v[n] = DIR_NONE;

		if(0 == strcmp(val, "N")) v[n] = DIR_N;
		if(0 == strcmp(val, "NE")) v[n] = DIR_NE;
		if(0 == strcmp(val, "E")) v[n] = DIR_E;
		if(0 == strcmp(val, "SE")) v[n] = DIR_SE;
		if(0 == strcmp(val, "S")) v[n] = DIR_S;
		if(0 == strcmp(val, "SW")) v[n] = DIR_SW;
		if(0 == strcmp(val, "W")) v[n] = DIR_W;
		if(0 == strcmp(val, "NW")) v[n] = DIR_NW;

		if(v[n] == DIR_NONE) {
			printf("Error: invalid direction in vector %s\n",values);
			exit(1);
		}

		free(val);
	}

	v[n] = DIR_NONE;

	return v;
}


/*
 * Purpose:
 * 	Turn an ascii list for a matrix into a matrix space
 *
 * Parameters:
 *	Text string with comma separated list of values
 *	Number of units matrix goes across
 *	Number of units matrix goes down
 *
 * Returns:
 *	Pointer filled in matrix
 *
 * History:
 *	23/04/2000 (Mike Bennett): Created function
*/
MATRIX *parseGSpace(char *values, int x, int y) {
	MATRIX *matrix;
	char *num;
	int i, n, m;

	/* Create mem for the matrix */
	matrix = createMat(x, y);

	/* Fill in the matrix */
	for(i = 0, n = 0; n < matrix->n; n++) {
		for(m = 0; m < matrix->m; m++) {
			/* Get the next value */
			if((num = getCommaText(values, &i)) != NULL) {
				matrix->me[m][n] = atoi(num);
				free(num);
			} else {
				matrix->me[m][n] = 0;
			}			
		}
	}

    return matrix;
}


/*
 * Purpose:
 * 	Turn a matrix into an ascii comma delimited representation
 *
 * Parameters:
 * 	Original Matrix
 *
 * Returns:
 *	Pointer to comma delimited ascii representation
 *
 * History:
 *	06/05/2000 (Mike Bennett): Created function
 *	08/05/2000 (Mike Bennett): Fixed silly memory error
 *	15/06/2000 (Mike Bennett): Slight change to be cleaner output
*/
char *parseMSpace(MATRIX *matrix) {
	char *ascii_rep;
	int i, n, m, no_chars = 100;

	ascii_rep = (char *)malloc(no_chars);

	/* Walk through the matrix converting */
	for(i = 0, n = 0; n < matrix->n; n++) {
		for(m = 0; m < matrix->m; m++) {
			/* Make sure we don't run out of memory */
			if(strlen(ascii_rep) >= (no_chars - 50)) {
				no_chars <<= 1;
				ascii_rep = (char *)realloc(ascii_rep, no_chars);
			}

			if(m == 0 && n == 0)
				sprintf(ascii_rep, "\n\t  %d", (int)matrix->me[m][n]);
			else
				sprintf(ascii_rep, "%s, %d", ascii_rep, (int)matrix->me[m][n]);
		}

		strcat(ascii_rep, "\n\t");
	}

    return ascii_rep;
}


/*
 * Purpose:
 * 	Turn a vector into an ascii comma delimited representation
 *
 * Parameters:
 * 	Array of VECTORs
 *
 * Returns:
 *	Pointer to comma delimited ascii representation
 *
 * History:
 *	03/08/2000 (Simon P Hart): Created function (Copy and Paste :)       
*/
char *parseVSpace(VECTOR *v) {
	char *ascii_rep;
	int i, n, no_chars = 100;

	if((ascii_rep = (char *)malloc(no_chars * sizeof(char))) == NULL) {
		fputs("no mem\n", stderr);
		exit(1);
	}

	/* Walk through the vector converting */
	for(i = 0, n = 0; v[n] != DIR_NONE; n++) {
		/* Make sure we don't run out of memory */
		if(strlen(ascii_rep) >= (no_chars - 50)) {
			no_chars <<= 1;
			if((ascii_rep = (char *)realloc(ascii_rep, no_chars * sizeof(char))) == NULL) {
				fputs("no mem\n", stderr);
				exit(1);
			}
		}
		
		if(n == 0)
			sprintf(ascii_rep, "\n\t  %d", (int)v[0]);
		else
			sprintf(ascii_rep, "%s, %d", ascii_rep, (int)v[n]);
	}

	strcat(ascii_rep, "\n");
	
	return ascii_rep;
}


/*
 * Purpose:
 * 	Turn a process list into a list of sub-actions
 *
 * Parameters:
 *	Text string with comma separated list of sub-actions
 *
 * Returns:
 *	Pointer to filled in WACTION_PROCESS list
 *
 * History:
 *	21/08/2000 (Mike Bennett): Moved command line parsing elsewhere
 *	01/03/2001 (Mike Bennett): Added the kill command
 *	03/03/2001 (Mike Bennett): Updated to handle sending keypress'
 *
 * NOTE:
 *	Might be a bug in the code below, counting of comma's is
 *	probably out by one (MB)
*/
WA_PROCESS **parseProcess(char *process) {
	WA_PROCESS **subs;
	int i, n;
	char *saction;

	/* Work out the number of sub-actions in this process */
	for(i = 0, n = 0; getCommaText(process, &i) != NULL; n++);

	subs = (WA_PROCESS **)malloc(sizeof(WA_PROCESS *) * (n + 1));

	/* Walk through the process list breaking down each sub-action */
	for(i = 0, n = 0; (saction = getCommaText(process, &i)) != NULL; n++) {
		switch(saction[0]) {
			/* Was it an execute sub-action? */
			case 'e':
				subs[n] = (WA_PROCESS *)malloc(sizeof(WA_PROCESS));
				subs[n]->perform = WA_EXEC;
				subs[n]->tparam = getQuoteText(saction);
			break;

			/* Was it a warp pointer back to start position sub-action? */
			case 'w':
				subs[n] = (WA_PROCESS *)malloc(sizeof(WA_PROCESS));
				subs[n]->perform = WA_WARP;
			break;

			/* Was it a delay for X milliseconds sub-action? */
			case 'd':
				subs[n] = (WA_PROCESS *)malloc(sizeof(WA_PROCESS));
				subs[n]->perform = WA_DELAY;
				subs[n]->nparam = atoi(getQuoteText(saction));
			break;

			/* Was it kill the last started process */
			case 'k':
				subs[n] = (WA_PROCESS *)malloc(sizeof(WA_PROCESS));
				subs[n]->perform = WA_KILL;
			break;

			/* Was it send a key press */
			case 's':
				subs[n] = (WA_PROCESS *)malloc(sizeof(WA_PROCESS));
				subs[n]->perform = WA_SENDKEY;
				subs[n]->tparam = getQuoteText(saction);
			break;
		}
	}

	subs[n] = NULL;

	return subs;
}


/*
 * Purpose:
 *	Read in an include file and parse it
 *
 * Parameters:
 *	1st - general wayV config structure
 *	2nd - type of include file, i.e. Keymap
 *	3rd - file name that has include information
 *
 * Returns:
 *	GOOD on success
 *	BAD on failure
 *
 * History:
 *	14/04/2001 (Mike Bennett): Created
 *	19/04/2001 (Mike Bennett): Ammended for clearer error reporting
*/
int parseInclude(WSETUP *wayv, char *type, char *fileName) {
	char *temp, *token;
	FILE *setupFile;
	int i;

	if(strcmp(type, WF_KEYMAP) == 0) {
		temp = malloc(LINEMAX);

		if((setupFile = fopen(fileName, "r")) == NULL) {
			printf("Error: Cannot open the keymap file '%s'\n", fileName);
			return BAD;
		}

		for(i = 0; fgets(temp, LINEMAX - 1, setupFile) != NULL; i++) {
			wayv->keymap[i] = (WKEYMAP *)malloc(sizeof(WKEYMAP));
			wayv->keymap[i]->kcode = 0;
			wayv->keymap[i]->normal = NULL;
			wayv->keymap[i]->shift = NULL;

			if((token = strtok(temp, " ")) != NULL)
				wayv->keymap[i]->kcode = (KeyCode)atoi(token);

			if((token = strtok(NULL, " ")) != NULL) {
				if(token[strlen(token) - 1] == '\n')
					token[strlen(token) - 1] = '\0';

				wayv->keymap[i]->normal = strdup(token);
			}

			if((token = strtok(NULL, " ")) != NULL) {
				if(token[strlen(token) - 1] == '\n')
					token[strlen(token) - 1] = '\0';

				wayv->keymap[i]->shift = strdup(token);
			}
		}

		wayv->keymap[i] = NULL;
		fclose(setupFile);
		free(temp);

		return GOOD;
	}

	return BAD;
}
