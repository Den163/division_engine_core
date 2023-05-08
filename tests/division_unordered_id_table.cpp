#include <catch2/catch_all.hpp>
#include "division_id_table/unordered_id_table.h"

#define TEST_ID_TABLE_SIZE 10

TEST_CASE("alloc check")
{
    DivisionUnorderedIdTable table;
    division_unordered_id_table_alloc(&table, TEST_ID_TABLE_SIZE);

    REQUIRE(table.free_ids_count == TEST_ID_TABLE_SIZE);

    division_unordered_id_table_free(&table);
}

TEST_CASE("insert check")
{
    DivisionUnorderedIdTable table;
    division_unordered_id_table_alloc(&table, TEST_ID_TABLE_SIZE);

    uint32_t id = division_unordered_id_table_insert(&table);
    REQUIRE(id == 0);
    REQUIRE(table.free_ids_count == TEST_ID_TABLE_SIZE - 1);
    REQUIRE(division_unordered_id_table_contains(&table, id));

    division_unordered_id_table_free(&table);
}

TEST_CASE("remove check")
{
    DivisionUnorderedIdTable table;
    division_unordered_id_table_alloc(&table, TEST_ID_TABLE_SIZE);

    uint32_t id = division_unordered_id_table_insert(&table);
    division_unordered_id_table_remove(&table, id);

    REQUIRE_FALSE(division_unordered_id_table_contains(&table, id));
    REQUIRE(table.free_ids_count == TEST_ID_TABLE_SIZE);

    division_unordered_id_table_free(&table);
}

TEST_CASE("Mixed operations") {
    DivisionUnorderedIdTable table;
    division_unordered_id_table_alloc(&table, TEST_ID_TABLE_SIZE);

    uint32_t id0 = division_unordered_id_table_insert(&table);
    uint32_t id1 = division_unordered_id_table_insert(&table);
    uint32_t id2 = division_unordered_id_table_insert(&table);
    uint32_t id3 = division_unordered_id_table_insert(&table);

    division_unordered_id_table_remove(&table, id2);
    REQUIRE(division_unordered_id_table_contains(&table, id0));
    REQUIRE(division_unordered_id_table_contains(&table, id1));
    REQUIRE_FALSE(division_unordered_id_table_contains(&table, id2));
    REQUIRE(division_unordered_id_table_contains(&table, id3));

    division_unordered_id_table_free(&table);
}