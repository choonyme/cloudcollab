## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES =\
	 -I../include -I../../LibVNCServer-0.8.2

AM_CFLAGS =\
	 -Wall\
	 -g

AM_CXXFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = drawtop

drawtop_SOURCES = \
	main.cc\
	drawtop.cc\
	drawtop_bonjour.cc\
	vpclient.cc

drawtop_LDFLAGS = \
	 -L../../LibVNCServer-0.8.2/libvncserver -L../../LibVNCServer-0.8.2/libvncclient

drawtop_LDADD = \
	 -lvncserver -lvncclient -ldns_sd -ljpeg -lz -lpthread

