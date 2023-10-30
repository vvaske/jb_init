CFLAGS += -miphoneos-version-min=14.0 -std=gnu17 -Os
LDFLAGS += -Wl,-dead_strip -Wno-unused-command-line-argument
OBJDIR = obj
C_SRC = $(wildcard *.c)
OBJC_SRC = $(wildcard *.m)
OBJCFLAGS = -fobjc-arc 
ELLEKIT = ellekit_build/Build/Products/Release-iphoneos/libellekit.dylib
LIBS = -lobjc -framework Foundation
OBJS = $(patsubst %,$(OBJDIR)/%,$(OBJC_SRC:.m=.m.o) $(C_SRC:.c=.c.o))

all: rootlesshooks.dylib

$(OBJDIR):
	@mkdir -p $(OBJDIR)

$(OBJDIR)/%.m.o: %.m $(OBJDIR)
	$(OBJC) $(CFLAGS) $(OBJCFLAGS) -c -o $@ $<

$(OBJDIR)/%.c.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ELLEKIT): ../../.git/modules/src/rootlesshooks/ellekit/refs/heads/main
	cd ellekit; \
		CC="" CXX="" LD="" xcodebuild build -arch arm64 -sdk iphoneos -scheme ellekit -configuration Release -derivedDataPath ../ellekit_build \
		IPHONEOS_DEPLOYMENT_TARGET=14.0 \
		CODE_SIGNING_ALLOWED=NO \
		LD_DYLIB_INSTALL_NAME="@rpath/CydiaSubstrate.framework/CydiaSubstrate"
	$(LDID) -S $(ELLEKIT);

rootlesshooks.dylib: $(OBJS) $(ELLEKIT)
	xcrun -sdk iphoneos swiftc -Osize $(OBJS) $(ELLEKIT) \
		-lto=llvm-thin \
		--target=arm64-apple-ios14.0 \
		-sdk $(shell xcrun -sdk iphoneos --show-sdk-path) \
		-Xlinker -rpath -Xlinker /var/jb/Library/Frameworks \
		-Xlinker -rpath -Xlinker /var/jb/usr/lib \
		-Xlinker -rpath -Xlinker /cores/binpack/Library/Frameworks \
		-Xlinker -rpath -Xlinker /cores/binpack/usr/lib \
		-Xlinker -install_name -Xlinker /cores/binpack/usr/lib/rootlesshooks.dylib \
		-Xlinker -dead_strip \
		-emit-library \
		-framework Foundation \
		-framework CoreFoundation \
		-o rootlesshooks.dylib
	ldid -S rootlesshooks.dylib 

clean:
	rm -rf rootlesshooks.dylib* ellekit_build

.PHONY: all clean
