INCLUDE='./include'

CXX=g++
CXXFLAGS=-g -std=c++0x -I$(INCLUDE)
CC=gcc
CFLAGS=-g -I$(INCLUDE)

CLI_LIB=rpc_cli
SVR_DS=svr_ds
SVR_CS=svr_cs
STUB_GEN=stub_gen

cchighlight=\033[0;31m
ccend=\033[0m

# compiling cpp files
.cpp.obj:
	${CXX} ${CXXFLAGS} -c $(.SOURCE)

# compiling c files
.c.obj:
	${CC} ${CFLAGS} -c $(.SOURCE)

# compiling the binary
all: $(SVR_DS) $(SVR_CS) $(STUB_GEN)
	@echo -e "$(cchighlight)finish compiling$(ccend)"

# object files needed in directory server
DS_OBJS=main_svr_ds.o \
	src/svr_base.o \
	src/svr_thrd.o \
	src/rpc_net.o \
	src/rpc_http.o \
	src/io_event.o \
	src/accept_event.o \
	src/http_event.o \
	src/ezxml.o \
	src/ds_svr.o \
	src/basic_proto.o \
	src/template.o

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
	src/add_stub_cli.o \
	src/wc_stub_cli.o \
	src/ds_svc.o \
	src/rpc_net.o \
	src/rpc_http.o \
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

# compiling svr_ds
$(SVR_DS): $(DS_OBJS)
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_DS) $(DS_OBJS)

.PHONY: clean
clean:
	rm -f src/*.o
	rm -f *.o
	rm -f $(SVR_DS)
	rm -f $(SVR_CS)
	rm -f $(CLI_LIB)
	rm -f $(STUB_GEN)
