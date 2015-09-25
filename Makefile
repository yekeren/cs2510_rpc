CXX='g++'
CXXFLAGS='-g'

CLI_LIB='rpc.a'
SVR_BIN='rpc'

INCLUDE='./include'

all: $(SVR_BIN)
	@echo "compiling $(SVR_BIN)..."

$(SVR_BIN): main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o
	$(CXX) $(CXXFLAGS) -lpthread -o $(SVR_BIN) main.o svr_base.o svr_thrd.o rpc_net.o rpc_http.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c main.cpp

svr_base.o: src/svr_base.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_base.cpp

svr_thrd.o: src/svr_thrd.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/svr_thrd.cpp

rpc_net.o: src/rpc_net.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_net.cpp

rpc_http.o: src/rpc_http.cpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDE) -c src/rpc_http.cpp

.PHONY: clean
clean:
	rm -f *.o
	rm $(SVR_BIN)
