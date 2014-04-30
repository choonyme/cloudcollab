/***************************************************************************
 *            vncproxy.h
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
 
#ifndef VNCPROXY_H
#define VNCPROXY_H

/*#ifdef __cplusplus
extern "C"{
#endif
#include <rfb/rfb.h>
#include <rfb/rfbclient.h>
#ifdef __cplusplus
}
#endif*/
#include "vp.h"

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

extern char *inputPassword(rfbClient* client);

resource_t initvnc(const char* server,int serverPort,int listenPort);
resource_t initServiceDiscovery();
void closevnc(resource_t res);
pthread_t process(resource_t res,timeout_t seconds);
void resolveService(const char *name, int interfaceIndex);

#endif //DRAWTOP_H
