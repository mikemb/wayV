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
 *	Handle the setup and configuration
*/

#ifndef __SETUP_H__
#define __SETUP_H__

#include <X11/Xlib.h>
#include "gesture.h"
#include "aimath.h"

/* Type of sub-actions that are possible, used in WA_PROCESS struct */
#define WA_EXEC		1		/* Execute the program listed in tparam */
#define WA_WARP		2		/* Move the mouse back to the start position */
#define WA_DELAY	3		/* Delay nparam seconds */
#define WA_KILL		4		/* Kill last wayV started program */
#define WA_SENDKEY	5		/* Send key press */

/* Different type of include files */
#define WF_KEYMAP	"Keymap"

/* Parameters for setup.c:getKeyText(), SKT = Strip Key Text */
#define SKT_NONE			0
#define SKT_QUOTES			1

/* Defines for accessing window box information */
#define	WI_X		0
#define WI_Y		1
#define WI_WIDTH	2
#define WI_HEIGHT	3

/* Defaults */
#define DEF_UNI_XGRID		8
#define DEF_UNI_YGRID		8
#define DEF_UNI_MAXGPOINTS	2000
#define DEF_UNI_MAXVECTORS	10
#define DEF_PRETTY_SIZE		2
#define DEF_PRETTY_WAIT		250

/* Structure of the configuration file */
typedef struct {
	char *method;
	unsigned int pbutton;
	unsigned int kmodifier;
} WLAUNCH;

typedef struct {
	char *display;
	char *managed;
	char *color;
	int size;
	char *feedback;
	int wait;

	int window[4];
} WPRETTY;

typedef struct {
	char *griddle;
	int xgrid;
	int ygrid;
	int maxgpoints;
	int maxvectors;
} WUNIVERSE;

typedef struct {
	KeyCode kcode;
	char *normal;
	char *shift;
} WKEYMAP;

/* Component parts of an action process */
typedef struct {
	int perform;
	char *tparam;
	int nparam;
} WA_PROCESS;

typedef struct {
	char *name;
	char *process;
	char *match;
	WA_PROCESS **subs;
} WACTION;

typedef struct {
	char *name;
	char *desc;
	char *shape;
	char *action;
	char *vector;
	MATRIX *wshape;
	VECTOR *wvector;
} WGESTURE;

typedef struct {
	char *name;
	char *path;

	GPOINT *wpath;
} WORBIT;

typedef struct {
	char *version;
	WLAUNCH *launch;
	char *think;
	WPRETTY *pretty;
	WUNIVERSE *universe;
	WKEYMAP **keymap;
	WGESTURE **gestures;
	WORBIT **orbits;
	WACTION **actions;	
} WSETUP;


/* Function prototypes */
int writeSetup(WSETUP *, char *);
WSETUP *readSetup(char *);
WSETUP *processSetup(WSETUP *);
WSETUP *parseSetup(WSETUP *, char *);

char *stripPadding(char *);

char *getKeyText(char *, int *, int);
int getKeyNum(char *, int *);
char *getCommaText(char *, int *);
char *getDelimText(char *, int *, char);
char *getQuoteText(char *);

GPOINT *parseOrbitPath(char *);
VECTOR *parseVector(char *, int);
MATRIX *parseGSpace(char *, int, int);
char *parseMSpace(MATRIX *);
char *parseVSpace(VECTOR *);
WA_PROCESS **parseProcess(char *);
int parseInclude(WSETUP *, char *, char *);


/* Fields in the setup file */
#define SF_HEADER "# wayV configuration file\n#\n# Comments begin with #\n#\n# Empty lines are ignored\n#\n# Keys must go along the lines of:\n#		key = value;\n# each key must be terminated with ;\n\n"

#define SF_VERSION	"Version = %s;\n"

#define SF_THINK	"Think = \"%s\";\n"

#define SF_LAUNCH	"Launch = \"M%d\";\n\n"

#define SF_PRETTY	"Pretty {\n\tdisplay = %s;\n\tcolor = %s;\n\tsize = %d;\n\tfeedback = %s;\n\twait = %d;\n}\n\n"

#define SF_UNIVERSE	"Universe {\n\tgriddle = \"%s\";\n\txgrid = %d;\n\tygrid = %d;\n\tmaxgpoints = %d;\n\tmaxvectors = %d;\n}\n\n"

#define SF_GESTURE      "Gesture {\n\tname = \"%s\";\n\tdescription = \"%s\";\n\tshape = %s;\n\taction = \"%s\";\n\tvector = %s;\n}\n\n"

#define SF_ORBIT        "Orbit {\n\tname = \"%s\";\n\tpath = %s;\n}\n\n"

#define SF_ACTION       "Action {\n\tname = \"%s\";\n\tprocess = %s;\n\tmatch = \"%s\";\n}\n\n"

#endif /* __SETUP_H__ */
