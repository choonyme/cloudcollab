#ifndef RFBCLIENT_H
#define RFBCLIENT_H

/*
 *  Copyright (C) 2000, 2001 Const Kaplinsky.  All Rights Reserved.
 *  Copyright (C) 2000 Tridia Corporation.  All Rights Reserved.
 *  Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */

/*
 * vncviewer.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <rfb/rfbproto.h>
#include <rfb/keysym.h>

#define rfbClientSwap16IfLE(s) \
    (*(char *)&client->endianTest ? ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)) : (s))

#define rfbClientSwap32IfLE(l) \
    (*(char *)&client->endianTest ? ((((l) & 0xff000000) >> 24) | \
			     (((l) & 0x00ff0000) >> 8)  | \
			     (((l) & 0x0000ff00) << 8)  | \
			     (((l) & 0x000000ff) << 24))  : (l))

#define FLASH_PORT_OFFSET 5400
#define LISTEN_PORT_OFFSET 5500
#define TUNNEL_PORT_OFFSET 5500
#define SERVER_PORT_OFFSET 5900

#define DEFAULT_SSH_CMD "/usr/bin/ssh"
#define DEFAULT_TUNNEL_CMD  \
  (DEFAULT_SSH_CMD " -f -L %L:localhost:%R %H sleep 20")
#define DEFAULT_VIA_CMD     \
  (DEFAULT_SSH_CMD " -f -L %L:%H:%R %G sleep 20")

/* vncrec */

typedef struct {
  FILE* file;
  struct timeval tv;
  rfbBool readTimestamp;
  rfbBool doNotSleep;
} rfbVNCRec;

/* client data */

typedef struct rfbClientData {
	void* tag;
	void* data;
	struct rfbClientData* next;
} rfbClientData;

/* app data (belongs into rfbClient?) */

typedef struct {
  rfbBool shareDesktop;
  rfbBool viewOnly;

  const char* encodingsString;

  rfbBool useBGR233;
  int nColours;
  rfbBool forceOwnCmap;
  rfbBool forceTrueColour;
  int requestedDepth;

  int compressLevel;
  int qualityLevel;
  rfbBool enableJPEG;
  rfbBool useRemoteCursor;
  rfbBool palmVNC;  /* use palmvnc specific SetScale (vs ultravnc) */
  int scaleSetting; /* 0 means no scale set, else 1/scaleSetting */
} AppData;


struct _rfbClient;

typedef void (*HandleTextChatProc)(struct _rfbClient* client, int value, char *text);
typedef void (*HandleKeyboardLedStateProc)(struct _rfbClient* client, int value, int pad);
typedef rfbBool (*HandleCursorPosProc)(struct _rfbClient* client, int x, int y);
typedef void (*SoftCursorLockAreaProc)(struct _rfbClient* client, int x, int y, int w, int h);
typedef void (*SoftCursorUnlockScreenProc)(struct _rfbClient* client);
typedef void (*GotFrameBufferUpdateProc)(struct _rfbClient* client, int x, int y, int w, int h);
typedef char* (*GetPasswordProc)(struct _rfbClient* client);
typedef rfbBool (*MallocFrameBufferProc)(struct _rfbClient* client);
typedef void (*BellProc)(struct _rfbClient* client);

typedef struct _rfbClient {
	uint8_t* frameBuffer;
	int width, height;

	int endianTest;

	AppData appData;

	const char* programName;
	char* serverHost;
	int serverPort; /* if -1, then use file recorded by vncrec */
	rfbBool listenSpecified;
	int listenPort, flashPort;

	/* Note that the CoRRE encoding uses this buffer and assumes it is big enough
	   to hold 255 * 255 * 32 bits -> 260100 bytes.  640*480 = 307200 bytes.
	   Hextile also assumes it is big enough to hold 16 * 16 * 32 bits.
	   Tight encoding assumes BUFFER_SIZE is at least 16384 bytes. */

#define RFB_BUFFER_SIZE (640*480)
	char buffer[RFB_BUFFER_SIZE];

	/* rfbproto.c */

	int sock;
	rfbBool canUseCoRRE;
	rfbBool canUseHextile;
	char *desktopName;
	rfbPixelFormat format;
	rfbServerInitMsg si;
	char *serverCutText;
	rfbBool newServerCutText;

	/* sockets.c */
#define RFB_BUF_SIZE 8192
	char buf[RFB_BUF_SIZE];
	char *bufoutptr;
	int buffered;

	/* The zlib encoding requires expansion/decompression/deflation of the
	   compressed data in the "buffer" above into another, result buffer.
	   However, the size of the result buffer can be determined precisely
	   based on the bitsPerPixel, height and width of the rectangle.  We
	   allocate this buffer one time to be the full size of the buffer. */

	/* Ultra Encoding uses this buffer too */
	
	int ultra_buffer_size;
	char *ultra_buffer;

	int raw_buffer_size;
	char *raw_buffer;

#ifdef LIBVNCSERVER_HAVE_LIBZ
	z_stream decompStream;
	rfbBool decompStreamInited;
#endif


#ifdef LIBVNCSERVER_HAVE_LIBZ
#ifdef LIBVNCSERVER_HAVE_LIBJPEG
	/*
	 * Variables for the ``tight'' encoding implementation.
	 */

	/* Separate buffer for compressed data. */
#define ZLIB_BUFFER_SIZE 30000
	char zlib_buffer[ZLIB_BUFFER_SIZE];

	/* Four independent compression streams for zlib library. */
	z_stream zlibStream[4];
	rfbBool zlibStreamActive[4];

	/* Filter stuff. Should be initialized by filter initialization code. */
	rfbBool cutZeros;
	int rectWidth, rectColors;
	char tightPalette[256*4];
	uint8_t tightPrevRow[2048*3*sizeof(uint16_t)];

	/* JPEG decoder state. */
	rfbBool jpegError;

	struct jpeg_source_mgr* jpegSrcManager;
	void* jpegBufferPtr;
	size_t jpegBufferLen;

#endif
#endif


	/* cursor.c */
	uint8_t *rcSource, *rcMask;

	/* private data pointer */
	rfbClientData* clientData;

	rfbVNCRec* vncRec;

	/* Keyboard State support (is 'Caps Lock' set on the remote display???) */
	int KeyboardLedStateEnabled;
	int CurrentKeyboardLedState;

	int canHandleNewFBSize;

	/* hooks */
	HandleTextChatProc         HandleTextChat;
	HandleKeyboardLedStateProc HandleKeyboardLedState;
	HandleCursorPosProc HandleCursorPos;
	SoftCursorLockAreaProc SoftCursorLockArea;
	SoftCursorUnlockScreenProc SoftCursorUnlockScreen;
	GotFrameBufferUpdateProc GotFrameBufferUpdate;
	/* the pointer returned by GetPassword will be freed after use! */
	GetPasswordProc GetPassword;
	MallocFrameBufferProc MallocFrameBuffer;
	BellProc Bell;

	/* Which messages are supported by the server
	 * This is a *guess* for most servers.
	 * (If we can even detect the type of server)
	 *
	 * If the server supports the "rfbEncodingSupportedMessages"
	 * then this will be updated when the encoding is received to
	 * accurately reflect the servers capabilities.
	 */
	rfbSupportedMessages supportedMessages;

	/* negotiated protocol version */
	int major, minor;

	void* genericData;
} rfbClient;

