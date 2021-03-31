#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files_and_strs_manage.h"

char* file_upload(char* filename, size_t *file_size) {
    if (filename == NULL) {
        printf("No filepath error\n");
        return NULL;
    }

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Failed to open file\n");
        return NULL;
    }
    struct stat st;
    stat(filename, &st);

    *file_size = st.st_size;

    char *region = mmap(NULL, *file_size, PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (region == MAP_FAILED) {
        printf("Failed to open file\n");
        close(fd);
        return NULL;
    }
    region[strlen(region) - 1] = '\0';

    close(fd);
    return region;
}

int file_delete(char* region, size_t file_size) {
    if (region == NULL) {
        printf("Nothing to unmap\n");
        return -1;
    }
    if (munmap(region, file_size) == -1) {
        printf("Failed to delete file from DRAM\n");
        return -1;
    }

    return 0;
}

char **create_substr_arr(char *s_region, size_t *num) {
    if (s_region == NULL) {
        printf("Empty substring file\n");
        return NULL;
    }

    *num = 1;
    char *temp = s_region;
    for(;;) {
        temp = strchr(temp, '\n');
        if (temp == NULL) {
            break;
        }
        temp[0] = '\0';
        ++temp;
        ++*num;
    }

    char **substrs = (char**)calloc(*num, sizeof(char*));
    if (substrs == NULL) {
        printf("Failed to allocate memory for substrings\n");
        return NULL;
    }
    temp = s_region;
    for (size_t i = 0; i < *num; ++i) {
        substrs[i] = temp;
        temp += strlen(substrs[i]) + 1;
    }

    return substrs;
}

int substr_search(char *o_region, char *s_region) {
    if (o_region == NULL || s_region == NULL) {
        return -1;
    }

    size_t o_len = strlen(o_region);
    if (o_len == 0) {
        return -1;
    }

    size_t s_len = strlen(s_region);
    if (s_len == 0) {
        return -1;
    }

    if (s_len > o_len) {
        return -1;
    }

    size_t len_diff = o_len - s_len + 1;
    for (size_t i = 0; i < len_diff; ++i) {
        if (strncmp(o_region + i, s_region, s_len) == 0) {
            return i;
        }
    }

    return -2;
}
