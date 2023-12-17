#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOTypes.h>
#include <dlfcn.h>
#include <CoreGraphics/CoreGraphics.h>
#define WHITE 0xffffffff
#define BLACK 0x00000000
typedef IOReturn IOMobileFramebufferReturn;
typedef struct __IOMobileFramebuffer *IOMobileFramebufferRef;
typedef CGSize IOMobileFramebufferDisplaySize;
typedef struct __IOSurface *IOSurfaceRef;
void *base = NULL;
int bytesPerRow = 0;
int height = 0;
int width = 0;

int init_display() {
    void *IOMobileFramebuffer = dlopen("/System/Library/PrivateFrameworks/IOMobileFramebuffer.framework/IOMobileFramebuffer", RTLD_LAZY);
    if (!IOMobileFramebuffer) {
        printf("bootscreend: failed to open IOMobileFramebuffer\n");
        return 1;
    }
    IOMobileFramebufferReturn (*IOMobileFramebufferGetMainDisplay)(IOMobileFramebufferRef *pointer) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferGetMainDisplay");
    IOMobileFramebufferRef display;
    IOMobileFramebufferGetMainDisplay(&display);
    IOMobileFramebufferReturn (*IOMobileFramebufferGetDisplaySize)(IOMobileFramebufferRef pointer, IOMobileFramebufferDisplaySize *size) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferGetDisplaySize");
    IOMobileFramebufferDisplaySize size;
    IOMobileFramebufferGetDisplaySize(display, &size);
    IOMobileFramebufferReturn (*IOMobileFramebufferGetLayerDefaultSurface)(IOMobileFramebufferRef pointer, int surface, IOSurfaceRef *buffer) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferGetLayerDefaultSurface");
    IOSurfaceRef buffer;
    IOMobileFramebufferGetLayerDefaultSurface(display, 0, &buffer);
    IOMobileFramebufferReturn (*IOMobileFramebufferSwapBegin)(IOMobileFramebufferRef pointer, int *token) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferSwapBegin");
    IOMobileFramebufferReturn (*IOMobileFramebufferSwapEnd)(IOMobileFramebufferRef pointer) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferSwapEnd");
    IOMobileFramebufferReturn (*IOMobileFramebufferSwapSetLayer)(IOMobileFramebufferRef pointer, int layerid, IOSurfaceRef buffer, CGRect bounds, CGRect frame, int flags) = dlsym(IOMobileFramebuffer, "IOMobileFramebufferSwapSetLayer");
    dlclose(IOMobileFramebuffer);
    printf("got display %p\n", display);
    width = size.width;
    height = size.height;
    printf("width: %d, height: %d\n", width, height);
    void *IOSurface = dlopen("/System/Library/Frameworks/IOSurface.framework/IOSurface", RTLD_LAZY);
    void (*IOSurfaceLock)(IOSurfaceRef buffer, int something, int something2) = dlsym(IOSurface, "IOSurfaceLock");
    void *(*IOSurfaceGetBaseAddress)(IOSurfaceRef buffer) = dlsym(IOSurface, "IOSurfaceGetBaseAddress");
    int (*IOSurfaceGetBytesPerRow)(IOSurfaceRef buffer) = dlsym(IOSurface, "IOSurfaceGetBytesPerRow");
    void (*IOSurfaceUnlock)(IOSurfaceRef buffer, int something, int something2) = dlsym(IOSurface, "IOSurfaceUnlock");
    IOSurfaceRef (*IOSurfaceCreate)(CFDictionaryRef properties) = dlsym(IOSurface, "IOSurfaceCreate");
    dlclose(IOSurface);

    // create buffer
    CFMutableDictionaryRef properties = CFDictionaryCreateMutable(NULL, 0, NULL, NULL);
    CFDictionarySetValue(properties, CFSTR("IOSurfaceIsGlobal"), kCFBooleanTrue);
    CFDictionarySetValue(properties, CFSTR("IOSurfaceWidth"), CFNumberCreate(NULL, kCFNumberIntType, &width));
    CFDictionarySetValue(properties, CFSTR("IOSurfaceHeight"), CFNumberCreate(NULL, kCFNumberIntType, &height));
    CFDictionarySetValue(properties, CFSTR("IOSurfacePixelFormat"), CFNumberCreate(NULL, kCFNumberIntType, &(int){ 0x42475241 }));
    CFDictionarySetValue(properties, CFSTR("IOSurfaceBytesPerElement"), CFNumberCreate(NULL, kCFNumberIntType, &(int){ 4 }));
    buffer = IOSurfaceCreate(properties);
    printf("created buffer at: %p\n", buffer);
    IOSurfaceLock(buffer, 0, 0);
    printf("locked buffer\n");
    base = IOSurfaceGetBaseAddress(buffer);
    printf("got base address at: %p\n", base);
    bytesPerRow = IOSurfaceGetBytesPerRow(buffer);
    printf("got bytes per row: %d\n", bytesPerRow);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int offset = i * bytesPerRow + j * 4;
            *(int *)(base + offset) = 0xFFFFFFFF;
        }
    }
    printf("wrote to buffer\n");
    IOSurfaceUnlock(buffer, 0, 0);
    printf("unlocked buffer\n");

    int token;
    IOMobileFramebufferSwapBegin(display, &token);
    IOMobileFramebufferSwapSetLayer(display, 0, buffer, (CGRect){ 0, 0, width, height }, (CGRect){ 0, 0, width, height }, 0);
    IOMobileFramebufferSwapEnd(display);
    return 0;
}

int main(int argc, char **argv) {
    init_display();
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int offset = i * bytesPerRow + j * 4;
            *(int *)(base + offset) = 0x00000000;
        }
    }
    FILE *image = fopen("/cores/binpack/splash.png", "rb");
    if (image == NULL) {
        printf("bootscreend: failed to open image\n");
        return 1;
    }
    fseek(image, 0, SEEK_END);
    int imageSize = ftell(image);
    fseek(image, 0, SEEK_SET);
    char *imageData = malloc(imageSize);
    fread(imageData, 1, imageSize, image);
    fclose(image);
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, imageData, imageSize, NULL);
    CGImageRef cgImage = CGImageCreateWithPNGDataProvider(provider, NULL, false, kCGRenderingIntentDefault);
    CGDataProviderRelease(provider);
    CGContextRef context = CGBitmapContextCreate(base, width, height, 8, bytesPerRow, CGColorSpaceCreateDeviceRGB(), kCGImageAlphaPremultipliedFirst);
    CGContextDrawImage(context, CGRectMake(0, 0, width, height), cgImage);
    CGContextRelease(context);
    CGImageRelease(cgImage);
    printf("bootscreend: done\n");
    sleep(5);
    return 0;
}
