CC='gcc'
CXX='g++'
CXXFLAGS='-g'

CLI_LIB='rpc_cli'
SVR_BIN='rpc'

INCLUDE='./include'

cchighlight=\033[0;31m
ccend=\033[0m

#all: $(SVR_BIN) $(CLI_LIB)
all: $(SVR_BIN)
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
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $(.SOURCE)

add_stub_cli.o: src/add_stub_cli.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $(.SOURCE)

ds_svc.o: src/ds_svc.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c $(.SOURCE)

# making the svr_bin
$(SVR_BIN): main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o io_event.o accept_event.o http_event.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_BIN) main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o io_event.o accept_event.o http_event.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main.cpp

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

# common library for both server and client
rpc_net.o: src/rpc_net.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_net.cpp

rpc_http.o: src/rpc_http.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_http.cpp

ezxml.o: src/ezxml.c
	$(CC) $(CXXFLAGS) -I$(INCLUDE) -c src/ezxml.c

.PHONY: clean
clean:
	rm -f *.o
	rm -f $(SVR_BIN)
	rm -f $(CLI_LIB)
