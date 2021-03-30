#############################################################  
# Generic Makefile for C/C++ Program  
#  
# License: GPL (General Public License)  
# Author:  whyglinux <whyglinux AT gmail DOT com>  
# Date:    2006/03/04 (version 0.1)  
#          2007/03/24 (version 0.2)  
#          2007/04/09 (version 0.3)  
#          2007/06/26 (version 0.4)  
#          2008/04/05 (version 0.5)  
#  
# Description:  
# ------------  
# This is an easily customizable makefile template. The purpose is to  
# provide an instant building environment for C/C++ programs.  
#  
# It searches all the C/C++ source files in the specified directories,  
# makes dependencies, compiles and links to form an executable.  
#  
# Besides its default ability to build C/C++ programs which use only  
# standard C/C++ libraries, you can customize the Makefile to build  
# those using other libraries. Once done, without any changes you can  
# then build programs using the same or less libraries, even if source  
# files are renamed, added or removed. Therefore, it is particularly  
# convenient to use it to build codes for experimental or study use.  
#  
# GNU make is expected to use the Makefile. Other versions of makes  
# may or may not work.  
#  
# Usage:  
# ------  
# 1. Copy the Makefile to your program directory.  
# 2. Customize in the "Customizable Section" only if necessary:  
#    * to use non-standard C/C++ libraries, set pre-processor or compiler  
#      options to <MY_CFLAGS> and linker ones to <MY_LIBS>  
#      (See Makefile.gtk+-2.0 for an example)  
#    * to search sources in more directories, set to <SRCDIRS>  
#    * to specify your favorite program name, set to <PROGRAM>  
# 3. Type make to start building your program.  
#  
# Make Target:  
# ------------  
# The Makefile provides the following targets to make:  
#   $ make           compile and link  
#   $ make NODEP=yes compile and link without generating dependencies  
#   $ make objs      compile only (no linking)  
#   $ make tags      create tags for Emacs editor  
#   $ make ctags     create ctags for VI editor  
#   $ make clean     clean objects and the executable file  
#   $ make distclean clean objects, the executable and dependencies  
#   $ make help      get the usage of the makefile  
#  
#===========================================================================  
  
## Customizable Section: adapt those variables to suit your program.  
##==========================================================================  

