CXX			= g++
LD			= g++
CXXFLAGS	= -std=c++11 -g -Wall
LDFLAGS		= -lcaf_core -lcaf_io
TARGETS		= hello echo_server concurrency_hello

all: $(TARGETS)

hello: hello.o
	$(LD) $^ $(LDFLAGS) -o $@

echo_server: echo_server.o
	$(LD) $^ $(LDFLAGS) -o $@

concurrency_hello: concurrency_hello.o
	$(LD) $^ $(LDFLAGS) -o $@

hello.o: hello.cpp
echo_server.o: echo_server.cpp
concurrency_hello.o: concurrency_hello.cpp

clean:
	$(RM) *.o
	$(RM) $(TARGETS)

.PHONY: all clean
