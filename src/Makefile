C = gcc
CXX = g++
LINK = g++

CFLAGS = -Wall -I ../include -I ./util
CXXFLAGS = $(CFLAGS)

ifeq ($(DEBUG), 1)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

LIBDIR = ../lib
LIBS = -L $(LIBDIR) -lJudy

PUBLIBS =

OBJS = util/utils.o util/urlencode.o BOModuleApp.o BOModuleInterface.o

.PHONY:all

all: lib testbo

lib: $(OBJS)
	ar srv $(LIBDIR)/libbo.a $(OBJS)

testbo: testbo.o lib
	$(C) $(CXXFLAGS) -o $@ $< $(PUBLIBS) -lbo $(LIBS) -lstdc++

clean:
	rm -rf $(OBJS) $(LIBDIR)/libbo.a testbo *.o
