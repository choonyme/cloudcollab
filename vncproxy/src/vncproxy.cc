/***************************************************************************
 *            vncproxy.cc
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
 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <pthread.h>
#include <termios.h>
#include "UIManager.h"
#include "vncproxy.h"
#include "bonjour_browser.h"
#include "service_handler.h"
#include "vp.h"


/* this is a VNC connection */
typedef struct private_resource_t private_resource_t;

struct private_resource_t
{
    int listen_port;
    rfbScreenInfo* server; //server clients
    rfbClient* client; //connect to server
	rfbClient* activeClient;
	int floorClient;
	
    result_t result;
};

static private_resource_t res;

typedef struct MainLoopArgs MainLoopArgs;
	
struct MainLoopArgs{
	resource_t res;
	timeout_t timeout;
};

//User Interface Manager for managing framebuffers
static UIManager *uiManager;

//service handler for handling fds
ServiceHandler serviceHandler;

// hooks

static void got_key(rfbBool down,rfbKeySym keysym, int id, rfbClientRec* cl)
{
    res.result|=RESULT_KEY;

	if (down && (keysym == XK_F2)){
		rfbClient* rc = uiManager->switchControlToOverlay();
		if( rc != NULL )
			res.activeClient = rc;
	}else if (down && (keysym == XK_F3)){
		printf("F3 detected, moving window\n");
		ClientData* cd;
		cd = (ClientData*) cl->clientData;
		uiManager->moveWindow(res.activeClient, cd->old_x, cd->old_y);
		rfbMarkRectAsModified(res.server, 0, 0, res.server->width, res.server->height);
    }else if (down && (keysym == XK_F4)){
		uiManager->hideWindow(res.activeClient);
		rfbMarkRectAsModified(res.server, 0, 0, res.server->width, res.server->height);
	}else if (down && (keysym == XK_F5)){
		uiManager->showAllWindows();
		rfbMarkRectAsModified(res.server,0 ,0 ,res.server->width,res.server->height);
	}else if (down && (keysym == XK_F6)){
		res.floorClient = ((ClientData*)(cl->clientData))->id;
	}else{
		SendKeyEvent(res.activeClient, keysym, down, ((ClientData*)(cl->clientData))->id);
	}

}

static void got_mouse(int button_mask,int x,int y,int id,rfbClientRec* cl)
{
    ClientData* cd;
	rfbClient *client;
    cd = (ClientData*) cl->clientData;

    res.result |= RESULT_MOUSE;
	client = uiManager->activeWindowAt(x,y);
	uiManager->updatePointer(cd->id, x, y);
	
	cd->old_x = x;
    cd->old_y = y;
    cd->old_button = button_mask;
	
	if(client != NULL){
		if( client == res.client && cd->id != res.floorClient)
				return;
		
		SendPointerEvent(client,x - ((WindowData*) client->genericData)->x_pos,y - ((WindowData*) client->genericData)->y_pos,button_mask, cl->sock);
		if(button_mask && ((WindowData*) client->genericData)->attribute != WindowData::OVERLAY){
			res.activeClient = client;
		}
	}
}

//for VNC Server
static void
clientgone(rfbClientPtr cl)
{
	if(res.activeClient != res.client)
		SendPointerEvent(res.activeClient,0,0, 255, ((ClientData*)(cl->clientData))->id); //Should send to all services, not just activeClient
	serviceHandler.removeSocket(((ClientData*)(cl->clientData))->id);
    free(cl->clientData);
}

//for VNC Server
static enum rfbNewClientAction
new_client(rfbClientPtr cl)
{
    cl->clientData = (ClientData*) malloc(sizeof(ClientData));
	((ClientData*)(cl->clientData))->id = cl->sock;
	
	//Add newly connected client's socket number to master_fds
	serviceHandler.addSocket(cl->sock);
	
    cl->clientGoneHook = clientgone;
    cl->ptrAddEvent = got_mouse;
	cl->kbdAddEvent = got_key;

    return RFB_CLIENT_ACCEPT;
}

