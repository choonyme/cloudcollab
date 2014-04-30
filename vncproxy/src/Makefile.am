## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES =\
	$(PACKAGE_CFLAGS)\
	 -I../include -I../../LibVNCServer-0.8.2

AM_CFLAGS =\
	 -Wall\
	 -g

AM_CXXFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = vncproxy

vncproxy_SOURCES = \
	main.cc\
	bonjour_browser.cc\
	vncproxy.cc\
	service_handler.cc\
	UIManager.cc\
	wndMain.cc\
	wndMain_glade.cc\
	dialogPassword.cc\
	dialogPassword_glade.cc\
	pointer.cc                               

vncproxy_LDFLAGS = \
	 -L../../LibVNCServer-0.8.2/libvncserver -L../../LibVNCServer-0.8.2/libvncclient

vncproxy_LDADD = \
	$(PACKAGE_LIBS)\
	 -ljpeg -lvncserver -lvncclient -lz -lpthread -ldns_sd

