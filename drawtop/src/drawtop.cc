/***************************************************************************
 *            drawtop.cc
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
#define OVERLAY

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <map>
#include "drawtop.h"

#define UserMap map<int, ClientData*>

using namespace std;

typedef struct ClientData ClientData;

struct ClientData
{
    int old_x, old_y;
    int old_button;
    
    unsigned char colour[4];
};

//User Map
UserMap userMap;

private_resource_t resource;

//Random colour generator
static void
assign_random_colour(unsigned char* colour)
{
    colour[0] = (unsigned char)(255.0*rand()/(RAND_MAX));
    colour[1] = (unsigned char)(255.0*rand()/(RAND_MAX));
    colour[2] = (unsigned char)(255.0*rand()/(RAND_MAX));
    colour[3] = 255;
}

//Keyboard event hook
static void got_key(rfbBool down,rfbKeySym keysym, int id, rfbClientRec* cl)
{
    resource.result|=RESULT_KEY;
	fprintf(stderr, "Got key!!!\n");
    
	//Clear screem is PageUp is pressed.
    if(down && (keysym == XK_Page_Up))
    {
		#ifdef OVERLAY
			fprintf(stderr, "OVERLAY: clearing screen\n");
			fill_screen(&resource, OVERLAY_PIXEL);
		#else
			fill_screen(&resource, 225);
		#endif
		
		rfbMarkRectAsModified(resource.server, 0, 0,
					  resource.server->width, resource.server->height);
    }else if (down && (keysym == XK_F3)) //Select another random colour
    {
		fprintf(stderr, "F3 detected.\n");
		ClientData *cd = userMap[id];
		
		if(cd != NULL)
			assign_random_colour((unsigned char*)&(cd->colour));
    }
}

// From example.c (originally "drawline()")
static void
draw_line(unsigned char* buffer,
	  int width,
	  int height,
	  int bpp, 
	  int x1,
	  int y1,
	  int x2,
	  int y2,
	  unsigned char* colour)
{
    int i;
    int j;
    
    i = x1 - x2;
    j = y1 - y2;

    if (i == 0 && j == 0)
    {
		set_pixel(&resource, x1, y1, colour);

		return;
    }
    
    if (i < 0)
	i = -i;

    if (j < 0)
	j = -j;

    if (i < j)
    {
	if (y1 > y2)
	{
	    i = y2;
	    y2 = y1;
	    y1 = i;
	    i = x2;
	    x2 = x1;
	    x1 = i;
	}
	for(j = y1; j <= y2; j++)
		set_pixel(&resource,
		      x1+(j-y1)*(x2-x1)/(y2-y1), j, colour);
    }
    else
    {
		if (x1 > x2)
		{
			i = y2;
			y2 = y1;
			y1 = i;
			i = x2;
			x2 = x1;
			x1 = i;
		}
		
		for(i = x1 ; i <= x2; i++)
			set_pixel(&resource,
		      i, (y1+(i-x1)*(y2-y1)/(x2-x1)), colour);
    }
}

//Mouse event hook
static void got_mouse(int button_mask,int x,int y,int id,rfbClientRec* cl)
{
    ClientData* cd;

	if( (cd = userMap[id]) == NULL && button_mask != 255){
		cd = (ClientData*) malloc(sizeof(ClientData));
		userMap[id] = cd;
		assign_random_colour((unsigned char*)&(cd->colour));
	}else if(button_mask == 255){
		if(cd != NULL){
			free(cd);
			userMap[id] = NULL;
			userMap.erase(id);
		}
		return;
	}

    resource.result|=RESULT_MOUSE;

	// draw line
	if (button_mask)
	{
	    if (cd->old_button == button_mask)
	    {
		int x1;
		int y1;
		int w;
		int h;
		
		draw_line((unsigned char*)resource.server->frameBuffer,
			  resource.server->width,
			  resource.server->height,
			  cl->screen->depth / 8,
			  x,
			  y,
			  cd->old_x,
			  cd->old_y,
			  cd->colour);
		
		//may not be required - added to remove artifacts
		x1 = (x < cd->old_x ? x : cd->old_x);
		y1 = (y < cd->old_y ? y : cd->old_y);
		w = abs(x - cd->old_x) + 1;
		h = abs(y - cd->old_y) + 1;
		
		{
		    int x2;
		    int y2;

		    x2 = x1 + w;
		    y2 = y1 + h;

		    if (x1 < 0)
			x1 = 0;
		    if (y1 < 0)
			y1 = 0;

		    if (x2 >= resource.server->width)
			x2 = resource.server->width - 1;
		
		    if (y2 >= resource.server->height)
			y2 = resource.server->height - 1;


		    rfbMarkRectAsModified(resource.server, x1, y1, x2, y2);
		}
	    }
	}
    
    cd->old_x = x;
    cd->old_y = y;
    
    cd->old_button = button_mask;
}

//Screen initialization hook
static void init_screen(){
	#ifdef OVERLAY
		fill_screen(&resource, OVERLAY_PIXEL);
	#else
		fill_screen(&resource, 225);
	#endif
}

//New client initialization hook (For connection between VNCProxy and DrawTop)
static enum rfbNewClientAction
new_client(rfbClientPtr cl)
{
    cl->ptrAddEvent = got_mouse;
	cl->kbdAddEvent = got_key;

    return RFB_CLIENT_ACCEPT;
}

resource_t init_drawtop(int width, int height, int listen_port){
	resource.new_client = new_client;
	resource.init_screen = init_screen;
	resource.listen_port = listen_port;
	
	#ifdef OVERLAY
		return initvnc(&resource, width, height, OVERLAY_ON);
	#else
		return initvnc(&resource, width, height, OVERLAY_OFF);
	#endif
}

result_t run_drawtop(timeout_t seconds){
	return process(&resource, seconds);
}
