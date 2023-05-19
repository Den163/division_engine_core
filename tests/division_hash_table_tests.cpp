#include <catch2/catch_all.hpp>

#include "division_engine_core/data_structures/hash_table.h"

#define HASH_TABLE_INIT_CAPACITY 10

TEST_CASE("Hash table alloc test")
{
    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, HASH_TABLE_INIT_CAPACITY);

    REQUIRE(hash_table.buckets_capacity == HASH_TABLE_INIT_CAPACITY);
    REQUIRE(hash_table.buckets_size == 0);
    REQUIRE(hash_table.buckets != NULL);

    division_hash_table_free(&hash_table);
}

TEST_CASE("Hash table insert")
{
    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, HASH_TABLE_INIT_CAPACITY);

    uint32_t hash1 = 5, hash2 = 15;
    size_t insert_idx1, insert_idx2;

    REQUIRE(division_hash_table_insert(&hash_table, hash1, &insert_idx1));
    REQUIRE(division_hash_table_insert(&hash_table, hash2, &insert_idx2));

    REQUIRE(hash_table.buckets[insert_idx1] == 5);
    REQUIRE(hash_table.buckets_size == 2);
    REQUIRE(insert_idx2 == insert_idx1 + 1);

    division_hash_table_free(&hash_table);
}

TEST_CASE("Hash table contains")
{
    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, HASH_TABLE_INIT_CAPACITY);

    size_t hashes[] = { 5, 15, 9 };
    size_t hashes_count = sizeof(hashes) / sizeof(size_t);
    size_t insert_idx[hashes_count];
    size_t out_idx[hashes_count];

    for (int i = 0; i < hashes_count; i++)
    {
        REQUIRE(division_hash_table_insert(&hash_table, hashes[i], &insert_idx[i]));
    }

    for (int i = 0; i < hashes_count; i++)
    {
        REQUIRE(division_hash_table_find(&hash_table, hashes[i], &out_idx[i]));
        REQUIRE(insert_idx[i] == out_idx[i]);
    }

    division_hash_table_free(&hash_table);
}

TEST_CASE("Hash table remove")
{
    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, HASH_TABLE_INIT_CAPACITY);

    size_t hash1 = 5;
    size_t hash2 = 9;
    size_t hash3 = 15;

    size_t bucket_idx;
    division_hash_table_insert(&hash_table, hash1, &bucket_idx);
    division_hash_table_insert(&hash_table, hash2, &bucket_idx);
    division_hash_table_insert(&hash_table, hash3, &bucket_idx);

    division_hash_table_remove(&hash_table, hash1);

    size_t _;
    REQUIRE_FALSE(division_hash_table_find(&hash_table, hash1, &_));
    REQUIRE(division_hash_table_find(&hash_table, hash2, &_));
    REQUIRE(division_hash_table_find(&hash_table, hash3, &_));

    division_hash_table_free(&hash_table);
}

TEST_CASE("Hash table increase capacity")
{
    uint32_t hashes[] = { 1, 2, 3 };
    size_t hashes_count = sizeof(hashes) / (sizeof(uint32_t));

    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, hashes_count);

    size_t _;
    for (int i = 0; i < hashes_count; i++)
    {
        division_hash_table_insert(&hash_table, hashes[i], &_);
    }

    const size_t new_capacity = 10;
    division_hash_table_increase_capacity(&hash_table, new_capacity);

    REQUIRE(hash_table.buckets_capacity == new_capacity);
    REQUIRE(hash_table.buckets_size == hashes_count);
    for (int i = 0; i < hashes_count; i++)
    {
        REQUIRE(division_hash_table_find(&hash_table, hashes[i], &_));
    }

    division_hash_table_free(&hash_table);
}

TEST_CASE("Hash table auto increase capacity after insert (load factor limit check)")
{
    uint32_t hashes[] = { 1, 3, 10, 5 };
    size_t hashes_count = sizeof(hashes) / sizeof(uint32_t);

    DivisionHashTable hash_table;
    division_hash_table_alloc(&hash_table, (size_t) ((float) hashes_count * DIVISION_HASH_TABLE_STD_LOAD_FACTOR_LIMIT));

    size_t _;
    for (int i = 0; i < hashes_count; i++)
    {
        division_hash_table_insert(&hash_table, hashes[i], &_);
    }

    REQUIRE(hash_table.buckets_size == hashes_count);
    REQUIRE(hash_table.buckets_capacity > hashes_count);
    for (int i = 0; i < hashes_count; i++)
    {
        REQUIRE(division_hash_table_find(&hash_table, hashes[i], &_));
    }

    division_hash_table_free(&hash_table);
}

static inline void benchmark_array(
    const uint32_t* input_arr,
    size_t input_size,
    const uint32_t* to_search,
    size_t to_search_size,
    size_t* output_indices)
{
    for (int searchIdx = 0; searchIdx < to_search_size; searchIdx++)
    {
        for (int i = 0; i < input_size; i++)
        {
            uint32_t val = input_arr[i];
            if (val == to_search[searchIdx])
            {
                output_indices[searchIdx] = val;
            }
        }
    }
}

static inline void benchmark_hash_table(
    DivisionHashTable* hash_table, const uint32_t* to_search, size_t to_search_size, size_t* output_indices)
{
    for (int searchIdx = 0; searchIdx < to_search_size; searchIdx++)
    {
        size_t output_idx;
        if (division_hash_table_find(hash_table, to_search[searchIdx], &output_idx))
        {
            output_indices[searchIdx] = output_idx;
        }
    }
}

TEST_CASE("Hash table vs array find benchmark")
{
    size_t to_search_size = 100;
    size_t test_size = to_search_size / 2;
    uint32_t* to_search = (uint32_t*) malloc(sizeof(uint32_t[to_search_size]));
    size_t* arr_output = (size_t*) malloc(sizeof(size_t[test_size]));

    uint32_t* test_arr = (uint32_t*) malloc(sizeof(uint32_t[test_size]));
    DivisionHashTable test_hash_table;
    division_hash_table_alloc(&test_hash_table, (size_t) ((float) test_size));

    time_t t = time(NULL);
    tm* a = localtime(&t);
    int rand_seed = a->tm_sec;
    srand((unsigned int) rand_seed);

    for (int i = 0; i < to_search_size; i++)
    {
        to_search[i] = rand();
    }

    for (int i = 0; i < test_size; i++)
    {
        size_t _;
        float norm_rand = (float) rand() / RAND_MAX;
        int rand_idx = (int) (norm_rand * (float) (test_size - 1));
        uint32_t search_val = to_search[rand_idx];
        test_arr[i] = search_val;
        division_hash_table_insert(&test_hash_table, search_val, &_);
    }

    BENCHMARK("Test array") { benchmark_array(test_arr, test_size, to_search, to_search_size, arr_output); };
    BENCHMARK("Test hash table") { benchmark_hash_table(&test_hash_table, to_search, to_search_size, arr_output); };

    free(test_arr);
    division_hash_table_free(&test_hash_table);
}