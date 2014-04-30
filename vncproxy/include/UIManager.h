/***************************************************************************
 *            UIManager.h
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

#ifndef UIMANAGER_H
#define UIMANAGER_H
#include <iostream>
#include <vector>
#include <cairomm/cairomm.h>
#include "vp.h"
#include "pointer.h"

#define rfbClientVec vector<rfbClient*>
#define UserPointerMap map<int, PointerData>

using namespace std;

struct SortLayer{
	bool operator()(rfbClient* first, rfbClient* second){
		return ((WindowData*) first->genericData)->layer < ((WindowData*) second->genericData)->layer;
	}
};

class UIManager{
	private:
		ScreenData screenData;
		rfbClientVec vecClients;
		rfbClientVec vecOverlayClients;
		int last_layer;
		rfbClient *overlayControl;
		UserPointerMap userPointers;
		rfbScreenInfo* server;
		
	public:
		UIManager(rfbScreenInfo* server);
		~UIManager();
		void createWindow(rfbClient *cl, int attribute);
		void showWindow(rfbClient *cl);
		void showAllWindows();
		void hideOverlay(rfbClient *cl);
		void hideWindow(rfbClient *cl);
		void destroyWindow(rfbClient *cl);
		void moveWindow(rfbClient* cl, int new_x, int new_y);
		rfbClient* switchControlToOverlay();
		rfbClient* activeWindowAt(int x_pos, int y_pos);
		void processOverlay(rfbClient* cl, int x, int y, int width, int height);
		void updateRegion(int x1, int y1, int x2, int y2);
		void updatePointer(int id, int x, int y);
		void drawPointer(int x, int y);
};

#endif
