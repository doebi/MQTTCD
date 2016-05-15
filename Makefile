PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
SYS := $(shell gcc -dumpmachine)
#GITVER := $(shell git describe --tags)
INSTALL_DATA := -pDm755

ifeq ($(GITVER),)
GITVER = "unknown"
endif

# LINUX
# The automated regression tests run on Linux, so this is the one
# environment where things likely will work -- as well as anything
# works on the bajillion of different Linux environments
ifneq (, $(findstring linux, $(SYS)))
LIBS = -lpaho-mqtt3c -lpthread
INCLUDES =
FLAGS2 = 
endif

# this works on llvm or real gcc
CC = gcc

DEFINES = 
CFLAGS = -g -ggdb $(FLAGS2) $(INCLUDES) $(DEFINES) -Wall -O3
.SUFFIXES: .c .cpp

all: bin/mqttcd

tmp/main-conf.o: src/main-conf.c src/*.h
	$(CC) $(CFLAGS) -c $< -o $@ -DGIT=\"$(GITVER)\"

# just compile everything in the 'src' directory. Using this technique
# means that include file dependencies are broken, so sometimes when
# the program crashes unexpectedly, 'make clean' then 'make' fixes the
# problem that a .h file was out of date
tmp/%.o: src/%.c src/*.h
	$(CC) $(CFLAGS) -c $< -o $@


SRC = $(wildcard src/*.c)
OBJ = $(addprefix tmp/, $(notdir $(addsuffix .o, $(basename $(SRC))))) 


bin/mqttcd: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS) $(LIBS)

clean:
	rm -f tmp/*.o
	rm -f bin/mqttcd

regress: bin/mqttcd
	bin/mqttcd --selftest

test: regress

install: bin/mqttcd
	install $(INSTALL_DATA) bin/mqttcd $(DESTDIR)$(BINDIR)/mqttcd
	
default: bin/mqttcd
