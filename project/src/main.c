#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "order_struct.h"
#include "funcs.h"

const size_t sku_max_length = 20;

int main() {
    int num = 0;  // Obrabotat otricatelnoe znachenie
    printf("Enter number of orders in order list\n");
    if (scanf("%d", &num) != 1 || num < 2) {
        printf("Number of orders must be more than 1\n");
        return -1;
    }

    order *orders = (order*)calloc(num, sizeof(order));
    if (orders == NULL) {
        return -1;
    }

    for (int i = 0; i < num; ++i) {
        printf("Item #%d\n", i + 1);
        printf("Enter SKU number: \n");
        getchar();
        fgets(orders[i].sku_number, sku_max_length, stdin);
        orders[i].sku_number[strlen(orders[i].sku_number) - 1] = '\0';

        printf("Enter quantity of items: \n");
        if (scanf("%d", &orders[i].quantity) != 1 || orders[i].quantity < 1) {
            printf("Incorrect input\n");
            free(orders);
            return -1;
        }

        printf("Enter price of item: \n");
        if (scanf("%f", &orders[i].price) != 1 || orders[i].price <= 0.) {
            printf("Incorrect input\n");
            free(orders);
            return -1;
        }
        printf("Enter weight of item: \n");
        if (scanf("%f", &orders[i].weight) != 1 || orders[i].weight <= 0.) {
            printf("Incorrect input\n");
            free(orders);
            return -1;
        }
    }

    order *sub_orders1 = (order*)calloc(num, sizeof(order));
    if (sub_orders1 == NULL) {
        printf("Memory error\n");
        free(orders);
        return -1;
    }

    order *sub_orders2 = (order*)calloc(num, sizeof(order));
    if (sub_orders2 == NULL) {
        printf("Memory error\n");
        free(sub_orders1);
        free(orders);
        return -1;
    }

    size_t sub_num1 = 0;
    size_t sub_num2 = 0;

    if (split_manager(orders, num, sub_orders1, &sub_num1, sub_orders2, &sub_num2) != 0) {
        printf("Memory error\n");
        free(orders);
        free(sub_orders1);
        free(sub_orders2);
        return -1;
    }

    printf("\nYOUR ORDER SUBLISTS\n");
    printf("----------Order list #1----------\n");
    print_orders(sub_orders1, sub_num1);
    printf("----------Order list #2----------\n");
    print_orders(sub_orders2, sub_num2);


    free(orders);
    free(sub_orders1);
    free(sub_orders2);
    return 0;
}
