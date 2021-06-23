#include "gtest/gtest.h"
#include <string.h>


extern "C" {
    #include "order_struct.h"
    #include "funcs.h"
}

const size_t test_num = 3;
const size_t sub1_w_num = 2;
const size_t sub2_w_num = 3;
const size_t sub1_p_num = 2;
const size_t sub2_p_num = 3;

order items[3] = {{"item1", 2, 500, 0.4}, {"item2", 1, 250, 0.7}, {"item3", 4, 700, 0.5}};
order test[3] = {{"item1", 2, 500, 0.4}, {"item2", 1, 250, 0.7}, {"item3", 4, 700, 0.5}};

void ResetTestOrders() {
    for (size_t i = 0; i < test_num; ++i) {
        test[i] = items[i];
    }
}

bool CompareOrders(order *o1, order *o2, size_t num) {
    for (size_t i = 0; i < num; ++i) {
        if (strcmp(o1[i].sku_number, o2[i].sku_number) != 0 || o1[i].quantity != o2[i].quantity) {
            print_orders(o1, num);
            print_orders(o2, num);
            return false;
        }
        if (o1[i].price != o2[i].price || o1[i].weight != o2[i].weight) {
            return false;
        }
    }
    return true;
}

TEST(GetOrderData, InvalidInput) {
    EXPECT_EQ(get_order_data(NULL, 0, O_PRICE), -1);
    EXPECT_EQ(get_order_data(NULL, 0, O_WEIGHT), -1);
}

TEST(GetOrderData, CorrectInput) {
    ASSERT_FLOAT_EQ(get_order_data(test, 0, O_PRICE), test[0].price);
    ASSERT_FLOAT_EQ(get_order_data(test, 0, O_WEIGHT), test[0].weight);
}

TEST(PrintOrders, InvalidInput) {
    EXPECT_EQ(print_orders(NULL, 2), -1);
}

TEST(PrintOrders, CorrectInput) {
    ASSERT_EQ(print_orders(test, test_num), 0);
}

