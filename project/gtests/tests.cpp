#include "gtest/gtest.h"
#include <fstream>

extern "C" {
    #include "files_and_strs_manage.h"
    #include "search_substr_c.h"
    #include "search_substr_p.h"
}

int *LoadAnswers(const char *filename, size_t &num) {
    std::ifstream fin(filename);
    fin >> num;
    int *res = new int[num];
    for (size_t i = 0; i < num; ++i) {
        fin >> res[i];
    }
    fin.close();
    return res;
}

bool CompareOffsets(int *offsets1, int *offsets2, size_t num) {
    for (size_t i = 0; i < num; ++i) {
        if (offsets1[i] != offsets2[i]) {
            return false;
        }
    }
    return true;
}

TEST(FileUpload, InvalidInput) {
    size_t size = 0;
    char filename[] = "some_file.fake";
    EXPECT_EQ(file_upload(NULL, &size), nullptr);
    EXPECT_EQ(file_upload(filename, NULL), nullptr);
}

TEST(FileDelete, InvalidInput) {
    size_t size = 5;
    char *region = new char[10];
    EXPECT_EQ(file_delete(NULL, size), -1);
    EXPECT_EQ(file_delete(region, size), -1);
    delete[] region;
}

TEST(FileUploadANDFileDelete, ValidInput) {
    size_t size = 0;
    char filename[] = "files/test1_o.txt";
    char *region = file_upload(filename, &size);
    ASSERT_NE(region, nullptr);
    ASSERT_EQ(file_delete(region, size), 0);
}

TEST(CreateSubstrArr, InvalidInput) {
    size_t size = 0;
    EXPECT_EQ(create_substr_arr(NULL, &size), nullptr);
    EXPECT_EQ(create_substr_arr(NULL, NULL), nullptr);
}

TEST(CreateSubstrArr, ValidInput) {
    size_t size = 0;
    char filename[] = "files/test1_s.txt";
    char *region = file_upload(filename, &size);
    ASSERT_NE(region, nullptr);

    size_t num = 0;
    char **substrs = create_substr_arr(region, &num);
    EXPECT_NE(substrs, nullptr);
    EXPECT_EQ(num, 3);

    char test_substrs[3][5] = {{"nnnn"}, {"eol'"}, {"naev"}};
    for (size_t i = 1; i < num; ++i) {
        EXPECT_STREQ(substrs[i], test_substrs[i]);
    }

    free(substrs);
    file_delete(region, size);
}

TEST(OneThreadSearch, InvalidInput) {
    EXPECT_EQ(one_thread_search(NULL, NULL, 0), nullptr);
    char *str = new char[10];
    EXPECT_EQ(one_thread_search(NULL, &str, 1), nullptr);
    EXPECT_EQ(one_thread_search(str, NULL, 1), nullptr);
    EXPECT_EQ(one_thread_search(str, &str, 0), nullptr);
    delete[] str;
}

TEST(OneThreadSearch, ValidInput) {
    size_t o_size = 0;
    char o_filename[] = "files/test1_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test1_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets = one_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer1.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(MultiThreadSearch, InvalidInput) {
    EXPECT_EQ(multi_thread_search(NULL, NULL, 0), nullptr);
    char *str = new char[10];
    EXPECT_EQ(multi_thread_search(NULL, &str, 1), nullptr);
    EXPECT_EQ(multi_thread_search(str, NULL, 1), nullptr);
    EXPECT_EQ(multi_thread_search(str, &str, 0), nullptr);
    delete[] str;
}

TEST(MultiThreadSearch, ValidInput) {
    size_t o_size = 0;
    char o_filename[] = "files/test1_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test1_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer1.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(BothThreadsSearch, HeavyData) {
    size_t o_size = 0;
    char o_filename[] = "files/test2_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test2_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets_c = one_thread_search(o_region, substrs, num);
    int *offsets_p = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer2.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_c, (num < right_num ? num : right_num)));
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_p, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets_c);
    free(offsets_p);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(BothThreadsSearch, TheSameSubstrs) {
    size_t o_size = 0;
    char o_filename[] = "files/test3_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test3_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets_c = one_thread_search(o_region, substrs, num);
    int *offsets_p = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer3.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_c, (num < right_num ? num : right_num)));
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_p, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets_c);
    free(offsets_p);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(BothThreadsSearch, FullSizeSubstr) {
    size_t o_size = 0;
    char o_filename[] = "files/test4_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test4_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets_c = one_thread_search(o_region, substrs, num);
    int *offsets_p = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer4.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_c, (num < right_num ? num : right_num)));
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_p, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets_c);
    free(offsets_p);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(BothThreadsSearch, OneCharSubstrs) {
    size_t o_size = 0;
    char o_filename[] = "files/test5_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test5_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets_c = one_thread_search(o_region, substrs, num);
    int *offsets_p = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer5.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_c, (num < right_num ? num : right_num)));
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_p, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets_c);
    free(offsets_p);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}

TEST(BothThreadsSearch, AllNotFound) {
    size_t o_size = 0;
    char o_filename[] = "files/test6_o.txt";
    char *o_region = file_upload(o_filename, &o_size);

    size_t s_size = 0;
    char s_filename[] = "files/test6_s.txt";
    char *s_region = file_upload(s_filename, &s_size);

    size_t num = 0;
    char **substrs = create_substr_arr(s_region, &num);

    int *offsets_c = one_thread_search(o_region, substrs, num);
    int *offsets_p = multi_thread_search(o_region, substrs, num);
    size_t right_num = 0;
    int *right_offsets = LoadAnswers("files/answers/answer6.txt", right_num);
    EXPECT_EQ(num, right_num);
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_c, (num < right_num ? num : right_num)));
    EXPECT_TRUE(CompareOffsets(right_offsets, offsets_p, (num < right_num ? num : right_num)));

    free(substrs);
    free(offsets_c);
    free(offsets_p);
    file_delete(o_region, o_size);
    file_delete(s_region, s_size);
    delete[] right_offsets;
}