//for VNC Client
static rfbBool malloc_frame_buffer(rfbClient* cl)
{
	int w=cl->width,h=cl->height;
	
	cl->frameBuffer = (uint8_t*) malloc(w * 4 * h);
	
    if(!res.server){		
		res.server = rfbGetScreen(NULL, NULL, w, h, 8, 3, 4);
		res.server->screenData = NULL;
		res.server->port = res.listen_port;
		res.server->frameBuffer = (char*) malloc(w*4*h);
		res.server->kbdAddEvent = got_key;
		res.server->ptrAddEvent = got_mouse;
		res.server->alwaysShared = TRUE;
		res.server->newClientHook = new_client;
	
		rfbInitServer(res.server);
		
		printf("Server framebuffer size %d %d\n", w, h);
    }
	
	return TRUE;
}

//for VNC client
static void got_frame_buffer(rfbClient* cl,int x,int y,int w,int h)
{
    assert(res.server);
    if(res.server)
    {
		int absposx = ((WindowData*) cl->genericData)->x_pos + x;
		int absposy = ((WindowData*) cl->genericData)->y_pos + y;
		
		if( ((WindowData*) cl->genericData)->attribute == WindowData::OVERLAY )
			uiManager->processOverlay(cl, x, y, w, h);
			
		uiManager->updateRegion(absposx, absposy, absposx + w, absposy + h);
		rfbMarkRectAsModified(res.server, absposx, absposy, absposx + w, absposy + h);
    }

    res.result |= RESULT_SCREEN;
}

resource_t initServiceDiscovery(){
	serviceHandler.setBrowser(MyDNSServiceBrowse());
	//Add Bonjour Browser socket fd to master_fds
	serviceHandler.addSocket(DNSServiceRefSockFD(serviceHandler.getBrowser()));
	
	return 0;
}

resource_t initvnc(const char* server,int server_port, int listen_port)
{
    int dummy=0;

    res.listen_port = listen_port;
    res.client = rfbGetClient(8,3,4);	
    res.client->GotFrameBufferUpdate = got_frame_buffer;
    res.client->MallocFrameBuffer = malloc_frame_buffer;
    res.client->serverHost = strdup(server);
    res.client->serverPort = server_port;
    res.client->appData.encodingsString = "raw";
	res.client->GetPassword = inputPassword;
	
	res.floorClient = -1;
	
    if(!rfbInitClient(res.client,&dummy,NULL)){
		res.client = NULL;
		return -1;
    }
	
	//Add VNC Desktop socket to master_fds
	serviceHandler.addSocket(res.client->sock);
	//Add VNC Server socket to master_fds
	serviceHandler.addSocket(res.server->listenSock);
	
	printf("VNC Desktop and VNC Server socket is %d %d.\n", res.client->sock, res.server->listenSock);
	
	uiManager = new UIManager(res.server);
	uiManager->createWindow(res.client, WindowData::DESKTOP);
	
	res.activeClient = res.client;
	
	int status = DNSServiceRegister(res.listen_port, res.server->width, res.server->height);
	if(!status)
		return -1;

    return 0;
}

void closevnc(resource_t resource)
{
    if(res.server)
		rfbScreenCleanup(res.server);

    assert(res.client);

    rfbClientCleanup(res.client);
    res.client=NULL;

	//Clean up registered service "vncproxy"
	DNSServiceDeregister();
	
	DNSServiceRefDeallocate(serviceHandler.getBrowser());
}

void connectService(const char *fullname, const char *ip, int port, uint16_t mode){
	fprintf(stdout, "Connecting service %s %s %d...\n", fullname, ip, port);
	
	int dummy=0;

	printf("Getting service map, finding for service %s\n", fullname);
	ServiceDetails *sd = serviceHandler.getServiceDetails(fullname);
	if(sd == NULL){
		printf("Can't find service\n");
		fflush(stdout);
	}else{
		sd->client = rfbGetClient(8,3,4);
		sd->client->GotFrameBufferUpdate = got_frame_buffer;
		sd->client->MallocFrameBuffer = malloc_frame_buffer;
		sd->client->serverHost=strdup(ip);
		sd->client->serverPort=port;
		sd->client->appData.encodingsString="raw";
		sd->client->GetPassword = inputPassword;
		
		printf("Initializing client...");
		if(!rfbInitClient(sd->client,&dummy,NULL)){
			sd->client = NULL;
			printf("rfbInitClient() failed\n");
		}
		printf("Done\n");
		
		//Add service's socket to master_fds
		serviceHandler.addSocket(sd->client->sock);

		printf("Created new client with address %p\n", sd->client);
		if(mode == SERVICE_OVERLAY)
			uiManager->createWindow(sd->client, WindowData::OVERLAY);
		else
			uiManager->createWindow(sd->client, WindowData::NORMAL);
	}
	
	printf("Data init finished\n");
}

