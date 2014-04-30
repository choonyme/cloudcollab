/***************************************************************************
 *            UIManager.cc
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

#include "UIManager.h"

UIManager::UIManager(rfbScreenInfo* server){
	this->server = server;
	screenData.buffer = (uint8_t*) server->frameBuffer;
	screenData.z_buffer = new rfbClient*[ server->width * server->height ];
	screenData.overlayz_buffer = new rfbClient*[ server->width * server->height ];
	screenData.width = server->width;
	screenData.height = server->height;
	last_layer = -1;
	overlayControl = NULL;

	for(int i=0; i < server->width * server->height; ++i){
		screenData.z_buffer[i] = NULL;
		screenData.overlayz_buffer[i] = NULL;
	}
	
}

UIManager::~UIManager(){
	if(screenData.z_buffer != NULL)
		delete[] screenData.z_buffer;
	
	if(screenData.overlayz_buffer != NULL)
		delete[] screenData.overlayz_buffer;
	
	for(unsigned int i=0; i < vecClients.size(); ++i){
		if(vecClients[i] != NULL)
			delete vecClients[i];
	}
	
	for(unsigned int i=0; i < vecOverlayClients.size(); ++i){
		if(vecOverlayClients[i] != NULL)
			delete vecOverlayClients[i];
	}
}

void UIManager::createWindow(rfbClient *cl, int attribute){
	WindowData *windowData;
	
	if(cl == NULL)
		return;
	
	windowData = new WindowData;
	windowData->attribute = attribute;
	if(attribute == WindowData::DESKTOP || attribute == WindowData::OVERLAY){
		windowData->x_pos = 0;
		windowData->y_pos = 0;
		windowData->layer = (attribute == WindowData::DESKTOP) ? 0 : ++last_layer;
	}
	else{
		windowData->layer = ++last_layer; //New window will be in front
		//Generate window at random position
		windowData->x_pos = rand() % (screenData.width / 2);
		windowData->y_pos = rand() % (screenData.height / 2);
	}
	
	windowData->visible = false;
	cl->genericData = (void*) windowData;
	if(attribute != WindowData::OVERLAY){
		vecClients.push_back(cl);
		sort(vecClients.begin(), vecClients.end(), SortLayer());
	}
	else{
		vecOverlayClients.push_back(cl);
		sort(vecOverlayClients.begin(), vecOverlayClients.end(), SortLayer());
	}
	
	showWindow(cl);
}

void UIManager::destroyWindow(rfbClient *cl){
	WindowData* wndtmp = (WindowData*) cl->genericData;
	
	if(wndtmp->attribute != WindowData::OVERLAY){
		hideWindow(cl);
		for(rfbClientVec::iterator i = vecClients.begin(); i != vecClients.end(); i++){
			if(*i == cl){
				vecClients.erase(i);
				break;
			}
		}
	}else{
		if(overlayControl == cl)
			overlayControl = switchControlToOverlay();
		
		hideOverlay(cl);
		for(rfbClientVec::iterator i = vecOverlayClients.begin(); i != vecOverlayClients.end(); i++){
			if(*i == cl){
				vecOverlayClients.erase(i);
				break;
			}
		}
	}
	
	delete wndtmp;
}

void UIManager::moveWindow(rfbClient* cl, int new_x, int new_y){
	WindowData* wndtmp = (WindowData*) cl->genericData;
	
	if( wndtmp->attribute != WindowData::NORMAL )
		return;
	
	hideWindow(cl);
		
	wndtmp->x_pos = new_x;
	wndtmp->y_pos = new_y;
		
	showWindow(cl);
	updateRegion(0, 0, screenData.width, screenData.height);
}

void UIManager::showWindow(rfbClient *cl){
	int tx = ((WindowData*) cl->genericData)->x_pos;
	int ty = ((WindowData*) cl->genericData)->y_pos;
	int attribute = ((WindowData*) cl->genericData)->attribute;
	
	((WindowData*) cl->genericData)->visible = true;
	
	if( attribute != WindowData::OVERLAY ){	
		for(int i=0; i < cl->height; ++i){
			for(int j=0; j < cl->width; ++j){
				int coord_x = j + tx;
				int coord_y = i + ty;
				
				if(coord_x < screenData.width && coord_y < screenData.height)
					screenData.z_buffer[(coord_y * screenData.width) + coord_x] = cl;
			}
		}
	}else{//latest change - good for displaying overlay again if it has been hidden. For future use since we can't hide overlay now...
		//for each pixel, if not transparent, then mark overlayz_buffer
		//screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] = cl;		
		for(int i=0; i < cl->height; ++i){
			for(int j=0; j < cl->width; ++j){
				int coord_x = j + tx;
				int coord_y = i + ty;
				int channel;
					
				for(channel = 0; channel < PIXELSIZE; ++channel){
					if(cl->frameBuffer[(i * cl->width + j)*PIXELSIZE + channel])
						break;
				}
				
				if(channel < PIXELSIZE){
					rfbClient *tmp = screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x]; //change...
					if( ((WindowData*) tmp->genericData)->layer < ((WindowData*) cl->genericData)->layer ){ //change...
						screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] = cl;
					}
				}
				else{
					if(screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] == cl){
						screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] = NULL;
					}
				}
			}
		}
	}
}

void UIManager::showAllWindows(){ //questionable
	for(unsigned int i=0; i < vecClients.size(); ++i){
		rfbClient* tmp;
		tmp = vecClients[i];
		if( ((WindowData*) tmp->genericData)->visible == false ){
			showWindow(tmp);
		}
	}
	
	updateRegion(0, 0, screenData.width, screenData.height);
}


void UIManager::hideOverlay(rfbClient *cl){
	WindowData* wndData = (WindowData*) cl->genericData;
	int x_pos = wndData->x_pos;
	int y_pos = wndData->y_pos;

	if(wndData->attribute != WindowData::OVERLAY)
		return;
	
	wndData->visible = false;
		
	for(int i=0; i < cl->width * cl->height; ++i){
		if( screenData.overlayz_buffer[i] != cl )
			continue;
			
		screenData.overlayz_buffer[i] = NULL;
			
		//For that pixel, determine what's the underneath overlay (if there is)
		for(rfbClientVec::reverse_iterator ri = vecOverlayClients.rbegin(); ri != vecOverlayClients.rend(); ++ri){
			rfbClient *tmp = *ri;
			if( ((WindowData*) tmp->genericData)->visible == true && tmp != cl){
				//check each channel
				int channel;
					
				for(channel = 0; channel < PIXELSIZE; ++channel){
					if(tmp->frameBuffer[i * PIXELSIZE + channel])
						break;
				}
				//if pixel not transparent (eg. contains color), then mark overlayz_buffer and fill frameBuffer
				if(channel < PIXELSIZE){
					screenData.overlayz_buffer[i] = tmp;
					break;
				}
			}
		}
	}
	
	updateRegion(x_pos, y_pos, x_pos + cl->width, y_pos + cl->height);
}

void UIManager::hideWindow(rfbClient *cl){
	/* For each windows, in the accending order of the layers, see if it is 
	involed in that affected area. If so, draw the affected part of that
	window. */
	WindowData* wndData = (WindowData*) cl->genericData;
	if( wndData->attribute != WindowData::NORMAL )
		return;
	
	int x_pos_beg = wndData->x_pos;
	int y_pos_beg = wndData->y_pos;
	int x_pos_end = x_pos_beg + cl->width;
	
	int y_pos_end = y_pos_beg + cl->height;
	
	wndData->visible = false;
	for(unsigned int i=0; i < vecClients.size(); ++i){ //Redraw all lower windows in that closed region
		rfbClient* rfbtemp = vecClients[i];
		WindowData* wndtemp = (WindowData*) rfbtemp->genericData;
		int x_from, x_to, y_from, y_to;
		
		if(wndtemp->visible){
			if( !(
				(x_pos_beg < wndtemp->x_pos && x_pos_end < wndtemp->x_pos) ||
				(x_pos_beg > wndtemp->x_pos + rfbtemp->width && x_pos_end > wndtemp->x_pos + rfbtemp->width)
				) &&
				!(
				(y_pos_beg < wndtemp->y_pos && y_pos_end < wndtemp->y_pos) ||
				(y_pos_beg > wndtemp->y_pos + rfbtemp->height && y_pos_end > wndtemp->y_pos + rfbtemp->height)
				) )
			{
				x_from = (wndtemp->x_pos <= x_pos_beg) ? x_pos_beg : wndtemp->x_pos;
				x_to = (wndtemp->x_pos + rfbtemp->width >= x_pos_end) ? x_pos_end : wndtemp->x_pos + rfbtemp->width;
				y_from = (wndtemp->y_pos <= y_pos_beg) ? y_pos_beg : wndtemp->y_pos;
				y_to = (wndtemp->y_pos + rfbtemp->height >= y_pos_end) ? y_pos_end : wndtemp->y_pos + rfbtemp->height;

				for(int i=y_from; i < y_to && i < screenData.height; ++i){
					for(int j=x_from; j < x_to && j < screenData.width; ++j)
							screenData.z_buffer[(i * screenData.width) + j] = rfbtemp;
				}
			}
		}
	}

	updateRegion(x_pos_beg, y_pos_beg, x_pos_end, y_pos_end);
}

