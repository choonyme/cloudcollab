/***************************************************************************
 *            drawtop_bonjour.h
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

#ifndef VNC_BONJOUR_H
#define VNC_BONJOUR_H

#include <dns_sd.h>
#include <stdio.h>
#include <string.h>
#define TXT_RECORD_SIZE 255
#define OVERLAY_ON 1
#define OVERLAY_OFF 0

void DNSSD_API RegisterCallBack(DNSServiceRef service,
							   DNSServiceFlags flags,
							   DNSServiceErrorType errorCode,
							   const char *name,
							   const char *type,
							   const char *domain,
							   void *context);

DNSServiceErrorType DNSServiceRegister(DNSServiceRef *serviceRef, int port, int overlay);

void DNSServiceDeregister(DNSServiceRef serviceRef);

#endif