void disconnectService(const char *serviceName){
	fprintf(stdout, "Disconnecting service %s...\n", serviceName);
	
	ServiceDetails *sd = serviceHandler.getServiceDetails(serviceName);
	
	if(sd == NULL){
		fprintf(stderr, "Error: Can't find service to disconnect.\n");
	}
	
	if(sd->client == res.activeClient){
		res.activeClient = res.client;
		SendFramebufferUpdateRequest(res.activeClient, 0, 0, res.activeClient->width, res.activeClient->height, FALSE);
	}
	
	printf("Removing service socket %d\n", sd->client->sock);
	//Remove service's socket from master_fds
	serviceHandler.removeSocket(sd->client->sock);
	
	uiManager->destroyWindow(sd->client);
	rfbClientCleanup(sd->client);
	serviceHandler.removeService(serviceName);
}

/* this function returns only if res.result in return_mask */
static result_t
private_process(resource_t resource,
		timeout_t timeout_in_seconds,
		result_t return_mask)
{
    fd_set fds;
    struct timeval tv,tv_start,tv_end;
	int count,max_fd;
    unsigned long timeout = (unsigned long)(timeout_in_seconds*1000000);
	
    gettimeofday(&tv_start,NULL);
    res.result=0;

    do{
		unsigned long timeout_done;
		
		if(res.server)
		{
			rfbBool loop;

			do
			{
				loop=rfbProcessEvents(res.server,
						  res.server->deferUpdateTime);
			}
			while(loop
			  && (res.result&return_mask) == 0
			  && rfbIsActive(res.server));
			
			if(!rfbIsActive(res.server))
				return RESULT_SHUTDOWN;
			
			if((res.result&return_mask)!=0)
				return res.result;
		}
		
		fds = serviceHandler.getAllSockets();
		max_fd = serviceHandler.getMaxFD();
	
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
			ServiceDetails *sd;
			
			//Desktop
			if(res.client){
				if(FD_ISSET(res.client->sock,&fds))
				{
					if(!HandleRFBServerMessage(res.client))
					{
						closevnc(resource);
						return 0;
					}
					
					if((res.result&return_mask)!=0)
						return res.result;
				}
			}
			
			//DNS-SD
			if(FD_ISSET(DNSServiceRefSockFD(serviceHandler.getBrowser()), &fds)){
				DNSServiceErrorType err = kDNSServiceErr_NoError;	
				err = DNSServiceProcessResult(serviceHandler.getBrowser());
				if(err)
					return 0;
			}
			
			//Services
			if( (sd = serviceHandler.getService(&fds)) != NULL ){
				HandleRFBServerMessage(sd->client);
			}
		}else{
			res.result |= RESULT_TIMEOUT;
			return res.result;
		}
    }while(1);

    return RESULT_TIMEOUT;
}

void *main_process(void *args)
{
	result_t result;
	resource_t res;
	timeout_t timeout;
	MainLoopArgs *mainLoopArgs = (MainLoopArgs *) args;
	
	res = mainLoopArgs->res;
	timeout = mainLoopArgs->timeout;
	
    for(;;)
    {
   	    result = private_process(res, timeout, RESULT_TIMEOUT);

		if (result == 0)
    	{
         	printf("Server dissapeared\n");
		}
    }
}

pthread_t process(resource_t res,timeout_t timeout)
{
	pthread_t pt_MainLoop;
	MainLoopArgs *mainLoopArgs = (MainLoopArgs*) malloc(sizeof(MainLoopArgs));
	mainLoopArgs->res = res;
	mainLoopArgs->timeout = timeout;

	pthread_create(&pt_MainLoop, NULL, main_process, mainLoopArgs);
	
	return pt_MainLoop;
}

void resolveService(const char *name, int interfaceIndex){
	serviceHandler.addService(name);
	MyDNSServiceResolve(name, interfaceIndex);
}
