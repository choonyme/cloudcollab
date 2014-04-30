/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
#ifndef VP_H
#define VP_H

#ifdef __cplusplus
extern "C"{
#endif
#include <rfb/rfb.h>
#include <rfb/rfbclient.h>
#ifdef __cplusplus
}
#endif

#define PIXELSIZE 4

typedef struct ClientData ClientData;

struct ClientData
{
	rfbClient *activeScreen;
    int id;
    int old_x, old_y;
    int old_button;
	
    unsigned char colour[4];
};

typedef struct WindowData{
	enum windowAttribute {OVERLAY, DESKTOP, NORMAL};
	int layer;
	int x_pos, y_pos;
	int attribute;
	bool visible;
}WindowData;

typedef struct ScreenData
{
	uint8_t *buffer;
	
	rfbClient **z_buffer;
	rfbClient **overlayz_buffer;
    int height, width;
}ScreenData;

typedef struct PointerData{
	int x, y;
	int old_x, old_y;
	unsigned char *label_img;
	int pointer_width, pointer_height;
	int label_width, label_height;
}PointerData;

#endif
