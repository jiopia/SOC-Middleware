TARGET = AudioService

CROSS_CHAIN = aarch64-linux-gnu-

CC = $(CROSS_CHAIN)gcc
CXX = $(CROSS_CHAIN)g++
AR = $(CROSS_CHAIN)ar
STRIP = $(CROSS_CHAIN)strip

PROJECT_ROOT :=$(patsubst %/, %, $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

CUR_SRC_DIR  = $(shell ls -AxR $(PROJECT_ROOT)|grep ":"|tr -d ':')
CUR_SRC_LIST:= $(foreach subdir,$(CUR_SRC_DIR),$(wildcard $(subdir)/*.c $(subdir)/*.cpp))

CUR_C_PATH  := $(filter %.c, $(CUR_SRC_LIST))
CUR_CPP_PATH:= $(filter %.cpp, $(CUR_SRC_LIST))

COMMON_SRC_PATH = \
	$(PROJECT_ROOT)/../common/Utils/cJSON/cJSON.cpp  \
	$(PROJECT_ROOT)/../common/Utils/JsonHandler/JsonHandler.cpp  \
	$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection/Mqtt/MqttConnection.cpp  \
	$(PROJECT_ROOT)/../common/Utils/MultiConnection/Forwarder/Forwarder.cpp  \
	$(PROJECT_ROOT)/../common/Utils/Thread/Thread.cpp  \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinystr.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxml.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxmlerror.cpp \
	$(PROJECT_ROOT)/../common/Utils/TinyXML/tinyxmlparser.cpp

SRC_PATH = \
	$(CUR_C_PATH) \
	$(CUR_CPP_PATH) \
	$(COMMON_SRC_PATH)

INCLUDE_FLAG = \
	-I$(PROJECT_ROOT)/../common/Utils \
	-I$(PROJECT_ROOT)/../common/Utils/cJSON \
	-I$(PROJECT_ROOT)/../common/Utils/JsonHandler \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Connection/Mqtt \
	-I$(PROJECT_ROOT)/../common/Utils/MultiConnection/Forwarder \
	-I$(PROJECT_ROOT)/../common/Utils/Singleton \
	-I$(PROJECT_ROOT)/../common/Utils/Thread \
	-I$(PROJECT_ROOT)/../common/Utils/TinyXML \
	-I$(PROJECT_ROOT)/../third_party/include \
	-I$(PROJECT_ROOT)/playControl \
	-I$(PROJECT_ROOT)/sound \
	-I$(PROJECT_ROOT)/xmlDoc

LIB_FLAG = \
	-L$(PROJECT_ROOT)/../third_party/lib/aarch64
	#-L$(PROJECT_ROOT)/../third_party/lib/aarch64/x86_64

LIB_FLAGS =
LIBXX_FLAGS = -lAudio -lrt -lpthread -lmosquitto #-l:libmosquitto.a

C_FLAGS = 
CXX_FLAGS = -std=c++11

all:
	$(CXX) $(SRC_PATH) $(INCLUDE_FLAG) $(LIB_FLAG) $(LIBXX_FLAGS) $(CXX_FLAGS) -o $(TARGET)
	$(STRIP) $(TARGET)

.PHONY:clean
clean:
	rm -rf $(TARGET)
