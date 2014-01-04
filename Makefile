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
# CXX ?= gcc

#
# Pick your YACC processor
#
YACC ?= bison -y
# YACC ?= yacc

#
# Define the FLEX processor
#    note - lex will not work
#
FLEX ?= flex

# Link in any necessary C++ libraries
#
LIBS ?= -lstdc++



# installation information
PREFIX?=/usr/local
BINDIR?=${PREFIX}/bin
MANDIR?=${PREFIX}/man/man1
MANEXT?=1


# other commands
CP?=cp -f
MKDIR?=mkdir
MV?=mv -f
RM?=rm -f

# CFLAGS
#CFLAGS = -pipe -Iplatform -Ircxlib -Inqc -Icompiler -Wall -Wstrict-prototypes -Wmissing-prototypes
CFLAGS += -Iplatform -Ircxlib -Inqc -Icompiler -Wall

USBOBJ = rcxlib/RCX_USBTowerPipe_none.o


#
# Platform specific tweaks
#
OSTYPE := $(strip $(shell uname -s))

ifneq (,$(strip $(findstring $(OSTYPE), Darwin)))
  # Mac OSX
  LIBS += -framework IOKit -framework CoreFoundation
  USBOBJ = rcxlib/RCX_USBTowerPipe_osx.o
  CXX = c++
  CFLAGS += -O3
  #CFLAGS += -DDEBUG -g -O0
else
ifneq (,$(strip $(findstring $(OSTYPE), Linux)))
  # Linux
# uncomment this next line if you have the USB tower library installed
#  USBOBJ = rcxlib/RCX_USBTowerPipe_linux.o
  CFLAGS += -I/usr/local/include/LegoUSB -Wno-deprecated
else
ifneq (,$(findstring $(OSTYPE), SunOS))
  # Solaris
  CFLAGS += -DSOLARIS
else
ifneq (,$(strip $(findstring $(OSTYPE), FreeBSD)))
  # FreeBSD
  USBOBJ = rcxlib/RCX_USBTowerPipe_fbsd.o
  DEFAULT_SERIAL_NAME = "/dev/cuad0"
  CFLAGS += -Wno-deprecated
ifneq (,$(strip $(findstring $(OSTYPE), OpenBSD)))
  # OpenBSD i386 with COM1, no USB
  DEFAULT_SERIAL_NAME = "/dev/cua00"
  CFLAGS += -Wno-deprecated -O2
else
  # default Unix build without USB support
  CFLAGS += -O2
endif
endif
endif
endif
endif


# this must happen after the platform tweaks just in case the platform
# wants to redefine the default serial name
#
ifndef DEFAULT_SERIAL_NAME
  DEFAULT_SERIAL_NAME = "/dev/ttyS0"
endif
CFLAGS += -DDEFAULT_SERIAL_NAME='$(DEFAULT_SERIAL_NAME)'

#
# Object files
#
OBJ = $(NQCOBJ) $(COBJ) $(RCXOBJ) $(POBJ)

RCXOBJ= rcxlib/RCX_Cmd.o rcxlib/RCX_Disasm.o rcxlib/RCX_Image.o \
	rcxlib/RCX_Link.o rcxlib/RCX_Log.o rcxlib/RCX_Target.o \
	rcxlib/RCX_Pipe.o rcxlib/RCX_PipeTransport.o rcxlib/RCX_Transport.o \
	rcxlib/RCX_SpyboticsLinker.o rcxlib/RCX_SerialPipe.o \
	$(USBOBJ)

POBJ= platform/PStream.o platform/PSerial_unix.o \
	platform/PHashTable.o platform/PListS.o

