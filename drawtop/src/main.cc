/***************************************************************************
 *            main.cc
 *
 *  Copyright  2006  Choon Jin NG
 *  Email: cjng@vislab.usyd.edu.au
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "drawtop.h"
#define DEBUG

static int listen_port;
static int width, height;

static resource_t resource;
static result_t result;

int main(int argc, char **argv){
	srand(time(0));

	if (argc != 4)
    {
		printf("usage: %s listen_port width height\n", argv[0]);
		exit(1);
    }

	listen_port = 5900 + atoi(argv[1]);
	width = atoi(argv[2]);
	height = atoi(argv[3]);
    resource = init_drawtop(width, height, listen_port);

	#ifdef DEBUG
	fprintf(stderr, "resource is %d\n", resource);
	#endif
	
    if (resource < 0)
    {
		printf("Could not initialise VNC\n");
		exit(1);
    }
	
    for(;;)
    {
		result = run_drawtop(999999);
		if (result == 0)
		{
			printf("Server dissapeared\n");
			exit(0);
		}
    }

	return 0;
}
