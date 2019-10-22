LIBRARY_FLAGS	:=				\
	-lm
INCLUDE_FLAGS   :=              \
	-I.                         \
    -I./ADT

PRE_CXX         := ccache
CXXFLAGS        := $(INCLUDE_FLAGS) -g -Wall -Wextra -Werror -pedantic-errors
CXX             := $(PRE_CXX) g++ -std=c++17

BUILD           := ./bin

#SOURCE          :=              \
#    $(wildcard *.cc)           \
#    $(wildcard Test/*.cc)
#OBJECTS			:=              \
#    main.o                      \
#    test.o                      \

all: test main

%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY:			all clean test

test: Test/main.o
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -o $(BUILD)/test $<
	@$(BUILD)/test

main: main.o
	@mkdir -p $(BUILD)
	$(CXX) $(CXXFLAGS) -o $(BUILD)/main $<

clean:
	-@echo "CLEAN"
	-@rm -f *.o
	-@rm -rf $(BUILD)
