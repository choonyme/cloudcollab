#!/bin/bash
LibVNCServer-0.8.2/configure
make -C LibVNCServer-0.8.2/libvncclient
make -C LibVNCServer-0.8.2/libvncserver
vncproxy/autogen.sh
make -C vncproxy
drawtop/autogen.sh
make -C drawtop
