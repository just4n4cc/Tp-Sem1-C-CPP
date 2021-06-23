#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "order_struct.h"
#include "funcs.h"

int split_manager(order *orders, int num, order *sub11, size_t *pnum11, order *sub12, size_t *pnum12) {
    if (orders == NULL || num < 2 || sub11 == NULL || sub12 == NULL || pnum11 == NULL || pnum12 == NULL) {
        return -1;
    }
    if (sub11 == sub12 || pnum11 == pnum12) {
        return -1;
    }

    order *sub21 = (order*)calloc(num, sizeof(order));
    if (sub21 == NULL) {
        return -1;
    }

    order *sub22 = (order*)calloc(num, sizeof(order));
    if (sub22 == NULL) {
        free(sub21);
        return -1;
    }

    size_t num21 = 0;
    size_t num22 = 0;

    float w_diff = greedy_split_orders(orders, num, sub11, pnum11, sub12, pnum12, O_WEIGHT);

    float p_diff = greedy_split_orders(orders, num, sub21, &num21, sub22, &num22, O_PRICE);

    printf("----Your original order list----\n");
    print_orders(orders, num);

    printf("\nOrders splitted by weight\n");
    printf("Weigth difference: %.3f\n", w_diff);
    printf("----------Order list #1----------\n");
    print_orders(sub11, *pnum11);
    printf("----------Order list #2----------\n");
    print_orders(sub12, *pnum12);

    printf("\nOrders splitted by price\n");
    printf("Price difference: %.2f\n", p_diff);
    printf("----------Order list #1----------\n");
    print_orders(sub21, num21);
    printf("----------Order list #2----------\n");
    print_orders(sub22, num22);

    char choice = '\0';
    do {
        printf("If you want to split by weight, press 1, otherwise hit 2:\n");
        scanf(" %c", &choice);
    } while (choice != '1' && choice != '2');

    if (choice == '2') {
        *pnum11 = num21;
        *pnum12 = num22;
        for (size_t i = 0; i < num21; ++i) {
            sub11[i] = sub21[i];
        }
        for (size_t i = 0; i < num22; ++i) {
            sub12[i] = sub22[i];
        }
    }

    free(sub21);
    free(sub22);
    return 0;
}

float get_order_data(order *orders, size_t offset, o_data O_DATA) {
    if (orders == NULL) {
        return -1;
    }
    if (O_DATA == O_PRICE) {
        return orders[offset].price;
    }
    return orders[offset].weight;
}

float greedy_split_orders(order *orders, int num, order *sub1, size_t *pnum1, order *sub2, size_t *pnum2, o_data O_DATA) {
    if (orders == NULL || num < 2 || sub1 == NULL || sub2 == NULL || pnum1 == NULL || pnum2 == NULL) {
        return -1;
    }
    if (sub1 == sub2 || pnum1 == pnum2) {
        return -1;
    }

    insert_sort(orders, num, O_DATA);
    float sum1 = 0.;
    float sum2 = 0.;
    size_t is_present1 = 0;
    size_t is_present2 = 0;

    for (int i = 0; i < num; ++i) {
        for (int j = 0; j < orders[i].quantity; j++) {
            if (sum1 < sum2) {
                if (is_present1 == 0) {
                    sub1[*pnum1] = orders[i];
                    sub1[*pnum1].quantity = ++is_present1;
                    sum1 += get_order_data(orders, i, O_DATA);
                    ++(*pnum1);
                } else {
                    ++sub1[(*pnum1) - 1].quantity;
                    sum1 += get_order_data(orders, i, O_DATA);
                }

            } else {
                if (is_present2 == 0) {
                    sub2[*pnum2] = orders[i];
                    sub2[*pnum2].quantity = ++is_present2;
                    sum2 += get_order_data(orders, i, O_DATA);
                    ++(*pnum2);
                } else {
                    ++sub2[(*pnum2) - 1].quantity;
                    sum2 += get_order_data(orders, i, O_DATA);
                }
            }
        }
        is_present1 = 0;
        is_present2 = 0;
    }

    if (sum1 > sum2) {
        return sum1 - sum2;
    }
    return sum2 - sum1;
}

int print_orders(order *orders, size_t num) {
    if (orders == NULL) {
        return -1;
    }

    float total_w = 0.;
    float total_p = 0.;

    printf("SKU number           Quantity     Price per unit     Weight of unit\n");
    for (size_t i = 0; i < num; ++i) {
        printf("%-21s%-13d%-19.2f%.3f\n", orders[i].sku_number, orders[i].quantity, orders[i].price, orders[i].weight);
        total_w += orders[i].quantity * orders[i].weight;
        total_p += orders[i].quantity * orders[i].price;
    }
    printf("Total price: %.2f\n", total_p);
    printf("Total weight: %.3f\n", total_w);

    return 0;
}

int insert_sort(order *orders, size_t num, o_data O_DATA) {
    if (orders == NULL) {
        return -1;
    }
    for (size_t i = 1; i < num; ++i) {
        size_t offset = binary_search(orders, 0, i - 1, get_order_data(orders, i, O_DATA), O_DATA);

        if (offset != i) {
            order temp = orders[i];
            memmove(&orders[offset + 1], &orders[offset], (i - offset) * sizeof(order));
            orders[offset] = temp;
        }
    }

    return 0;
}

size_t binary_search(order *orders, size_t left, size_t right, float val, o_data O_DATA) {
    if ((right - left) == 1) {
        if (val > get_order_data(orders, left, O_DATA)) {
            return left;
        }
        if (val < get_order_data(orders, left, O_DATA)) {
            ++left;
        } else {
            return right;
        }
    }

    if (left == right) {
        if (val > get_order_data(orders, right, O_DATA)) {
            return right;
        }
        return right + 1;
    }


    size_t mid = (left + right) / 2;
    if (get_order_data(orders, mid, O_DATA) < val) {
        return binary_search(orders, left, mid, val, O_DATA);
    }
    if (get_order_data(orders, mid, O_DATA) > val) {
        return binary_search(orders, mid, right, val, O_DATA);
    }

    return mid + 1;
}
