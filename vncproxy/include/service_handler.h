/***************************************************************************
 *            fdhandler.h
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
#ifndef FDHANDLER_H
#define FDHANDLER_H
#include <string>

#include <map>

#ifdef __cplusplus
extern "C"{
#endif
#include <dns_sd.h>
//#include <rfb/rfb.h>
#include <rfb/rfbclient.h>
#ifdef __cplusplus
}
#endif

#define ServiceMap map<string, ServiceDetails*>

using namespace std;

typedef struct serviceDetails{
	uint32_t interfaceIndex;
	rfbClient* client;
}ServiceDetails;

class ServiceHandler{
	private:
		int maxfd;
		fd_set master_fds;
		ServiceMap::iterator p;
		ServiceMap services;
		DNSServiceRef browser;
	
	public:
		typedef ServiceMap::iterator type;
	
	public:
		ServiceHandler();
		void setBrowser(DNSServiceRef serviceRef);
		DNSServiceRef getBrowser();
		void addService(const char* serviceName);
		void addSocket(int sockfd);
		void removeSocket(int sockfd);
		bool hasSocket(int sockfd);
		fd_set getAllSockets();
		int getMaxFD();
		void removeService(const char* serviceName);
		int merge_fdset(int nfd, fd_set *set);
		ServiceDetails* getService(fd_set *set);
		bool hasService(const char* serviceName);
		ServiceDetails* getCurrentService();
		ServiceDetails* getServiceDetails(const char* serviceName);
		ServiceDetails* getNextService();
		ServiceMap::iterator end();
		ServiceMap::iterator begin();
};
#endif
