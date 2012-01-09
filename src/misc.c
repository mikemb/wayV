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
 *	Misc functions that don't fall into any particular
 *	category or there ain't enough to create a new separate
 *	classification
*/

#include<sys/time.h>
#include<unistd.h>
#include "defines.h"
#include "misc.h"


/*
 * Purpose:
 * 	Wait a number of milli seconds before returning
 *
 * Parameters:
 * 	1st - number of milli seconds to wait
 * 		
 * Returns:
 * 	NONE
 *
 * NOTE:
 * 	There are 1000 milli-seconds in 1 second
 *				
 * History:
 * 	19/02/2001 (Mike Bennett): Created function
*/
void waitMilli(long int milli) {
	struct timeval millisec, elapse;

	gettimeofday(&millisec, NULL);

	millisec.tv_usec += milli * 1000;
	millisec.tv_sec += millisec.tv_usec / 1000000;
	millisec.tv_usec = millisec.tv_usec % 1000000;

	for(gettimeofday(&elapse, NULL); elapse.tv_sec < millisec.tv_sec
		|| elapse.tv_usec < millisec.tv_usec;
		gettimeofday(&elapse, NULL));
}
