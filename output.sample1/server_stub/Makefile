NAME=cs
OS=$(shell uname -s)
SVR=$(NAME)_server

CXX=g++
CXXFLAGS=-g -I./include -I../include
CC=gcc
CFLAGS=-g -I./include -I../include

cchighlight=\033[0;31m
ccend=\033[0m

SVR_OBJS= \
	src/$(NAME)_svr.o \
	src/$(NAME)_svr_callee.o \
	src/main_$(NAME)_svr.o

.cpp.obj:
	${CXX} ${CXXFLAGS} -c $(.SOURCE)

all: $(SVR)
	mv $(SVR) bin
	@echo -e "$(cchighlight)successfully compiling server stub$(ccend)"

$(SVR): $(SVR_OBJS)
	mkdir -p bin
ifeq ($(OS),Linux)
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR) -Xlinker "-(" ../lib/common_lib.a $(SVR_OBJS) -Xlinker "-)"
else
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR) -Xlinker ../lib/common_lib.a $(SVR_OBJS)
endif

.PHONY: clean
clean:
	rm -f src/*.o
	rm -f $(SVR)