/* cursor.c */

extern rfbBool HandleCursorShape(rfbClient* client,int xhot, int yhot, int width, int height, uint32_t enc);

/* listen.c */

extern void listenForIncomingConnections(rfbClient* viewer);

/* rfbproto.c */

extern rfbBool rfbEnableClientLogging;
typedef void (*rfbClientLogProc)(const char *format, ...);
extern rfbClientLogProc rfbClientLog,rfbClientErr;
extern rfbBool ConnectToRFBServer(rfbClient* client,const char *hostname, int port);
extern rfbBool InitialiseRFBConnection(rfbClient* client);
extern rfbBool SetFormatAndEncodings(rfbClient* client);
extern rfbBool SendIncrementalFramebufferUpdateRequest(rfbClient* client);
extern rfbBool SendFramebufferUpdateRequest(rfbClient* client,
					 int x, int y, int w, int h,
					 rfbBool incremental);
extern rfbBool SendScaleSetting(rfbClient* client,int scaleSetting);
extern rfbBool SendPointerEvent(rfbClient* client,int x, int y, int buttonMask, int id);
extern rfbBool SendKeyEvent(rfbClient* client,uint32_t key, rfbBool down, int id);
extern rfbBool SendClientCutText(rfbClient* client,char *str, int len);
extern rfbBool HandleRFBServerMessage(rfbClient* client);

extern rfbBool TextChatSend(rfbClient* client, char *text);
extern rfbBool TextChatOpen(rfbClient* client);
extern rfbBool TextChatClose(rfbClient* client);
extern rfbBool TextChatFinish(rfbClient* client);
extern rfbBool PermitServerInput(rfbClient* client, int enabled);

extern void PrintPixelFormat(rfbPixelFormat *format);

/* client data */

void rfbClientSetClientData(rfbClient* client, void* tag, void* data);
void* rfbClientGetClientData(rfbClient* client, void* tag);

/* protocol extensions */

typedef struct _rfbClientProtocolExtension {
	int* encodings;
	/* returns TRUE if the encoding was handled */
	rfbBool (*handleEncoding)(rfbClient* cl,
		rfbFramebufferUpdateRectHeader* rect);
	/* returns TRUE if it handled the message */
	rfbBool (*handleMessage)(rfbClient* cl,
		 rfbServerToClientMsg* message);
	struct _rfbClientProtocolExtension* next;
} rfbClientProtocolExtension;

void rfbClientRegisterExtension(rfbClientProtocolExtension* e);

/* sockets.c */

extern rfbBool errorMessageOnReadFailure;

extern rfbBool ReadFromRFBServer(rfbClient* client, char *out, unsigned int n);
extern rfbBool WriteToRFBServer(rfbClient* client, char *buf, int n);
extern int FindFreeTcpPort(void);
extern int ListenAtTcpPort(int port);
extern int ConnectClientToTcpAddr(unsigned int host, int port);
extern int AcceptTcpConnection(int listenSock);
extern rfbBool SetNonBlocking(int sock);

extern rfbBool StringToIPAddr(const char *str, unsigned int *addr);
extern rfbBool SameMachine(int sock);
extern int WaitForMessage(rfbClient* client,unsigned int usecs);

/* vncviewer.c */
rfbClient* rfbGetClient(int bitsPerSample,int samplesPerPixel,int bytesPerPixel);
rfbBool rfbInitClient(rfbClient* client,int* argc,char** argv);
/* rfbClientCleanup() does not touch client->frameBuffer */
void rfbClientCleanup(rfbClient* client);

#endif

