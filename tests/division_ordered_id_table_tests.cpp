#include "catch2/catch_all.hpp"

#include "data_structures/ordered_id_table.h"

TEST_CASE("Ordered id table alloc check")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    REQUIRE(id_table.orders_count == 0);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table insert check")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id = division_ordered_id_table_insert(&id_table);

    REQUIRE(id_table.orders[0] == id);
    REQUIRE(id_table.id_to_order[0] == id);
    REQUIRE(id_table.orders_count == 1);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table remove check")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id = division_ordered_id_table_insert(&id_table);
    division_ordered_id_table_remove(&id_table, id);

    REQUIRE_FALSE(division_ordered_id_table_contains(&id_table, id));
    REQUIRE(id_table.orders_count == 0);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table mixed operations")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id0 = division_ordered_id_table_insert(&id_table);
    uint32_t id1 = division_ordered_id_table_insert(&id_table);
    uint32_t id2 = division_ordered_id_table_insert(&id_table);

    division_ordered_id_table_remove(&id_table, id1);

    uint32_t id3 = division_ordered_id_table_insert(&id_table);

    REQUIRE(id_table.orders_count == 3);
    REQUIRE(id_table.orders[0] == id0);
    REQUIRE(id_table.orders[1] == id2);
    REQUIRE(id_table.orders[2] == id3);

    division_ordered_id_table_free(&id_table);
}