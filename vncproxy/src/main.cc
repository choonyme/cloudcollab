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
 
#include "wndMain.hh"

static wndMain *wndMain;

int main(int argc, char** argv)
{
	Gtk::Main m(&argc, &argv);
	wndMain = new class wndMain();
	m.run(*wndMain);
	delete wndMain;
    return 0;
}
