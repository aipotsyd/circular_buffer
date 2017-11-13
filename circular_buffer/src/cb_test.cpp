#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <limits>

#include "circular_buffer.h"

struct leak_checker {
	leak_checker(int value) : m_value{ value }
	{
		++count;
	}
	static std::size_t count;
	leak_checker() { ++count; }
	~leak_checker() { --count; }
	leak_checker(const leak_checker& lc) : m_value{ lc.m_value}
	{
		++count;
	}
	leak_checker(leak_checker&& lc) = default;
	leak_checker& operator=(const leak_checker&) = default;
	leak_checker& operator=(leak_checker&&) = default;

	int value() { return m_value; }
private:
	int m_value;
};
std::size_t leak_checker::count = 0;

TEST_CASE("Pushing and popping (allocating and deallocating)", "[circular_buffer]")
{
	leak_checker::count = 0;
	SECTION("Construct with capacity") {
		auto cb = circular_buffer<leak_checker>(5);
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());
		REQUIRE(cb.max_size() > 0);
		REQUIRE(leak_checker::count == 0);

		cb.push_back(1);
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 1);
		REQUIRE(cb.back().value() == 1);
		REQUIRE(leak_checker::count == 1);

		cb.push_back(2);
		REQUIRE(cb.size() == 2);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 1);
		REQUIRE(cb.back().value() == 2);
		REQUIRE(leak_checker::count == 2);

		cb.push_back(3);
		cb.push_back(4);
		cb.push_back(5);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 1);
		REQUIRE(cb.back().value() == 5);
		REQUIRE(leak_checker::count == 5);

		cb.push_back(6);
		REQUIRE(cb.size() == 5);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 2);
		REQUIRE(cb.back().value() == 6);
		REQUIRE(leak_checker::count == 5);

		cb.pop_front();
		REQUIRE(cb.size() == 4);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 3);
		REQUIRE(cb.back().value() == 6);
		REQUIRE(leak_checker::count == 4);

		cb.pop_front();
		REQUIRE(cb.size() == 3);
		REQUIRE(cb.front().value() == 4);
		REQUIRE(cb.back().value() == 6);
		REQUIRE(leak_checker::count == 3);

		cb.pop_front();
		REQUIRE(cb.size() == 2);
		REQUIRE(cb.front().value() == 5);
		REQUIRE(cb.back().value() == 6);
		REQUIRE(leak_checker::count == 2);

		cb.pop_front();
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.front().value() == 6);
		REQUIRE(cb.back().value() == 6);
		REQUIRE(leak_checker::count == 1);

		cb.pop_front();
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());
		REQUIRE(leak_checker::count == 0);

		cb.push_back(7);
		REQUIRE(cb.size() == 1);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 7);
		REQUIRE(cb.back().value() == 7);
		REQUIRE(leak_checker::count == 1);

		cb.push_back(8);
		cb.push_back(9);
		REQUIRE(cb.size() == 3);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(!cb.empty());
		REQUIRE(cb.front().value() == 7);
		REQUIRE(cb.back().value() == 9);
		REQUIRE(leak_checker::count == 3);

		cb.clear();
		REQUIRE(cb.size() == 0);
		REQUIRE(cb.capacity() == 5);
		REQUIRE(cb.empty());
		REQUIRE(leak_checker::count == 0);
	}

	SECTION("Check empty construction does not allocate members") {
		REQUIRE(leak_checker::count == 0);
		circular_buffer<leak_checker> cb(5);
		REQUIRE(leak_checker::count == 0);
	}
}