TEST(BinarySearch, CorrectInput) {
    order test_sorted_w[3] = {{"item2", 1, 250, 0.7}, {"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}};
    order test_sorted_p[3] = {{"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}, {"item2", 1, 250, 0.7}};

    ASSERT_EQ(binary_search(test_sorted_w, 0, 2, 0.6, O_WEIGHT), 1);
    ASSERT_EQ(binary_search(test_sorted_w, 0, 2, 0.8, O_WEIGHT), 0);
    ASSERT_EQ(binary_search(test_sorted_w, 0, 2, 0.1, O_WEIGHT), 3);

    ASSERT_EQ(binary_search(test_sorted_p, 0, 2, 300, O_PRICE), 2);
    ASSERT_EQ(binary_search(test_sorted_p, 0, 2, 800, O_PRICE), 0);
    ASSERT_EQ(binary_search(test_sorted_p, 0, 2, 100, O_PRICE), 3);
}

TEST(InsertSort, InvalidInput) {
    EXPECT_EQ(insert_sort(NULL, 0, O_WEIGHT), -1);
    EXPECT_EQ(insert_sort(NULL, 5, O_PRICE), -1);
}

TEST(InsertSort, CorrectInput) {
    order test_sorted_w[3] = {{"item2", 1, 250, 0.7}, {"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}};
    order test_sorted_p[3] = {{"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}, {"item2", 1, 250, 0.7}};

    insert_sort(test, test_num, O_WEIGHT);
    ASSERT_TRUE(CompareOrders(test, test_sorted_w, test_num));
    ResetTestOrders();
    insert_sort(test, test_num, O_PRICE);
    ASSERT_TRUE(CompareOrders(test, test_sorted_p, test_num));
    ResetTestOrders();
}

TEST(InsertSort, SortedInput) {
    order test_sorted_w[3] = {{"item2", 1, 250, 0.7}, {"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}};
    order test_sorted_p[3] = {{"item3", 4, 700, 0.5}, {"item1", 2, 500, 0.4}, {"item2", 1, 250, 0.7}};

    insert_sort(test, test_num, O_WEIGHT);
    insert_sort(test, test_num, O_WEIGHT);
    ASSERT_TRUE(CompareOrders(test, test_sorted_w, test_num));
    ResetTestOrders();

    insert_sort(test, test_num, O_PRICE);
    insert_sort(test, test_num, O_PRICE);
    ASSERT_TRUE(CompareOrders(test, test_sorted_p, test_num));
    ResetTestOrders();
}

TEST(GreedySplitOrders, InvalidInput) {
    float error = -1;
    size_t a = 0;
    size_t b = 0;
    ASSERT_EQ(greedy_split_orders(NULL, test_num, test, &a, items, &b, O_WEIGHT), error);
    ASSERT_EQ(greedy_split_orders(test, test_num, test, NULL, items, &b, O_WEIGHT), error);
    ASSERT_EQ(greedy_split_orders(test, test_num, test, &a, NULL, &b, O_WEIGHT), error);
    ASSERT_EQ(greedy_split_orders(test, test_num, items, &a, items, &b, O_WEIGHT), error);
    ASSERT_EQ(greedy_split_orders(items, test_num, items, &a, test, &a, O_WEIGHT), error);
}

TEST(GreedySplitOrders, CorrectInputWeight) {
    size_t num1 = 0;
    size_t num2 = 0;
    float w_diff = 0.3;
    order test_sub1_w[2] = {{"item3", 3, 700, 0.5}, {"item1", 1, 500, 0.4}};
    order test_sub2_w[3] = {{"item2", 1, 250, 0.7}, {"item3", 1, 700, 0.5}, {"item1", 1, 500, 0.4}};

    order *sub1 = new order[test_num];
    order *sub2 = new order[test_num];

    EXPECT_FLOAT_EQ(greedy_split_orders(test, test_num, sub1, &num1, sub2, &num2, O_WEIGHT), w_diff);
    EXPECT_EQ(num1, sub1_w_num);
    EXPECT_EQ(num2, sub2_w_num);
    EXPECT_TRUE(CompareOrders(sub1, test_sub1_w, sub1_w_num));
    EXPECT_TRUE(CompareOrders(sub2, test_sub2_w, sub2_w_num));
    ResetTestOrders();

    delete[] sub1;
    delete[] sub2;
}

TEST(GreedySplitOrders, CorrectInputPrice) {
    size_t num1 = 0;
    size_t num2 = 0;
    float p_diff = 250;
    order test_sub1_p[2] = {{"item3", 2, 700, 0.5}, {"item1", 1, 500, 0.4}};
    order test_sub2_p[3] = {{"item3", 2, 700, 0.5}, {"item1", 1, 500, 0.4}, {"item2", 1, 250, 0.7}};

    order *sub1 = new order[test_num];
    order *sub2 = new order[test_num];

    EXPECT_FLOAT_EQ(greedy_split_orders(test, test_num, sub1, &num1, sub2, &num2, O_PRICE), p_diff);
    EXPECT_EQ(num1, sub1_p_num);
    EXPECT_EQ(num2, sub2_p_num);
    EXPECT_TRUE(CompareOrders(sub1, test_sub1_p, sub1_p_num));
    EXPECT_TRUE(CompareOrders(sub2, test_sub2_p, sub2_p_num));
    ResetTestOrders();

    delete[] sub1;
    delete[] sub2;
}

TEST(SplitManger, InvalidInput) {
    int error = -1;
    size_t a = 0;
    size_t b = 0;
    EXPECT_EQ(split_manager(NULL, test_num, test, &a, items, &b), error);
    EXPECT_EQ(split_manager(test, test_num, test, NULL, items, &b), error);
    EXPECT_EQ(split_manager(test, test_num, test, &a, NULL, &b), error);
    EXPECT_EQ(split_manager(test, test_num, items, &a, items, &b), error);
    EXPECT_EQ(split_manager(items, test_num, items, &a, test, &a), error);
}
