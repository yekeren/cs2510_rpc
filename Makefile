OS=$(shell uname -s)
INCLUDE='./include'

IDLFILE=conf/idl1.xml

CXX=g++
CXXFLAGS=-g -I$(INCLUDE)
CC=gcc
CFLAGS=-g -I$(INCLUDE)

SVR_CS=svr_cs

COMMON_LIB=common_lib.a
DIRECTORY_SERVER=directory_server
STUB_GENERATOR=stub_generator
CLIENT_STUB=client_stub_lib.a
SERVER_STUB=server_stub

cchighlight=\033[0;31m
ccend=\033[0m

# compiling cpp files
.cpp.obj:
	${CXX} ${CXXFLAGS} -c $(.SOURCE)

# compiling c files
.c.obj:
	${CC} ${CFLAGS} -c $(.SOURCE)

COMMON_LIB_INCLUDES= \
	include/ezxml.h \
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
	src/ds_svr.o \
	src/main_ds_svr.o

STUB_GENERATOR_OBJS= \
	src/ezxml.o \
	src/main_stub_generator.o

# compiling all
all: $(COMMON_LIB) $(DIRECTORY_SERVER) $(STUB_GENERATOR) $(CLIENT_STUB) $(SERVER_STUB)
	cp -rf script output/
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
$(DIRECTORY_SERVER): $(COMMON_LIB) $(DIRECTORY_SERVER_OBJS)
	mkdir -p output/directory_server
	echo $(OS)
ifeq ($(OS),Linux)
	$(CXX) $(CXXFLAGS) -lpthread -o $(DIRECTORY_SERVER) -Xlinker "-(" $(COMMON_LIB) $(DIRECTORY_SERVER_OBJS) -Xlinker "-)"
else
	$(CXX) $(CXXFLAGS) -lpthread -o $(DIRECTORY_SERVER) -Xlinker $(COMMON_LIB) $(DIRECTORY_SERVER_OBJS)
endif
	cp $(DIRECTORY_SERVER) output/directory_server
	@echo -e "$(cchighlight)successfully compiling $(DIRECTORY_SERVER)$(ccend)"

# compiling stub_generator
$(STUB_GENERATOR): $(STUB_GENERATOR_OBJS)
	mkdir -p output/bin
	$(CXX) $(CXXFLAGS) -o $(STUB_GENERATOR) $(STUB_GENERATOR_OBJS)
	cp $(STUB_GENERATOR) output/bin
	@echo -e "$(cchighlight)successfully compiling $(STUB_GENERATOR)$(ccend)"

# generating client_stub
$(CLIENT_STUB): $(STUB_GENERATOR)
	mkdir -p output/client_stub/include
	mkdir -p output/client_stub/src
	mkdir -p output/client_stub/lib
	./$(STUB_GENERATOR) -x $(IDLFILE) -t client_stub -p output/client_stub
	mv output/client_stub/*.h output/client_stub/include
	mv output/client_stub/*.cpp output/client_stub/src
	cd output/client_stub/ && make
	@echo -e "$(cchighlight)successfully generating $(CLIENT_STUB)$(ccend)"

# generating server_stub
$(SERVER_STUB): $(STUB_GENERATOR)
	mkdir -p output/server_stub/include
	mkdir -p output/server_stub/src
	mkdir -p output/server_stub/bin
	./$(STUB_GENERATOR) -x $(IDLFILE) -t server_stub -p output/server_stub
	mv output/server_stub/*.h output/server_stub/include
	mv output/server_stub/*.cpp output/server_stub/src
	cd output/server_stub/ && make
	@echo -e "$(cchighlight)successfully generating $(SERVER_STUB)$(ccend)"

.PHONY: clean
clean:
	rm -f src/*.o
	rm -f *.o
	rm -f $(COMMON_LIB)
	rm -f $(DIRECTORY_SERVER)
	rm -f $(STUB_GENERATOR)
	rm -f $(CLIENT_STUB)
	rm -f $(SERVER_STUB)
	rm -rf output
