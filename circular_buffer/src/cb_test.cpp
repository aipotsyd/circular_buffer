#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>

#include "circular_buffer.h"

TEST_CASE("Constructor test" "[circular_buffer]")
{
	SECTION("Construct with capacity") {
		auto cb = circular_buffer<int>(20);
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 20);
		REQUIRE(cb.empty());
		REQUIRE(cb.max_size() > 0);
	}
}
