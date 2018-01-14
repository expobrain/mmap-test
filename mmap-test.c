#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef uint16_t row_t[16];

static const size_t FILE_SIZE = 256 * 1024 * 1024;

int main(int argc, char **argv)
{
    (void) argc; (void) argv;
    char *path = "array.data";
    printf("Open a file at %s\n", path);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return -1;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat");
        return -1;
    }

    int mmap_flags = MAP_SHARED;

    #ifdef __linux__
    mmap_flags |= MAP_POPULATE;
    #endif

    void *addr = mmap(NULL, st.st_size, PROT_READ, mmap_flags, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return -1;
    };

    close(fd);

    if (madvise(addr, st.st_size, MADV_SEQUENTIAL) < 0) {
        perror("madvise");
        return -1;
    };

    row_t *row = addr;
    long counter = 0;

    clock_t begin = clock();
    for(size_t i = 0; i < FILE_SIZE; i += 2*16) {
        for (size_t c = 0; c < 16; c++) {
            counter += (*row)[c];
        }
        row++;
    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f sec\n", time_spent);
    printf("%ld\n", counter);

    row = addr;
    counter = 0;
    begin = clock();
    for(size_t i = 0; i < FILE_SIZE; i += 2*16) {
        for (size_t c = 0; c < 16; c++) {
            counter += (*row)[c];
        }
        row++;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f sec\n", time_spent);
    printf("%ld\n", counter);

    row = addr;
    counter = 0;
    begin = clock();
    for(size_t i = 0; i < FILE_SIZE; i += 2*16) {
        for (size_t c = 0; c < 16; c++) {
            /* counter += 1; */
            counter += (*row)[c];
        }
        row++;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f sec\n", time_spent);
    printf("%ld\n", counter);


    if (munmap(addr, st.st_size) == -1) {
        perror("munmap");
        return -1;
    };

    return 0;
}
