# Project: VDMA

TARGET_APP := AVCapture

LOCAL_CFLAGS += -I../../../lib

CC = gcc
CXX = g++
LD = ld

CFLAGS += -O0 \
          -Wall \
          -Wextra \
          -Wno-unused-parameter \
          -Wno-sign-compare \
          -Wno-unused-variable \
          -Wno-unused-but-set-variable \
          -Wno-missing-field-initializers  \
          -fPIC \
          -fpermissive \
          $(LOCAL_CFLAGS) \

CLIB = -lGL -lGLEW `pkg-config --static --libs glfw3` -lpthread -ldl -lasound -L../../../lib -llscvdma -Wl,-rpath ../../../lib

#OPENGLSTR1=$(shell glxinfo | grep "OpenGL version string:")
#OPENGLSTR=$(subst ., ,$(OPENGLSTR1))
#OPENGLVERSION = $(word 4,$(OPENGLSTR))
ifeq ($(OPENGLVERSION),2)
all: CHECKOPENGLFAIL
else
ifeq ($(OPENGLVERSION),1)
all: CHECKOPENGLFAIL
else
ifeq ($(OPENGLVERSION),0)
all: CHECKOPENGLFAIL
else
all: $(TARGET_APP)
endif
endif
endif

CHECKOPENGLFAIL:
	echo -e "\033[31mplease updata opengl version to 3.0 or above \033[0m"

AVCAPTURE_OBJS = ./AVCapture.o ./MiscUtils.o \
	MemBuf.o AvCapView.o 
	
$(TARGET_APP) : $(AVCAPTURE_OBJS) 
	$(CXX) -o $(TARGET_APP) $(AVCAPTURE_OBJS) $(CLIB) $(LIBS)

%.o : %.cpp
	$(CXX) -c $(CFLAGS) $(LOCAL_CFLAGS) $(CXXFLAGS) $< -o $*.o

clean:	
	rm -rf  $(TARGET_APP) $(AVCAPTURE_OBJS)

