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
	ln -sf /payload ramdisk/sbin/launchd
	hdiutil create -fs HFS+ -layout NONE -format UDRW -size 768K -srcdir ramdisk -volname plooshra1nrd ramdisk.dmg

loader.dmg: palera1nLoader.ipa
	rm -rf loader.dmg palera1nLoader
	mkdir palera1nLoader
	cd palera1nLoader && unzip ../palera1nLoader.ipa
	hdiutil create -size 2m -layout NONE -format ULFO -volname palera1nLoader -srcfolder ./palera1nLoader/Payload -fs HFS+ ./loader.dmg
	rm -rf palera1nLoader

binpack.dmg: binpack.tar loader.dmg
	sudo rm -rf binpack.dmg binpack
	mkdir binpack
	sudo tar -C binpack --preserve-permissions -xf binpack.tar
	sudo rm -rf binpack/usr/share cores
	sudo ln -sf /cores/jbloader binpack/usr/sbin/p1ctl
	sudo mkdir -p binpack/Applications
	sudo mkdir -p binpack/usr/lib
	sudo mkdir -p binpack/Library/Frameworks/CydiaSubstrate.framework
	sudo mkdir -p binpack/Library/LaunchDaemons
	sudo cp -a $(ROOT)/shared/*.plist binpack/Library/LaunchDaemons
	sudo chown 0:0 binpack/Library/LaunchDaemons/*.plist
	sudo chmod 644 binpack/Library/LaunchDaemons/*.plist
	sudo cp rootlesshooks/rootlesshooks.dylib binpack/usr/lib
	sudo cp loader.dmg binpack
	sudo cp rootlesshooks/ellekit_build/Build/Products/Release-iphoneos/libellekit.dylib binpack/usr/lib
	sudo ln -s ../../../usr/lib/libellekit.dylib binpack/Library/Frameworks/CydiaSubstrate.framework/CydiaSubstrate
	hdiutil create -size 10m -layout NONE -format UDZO -imagekey zlib-level=9 -srcfolder ./binpack -volname plooshra1nfs -fs HFS+ ./binpack.dmg
	sudo rm -rf binpack

$(patsubst %, %-all, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) all

clean: $(patsubst %, %-clean, $(SUBDIRS))
	rm -rf binpack.dmg ramdisk.dmg palera1nLoader binpack ramdisk

$(patsubst %, %-clean, $(SUBDIRS)):
	$(MAKE) -C $$(echo $@ | cut -d- -f1) clean
	rm -rf ramdisk ramdisk.dmg
