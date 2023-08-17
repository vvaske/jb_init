#include <fakedyld/fakedyld.h>

void read_file(char* path, memory_file_handle_t* handle) {
    struct stat st;
    int ret;
    ret = stat(path, &st);
    if (ret != 0) {
        LOG("stat file %s failed: %d", path, ret);
        spin();
    }
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        LOG("open file %s failed", path);
        spin();
    }
    handle->file_p = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS, 0, 0);
    if (handle->file_p == MAP_FAILED) {
        LOG("mmap memory for reading file %s failed", path);
        spin();
    }
    ssize_t didRead = read(fd, handle->file_p, st.st_size);
    handle->file_len = st.st_size;
    if (didRead != st.st_size) {
        LOG("unexpected data size %llu while reading file %s of size %llu", didRead, path, st.st_size);
        spin();
    }
    close(fd);
}

void write_file(char* path, memory_file_handle_t* handle) {
    int fd = open(path, O_WRONLY, 0755);
    if (fd == -1) {
        LOG("open file %s failed", path);
        spin();
    }
    int ret;
    ssize_t didWrite = write(fd, handle->file_p, handle->file_len);
    if (didWrite != handle->file_len) {
        LOG("unexpected data size %llu while writing file %s of size %llu", didWrite, path, handle->file_len);
        spin();
    }
    munmap(handle->file_p, handle->file_len);
    handle->file_len = 0;
    close(fd);
}
