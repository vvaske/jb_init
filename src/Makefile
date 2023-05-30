CFLAGS += -miphoneos-version-min=15.0 -O0 -std=gnu17
CFLAGS += -Wall -Wextra -Wno-unused-parameter
CFLAGS += -Wno-unused-command-line-argument
export CFLAGS LDFLAGS

SUBDIRS = launchd

all: $(SUBDIRS) fakedyld

fakedyld: $(SUBDIRS)
	$(MAKE) -C fakedyld

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: all $(SUBDIRS)