rfbClient* UIManager::activeWindowAt(int x_pos, int y_pos){
	if(overlayControl != NULL)
		return overlayControl;
		
	rfbClient* layer;
	
	if(x_pos < screenData.width && y_pos < screenData.height){
		if( (layer = screenData.z_buffer[(y_pos	* screenData.width) + x_pos]) != NULL )
			return layer;
	}
	
	return NULL;
}

//Process overlay FB and convert them to overlayz_buffer representation
void UIManager::processOverlay(rfbClient* cl, int x, int y, int width, int height){
	if(cl == NULL)
		return;
	
	if( ((WindowData*) cl->genericData)->visible == false ){
		return;
	}
	
	int tx = ((WindowData*) cl->genericData)->x_pos;
	int ty = ((WindowData*) cl->genericData)->y_pos;
	
	//for overlay layer
	for(int i=y; i < y + height && i < screenData.height; ++i){
		for(int j=x; j < x + width && i < screenData.width; ++j){
			int channel;
			int coord_x = j + tx;
			int coord_y = i + ty;
				
			for(channel = 0; channel < PIXELSIZE; ++channel){
				if(cl->frameBuffer[(i * cl->width + j)*PIXELSIZE + channel])
					break;
			}
			//if pixel not transparent (eg. contains color), then mark overlayz_buffer and fill frameBuffer
			if(channel < PIXELSIZE){
				rfbClient *tmp = screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x]; //change...
				if(tmp != NULL){
					if( ((WindowData*) tmp->genericData)->layer > ((WindowData*) cl->genericData)->layer ){ //change...	
						continue;
					} //change...
				}
				
				screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] = cl;
			}
			else{
				//if pixel is transparent, do recovery steps if necessary
				if(screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] == cl){
					screenData.overlayz_buffer[(coord_y * screenData.width) + coord_x] = NULL; //recover overlay as well!!!!
				}
			}
		}
	}	
}

