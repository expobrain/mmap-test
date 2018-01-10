#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>

typedef uint16_t row_t[16];

void *mmap_file(char* path, size_t *size_mmaped)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        perror("stat");
        return NULL;
    }

    *size_mmaped = st.st_size;

    void *addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return NULL;
    };

    close(fd);

    return addr;
}

void unmmap_file(void *addr, size_t size_mmaped)
{
    if (munmap(addr, size_mmaped) == -1) {
        perror("munmap");
    };
}

void walk_table_rows(void *addr, size_t table_size, long *counter)
{
    row_t *row = addr;

    /* clock_t begin = clock(); */
    for(size_t i = 0; i < table_size; i++) {
        for (size_t c = 0; c < 16; c++) {
            *counter += (*row)[c];
        }
        row++;
    }

    /* clock_t end = clock(); */
    /* double time_spent = (double)(end - begin) / CLOCKS_PER_SEC; */
    /* printf("%f sec\n", time_spent); */
}

int main(int argc, char **argv)
{
    (void) argc; (void) argv;

    const size_t file_num = 100;
    const size_t table_file_size = 1 * 1024 * 1024;

    void *addrs[file_num];

    clock_t begin = clock();
    size_t size_mmaped = 0;
    for (size_t i = 1; i <= file_num; i++ ) {
        char path[sizeof "arrayXXXX.data"];
        sprintf(path, "array%04ld.data", i);

        void *addr = mmap_file(path, &size_mmaped);
        if (!addr)
            return -1;
        addrs[i - 1] = addr;
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%f sec spent mmaping\n", time_spent);

    long counter = 0;
    begin = clock();
    for (size_t i = 1; i <= file_num; i++ ) {
        walk_table_rows(addrs[i - 1], table_file_size, &counter);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("counter: %ld\n", counter);
    printf("%f sec spent walking\n", time_spent);

    for (size_t i = 1; i <= file_num; i++ ) {
        unmmap_file(addrs[i - 1], size_mmaped);
    }

    return 0;
}
