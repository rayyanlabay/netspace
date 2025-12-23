CXX := g++

CXXFLAGS := -std=c++11 -pedantic-errors -Wall -Wextra -Werror -Iinclude -I../../utils/include -Iframework/include

DEBUG_FLAGS := -g -D_DEBUG
RELEASE_FLAGS := -O3

SRC := main/netspace.cpp src/driver_data.cpp src/file_descriptor.cpp src/nbd_driver_communicator.cpp framework/src/pnp.cpp src/ram_storage.cpp framework/src/logger.cpp framework/src/thread_pool.cpp

BINDIR := bin
TARGET := netspace_app

.PHONY: all debug clean

all: release

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(BINDIR)/$(TARGET)

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ 

clean:
	rm -f $(BINDIR)/$(TARGET) 
