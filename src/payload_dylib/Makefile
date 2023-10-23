CFLAGS += -miphoneos-version-min=12.0 -std=gnu17 -Os
CFLAGS += -I$(ROOT)/apple-include -I$(ROOT)/include
LDFLAGS += -Wl,-dead_strip -Wno-unused-command-line-argument
OBJDIR = obj
C_SRC = $(wildcard *.c)
LIBS = -framework CoreFoundation
OBJS = $(patsubst %,$(OBJDIR)/%,$(C_SRC:.c=.c.o))

all: payload.dylib

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.c.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

payload.dylib: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $(OBJS) -shared -o payload.dylib
	$(LDID) -S payload.dylib

clean:
	rm -rf payload.dylib*

.PHONY: all clean
