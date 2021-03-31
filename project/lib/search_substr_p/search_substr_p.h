#ifndef SEARCH_SUBSTR_P_H
#define SEARCH_SUBSTR_P_H

typedef struct data_t {
    char *o_region;
    char **substrs;
    int *substrs_offset;
    size_t offset;
} data_t;

void *thread_routine(void *args);
int* multi_thread_search(char *o_region, char **substrs, size_t substrs_num);

#endif  // SEARCH_SUBSTR_P_H
