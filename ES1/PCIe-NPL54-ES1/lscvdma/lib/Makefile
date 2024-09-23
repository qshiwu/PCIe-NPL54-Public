#============================================================================= 
#                       Top Level LSC PCIe API Library Makefile
#
# This makefile compiles all the sub-projects to produce a library of classes
# that implement the basic functionality to open device drivers and access
# IP device registers.
#
#============================================================================= 

CC = g++
#CFLAGS = -std=c++11 -fPIC -Wall -Wextra -o2 -g -Wno-write-strings
CFLAGS += -std=c++11 -fPIC -Wall -Wextra -o2 -g -Wno-write-strings
LDFLAGS = -shared
RM = rm -f

TARGET_LIB = liblscvdma.so

LIBS = -lc -lrt
SRCS = LSCVDMA_IF.cpp LscVdmaController.cpp
OBJS = $(SRCS:.c=.o)

.PHONY:all

all:${TARGET_LIB}

$(TARGET_LIB):$(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
 
 .PHONY:clean
 
 clean:
	-$(RM) ${TARGET_LIB} *.o *.so
