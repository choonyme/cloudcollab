/***************************************************************************
 *            vpclient.h
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
#ifndef VPCLIENT_H
#define VPCLIENT_H

#include <rfb/rfb.h>
#include <rfb/rfbclient.h>
#include "drawtop_bonjour.h"

typedef int bool_t;
typedef int keysym_t;
typedef int coordinate_t;
typedef unsigned char buttons_t;
typedef int resource_t;
typedef double timeout_t;
typedef int result_t;

#define RESULT_TIMEOUT 1
#define RESULT_KEY 2
#define RESULT_MOUSE 4
#define RESULT_SCREEN 8
#define RESULT_SHUTDOWN 16

#define OVERLAY_PIXEL 0

using namespace std;

typedef void (*rfb_init_screen)();
typedef enum rfbNewClientAction (*rfb_new_client)(rfbClientPtr cl);
/* this is a VNC connection */
typedef struct private_resource_t private_resource_t;

struct private_resource_t
{
	rfb_new_client new_client;
	rfb_init_screen init_screen;
    int listen_port;
    rfbScreenInfo* server;

    result_t result;
};

resource_t initvnc(private_resource_t *resource, int width, int height, int overlay);
void closevnc(private_resource_t *resource);
result_t process(private_resource_t *resource, timeout_t seconds);
void set_pixel(private_resource_t *res, int x, int y, unsigned char* colour);
void fill_screen(private_resource_t *res, unsigned int value);
#endif
