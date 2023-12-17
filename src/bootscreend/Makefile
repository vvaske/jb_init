OBJECTS  = bootscreend.o
CFLAGS  += -miphoneos-version-min=15.0 -arch arm64 -Os -I.
LDFLAGS += $(SRC)/APFS.tbd -framework IOKit -framework CoreFoundation -framework Foundation -lobjc -framework CoreGraphics

all: bootscreend

bootscreend: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o bootscreend
	$(STRIP) bootscreend
	ldid -Sent.xml -Idev.mineek.bootscreend bootscreend

%.o: %.c
	$(CC) -c $(CFLAGS) $<

%.o: %.m
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f *.o bootscreend

.PHONY: clean all bootscreend