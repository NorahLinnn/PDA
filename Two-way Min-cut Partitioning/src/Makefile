CXX      := g++
CXXFLAGS := -std=c++11 -O3 -Wall -Wextra -MMD
LIBS     := -lm
EXEC     := ../bin/hw2
SRC_DIRS := .\
				Parser\
				Partition\
				ResultWriter\
				checkParser
SRCS     := $(wildcard $(SRC_DIRS:=/*.cpp))
OBJS     := $(SRCS:.cpp=.o)
DEPS     := $(OBJS:.o=.d)

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(LIBS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(EXEC) $(OBJS) $(DEPS)

ifeq (test, $(firstword $(MAKECMDGOALS)))
  TESTCASE := $(word 2, $(MAKECMDGOALS))
  $(eval $(TESTCASE):;@:)
endif

test: $(EXEC)
	@echo test on $(TESTCASE)
	./$(EXEC) ../testcase/$(TESTCASE).txt ../output/$(TESTCASE).out

.PHONY: all clean test
-include $(DEPS)