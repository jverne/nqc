# Makefile for nqc
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# The Initial Developer of this code is David Baum.
# Portions created by David Baum are Copyright (C) 1999 David Baum.
# All Rights Reserved.
#
# Portions created by John Hansen are Copyright (C) 2005 John Hansen.
# All Rights Reserved.
#
#   original author:
#		Dave Baum <dbaum@enteract.com>
#
#   other contributors:
#		Rodd Zurcher <rbz@enteract.com>
#		Patrick Nadeau <pnadeau@wave.home.com>
#		Jonathan W. Miner <jminer@fcmail.com>
#
#
.SUFFIXES: .cpp .c

#
# Pick your C++ compiler.
#
CXX ?= ${CXX}
# CXX ?= g++

#
# Pick your YACC processor
#
YACC ?= bison -y
# YACC ?= yacc

#
# Define the FLEX processor
# NOTE: lex will not work
#
FLEX ?= flex -Cfe

#
# Link in any necessary C++ libraries
#
LIBS ?= -lstdc++

# installation information
PREFIX ?= /usr/local
BINDIR ?= ${PREFIX}/bin
MANDIR ?= ${PREFIX}/man/man1
MANEXT ?= 1

# other commands
CP ?= cp -f
MKDIR ?= mkdir
MV ?= mv -f
RM ?= rm -f
DOXYGEN ?= doxygen

# Common local include directories
INCLUDE_DIRS = platform rcxlib nqc compiler
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

# Common compiler flags
CFLAGS += $(INCLUDES) -Wall

# Default to NO USB tower support. USB support can be enabled
# via the platform-specific settings, below.
USBOBJ = RCX_USBTowerPipe_none
TCPOBJ = RCX_TcpPipe_none

#
# Platform specific settings
#
OSTYPE := $(strip $(shell uname -s))

ifneq (,$(strip $(findstring $(OSTYPE), Darwin)))
	# Mac OS X
	LIBS += -framework IOKit -framework CoreFoundation
  	USBOBJ = RCX_USBTowerPipe_osx
  	CXX = c++
  	CFLAGS += -O3 -std=c++11 -Wno-deprecated-register
else
ifneq (,$(strip $(findstring $(OSTYPE), Linux)))
	# Linux
	USBOBJ = RCX_USBTowerPipe_linux
	TCPOBJ = rcxlib/RCX_TcpPipe_linux
	DEFAULT_SERIAL_NAME ?= "/dev/ttyS0"
	# Timeout value is 200 in kernel driver module legousbtower.c
	LEGO_TOWER_SET_READ_TIMEOUT?= 200
	CFLAGS += -DLEGO_TOWER_SET_READ_TIMEOUT='$(LEGO_TOWER_SET_READ_TIMEOUT)' -Wno-deprecated
else
ifneq (,$(findstring $(OSTYPE), SunOS))
  	# Solaris
  	CFLAGS += -DSOLARIS
else
ifneq (,$(strip $(findstring $(OSTYPE), FreeBSD)))
  	# FreeBSD
  	USBOBJ = RCX_USBTowerPipe_fbsd
  	DEFAULT_SERIAL_NAME?= "/dev/cuad0"
  	CFLAGS += -Wno-deprecated
else
ifneq (,$(strip $(findstring $(OSTYPE), OpenBSD)))
  	# OpenBSD i386
  	DEFAULT_SERIAL_NAME ?= "/dev/cua00"
  	CFLAGS += -O2 -std=gnu++98 -pipe
else
  	# default Unix build without USB support
  	CFLAGS += -O2
endif
endif
endif
endif
endif

CXX:=$(TOOLPREFIX)$(CXX)

#
# If the serial port is explicitly set, use it.
#
ifneq ($(strip $(DEFAULT_SERIAL_NAME)),)
	CFLAGS += -DDEFAULT_SERIAL_NAME='$(DEFAULT_SERIAL_NAME)'
endif

DEFAULT_USB_NAME ?= "/dev/usb/legousbtower0"
CFLAGS += -DDEFAULT_USB_NAME='$(DEFAULT_USB_NAME)'

DEFAULT_DEVICE_NAME ?= "usb"
CFLAGS += -DDEFAULT_DEVICE_NAME='$(DEFAULT_DEVICE_NAME)'

#
# Debug builds for most Clang/GCC environments.
# This implies -DDEBUG_TIMEOUT
#
#CFLAGS += -DDEBUG -DPDEBUG -g -O0

#
# Object files
#
OBJ = $(NQCOBJ) $(COBJ) $(RCXOBJ) $(POBJ)

RCXOBJS = RCX_Cmd RCX_Disasm RCX_Image RCX_Link RCX_Log \
	RCX_Target RCX_Pipe RCX_PipeTransport RCX_Transport \
	RCX_SpyboticsLinker RCX_SerialPipe \
	$(USBOBJ) $(TCPOBJ)
RCXOBJ = $(addprefix rcxlib/, $(addsuffix .o, $(RCXOBJS)))

POBJS = PStream PSerial_unix PHashTable PListS PDebug
POBJ = $(addprefix platform/, $(addsuffix .o, $(POBJS)))

