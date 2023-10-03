#include "catch2/catch_all.hpp"

#include "catch2/catch_test_macros.hpp"
#include "division_engine_core/data_structures/ordered_id_table.h"
#include "division_engine_core/data_structures/unordered_id_table.h"
#include <_types/_uint32_t.h>

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

    uint32_t id = division_ordered_id_table_new_id(&id_table);

    REQUIRE(id_table.orders[0] == id);
    REQUIRE(id_table.orders_count == 1);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table find order index")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id0 = division_ordered_id_table_new_id(&id_table);
    uint32_t id1 = division_ordered_id_table_new_id(&id_table);

    uint32_t order_idx0, order_idx1;
    REQUIRE(division_ordered_id_table_find_id_order(&id_table, id0, &order_idx0));
    REQUIRE(division_ordered_id_table_find_id_order(&id_table, id1, &order_idx1));
    REQUIRE(order_idx0 == 0);
    REQUIRE(order_idx1 == 1);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table remove check")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id0 = division_ordered_id_table_new_id(&id_table);
    uint32_t id1 = division_ordered_id_table_new_id(&id_table);
    division_ordered_id_table_remove_id(&id_table, id0);
    division_ordered_id_table_remove_id(&id_table, id1);

    REQUIRE_FALSE(division_ordered_id_table_contains(&id_table, id0));
    REQUIRE_FALSE(division_ordered_id_table_contains(&id_table, id1));
    REQUIRE(id_table.orders_count == 0);

    division_ordered_id_table_free(&id_table);
}

TEST_CASE("Ordered id table mixed operations")
{
    DivisionOrderedIdTable id_table;
    division_ordered_id_table_alloc(&id_table, 10);

    uint32_t id0 = division_ordered_id_table_new_id(&id_table);
    uint32_t id1 = division_ordered_id_table_new_id(&id_table);
    uint32_t id2 = division_ordered_id_table_new_id(&id_table);

    division_ordered_id_table_remove_id(&id_table, id1);

    uint32_t id3 = division_ordered_id_table_new_id(&id_table);

    REQUIRE(id_table.orders_count == 3);
    REQUIRE(id_table.orders[0] == id0);
    REQUIRE(id_table.orders[1] == id2);
    REQUIRE(id_table.orders[2] == id3);

    division_ordered_id_table_free(&id_table);
}