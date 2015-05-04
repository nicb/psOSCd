#
# $Id: Makefile 39 2010-11-02 23:42:57Z nicb $
#
# if you need new architectures just add files in the arch directory
#
DEBUG=0
ARCHDIR=arch
ARCH=$(shell uname -m)
include $(ARCHDIR)/rules.$(ARCH)
#
# nothing should be touched below here
#
TARGET=psOSCd$(TARGET_SUFFIX)
CC=gcc
INCLUDES=-I. -I$(PHASESPACE_DIR)/include
CFLAGS=-Wall $(DEBUG_OPTION) $(OPTIM) $(INCLUDES) $(PHASESPACE_MODE) $(PHASESPACE_DEBUG)
PHASESPACE_DIR=../../../vendor/phasespace-$(TYPE)
PHASESPACE_LIBDIR=$(PHASESPACE_DIR)
PHASESPACE_SRCDIR=$(PHASESPACE_DIR)/src
PHASESPACE_DEBUG=-DPHASESPACE_DEBUG=$(DEBUG)
PHASESPACE_MODE=-DPS_SIMULATION_MODE=$(SIMULATION_MODE)
PHASESPACE_LIB=libowlsock.$(LIB_SUFFIX)
LDFLAGS=-L$(PHASESPACE_LIBDIR)
LIBS=-lowlsock -llo -lconfig -lm
OBJS=main.o options.o errors.o daemon.o log.o loop.o phasespace.o system.o\
     simulation.o osc.o config.o osc_services.o osc_reply_to.o osc_query_markers.o \
		 phasespace_markers.o phasespace_rigids.o config_rigids.o safe_malloc.o signals.o
HEADERS=psOSCd.h phasespace.h osc.h
DEBUG_OPTION=-g
OPTIM=#-O3
INSTALL=install
INSTALL_BINDIR=/usr/bin
INSTALL_LIBDIR=/usr/lib
DEBIAN_BUILDER=dpkg-buildpackage


all: $(TARGET)

$(TARGET): 	$(OBJS) $(PHASESPACE_LIB)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): $(HEADERS)

clean:
		$(RM) *.o $(TARGET) $(TARGET)-* *-stamp
		$(MAKE) -C $(PHASESPACE_SRCDIR) -$(MAKEFLAGS) $@

$(PHASESPACE_LIB): simulation_lib

simulation_lib:
	$(MAKE) -C $(PHASESPACE_SRCDIR) -$(MAKEFLAGS) all

install:
	$(INSTALL) -m 755 -d $(DESTDIR)$(INSTALL_BINDIR) $(DESTDIR)$(INSTALL_LIBDIR)
	$(INSTALL) -s -m 644 $(PHASESPACE_LIBDIR)/$(PHASESPACE_LIB) $(DESTDIR)$(INSTALL_LIBDIR)/$(PHASESPACE_LIB)
	$(INSTALL) -s -m 755 $(TARGET) $(DESTDIR)$(INSTALL_BINDIR)/$(TARGET)

dist:
	$(DEBIAN_BUILDER) -rfakeroot
