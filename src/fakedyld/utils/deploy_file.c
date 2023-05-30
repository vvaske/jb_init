#include <fakedyld/fakedyld.h>

#define MAX_DECOMPRESSED_SIZE 8388608

int deploy_file(
    unsigned char buf[],
    unsigned int bufsize,
    int decompressed_size,
    char* path, 
    int mode
    ) {
    char* decomp_buf = mmap(NULL, (size_t)decompressed_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS , 0, 0);
    if (decomp_buf == MAP_FAILED) {
        LOG("cannot allocate memory for %s decompression", path);
        spin();
    }
    int ret = lz4dec(buf, decomp_buf, bufsize, decompressed_size);
    if (ret != decompressed_size) {
        LOG("LZ4 decompression for %s failed, ret: %d", path, ret);
        spin();
    }
    int dstfd = open(path, O_CREAT, mode);
    if (dstfd == -1) {
        LOG("failed to open destination file %s: %d (%s)\n", path, errno, strerror(errno));
        spin();
    }
    ssize_t written = write(dstfd, decomp_buf, decompressed_size);
    if (written == -1) {
        LOG("writing output %s failed: %d (%s)\n", path, errno, strerror(errno));
        spin();
    }
    close(dstfd);
    munmap(decomp_buf, decompressed_size);
    return 0;
}
