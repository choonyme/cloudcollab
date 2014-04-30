/***************************************************************************
 *            service_handler.cc
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
 
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include "service_handler.h"
#include "bonjour_browser.h"

ServiceHandler::ServiceHandler(){
	p = services.end();
	FD_ZERO(&master_fds);
	maxfd = -1;}

void ServiceHandler::setBrowser(DNSServiceRef serviceRef){
	browser = serviceRef;
	addSocket(DNSServiceRefSockFD(serviceRef));
}

DNSServiceRef ServiceHandler::getBrowser(){
	return browser;
}

void ServiceHandler::addService(const char* serviceName){
	printf("Inserting service %s\n", serviceName);

	ServiceDetails *sd = new ServiceDetails;
	sd->interfaceIndex = -1;
	sd->client = NULL;

	services[serviceName] = sd;
}

void ServiceHandler::addSocket(int sockfd){
	FD_SET(sockfd, &master_fds);
	if(maxfd < sockfd)
		maxfd = sockfd;
}

void ServiceHandler::removeSocket(int sockfd){
	FD_CLR(sockfd, &master_fds);
	
	//find next largest fd number
	if(maxfd == sockfd){
		int i;
		
		printf("Performing socket reduction...\n");
		for(i=maxfd; i >= 0; i--){
			if( FD_ISSET(i, &master_fds) ){
				printf("Socket reduced to %d\n", i);
				maxfd = i;
				return;
			}
		}
	}
}

bool ServiceHandler::hasSocket(int sockfd){
	if(FD_ISSET(sockfd, &master_fds))
		return true;
	
	return false;
}

void ServiceHandler::removeService(const char* serviceName){
	ServiceMap::iterator p;
	
	p = services.find(serviceName);
	if(p != services.end()){
		//NOTE: Check p is correct
		free(p->second);
		services.erase(serviceName);
	}
}

fd_set ServiceHandler::getAllSockets(){
	return master_fds;
}

int ServiceHandler::getMaxFD(){
	return maxfd;
}

int ServiceHandler::merge_fdset(int nfd, fd_set *set){
	int maxfd = nfd;
	
	ServiceMap::iterator p;
	for(p = services.begin(); p != services.end(); p++){
		int sock = p->second->client->sock;
        FD_SET(sock, set);
		if(maxfd < sock)
			maxfd = sock;
    }

	return maxfd;
}

ServiceDetails* ServiceHandler::getService(fd_set *set){
	ServiceMap::iterator p;
	for(p = services.begin(); p != services.end(); p++){
		int sock = p->second->client->sock;
        if(FD_ISSET(sock, set))
			return p->second;
    }
	
	return NULL;
}

bool ServiceHandler::hasService(const char* serviceName){
	ServiceMap::iterator p;

	p = services.find(serviceName);
	if(p != services.end()){
		return true;
	}
	
	return false;
}

ServiceDetails* ServiceHandler::getServiceDetails(const char* serviceName){
	ServiceMap::iterator p;

	p = services.find(serviceName);
	if(p != services.end()){
		return p->second;
	}
	
	return NULL;
}

ServiceDetails* ServiceHandler::getCurrentService(){
	if(p != services.end()){
		return p->second;
	}
	
	return NULL;
}

//Return NULL if service reaches end OR there is no services
ServiceDetails* ServiceHandler::getNextService(){
	if(p != services.end())
		p++;
	else
		p = services.begin();
	
	if(p != services.end())
		return p->second;
	
	return NULL;
}

ServiceMap::iterator ServiceHandler::end(){
	return services.end();
}

ServiceMap::iterator ServiceHandler::begin(){
	return services.begin();
}
