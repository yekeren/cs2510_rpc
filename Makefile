INCLUDE='./include'

CXX=g++
CXXFLAGS=-g -I$(INCLUDE)
CC=gcc
CFLAGS=-g -I$(INCLUDE)

CLI_LIB=rpc_cli
SVR_CS=svr_cs
STUB_GEN=stub_gen

COMMON_LIB=common_lib.a
DIRECTORY_SERVER=directory_server

cchighlight=\033[0;31m
ccend=\033[0m

# compiling cpp files
.cpp.obj:
	${CXX} ${CXXFLAGS} -c $(.SOURCE)

# compiling c files
.c.obj:
	${CC} ${CFLAGS} -c $(.SOURCE)

COMMON_LIB_INCLUDES= \
	include/svr_base.h \
	include/svr_thrd.h \
	include/io_event.h \
	include/http_event.h \
	include/rpc_log.h \
	include/rpc_lock.h \
	include/rpc_net.h \
	include/rpc_http.h \
	include/rpc_common.h \
	include/basic_proto.h \
	include/template.h

COMMON_LIB_OBJS= \
	src/ezxml.o \
	src/svr_base.o \
	src/svr_thrd.o \
	src/io_event.o \
	src/http_event.o \
	src/accept_event.o \
	src/rpc_net.o \
	src/rpc_http.o \
	src/rpc_common.o \
	src/basic_proto.o \
	src/template.o

DIRECTORY_SERVER_OBJS= \
	main_svr_ds.o \
	src/ds_svr.o

# compiling all
all: $(COMMON_LIB) $(DIRECTORY_SERVER)
	@echo -e "$(cchighlight)finish compiling$(ccend)"

# compiling common_lib
$(COMMON_LIB): $(COMMON_LIB_OBJS)
	mkdir -p output/include
	mkdir -p output/lib
	$(foreach file, $(COMMON_LIB_INCLUDES), cp $(file) output/include;)
	ar rcs $(COMMON_LIB) $(COMMON_LIB_OBJS)
	cp $(COMMON_LIB) output/lib
	@echo -e "$(cchighlight)successfully compiling $(COMMON_LIB)$(ccend)"

# compiling directory_server
$(DIRECTORY_SERVER): $(DIRECTORY_SERVER_OBJS) $(COMMON_LIB)
	mkdir -p output/bin
	$(CXX) $(CXXFLAGS) -lpthread -o $(DIRECTORY_SERVER) -Xlinker $(COMMON_LIB) $(DIRECTORY_SERVER_OBJS)
	cp $(DIRECTORY_SERVER) output/bin
	@echo -e "$(cchighlight)successfully compiling $(DIRECTORY_SERVER)$(ccend)"

# object files needed in cs
CS_OBJS=main_svr_cs.o \
	src/svr_base.o \
	src/svr_thrd.o \
	src/rpc_net.o \
	src/rpc_http.o \
	src/io_event.o \
	src/accept_event.o \
	src/http_event.o \
	src/ezxml.o \
	src/cs_svr.o \
	src/basic_proto.o \
	src/template.o

CLI_OBJS=main_cli.o \
	src/multiply_stub_cli.o \
	src/rpc_net.o \
	src/rpc_http.o \
	src/rpc_common.o \
	src/ezxml.o \
	src/basic_proto.o \
	src/template.o

STUB_OBJS=main_gen.o \
	src/ezxml.o

$(STUB_GEN): $(STUB_OBJS)
	$(CXX) $(CXXFLAGS) -lpthread -o $(STUB_GEN) $(STUB_OBJS)

# making the cli_lib
$(CLI_LIB): $(CLI_OBJS)
	$(CXX) $(CXXFLAGS) -lpthread -o $(CLI_LIB) $(CLI_OBJS)

# compiling svr_cs
$(SVR_CS): $(CS_OBJS) 
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_CS) $(CS_OBJS)

.PHONY: clean
clean:
	rm -f src/*.o
	rm -f *.o
	rm -f $(DIRECTORY_SERVER)
	rm -f $(SVR_CS)
	rm -f $(CLI_LIB)
	rm -f $(STUB_GEN)
	rm -rf output
