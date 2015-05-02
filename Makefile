CXX			= g++
LD			= g++
CXXFLAGS	= -std=c++11 -g -Wall -O3
LDFLAGS		= -lcaf_core -lcaf_io
TARGETS		= hello echo_server concurrency_hello parallel_sort typed_hello

all: $(TARGETS)

hello: hello.o
	$(LD) $^ $(LDFLAGS) -o $@

echo_server: echo_server.o
	$(LD) $^ $(LDFLAGS) -o $@

concurrency_hello: concurrency_hello.o
	$(LD) $^ $(LDFLAGS) -o $@

parallel_sort: parallel_sort.o
	$(LD) $^ $(LDFLAGS) -o $@

typed_hello: typed_hello.o
	$(LD) $^ $(LDFLAGS) -o $@

hello.o: hello.cpp
echo_server.o: echo_server.cpp
concurrency_hello.o: concurrency_hello.cpp
parallel_sort.o: parallel_sort.cpp
typed_hello.o: typed_hello.cpp

clean:
	$(RM) *.o
	$(RM) $(TARGETS)

.PHONY: all clean