TEST_CASE("Indexing", "[circular_buffer]") {
	auto cb = circular_buffer<int>(5);

	for (int i = 0; i < 10; ++i) {
		REQUIRE(cb.empty());

		//const auto &const_cb = cb;
		const circular_buffer<int> &const_cb(cb);
		REQUIRE_THROWS_AS(cb.at(0), std::out_of_range);
		REQUIRE_THROWS_AS(cb.at(1), std::out_of_range);
		REQUIRE_THROWS_AS(const_cb.at(0), std::out_of_range);
		REQUIRE_THROWS_AS(const_cb.at(1), std::out_of_range);

		REQUIRE(cb.push_back(0));
		REQUIRE(cb.push_back(1));
		REQUIRE(cb.push_back(2));
		REQUIRE(cb.at(0) == 0);
		REQUIRE(cb.at(1) == 1);
		REQUIRE(cb.at(2) == 2);
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 1);
		REQUIRE(cb[2] == 2);
		REQUIRE(const_cb.at(0) == 0);
		REQUIRE(const_cb.at(1) == 1);
		REQUIRE(const_cb.at(2) == 2);
		REQUIRE(const_cb[0] == 0);
		REQUIRE(const_cb[1] == 1);
		REQUIRE(const_cb[2] == 2);

		REQUIRE(cb.front() == 0);
		cb[0] = 3;
		REQUIRE(cb.front() == 3);
		REQUIRE(cb.at(0) == 3);
		REQUIRE(cb.at(1) == 1);
		REQUIRE(cb.at(2) == 2);
		REQUIRE(cb[0] == 3);
		REQUIRE(cb[1] == 1);
		REQUIRE(cb[2] == 2);
		REQUIRE(const_cb.at(0) == 3);
		REQUIRE(const_cb.at(1) == 1);
		REQUIRE(const_cb.at(2) == 2);
		REQUIRE(const_cb[0] == 3);
		REQUIRE(const_cb[1] == 1);
		REQUIRE(const_cb[2] == 2);

		cb[1] = 4;
		REQUIRE(cb.at(0) == 3);
		REQUIRE(cb.at(1) == 4);
		REQUIRE(cb.at(2) == 2);
		REQUIRE(cb[0] == 3);
		REQUIRE(cb[1] == 4);
		REQUIRE(cb[2] == 2);
		REQUIRE(const_cb.at(0) == 3);
		REQUIRE(const_cb.at(1) == 4);
		REQUIRE(const_cb.at(2) == 2);
		REQUIRE(const_cb[0] == 3);
		REQUIRE(const_cb[1] == 4);
		REQUIRE(const_cb[2] == 2);

		cb.pop_front();
		REQUIRE(cb.at(0) == 4);
		REQUIRE(cb.at(1) == 2);
		REQUIRE(cb[0] == 4);
		REQUIRE(cb[1] == 2);
		REQUIRE(const_cb.at(0) == 4);
		REQUIRE(const_cb.at(1) == 2);
		REQUIRE(const_cb[0] == 4);
		REQUIRE(const_cb[1] == 2);

		cb.pop_front();
		REQUIRE(cb.at(0) == 2);
		REQUIRE(cb[0] == 2);
		REQUIRE(const_cb.at(0) == 2);
		REQUIRE(const_cb[0] == 2);

		cb.pop_front();
	}
}

TEST_CASE("Using Iterators", "[circular_buffer]") {
	auto cb = circular_buffer<int>(5);

	cb.push_back(0);
	cb.push_back(1);
	cb.push_back(2);

	REQUIRE(cb[0] == 0);
	REQUIRE(cb[1] == 1);
	REQUIRE(cb[2] == 2);

	SECTION("Range-based for loop") {
		for (auto& i : cb)
		{
			i *= 10;
		}
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 10);
		REQUIRE(cb[2] == 20);
	}

	SECTION("Raw for loop with iterators (pre-increment") {
		for (auto i = cb.begin(); i != cb.end(); ++i)
		{
			*i *= 10;
		}
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 10);
		REQUIRE(cb[2] == 20);
	}

	SECTION("Raw for loop with iterators (post-increment") {
		for (auto i = cb.begin(); i != cb.end(); i++)
		{
			*i *= 10;
		}
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 10);
		REQUIRE(cb[2] == 20);
	}

	SECTION("Raw for loop with reverse iterators (pre-increment") {
		for (auto i = cb.rbegin(); i != cb.rend(); ++i)
		{
			*i *= 10;
		}
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 10);
		REQUIRE(cb[2] == 20);
	}

	SECTION("Raw for loop with reverse iterators (post-increment") {
		for (auto i = cb.rbegin(); i != cb.rend(); i++)
		{
			*i *= 10;
		}
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 10);
		REQUIRE(cb[2] == 20);
	}

	SECTION("Copy with iterators") {
		auto dest = circular_buffer<int>(5);
		std::copy(cb.begin(), cb.end(), dest.begin());
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 1);
		REQUIRE(cb[2] == 2);
		REQUIRE(dest[0] == 0);
		REQUIRE(dest[1] == 1);
		REQUIRE(dest[2] == 2);
	}

	SECTION("Copy with reverse iterators") {
		auto dest = circular_buffer<int>(5);
		std::copy(cb.rbegin(), cb.rend(), dest.begin());
		REQUIRE(cb[0] == 0);
		REQUIRE(cb[1] == 1);
		REQUIRE(cb[2] == 2);
		REQUIRE(dest[0] == 2);
		REQUIRE(dest[1] == 1);
		REQUIRE(dest[2] == 0);
	}
}