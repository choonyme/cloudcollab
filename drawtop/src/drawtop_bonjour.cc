/***************************************************************************
 *            drawtop_bonjour.cc
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

#include "drawtop_bonjour.h"
#include <netinet/in.h>

TXTRecordRef txtRecord;
char txtRecordStorage[TXT_RECORD_SIZE];
	
void DNSSD_API RegisterCallBack(DNSServiceRef service, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context){
#pragma unused(flags)
#pragma unused(context)

	if(errorCode != kDNSServiceErr_NoError)
		fprintf(stderr, "MyRegisterCallBack returned %d\n", errorCode);
	else
		printf("%-15s %s.%s%s\n", "REGISTER", name, type, domain);
}

DNSServiceErrorType DNSServiceRegister(DNSServiceRef *serviceRef, int port, int overlay){
	DNSServiceErrorType error;

	TXTRecordCreate(&txtRecord, TXT_RECORD_SIZE, txtRecordStorage);
	
	if(overlay == OVERLAY_ON){
		fprintf(stderr, "OVERLAY: Setting overlay attribute\n");
		TXTRecordSetValue(&txtRecord, "overlay", 1, "a");
	}
	
	error = DNSServiceRegister(serviceRef,
		0,
		0,
		"drawtop-nextX",
		"_vnc._tcp",
		"",
		NULL,
		htons(port),
		TXTRecordGetLength(&txtRecord),
		TXTRecordGetBytesPtr(&txtRecord),
		RegisterCallBack,
		NULL);

	return error;
}

void DNSServiceDeregister(DNSServiceRef serviceRef){
	DNSServiceRefDeallocate(serviceRef);
	TXTRecordDeallocate(&txtRecord);
}
