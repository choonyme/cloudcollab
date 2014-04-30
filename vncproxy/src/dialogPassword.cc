// generated 2007/1/5 1:33:58 EST by cjx@cjx-laptop.(none)
// using glademm V2.6.0
//
// newer (non customized) versions of this file go to dialogPassword.cc_new

// This file is for your program, I won't touch it again!

#include "config.h"
#include "dialogPassword.hh"

dialogPassword::dialogPassword(){
}

Glib::ustring dialogPassword::getPassword(){
	return m_txtPassword->get_text();
}
