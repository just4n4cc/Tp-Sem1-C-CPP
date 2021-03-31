#ifndef FILES_AND_STRS_MANAGE_H
#define FILES_AND_STRS_MANAGE_H

char* file_upload(char* filename, size_t *file_size);
int file_delete(char* region, size_t file_size);
char **create_substr_arr(char *s_region, size_t *num);
int substr_search(char *o_region, char *s_region);

#endif  // FILES_AND_STRS_MANAGE_H
