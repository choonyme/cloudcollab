/***************************************************************************
 *            bonjour_browser.cc
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
 
#include "bonjour_browser.h"
#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define STRING_SIZE 100
#define TXT_RECORD_SIZE 255

//DNS TXT records
TXTRecordRef txtRecord;
char txtRecordStorage[TXT_RECORD_SIZE];

//Bonjour socket reference for advertising this (VNCProxy) service
int dns_sd_fd_advertise;
DNSServiceRef serviceRefAdvertise;

void DNSSD_API MyBrowseCallBack(DNSServiceRef service, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context){
#pragma unused(context)
	if(errorCode != kDNSServiceErr_NoError)
		fprintf(stderr, "MyBrowseCallBack returned %d\n", errorCode);
	else{
		const char *addString = (flags & kDNSServiceFlagsAdd) ? "ADD" : "REMOVE";
		const char *moreString = (flags & kDNSServiceFlagsMoreComing) ? "MORE" : "	";
		printf("%-7s%-5s %d %s . %s %s\n", addString, moreString, interfaceIndex, name, type, domain);
		
		//Service discovered, send the name to list.
		if(flags & kDNSServiceFlagsAdd){
			//if not in list, add name to list. Hand over to HandleServiceDiscovered().
			if( strcmp(name, "vncproxy") )
				HandleServiceDiscovered(name);
		}
		else{ //Service removed
			HandleServiceDisconnected(name);
		}
	}
	if(!(flags & kDNSServiceFlagsMoreComing)) fflush(stdout);
}

DNSServiceRef MyDNSServiceBrowse(){
	DNSServiceErrorType error;
	DNSServiceRef serviceRef;
	
	error = DNSServiceBrowse(&serviceRef,
		0,
		0,
		"_vnc._tcp",
		"",
		MyBrowseCallBack,
		NULL);

	
	if(error == kDNSServiceErr_NoError){
		return serviceRef;
	}
	
	return NULL;
}

void DNSSD_API MyResolveCallBack(DNSServiceRef serviceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType errorCode, const char *fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const unsigned char *txtRecord, void *context){
#pragma unused(flags)
#pragma unused(fullname)
	struct hostent *hp;
	hp = gethostbyname(hosttarget);
	if(hp == NULL)
		return;

	if(errorCode != kDNSServiceErr_NoError)
		fprintf(stderr, "MyResolveCallBack returned %d\n", errorCode);
	else{
		char *ip;
		ip = inet_ntoa( *( struct in_addr*)(hp -> h_addr_list[0]) );
		printf("RESOLVE: %s is at %s %s:%d interface %d\n", fullname, ip, hosttarget, ntohs(port), interfaceIndex);
		
		if( TXTRecordContainsKey(txtLen, txtRecord, "overlay") ){
			//add overlay service
			fprintf(stderr, "OVERLAY service detected.\n");
			connectService((char*) context, ip, ntohs(port), SERVICE_OVERLAY);
		}else{
			//add standard service
			fprintf(stderr, "STANDARD service detected.\n");
			connectService((char*) context, ip, ntohs(port), SERVICE_STANDARD);
		}
	}		
	
	if(!(flags & kDNSServiceFlagsMoreComing))
		fflush(stdout);
}

DNSServiceErrorType MyDNSServiceResolve(const char *serviceName, uint32_t interfaceIndex){
	DNSServiceErrorType error;
	DNSServiceRef serviceRef;

	error = DNSServiceResolve(&serviceRef,
		0,
		interfaceIndex,
		serviceName,
		"_vnc._tcp",
		"local",
		MyResolveCallBack,
		(void*) serviceName);

	if(error == kDNSServiceErr_NoError){
		DNSServiceProcessResult(serviceRef);
		DNSServiceRefDeallocate(serviceRef);
	}

	return error;
}

void DNSSD_API RegisterCallBack(DNSServiceRef service, DNSServiceFlags flags, DNSServiceErrorType errorCode, const char *name, const char *type, const char *domain, void *context){
#pragma unused(flags)
#pragma unused(context)

	if(errorCode != kDNSServiceErr_NoError)
		fprintf(stderr, "MyRegisterCallBack returned %d\n", errorCode);
	else
		printf("%-15s %s.%s%s\n", "REGISTER", name, type, domain);
}

int DNSServiceRegister(int port, int screenWidth, int screenHeight){
	//DNS service status
	DNSServiceErrorType error;

	//Screen size in string format
	char strHeight[STRING_SIZE],
		 strWidth[STRING_SIZE];
	
	//Convert screen size to string
	snprintf(strHeight, STRING_SIZE, "%d", screenWidth);
	snprintf(strWidth, STRING_SIZE, "%d", screenHeight);
	
	//Set TXT records
	fprintf(stderr, "Registering screen size...\n");
	TXTRecordCreate(&txtRecord, TXT_RECORD_SIZE, txtRecordStorage);
	TXTRecordSetValue(&txtRecord, "screenHeight", strlen(strHeight), strHeight);
	TXTRecordSetValue(&txtRecord, "screenWidth", strlen(strWidth), strWidth);
	
	//Bonojour DNS Register
	error = DNSServiceRegister(&serviceRefAdvertise,
		0,
		0,
		"vncproxy",
		"_vnc._tcp",
		"",
		NULL,
		htons(port),
		TXTRecordGetLength(&txtRecord),
		TXTRecordGetBytesPtr(&txtRecord),
		RegisterCallBack,
		NULL);

	if(error == kDNSServiceErr_NoError){
		dns_sd_fd_advertise = DNSServiceRefSockFD(serviceRefAdvertise);
	}
	else{
		fprintf(stderr, "DNSServiceDiscovery returned %d\n", error);
		return -1;
	}
	
	return dns_sd_fd_advertise;
}

void DNSServiceDeregister(){
	DNSServiceRefDeallocate(serviceRefAdvertise);
	TXTRecordDeallocate(&txtRecord);
}
