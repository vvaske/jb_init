SUBDIRS = mount_cores fakedyld rootlesshooks payload_dylib payload
CFLAGS += -I$(ROOT)/include
LDID = ldid

.PHONY: all clean $(patsubst %, %-all, $(SUBDIRS)) $(patsubst %, %-clean, $(SUBDIRS))

export CFLAGS LDID

all: ramdisk.dmg binpack.dmg

ramdisk.dmg: $(patsubst %, %-all, $(SUBDIRS))
	rm -f ramdisk.dmg
	mkdir -p ramdisk/{sbin,dev,usr/lib}
	install -m755 fakedyld/dyld ramdisk/usr/lib/
	install -m644 mount_cores/mount_cores.* ramdisk/
	install -m755 payload_dylib/payload.dylib ramdisk/
	install -m755 payload/payload ramdisk/
	hdiutil create -fs HFS+ -layout NONE -format UDRW -size 1m -srcdir ramdisk -volname plooshra1nrd ramdisk.dmg

loader.dmg: palera1nLoader.ipa
	rm -rf loader.dmg palera1nLoader
	mkdir palera1nLoader
	cd palera1nLoader && unzip palera1nLoader.ipa
	fakeroot hdiutil create -size 2m -layout NONE -format ULFO -volname palera1nLoader -srcfolder ./palera1nLoader/Payload -fs HFS+ ./loader.dmg
	rm -rf palera1nLoader

binpack.dmg: binpack.tar loader.dmg
	rm -rf binpack.dmg binpack
	mkdir binpack
	fakeroot tar -C binpack --preserve-permissions -xf binpack.tar
	rm -rf binpack/usr/share cores
	ln -sf /cores/jbloader binpack/usr/sbin/p1ctl
	mkdir -p binpack/Applications
	mkdir -p binpack/usr/lib
	mkdir -p binpack/Library/Frameworks/CydiaSubstrate.framework
	mkdir -p binpack/Library/LaunchDaemons
	cp -a $(ROOT)/dropbear-plist/*.plist binpack/Library/LaunchDaemons
	cp rootlesshooks/rootlesshooks.dylib binpack/usr/lib
	cp loader.dmg binpack
	cp rootlesshooks/ellekit_build/Build/Products/Release-iphoneos/libellekit.dylib binpack/usr/lib
	ln -s ../../../usr/lib/libellekit.dylib binpack/Library/Frameworks/CydiaSubstrate.framework/CydiaSubstrate
	fakeroot hdiutil create -size 8m -layout NONE -format UDZO -imagekey zlib-level=9 -srcfolder ./binpack -volname plooshra1nfs -fs HFS+ ./binpack.dmg
	rm -rf binpack

$(patsubst %, %-all, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) all

clean: $(patsubst %, %-clean, $(SUBDIRS))
	rm -rf binpack.dmg ramdisk.dmg palera1nLoader binpack ramdisk

$(patsubst %, %-clean, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) clean
	rm -rf ramdisk ramdisk.dmg
