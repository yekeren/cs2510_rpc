CXX='g++'
CXXFLAGS='-g' '-std=c++0x'
CC='gcc'
CFLAGS='-g'

CLI_LIB='rpc_cli'
SVR_DS='rpc'
SVR_DS='svr_ds'

INCLUDE='./include'

cchighlight=\033[0;31m
ccend=\033[0m

#all: $(SVR_DS) $(CLI_LIB)
#all: $(CLI_LIB)
all: $(SVR_DS)
	@echo -e "$(cchighlight)finish compiling$(ccend)"

# making the cli_lib
$(CLI_LIB): main_cli.o add_stub_cli.o ds_svc.o rpc_net.o rpc_http.o ezxml.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(CLI_LIB) \
		main_cli.o \
		add_stub_cli.o \
		ds_svc.o \
		rpc_net.o \
		rpc_http.o \
		ezxml.o 

main_cli.o: main_cli.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main_cli.cpp

add_stub_cli.o: src/add_stub_cli.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/add_stub_cli.cpp

ds_svc.o: src/ds_svc.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/ds_svc.cpp

# compiling svr_ds
$(SVR_DS): main_svr_ds.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o io_event.o accept_event.o http_event.o ezxml.o ds_svr.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_DS) main_svr_ds.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o io_event.o accept_event.o http_event.o ezxml.o ds_svr.o

main_svr_ds.o: main_svr_ds.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main_svr_ds.cpp

svr_base.o: src/svr_base.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_base.cpp

svr_thrd.o: src/svr_thrd.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_thrd.cpp

io_event.o: src/io_event.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/io_event.cpp

accept_event.o: src/accept_event.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/accept_event.cpp

http_event.o: src/http_event.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/http_event.cpp

ds_svr.o: src/ds_svr.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/ds_svr.cpp

# common library for both server and client
rpc_net.o: src/rpc_net.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_net.cpp

rpc_http.o: src/rpc_http.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_http.cpp

ezxml.o: src/ezxml.c
	$(CC) $(CFLAGS) -I$(INCLUDE) -c src/ezxml.c

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(SVR_DS)
	rm -f $(CLI_LIB)
