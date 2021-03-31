#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "files_and_strs_manage.h"
#include "search_substr_c.h"
#include "search_substr_p.h"

int print_res(int count) {
    if (count == -1) {
        printf("ERROR occured\n");
        return -1;
    }
    if (count == -2) {
        printf("No matches were found\n");
        return -2;
    }
    printf("Starting offset in origin string: %d\n", count);
    return 0;
}

int main (int argc, char *argv[]) {
    char *filename;
    size_t fp_size = 0;

    if (argc < 2) {
        fp_size = 100;
        filename = (char*)calloc(fp_size, sizeof(char));
        if (filename == NULL) {
            return -1;
        }
        printf("Enter origin file path\n");
        fgets(filename, fp_size, stdin);
        filename[strlen(filename) - 1] = '\0';
    } else {
        filename = argv[1];
    }
    size_t o_file_size = 0;
    char *o_region = file_upload(filename, &o_file_size);
    if (o_region == NULL) {
        if (fp_size != 0) {
            free(filename);
        }
        return -1;
    }

    if (fp_size != 0) {
        fp_size = 0;
        free(filename);
    }

    if (argc < 3) {
        fp_size = 100;
        filename = (char*)calloc(fp_size, sizeof(char));
        if (filename == NULL) {
            file_delete(o_region, o_file_size);
            return -1;
        }
        printf("Enter substring file path\n");
        fgets(filename, fp_size, stdin);
        filename[strlen(filename) - 1] = '\0';
    } else {
        filename = argv[2];
    }
    size_t s_file_size = 0;
    char *s_region = file_upload(filename, &s_file_size);
    if (s_region == NULL) {
        if (fp_size != 0) {
            free(filename);
        }
        file_delete(o_region, o_file_size);
        return -1;
    }

    if (fp_size != 0) {
        free(filename);
    }

    size_t substrs_num = 0;
    char **substrs = create_substr_arr(s_region, &substrs_num);
    if (substrs == NULL) {
        printf("Failed to allocate memory for substr array\n");
        file_delete(o_region, o_file_size);
        file_delete(s_region, s_file_size);
        free(substrs);
        return -1;
    }

    struct timespec t_start_p;
    struct timespec t_end_p;
    clock_gettime(CLOCK_REALTIME, &t_start_p);
    int *substrs_offset_p = multi_thread_search(o_region, substrs, substrs_num);
    clock_gettime(CLOCK_REALTIME, &t_end_p);
    if (substrs_offset_p == NULL) {
        printf("Failed to make multi thread search\n");
        file_delete(o_region, o_file_size);
        file_delete(s_region, s_file_size);
        free(substrs);
        return -1;
    }
    double t_time_p = (double)(t_end_p.tv_sec - t_start_p.tv_sec) + (double)(t_end_p.tv_nsec - t_start_p.tv_nsec) / 1000000000.;
    printf("Multi thread time: %fs.\n", t_time_p);
    for (size_t i = 0; i < substrs_num; ++i) {
        printf("%zu)", i + 1);
        if (print_res(substrs_offset_p[i]) == -1) {
            file_delete(o_region, o_file_size);
            file_delete(s_region, s_file_size);
            free(substrs_offset_p);
            free(substrs);
            return -1;
        }
    }
    printf("_______________________________________\n\n");

    struct timespec t_start_c;
    struct timespec t_end_c;
    clock_gettime(CLOCK_REALTIME, &t_start_c);
    int *substrs_offset_c = one_thread_search(o_region, substrs, substrs_num);
    clock_gettime(CLOCK_REALTIME, &t_end_c);
    if (substrs_offset_c == NULL) {
        printf("Failed to make one thread search\n");
        file_delete(o_region, o_file_size);
        file_delete(s_region, s_file_size);
        free(substrs_offset_p);
        free(substrs);
        return -1;
    }
    double t_time_c = (double)(t_end_c.tv_sec - t_start_c.tv_sec) + (double)(t_end_c.tv_nsec - t_start_c.tv_nsec) / 1000000000.;
    printf("One thread time: %fs.\n", t_time_c);
    for (size_t i = 0; i < substrs_num; ++i) {
        printf("%zu)", i + 1);
        if (print_res(substrs_offset_c[i]) == -1) {
            file_delete(o_region, o_file_size);
            file_delete(s_region, s_file_size);
            free(substrs_offset_p);
            free(substrs_offset_c);
            free(substrs);
            return -1;
        }
    }
    printf("_______________________________________\n\n");

    for (size_t i = 0; i < substrs_num; ++i) {
        if (substrs_offset_c[i] != substrs_offset_p[i]) {
            printf("One thread and multi thread realizations answers are different\n");
            file_delete(o_region, o_file_size);
            file_delete(s_region, s_file_size);
            free(substrs);
            free(substrs_offset_p);
            free(substrs_offset_c);
            return -1;
        }
    }

    printf("One thread and multi thread realization answers are the same\n");
    file_delete(o_region, o_file_size);
    file_delete(s_region, s_file_size);
    free(substrs);
    free(substrs_offset_p);
    free(substrs_offset_c);
    return 0;
}
