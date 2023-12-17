SUBDIRS = jbinit jbloader systemhooks bootscreend

all: $(SUBDIRS)
	@echo $(SUBDIRS)

jbinit:
	$(MAKE) -C $(SRC)/jbinit

jbloader:
	$(MAKE) -C $(SRC)/jbloader

systemhooks:
	$(MAKE) -C $(SRC)/systemhooks payload.dylib

bootscreend:
	$(MAKE) -C $(SRC)/bootscreend

.PHONY: all $(SUBDIRS)
