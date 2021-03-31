#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "search_substr_p.h"
#include "files_and_strs_manage.h"

void *thread_routine(void *args) {
    data_t *data = (data_t*)args;
    data->substrs_offset[data->offset] = substr_search(data->o_region, data->substrs[data->offset]);
    return NULL;
}

int* multi_thread_search(char *o_region, char **substrs, size_t substrs_num) {
    if (o_region == NULL || substrs == NULL || substrs_num == 0) {
        return NULL;
    }

    int *substrs_offset = (int*)calloc(substrs_num, sizeof(int));
    if (substrs_offset == NULL) {
        return NULL;
    }

    size_t cores_num = sysconf(_SC_NPROCESSORS_ONLN);
    if (cores_num < 1) {
        free(substrs_offset);
        return NULL;
    }

    pthread_t *threads = (pthread_t*)calloc(cores_num, sizeof(pthread_t));
    if (threads == NULL) {
        free(substrs_offset);
        return NULL;
    }

    data_t *data_arr = (data_t*)calloc(cores_num, sizeof(data_t));
    if (data_arr == NULL) {
        free(substrs_offset);
        free(threads);
        return NULL;
    }
    for (size_t i = 0; i < cores_num; ++i) {
        data_arr[i].o_region = o_region;
        data_arr[i].substrs = substrs;
        data_arr[i].substrs_offset = substrs_offset;
        data_arr[i].offset = 0;
    }

    size_t i = 0;
    for (; i < substrs_num / cores_num; ++i) {
        for (size_t j = 0; j < cores_num; ++j) {
            data_arr[j].offset = i * cores_num + j;
            int errflag = pthread_create(&threads[j], NULL, thread_routine, (void*)&data_arr[j]);
            if (errflag != 0) {
                for (size_t k = j; k > j; --k) {
                    pthread_cancel(threads[k]);
                }
                free(threads);
                free(substrs_offset);
                free(data_arr);
                return NULL;
            }
        }
        for (size_t j = 0; j < cores_num; ++j) {
            int errflag = pthread_join(threads[j], NULL);
            if (errflag != 0) {
                for (size_t k = j; k > j; --k) {
                    pthread_cancel(threads[k]);
                }
                free(threads);
                free(substrs_offset);
                free(data_arr);
                return NULL;
            }
        }

    }

    for (size_t j = 0; j < substrs_num % cores_num; ++j) {
        data_arr[j].offset = i * cores_num + j;
        int errflag = pthread_create(&threads[j], NULL, thread_routine, (void*)&data_arr[j]);
        if (errflag != 0) {
            for (size_t k = j; k > j; --k) {
                pthread_cancel(threads[k]);
            }
            free(threads);
            free(substrs_offset);
            free(data_arr);
            return NULL;
        }
    }
    for (size_t j = 0; j < substrs_num % cores_num; ++j) {
        int errflag = pthread_join(threads[j], NULL);
        if (errflag != 0) {
            for (size_t k = j; k > j; --k) {
                pthread_cancel(threads[k]);
            }
            free(threads);
            free(substrs_offset);
            free(data_arr);
            return NULL;
        }
    }

    free(threads);
    free(data_arr);
    return substrs_offset;
}
