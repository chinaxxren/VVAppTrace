
#include "VVLog.h"

#include <stdio.h>
#include <pthread.h>
#include <mach/mach_time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

static size_t block_size = 16 * 1024 * 1024;  // 16MB
static int fd_ = 0;
static char *file_start_ = NULL;
static char *file_cur_ = NULL;
static size_t cur_size_ = 0;

bool vv_log_open(const char *log_path);

void vv_log_close(void);

bool vv_log_add_line(const char *line);

bool vv_log_open(const char *log_path) {
    vv_log_close();

    remove(log_path);
    fd_ = open(log_path, O_CREAT | O_RDWR, (mode_t) 0600);
    if (fd_ == -1) {
        printf("open file failed");
        return false;
    }
    lseek(fd_, block_size - 1, SEEK_SET);
    ssize_t wrote_bytes = write(fd_, "", 1);
    if (wrote_bytes != 1) {
        printf("wrote error");
        vv_log_close();
        return false;
    }
    lseek(fd_, 0, SEEK_SET);

    file_start_ = (char *) mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (file_start_ == MAP_FAILED) {
        printf("map failed");
        vv_log_close();
        return false;
    }
    file_cur_ = file_start_;
    return true;
}

void vv_log_close() {
    if (file_start_) {
        munmap(file_start_, block_size);
    }

    if (fd_) {
        close(fd_);
    }

    fd_ = 0;
    file_start_ = NULL;
    file_cur_ = NULL;
    cur_size_ = 0;
}

bool vv_log_add_line(const char *line) {
    if (!file_cur_)
        return false;

    size_t len = strlen(line);
    if (cur_size_ + len + 1 > block_size) {
        printf("file full");
        return false;
    }

    memcpy(file_cur_, line, len);
    file_cur_ += len;
    memcpy(file_cur_, (const char *) "\n", 1);
    file_cur_ += 1;

    cur_size_ += len + 1;
    return true;
}

static char *base_path;
static int file_cout;
static char path[1024];

void log_base_path(char *ios_path) {
    base_path = ios_path;
}

char *get_file_path() {
    sprintf(path, "%s/trace_%d.appletrace", base_path, file_cout);
    return path;
}

bool log_open() {
    char *file_path = get_file_path();
    if (!vv_log_open(file_path)) {
        return false;
    }

    file_cout++;

    return true;
}

void log_add_line(const char *line) {
    if (vv_log_add_line(line))
        return;

    printf("will map a new file");

    // map a new file
    if (!log_open())
        return;

    if (!vv_log_add_line(line)) {
        // error
        printf("still error add line");
    }
}
