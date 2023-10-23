CFLAGS += -miphoneos-version-min=12.0 -std=gnu17 -Os -U__nonnull
LDFLAGS += -Wl,-dead_strip -Wno-unused-command-line-argument
SRC = $(wildcard *.c)
OBJDIR = obj
OBJS = $(patsubst %,$(OBJDIR)/%,$(SRC:.c=.o))
LIBS = -framework CoreFoundation -framework IOKit
PLATFORMS = 2 3 5

all: $(patsubst %, mount_cores.%, $(PLATFORMS))

OBJS = $(patsubst %,$(OBJDIR)/%,$(SRC:.c=.o))

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

mount_cores: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

mount_cores.%: mount_cores
	vtool -remove-build-version 2 -output $@ $<
	if [ "$@" = "mount_cores.5" ]; then \
		os=3; \
	else os=12; \
	fi; \
	vtool -set-build-version $$(echo $@ | cut -d. -f2) $$os $$os -replace -output $@ $@
	$(LDID) -S $@

clean:
	rm -f mount_cores*
