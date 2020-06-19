#This makefile is for project klotski
#Copyright (C). All rights reserved

CXX := g++
CXXFLAGES := -Wall
#change it to "del" if you're using windows
RM := rm
TARGET_NAME := klotski
objs := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

$(TARGET_NAME): $(objs) $(TARGET_NAME).o
	$(CXX) $(CXXFLAGES) $^ -o $@

%.d: %.cpp
	$(CXX) -MM $< > $@

include $(objs:.o=.d)

.PHONY: clean
clean:
	-$(RM) *.o
	-$(RM) *.d

