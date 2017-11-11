#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>

#include "circular_buffer.h"

struct leak_checker {
	static size_t count;
	leak_checker() { ++count; }
	~leak_checker() { --count; }
};
size_t leak_checker::count = 0;

TEST_CASE("Constructor test" "[circular_buffer]")
{
	SECTION("Construct with capacity") {
		auto cb = circular_buffer<int>(5);
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());
		REQUIRE(cb.max_size() > 0);

		cb.push_back(1);
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 1);

		cb.push_back(2);
		REQUIRE(cb.size() == 2);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 1);

		cb.push_back(3);
		cb.push_back(4);
		cb.push_back(5);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 1);

		cb.push_back(6);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 2);

		cb.pop_front();
		REQUIRE(cb.size() == 4);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 3);

		cb.pop_front();
		REQUIRE(cb.size() == 3);
		REQUIRE(cb.front() == 4);

		cb.pop_front();
		REQUIRE(cb.size() == 2);
		REQUIRE(cb.front() == 5);

		cb.pop_front();
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.front() == 6);

		cb.pop_front();
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());

		cb.push_back(7);
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 7);

		cb.push_back(8);
		cb.push_back(9);
		REQUIRE(cb.size() == 3);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front() == 7);

		cb.clear();
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());


	}

	SECTION("Check empty construction does not allocate members") {
		REQUIRE(leak_checker::count == 0);
		circular_buffer<leak_checker> cb(5);
		REQUIRE(leak_checker::count == 0);
	}
}