COBJ=	compiler/AsmStmt.o compiler/AssignStmt.o compiler/BlockStmt.o compiler/Bytecode.o \
	compiler/Conditional.o compiler/CondParser.o compiler/DoStmt.o \
	compiler/Expansion.o compiler/Fragment.o compiler/IfStmt.o compiler/JumpStmt.o \
	compiler/lexer.o compiler/Macro.o compiler/parse.o \
	compiler/PreProc.o compiler/Program.o compiler/RepeatStmt.o compiler/AssignMathStmt.o \
	compiler/Stmt.o compiler/Symbol.o compiler/TaskStmt.o compiler/WhileStmt.o compiler/Error.o \
	compiler/AutoFree.o compiler/parse_util.o compiler/Expr.o \
	compiler/GosubStmt.o compiler/Scope.o compiler/InlineStmt.o compiler/CallStmt.o \
	compiler/CaseStmt.o compiler/SwitchStmt.o compiler/MonitorStmt.o compiler/AcquireStmt.o \
	compiler/DeclareStmt.o compiler/ScopeStmt.o compiler/ForStmt.o compiler/CatchStmt.o\
	compiler/LabelStmt.o compiler/GotoStmt.o \
	compiler/Compiler.o compiler/Buffer.o compiler/Mapping.o \
	compiler/FunctionDef.o compiler/ExprStmt.o compiler/AtomExpr.o compiler/IncDecExpr.o \
	compiler/BinaryExpr.o compiler/UnaryExpr.o compiler/ValueExpr.o compiler/TypeExpr.o \
	compiler/ModExpr.o compiler/EventSrcExpr.o compiler/SensorExpr.o \
	compiler/ArrayExpr.o compiler/TaskIdExpr.o compiler/RelExpr.o compiler/LogicalExpr.o \
	compiler/NegateExpr.o compiler/IndirectExpr.o compiler/NodeExpr.o compiler/ShiftExpr.o compiler/TernaryExpr.o \
	compiler/VarAllocator.o compiler/VarTranslator.o compiler/Resource.o \
	compiler/AddrOfExpr.o compiler/DerefExpr.o compiler/GosubParamStmt.o

NQCOBJ = nqc/nqc.o nqc/SRecord.o nqc/DirList.o nqc/CmdLine.o

# We need a 'bin' directory because the names of the binaries clash
# with existing directory names.

all : bin nqh nub bin/nqc

# Create the bin directory in the Makefile because it is not part
# of the original distribution.  This prevents the need to tell the user
# to do it.

bin:
	$(MKDIR) bin

bin/nqc : compiler/parse.cpp $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS)

bin/mkdata : mkdata/mkdata.cpp nqc/SRecord.cpp
	$(CXX) -o bin/mkdata -Inqc/ -Iplatform/ mkdata/mkdata.cpp nqc/SRecord.cpp

#
# clean up stuff
#
clean: clean-parser clean-lexer clean-obj clean-nqh clean-nub

clean-obj:
	$(RM) bin/*
	$(RM) */*.o


clean-parser:
	$(RM) compiler/parse.cpp compiler/parse.tab.h

clean-lexer:
	$(RM) compiler/lexer.cpp

clean-nqh:
	$(RM) compiler/rcx1_nqh.h compiler/rcx2_nqh.h

clean-nub:
	$(RM) rcxlib/rcxnub.h

#
# create the parser files (parse.cpp and parse.tab.h)
#
compiler/parse.cpp: compiler/parse.y
	(cd compiler ; $(YACC) -d parse.y ; \
	$(MV) y.tab.c parse.cpp ; $(MV) y.tab.h parse.tab.h )

#
# create the lexer file (lexer.cpp)
#
compiler/lexer.cpp: compiler/lex.l
	(cd compiler ; $(FLEX) lex.l )

#
# NQH files
#
nqh : compiler/rcx1_nqh.h compiler/rcx2_nqh.h

compiler/rcx1_nqh.h: compiler/rcx1.nqh bin/mkdata
	bin/mkdata compiler/rcx1.nqh compiler/rcx1_nqh.h rcx1_nqh

compiler/rcx2_nqh.h: compiler/rcx2.nqh bin/mkdata
	bin/mkdata compiler/rcx2.nqh compiler/rcx2_nqh.h rcx2_nqh

#
# rcxnub.h
#
nub: rcxlib/rcxnub.h

rcxlib/rcxnub.h: rcxlib/fastdl.srec bin/mkdata
	bin/mkdata -s rcxlib/fastdl.srec rcxlib/rcxnub.h rcxnub

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
# Installation of binary and man page
#
install: all
	test -d $(BINDIR) || mkdir -p $(BINDIR)
	cp -r bin/* $(BINDIR)
	test -d $(MANDIR)  || mkdir -p $(MANDIR)
	cp nqc-man-2.1r1-0.man $(MANDIR)/nqc.$(MANEXT)
