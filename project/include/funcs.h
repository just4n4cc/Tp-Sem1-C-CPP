#ifndef FUNCS_H
#define FUNCS_H

int split_manager(order *orders, int num, order *sub11 , size_t *pnum11, order *sub12, size_t *pnum12);
float get_order_data(order *orders, size_t offset, o_data O_DATA);

float greedy_split_orders(order *orders, int num, order *sub1, size_t *pnum1, order *sub2, size_t *pnum2, o_data O_DATA);

int insert_sort(order *orders, size_t num, o_data O_DATA);

size_t binary_search(order *orders, size_t left, size_t right, float val, o_data O_DATA);

int print_orders(order *orders, size_t num);

#endif  // FUNCS_H
