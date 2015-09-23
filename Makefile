CXX='g++'
CXXFLAGS='-g'

PROJ_NAME='rpc'

all: ${PROJ_NAME}

${PROJ_NAME}: main.o add_stub.o ds_svc.o
	@echo "compiling ${PROJ_NAME}..."
	${CXX} -o ${PROJ_NAME} main.o add_stub.o ds_svc.o

main.o: main.cpp
	@echo "compiling main.o"
	${CXX} -c main.cpp

add_stub.o: add_stub.cpp
	@echo "compiling add_stub.o"
	${CXX} -c add_stub.cpp

ds_svc.o: ds_svc.cpp
	@echo "compiling ds_svc.o"
	${CXX} -c ds_svc.cpp

clean:
	rm -f *.o
	rm -f ${PROJ_NAME}
