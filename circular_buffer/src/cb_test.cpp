#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>

#include "circular_buffer.h"

TEST_CASE("Constructor test" "[circular_buffer]")
{
	circular_buffer<int> cb(20);
	REQUIRE(cb.capacity() == 20);
}
