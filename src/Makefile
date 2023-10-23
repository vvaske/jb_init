SUBDIRS = mount_cores fakedyld rootlesshooks payload_dylib payload
CFLAGS += -I$(ROOT)/include
LDID = ldid

.PHONY: all clean $(patsubst %, %-all, $(SUBDIRS)) $(patsubst %, %-clean, $(SUBDIRS))

export CFLAGS LDID

all: ramdisk.dmg

ramdisk.dmg: $(patsubst %, %-all, $(SUBDIRS))
	rm -f ramdisk.dmg
	mkdir -p ramdisk/{sbin,dev,usr/lib}
	install -m755 fakedyld/dyld ramdisk/usr/lib/
	install -m644 mount_cores/mount_cores.* ramdisk/
	install -m755 payload_dylib/payload.dylib ramdisk/
	install -m755 payload/payload ramdisk/
	hdiutil create -fs HFS+ -layout NONE -format UDRW -size 1m -srcdir ramdisk -volname plooshra1nrd ramdisk.dmg

binpack.dmg: binpack.tar loader.dmg


$(patsubst %, %-all, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) all

clean: $(patsubst %, %-clean, $(SUBDIRS))

$(patsubst %, %-clean, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) clean
	rm -rf ramdisk ramdisk.dmg