COBJS = AsmStmt AssignStmt BlockStmt Bytecode Conditional \
	CondParser DoStmt Expansion Fragment IfStmt JumpStmt \
	lexer Macro parse PreProc Program RepeatStmt \
	AssignMathStmt Stmt Symbol TaskStmt WhileStmt Error \
	AutoFree parse_util Expr GosubStmt Scope InlineStmt \
	CallStmt CaseStmt SwitchStmt MonitorStmt AcquireStmt \
	DeclareStmt ScopeStmt ForStmt CatchStmt LabelStmt \
	GotoStmt Compiler Buffer Mapping FunctionDef ExprStmt \
	AtomExpr IncDecExpr BinaryExpr UnaryExpr ValueExpr \
	TypeExpr ModExpr EventSrcExpr SensorExpr ArrayExpr \
	TaskIdExpr RelExpr LogicalExpr NegateExpr IndirectExpr \
	NodeExpr ShiftExpr TernaryExpr VarAllocator VarTranslator \
	Resource AddrOfExpr DerefExpr GosubParamStmt
COBJ = $(addprefix compiler/, $(addsuffix .o, $(COBJS)))

NQCOBJS = nqc SRecord DirList CmdLine
NQCOBJ = $(addprefix nqc/, $(addsuffix .o, $(NQCOBJS)))

# We need a 'bin' directory because the names of the binaries clash
# with existing directory names.
all : bin nqh nub bin/nqc

# Create the bin directory in the Makefile because it is not part
# of the original distribution.  This prevents the need to tell the user
# to do it.
bin:
	$(MKDIR) bin

bin/nqc: compiler/parse.cpp $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS)

bin/mkdata: mkdata/mkdata.cpp nqc/SRecord.cpp
	$(CXX) -o $@ $(INCLUDES) $^

#
# clean up stuff
#
clean: clean-parser clean-lexer clean-obj clean-nqh clean-nub

clean-obj:
	-$(RM) bin/*
	-$(RM) */*.o

clean-parser:
	-$(RM) compiler/parse.cpp compiler/parse.tab.h

clean-lexer:
	-$(RM) compiler/lexer.cpp

clean-nqh:
	-$(RM) compiler/rcx1_nqh.h compiler/rcx2_nqh.h

clean-nub:
	-$(RM) rcxlib/rcxnub.h

#
# create the parser files (parse.cpp and parse.tab.h)
#
compiler/parse.cpp: compiler/parse.y
	cd compiler ; \
	$(YACC) -d parse.y ; \
	$(MV) y.tab.c parse.cpp ; \
	$(MV) y.tab.h parse.tab.h

#
# create the lexer file (lexer.cpp)
#
compiler/lexer.cpp: compiler/lex.l
	cd compiler ; \
	$(FLEX) lex.l

#
# NQH files
#
nqh: compiler/rcx1_nqh.h compiler/rcx2_nqh.h

compiler/rcx1_nqh.h: compiler/rcx1.nqh bin/mkdata
	bin/mkdata $< $@ rcx1_nqh

compiler/rcx2_nqh.h: compiler/rcx2.nqh bin/mkdata
	bin/mkdata $< $@ rcx2_nqh

#
# rcxnub.h
#
nub: rcxlib/rcxnub.h

rcxlib/rcxnub.h: rcxlib/fastdl.srec bin/mkdata
	bin/mkdata -s $< $@ rcxnub

#
# general rule for compiling
#
.cpp.o:
	$(CXX) -c $(CFLAGS) $< -o $*.o

#
# Use these targets to use the default parser/lexer files.  This is handy if
# your system does not have a suitable flex and/or yacc tool.
#
default-parser:
	$(CP) default/parse.cpp default/parse.tab.h nqc

default-lexer:
	$(CP) default/lexer.cpp nqc

#
# This is used to create a default parser, lexer, and nqh files for later use.
# You shouldn't need to do this as part of a port.
#
DEF_FILES = compiler/parse.cpp compiler/lexer.cpp compiler/rcx1_nqh.h compiler/rcx2_nqh.h
default-snapshot: default $(DEF_FILES)
	$(CP) $(DEF_FILES) compiler/parse.tab.h default

default:
	$(MKDIR) default

#
# Generate API docs. Not part of a port.
#
docs:
	@$(DOXYGEN) Doxyfile

#
# Installation of binary and man page
#
install: all
	test -d $(DESTDIR)$(BINDIR) || mkdir -p $(DESTDIR)$(BINDIR)
	cp -r bin/* $(DESTDIR)$(BINDIR)
	test -d $(DESTDIR)$(MANDIR)  || mkdir -p $(DESTDIR)$(MANDIR)
	cp nqc-man-2.1r1-0.man $(DESTDIR)$(MANDIR)/nqc.$(MANEXT)

#
# Print some info about the environment
#
info:
	@echo "Building for: $(OSTYPE)"
	@echo CXX=$(CXX)
	@echo CFLAGS=$(CFLAGS)
	@echo YACC=$(YACC)
	@echo FLEX=$(FLEX)
	@echo LIBS=$(LIBS)
	@echo OBJ=$(OBJ)
	@echo USBOBJ=$(USBOBJ)
	@echo PREFIX=$(PREFIX)
