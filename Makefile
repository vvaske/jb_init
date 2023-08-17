ROOT := $(shell pwd)
CC = xcrun -sdk iphoneos clang

export ROOT CC

all:
	$(MAKE) -C $(ROOT)/src

clean:
	$(MAKE) -C $(ROOT)/src clean

.PHONY: all clean
