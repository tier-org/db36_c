#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static const int kMemAlignSize = 4096;
static const int kDataSize = 4096;

int fd;

int open_file(const char *file) {
  fd = open(file, O_RDWR|O_CREAT|O_DIRECT, 0666);
  if (fd < 0) {
    perror("open error!");
    return -1;
  }
  printf("fd open success!\n");
  return 0;
}

void close_file() {
  close(fd);
}

void test_write() {
  printf("-------------- write test --------------\n");
  char *buffer = nullptr;
  int code = posix_memalign((void **) &buffer, kMemAlignSize, kDataSize);
  assert(code == 0);

  strcpy(buffer, "hello, world!");
  printf("buffer: %s\n", buffer);
  code = pwrite64(fd, buffer, kDataSize, 0);
  if (code != kDataSize) {
    printf("write failed %d\n", code);
  } else {
    printf("write success %d\n", code);
  }
  free(buffer);
}

void test_read() {
  printf("-------------- read test --------------\n");
  char *buffer = nullptr;
  int code = posix_memalign((void **) &buffer, kMemAlignSize, kDataSize);
  assert(code == 0);

  code = pread64(fd, buffer, kDataSize, 0);
  if (code != kDataSize) {
    printf("read failed %d\n", code);
  } else {
    printf("read success %d\n", code);
    printf("buffer: ");
    for(int i = 0 ; i < 1024; i++) {
      printf("%c", buffer[i]);
    }
    printf("\n");
  }
  free(buffer);
}

int main() {
  static const char *file= "data/o_direct_test_file";

  int code = open_file(file);
  assert(code == 0);

  // test_write
  test_write();

  // test_read
  test_read();

  close_file();
  return 0;
}