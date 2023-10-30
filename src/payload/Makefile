SUBDIRS = loader p1ctl
CFLAGS += -miphoneos-version-min=12.0 -std=gnu17 -Os
CFLAGS += -I$(ROOT)/apple-include -I$(ROOT)/include -Wno-nullability-completeness
LDFLAGS += -Wl,-dead_strip -Wno-unused-command-line-argument
OBJDIR = obj
C_SRC = $(wildcard *.c)
LIBS = -framework CoreFoundation -framework IOKit ../APFS.tbd
OBJS = $(patsubst %,$(OBJDIR)/%,$(C_SRC:.c=.c.o))

export CFLAGS LDFLAGS

all: $(patsubst %, %-all, $(SUBDIRS)) payload

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.c.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

payload: $(patsubst %, %-all, $(SUBDIRS)) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(LIBS) $$(find . -name '*.o' | grep 'obj/.*o') -o payload
	$(LDID) -Sentitlements.xml -Icom.apple.payload payload

$(patsubst %, %-all, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1)

$(patsubst %, %-clean, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) clean

clean: $(patsubst %, %-clean, $(SUBDIRS))
	rm -f payload

.PHONY: all $(patsubst %, %-all, $(SUBDIRS)) $(patsubst %, %-clean, $(SUBDIRS))
