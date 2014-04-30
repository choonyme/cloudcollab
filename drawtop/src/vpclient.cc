/***************************************************************************
 *            vpclient.cc
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
 
#include "vpclient.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <map>

//Bonjour socket reference
static int dns_sd_fd;
static DNSServiceRef serviceRef;

static rfbBool malloc_frame_buffer(private_resource_t *res, int w, int h)
{
    if(!res->server) {
		res->server=rfbGetScreen(NULL, NULL, w, h, 8, 3, 4);
		res->server->screenData=&res;
		res->server->port=res->listen_port;
		res->server->frameBuffer = (char*) malloc(w*4*h);
		
		// Multiple clients
		res->server->alwaysShared = TRUE;
		res->server->newClientHook = res->new_client;

		rfbInitServer(res->server);
    }
}

/* init/shutdown functions */
resource_t initvnc(private_resource_t *resource, int width, int height, int overlay)
{
	malloc_frame_buffer(resource, width, height); /* INDICATE FRAME BUFFER SIZE!!! */
	resource->init_screen();
	
	DNSServiceErrorType error = DNSServiceRegister(&serviceRef, resource->listen_port, overlay);
	if(error == kDNSServiceErr_NoError){
		dns_sd_fd = DNSServiceRefSockFD(serviceRef);
	}
	else{
		fprintf(stderr, "DNSServiceDiscovery returned %d\n", error);
		return -1;
	}

    return 0;
}

void closevnc(private_resource_t *resource)
{
    if(resource->server)
		rfbScreenCleanup(resource->server);
	DNSServiceDeregister(serviceRef);
	fprintf(stderr, "Resources cleaned\n");
}

/* this function returns only if res.result in return_mask */
static result_t
private_process(private_resource_t *resource,
		timeout_t timeout_in_seconds,
		result_t return_mask)
{
    fd_set fds;
    struct timeval tv,tv_start,tv_end;
    unsigned long timeout = (unsigned long)(timeout_in_seconds*1000000);
    int count,max_fd;

    gettimeofday(&tv_start,NULL);
    resource->result=0;

    do
    {
	unsigned long timeout_done;

	if(resource->server)
	{
	    rfbBool loop;

	    do
	    {
		loop=rfbProcessEvents(resource->server,
				      resource->server->deferUpdateTime);
	    }
	    while(loop
		  && (resource->result&return_mask) == 0
		  && rfbIsActive(resource->server));
	    
	    if(!rfbIsActive(resource->server))
		return RESULT_SHUTDOWN;
	    
	    if((resource->result&return_mask)!=0)
		return resource->result;

	    memcpy((char*)&fds,
		   (const char*)&(resource->server->allFds),
		   sizeof(fd_set));
	    
	    max_fd=resource->server->maxFd;
	}
	else
	{
	    FD_ZERO(&fds);
	    max_fd=0;
	}
	
	FD_SET(dns_sd_fd, &fds);
	if(dns_sd_fd > max_fd)
		max_fd = dns_sd_fd;
	
	gettimeofday(&tv_end,NULL);
	
	timeout_done =
	    tv_end.tv_usec-tv_start.tv_usec
	    + 1000000L*(tv_end.tv_sec-tv_start.tv_sec);

	if(timeout_done>=timeout)
	    return RESULT_TIMEOUT;

	tv.tv_usec=((timeout-timeout_done)%1000000);
	tv.tv_sec=(timeout-timeout_done)/1000000;

	count=select(max_fd+1,&fds,NULL,NULL,&tv);
	
	if(count<0)
	    return 0;
	
	if(count>0)
	{
		DNSServiceErrorType err = kDNSServiceErr_NoError;
		if(FD_ISSET(dns_sd_fd, &fds))
			err = DNSServiceProcessResult(serviceRef);
		if(err)
			return 0;
	}
	else
	{
	    resource->result|=RESULT_TIMEOUT;
	    
	    return resource->result;
	}
    }
    while(1);

    return RESULT_TIMEOUT;
}

result_t process(private_resource_t *resource, timeout_t timeout)
{
    return private_process(resource, timeout,RESULT_TIMEOUT);
}

// "safe" set pixel function (with bounds check)
void set_pixel(private_resource_t *res, int x, int y, unsigned char* colour){
	unsigned char* buffer;
	int width, height;
	int bpp;
	
	buffer = (unsigned char*) res->server->frameBuffer;
	width = res->server->width;
	height = res->server->height;
	bpp = res->server->depth / 8;
	
	if ((x < width) && (y < height))
    {
		buffer[(y * width + x) * bpp] = colour[0]; //only set red channel atm
		buffer[(y * width + x) * bpp + 1] = colour[1]; //only set red channel atm
		buffer[(y * width + x) * bpp + 2] = colour[2]; //only set red channel atm
		buffer[(y * width + x) * bpp + 3] = colour[3]; //only set red channel atm
    }
}

void fill_screen(private_resource_t *res, unsigned int value){
	memset(res->server->frameBuffer,
			   value,
			   (res->server->depth /8) *
			   res->server->width * res->server->height);
}
