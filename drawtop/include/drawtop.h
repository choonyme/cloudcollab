/***************************************************************************
 *            drawtop.h
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

#ifndef DRAWTOP_H
#define DRAWTOP_H

#include "vpclient.h"

/* run the event loop for a while: process() and friends:
 * process() returns only on timeout,
 * waitforanything returns on any event (input, output or timeout),
 * waitforupdate() returns only on timeout or screen update,
 * waitforinput() returns only on timeout or user input,
 * visualgrep() returns only on timeout or if the specified PNM was found
 * 	(in that case, x_origin and y_origin are set to the upper left
 * 	 corner of the matched image). */

//result_t process(resource_t res,timeout_t seconds);
resource_t init_drawtop(int width,int height, int listen_port);
result_t run_drawtop(timeout_t seconds);

#endif //DRAWTOP_H
