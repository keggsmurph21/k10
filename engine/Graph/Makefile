LIBRARY_FLAGS	:=				\
	-lm
INCLUDE_FLAGS   := -I.

PRE_CXX         := ccache
CXXFLAGS        := $(INCLUDE_FLAGS) -g -Wall -Wextra -Werror -pedantic -std=c++17
CXX             := $(PRE_CXX) g++

all: main

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

main: main.o Graph.o
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	-@echo "CLEAN"
	-@rm -f *.o
	-@rm -f main
