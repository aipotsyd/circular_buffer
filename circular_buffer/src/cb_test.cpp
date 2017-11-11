#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>

#include "circular_buffer.h"

TEST_CASE("Constructor test" "[circular_buffer]")
{
	SECTION("Construct with capacity") {
		auto cb = circular_buffer<int>(5);
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());
		REQUIRE(cb.max_size() > 0);

		cb.push_back(1);
		REQUIRE(cb.front() == 1);
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());

		cb.push_back(2);
		REQUIRE(cb.size() == 2);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());

		cb.push_back(3);
		cb.push_back(4);
		cb.push_back(5);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());

		cb.push_back(6);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());

	}
}
