#include <stdlib.h>
#include <string.h>
#include "search_substr_c.h"
#include "files_and_strs_manage.h"

int* one_thread_search(char *o_region, char **substrs, size_t substrs_num) {
    if (o_region == NULL || substrs == NULL || substrs_num == 0) {
        return NULL;
    }

    int *substrs_offset = (int*)calloc(substrs_num, sizeof(int));
    if (substrs_offset == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < substrs_num; ++i) {
        substrs_offset[i] = substr_search(o_region, substrs[i]);
    }

    return substrs_offset;
}
