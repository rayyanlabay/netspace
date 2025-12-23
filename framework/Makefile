CXX := g++

CXXFLAGS := -std=c++11 -pedantic-errors -Wall -Wextra -Werror -rdynamic -Iinclude -I../../../utils/include

DEBUG_FLAGS := -g -D_DEBUG
RELEASE_FLAGS := -O3

SRC := src/thread_pool.cpp src/pnp.cpp test/re_fw_test.cpp 

TARGET := prog

.PHONY: all debug clean

all: release

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ 

clean:
	rm -f $(TARGET) 