//from the perspective of entire FB, frameBuffer assembler.
void UIManager::updateRegion(int x1, int y1, int x2, int y2){
	//Slap in normal windows
	for(int i=y1; i < y2 && i < screenData.height; ++i){
		for(int j=x1; j < x2 && j < screenData.width; ++j){
			int x_pos, y_pos;
			int coord_x, coord_y; //client perspective
			int channel;
			
			rfbClient* cl;
			rfbClient* overlayCl = screenData.overlayz_buffer[(i * screenData.width) + j];
			
			if(overlayCl == NULL) //Slap in normal window
				cl = screenData.z_buffer[(i * screenData.width) + j];
			else //Slap in overlay
				cl = overlayCl;
				
			if( cl == NULL )
					continue;
				
			x_pos = ((WindowData*) cl->genericData)->x_pos;
			y_pos = ((WindowData*) cl->genericData)->y_pos;
			
			//client perspective
			coord_x = j - x_pos;
			coord_y = i - y_pos;
	
			for(channel = 0; channel < PIXELSIZE; ++channel)
				screenData.buffer[(i * screenData.width + j) * PIXELSIZE + channel] = cl->frameBuffer[(coord_y * cl->width + coord_x) * PIXELSIZE + channel];
		}
	}
	
	//Slap in pointers
	UserPointerMap::iterator p;
	for(p = userPointers.begin(); p != userPointers.end(); p++){
		PointerData* pd = &p->second;
		if( pd->old_x >= x1 - 4 && pd->old_x <= x2 + 4&&
			pd->old_y >= y1 - 4 && pd->old_y <= y2 + 4){
				drawPointer(pd->old_x, pd->old_y);
		}
    }
}

rfbClient* UIManager::switchControlToOverlay(){
	rfbClientVec::iterator overlayIter;
	
	//Find current overlay position
	for(overlayIter = vecOverlayClients.begin(); overlayIter != vecOverlayClients.end() && overlayControl != NULL; overlayIter++){
		printf("Finding previous overlay\n");
		if( *overlayIter == overlayControl )
			break;
	}

	//Increment position until another overlay can be found or until EOF	
	if(overlayControl != NULL)
		overlayIter++;
	
	if(overlayIter != vecOverlayClients.end()){
		printf("Sending some overlay\n");
		overlayControl = *overlayIter;
	}else{
		printf("Sending NULL\n");
		overlayControl = NULL;
	}
	
	return overlayControl;
}

void UIManager::updatePointer(int id, int x, int y){
	int old_x, old_y;
	PointerData* pd = &userPointers[id];

	old_x = pd->old_x;
	old_y = pd->old_y;

	//clear old trail
	pd->old_x = x;
	pd->old_y = y;
		
	updateRegion(old_x, old_y, old_x + 4, old_y + 4);
	
	//draw new pointer
	updateRegion(pd->old_x, pd->old_y, pd->old_x + 4, pd->old_y + 4);

	rfbMarkRectAsModified(server, 
		old_x < x ? old_x : x,
		old_y < y ? old_y : y,
		old_x > x ? old_x + 4 : x + 4, 
		old_y > y ? old_y + 4 : y + 4);
}

void UIManager::drawPointer(int x, int y){
	/*unsigned char* fb;
	Cairo::RefPtr<Cairo::Context> cr;
	Cairo::RefPtr<Cairo::ImageSurface> surface;
	
	surface = Cairo::ImageSurface::create(fb, Cairo::FORMAT_ARGB32, 1024, 768, 0);
	cr = Cairo::Context::create(surface);
	cr->set_source_rgb(1.0, 0.0, 0.0);*/
	
	for(int i = y; i < y + 4; ++i)
		for(int j = x; j < x + 4; ++j)
			for(int channel = 0; channel < PIXELSIZE; ++channel)
				screenData.buffer[ (i * screenData.width + j) * PIXELSIZE + channel ] = 1;
}
