CXX='g++'
CXXFLAGS='-g'

CLI_LIB='rpc_cli'
SVR_BIN='rpc'

INCLUDE='./include'

cchighlight=\033[0;31m
ccend=\033[0m

all: $(SVR_BIN) $(CLI_LIB)
	@echo -e "$(cchighlight)finish compiling$(ccend)"

# making the cli_lib
$(CLI_LIB): main_cli.o add_stub_cli.o ds_svc.o rpc_net.o rpc_http.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(CLI_LIB) \
		main_cli.o \
		add_stub_cli.o \
		ds_svc.o \
		rpc_net.o \
		rpc_http.o

main_cli.o: main_cli.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main_cli.cpp

add_stub_cli.o: src/add_stub_cli.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/add_stub_cli.cpp

ds_svc.o: src/ds_svc.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/ds_svc.cpp

# making the svr_bin
$(SVR_BIN): main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_BIN) main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main.cpp

svr_base.o: src/svr_base.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_base.cpp

svr_thrd.o: src/svr_thrd.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_thrd.cpp

# common library for both server and client
rpc_net.o: src/rpc_net.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_net.cpp

rpc_http.o: src/rpc_http.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_http.cpp

.PHONY: clean
clean:
	rm -f *.o
	rm $(SVR_BIN)
	rm $(CLI_LIB)