#当前正在执行的Makefile文件所在目录的绝对路径(结果最后不包含反斜杠后缀/)
PROJECT_ROOT := $(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

# The pre-processor and compiler options.  
MY_CFLAGS =  \
	-I$(PROJECT_ROOT)/../common/bdsdk/include  \
	-I$(PROJECT_ROOT)/../common/Utils  \
	-I$(PROJECT_ROOT)/../common/Utils/cJSON  \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection  \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection/Mqtt  \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Forwarder  \
	-I$(PROJECT_ROOT)/../common/Utils/Singleton  \
	-I$(PROJECT_ROOT)/../common/Utils/Thread  \
	-I$(PROJECT_ROOT)/../common/Utils/TinyXML  \
	-I$(PROJECT_ROOT)/CoreMsgHandler

# The linker options.  
MY_LIBS   =  
  
# The pre-processor options used by the cpp (man cpp for more).  
CPPFLAGS  = -Wall -g -lrt -lmosquitto -lpthread -std=c++11
  
# The options used in linking as well as in any direct use of ld.  
LDFLAGS   =  
  
# The directories in which source files reside.  
# If not specified, only the current directory will be serached.  
SRCDIRS   = $(shell pwd)
  
# The executable file name.  
# If not specified, current directory name or `a.out‘ will be used.  
PROGRAM   = ViewService
  
## Implicit Section: change the following only when necessary.  
##==========================================================================  
  
# The source file types (headers excluded).  
# .c indicates C source files, and others C++ ones.  
SRCEXTS = .c .C .cc .cpp .CPP .c++ .cxx .cp  
  
# The header file types.  
HDREXTS = .h .H .hh .hpp .HPP .h++ .hxx .hp  
  
# The pre-processor and compiler options.  
# Users can override those variables from the command line.  
CFLAGS  = -g -O2  
CXXFLAGS= -g -O2  
  
# The C program compiler.  
CC     = gcc  
  
# The C++ program compiler.  
CXX    = g++  
  
# Un-comment the following line to compile C programs as C++ ones.  
CC     = $(CXX)  
  
# The command used to delete file.  
RM     = rm -f  
  
ETAGS = etags  
ETAGSFLAGS =  
  
CTAGS = ctags  
CTAGSFLAGS =  
  
## Stable Section: usually no need to be changed. But you can add more.  
##==========================================================================  
SHELL   = /bin/bash
EMPTY   =  
SPACE   = $(EMPTY) $(EMPTY)  
ifeq ($(PROGRAM),)  
  CUR_PATH_NAMES = $(subst /,$(SPACE),$(subst $(SPACE),_,$(CURDIR)))  
  PROGRAM = $(word $(words $(CUR_PATH_NAMES)),$(CUR_PATH_NAMES))  
  ifeq ($(PROGRAM),)  
    PROGRAM = a.out  
  endif
endif

# Get .c, .cpp source files by searching from current directory.
CUR_SRC_DIR	 = $(shell ls -AxR $(CURDIR)|grep ":"|tr -d ':')
CUR_SRC 	:= $(foreach subdir,$(CUR_SRC_DIR),$(wildcard $(subdir)/*.c $(subdir)/*.cpp))
CUR_C   	:= $(filter %.c, $(CUR_SRC))
CUR_CPP 	:= $(filter %.cpp, $(CUR_SRC))
CUR_CPP 	:= $(CUR_CPP) \
	$(PROJECT_ROOT)/../common/Utils  \
	$(PROJECT_ROOT)/../common/Utils/cJSON/cJSON.cpp  \
	$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection/Mqtt/MqttConnection.cpp  \
	$(PROJECT_ROOT)/../common/Utils/MultiConnection/Forwarder/Forwarder.cpp  \
	$(PROJECT_ROOT)/../common/Utils/Thread/Thread.cpp  \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinystr.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxml.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxmlerror.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxmlparser.cpp

# Get the include files, object files, dependent files by searching from PRG_INC_DIR.
CUR_INC_DIR = $(shell ls -AxR $(PRG_INC_DIR)|grep ":"|tr -d ':')
CUR_INC 	:= $(foreach subdir,$(CUR_INC_DIR),$(wildcard $(subdir)/*.h $(subdir)/*.hpp))
SRC_H   	:= $(filter %.h, $(CUR_INC))
SRC_HPP   	:= $(filter %.hpp, $(CUR_INC))

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))  
HEADERS = $(foreach d,$(SRCDIRS),$(wildcard $(addprefix $(d)/*,$(HDREXTS))))  
SRC_CXX = $(CUR_CPP)
OBJS    = $(strip $(CUR_CPP:.cpp=.o) $(CUR_C:.c=.o))
DEPS    = $(OBJS:.o=.d)

## Define some useful variables.  
DEP_OPT = $(shell if `$(CC) --version | grep "GCC" >/dev/null`; then 			echo "-MM -MP"; else echo "-M"; fi )  
DEPEND      = $(CC)  $(DEP_OPT) $(MY_CFLAGS) $(CFLAGS) $(CPPFLAGS)  
DEPEND.d    = $(subst -g ,,$(DEPEND))  
COMPILE.c   = $(CC)  $(MY_CFLAGS) $(CFLAGS)   $(CPPFLAGS) -c  
COMPILE.cxx = $(CXX) $(MY_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c  
LINK.c      = $(CC)  $(MY_CFLAGS) $(CFLAGS)   $(CPPFLAGS) $(LDFLAGS)  
LINK.cxx    = $(CXX) $(MY_CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)  
  
.PHONY: all objs tags ctags clean distclean help show  
  
# Delete the default suffixes  
.SUFFIXES:  
  
all: $(PROGRAM)  

# Rules for creating dependency files (.d).  
#------------------------------------------  

%.d:%.c  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.C  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.cc  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.cpp  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.CPP  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.c++  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.cp  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
%.d:%.cxx  
	@echo -n $(dir $<) > $@  
	@$(DEPEND.d) $< >> $@  
  
# Rules for generating object files (.o).  
#----------------------------------------  
objs:$(OBJS)  

%.o:%.c  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.C  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.cc  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.cpp  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.CPP  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.c++  
	$(COMPILE.cxx) $< -o $@  

%.o:%.cp  
	$(COMPILE.cxx) $< -o $@  
  
%.o:%.cxx  
	$(COMPILE.cxx) $< -o $@  
  
# Rules for generating the tags.  
#-------------------------------------  
tags: $(HEADERS) $(SOURCES)  
	$(ETAGS) $(ETAGSFLAGS) $(HEADERS) $(SOURCES)  
  
ctags: $(HEADERS) $(SOURCES)  
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)  
  
# Rules for generating the executable.  
#-------------------------------------  
$(PROGRAM):$(OBJS)  
ifeq ($(SRC_CXX),)              # C program  
	$(LINK.c)   $(OBJS) $(MY_LIBS) -o $@  
	@echo Type ./$@ to execute the program.  
else                            # C++ program
	$(LINK.cxx) $(OBJS) $(MY_LIBS) -o $@  
	@echo Type ./$@ to execute the program.  
endif  

clean:  
	$(RM) $(OBJS) $(PROGRAM) $(PROGRAM).exe  
  
distclean: clean  
	$(RM) $(DEPS) TAGS  
  
# Show help.  
help:  
	@echo "Generic Makefile for C/C++ Programs (gcmakefile) version 0.5"
	@echo "Copyright (C) 2007, 2008 whyglinux <whyglinux@hotmail.com>"
	@echo  
	@echo "Usage: make [TARGET]"
	@echo "TARGETS:"
	@echo "  all       (=make) compile and link." 
	@echo "  NODEP=yes make without generating dependencies." 
	@echo "  objs      compile only (no linking)." 
	@echo "  tags      create tags for Emacs editor." 
	@echo "  ctags     create ctags for VI editor." 
	@echo "  clean     clean objects and the executable file." 
	@echo "  distclean clean objects, the executable and dependencies." 
	@echo "  show      show variables (for debug use only)." 
	@echo "  help      print this message." 
	@echo  
	@echo "Report bugs to <whyglinux AT gmail DOT com>." 
  
# Show variables (for debug use only.)  
show:  
	@echo "PROGRAM     :"$(PROGRAM)  
	@echo "SRCDIRS     :"$(SRCDIRS)  
	@echo "HEADERS     :"$(HEADERS)  
	@echo "SOURCES     :"$(SOURCES)  
	@echo "SRC_CXX     :"$(SRC_CXX)  
	@echo "OBJS        :"$(OBJS)  
	@echo "DEPS        :"$(DEPS)  
	@echo "DEPEND      :"$(DEPEND)  
	@echo "COMPILE.c   :"$(COMPILE.c)  
	@echo "COMPILE.cxx :"$(COMPILE.cxx)  
	@echo "link.c      :"$(LINK.c)  
	@echo "link.cxx    :"$(LINK.cxx)  
  
## End of the Makefile ##  Suggestions are welcome  ## All rights reserved ##  
##############################################################