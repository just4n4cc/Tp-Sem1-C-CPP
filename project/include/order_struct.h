#ifndef ORDER_STRUCT_H
#define ORDER_STRUCT_H

typedef struct order {
    char sku_number[20];
    int quantity;
    float price;
    float weight;
} order;

typedef enum {
    O_PRICE,
    O_WEIGHT
} o_data;

#endif  // ORDER_STRUCT_H
