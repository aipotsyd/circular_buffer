// circular_buffer.h
//
// Based off Pete Goodlife's articles from ~2008
// 
#include <cstddef>
#include <limits>
#include <memory>
#include <numeric>

template <typename T>
class circular_buffer
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = int;
	using refernce = value_type&;
	using const_reference = const value_type&;
	using class_type = circular_buffer;

	explicit circular_buffer(std::size_t capacity)
		: m_capacity{ capacity },
		m_buffer{ std::make_unique<value_type[]>(capacity) },
		m_front{ m_buffer.get() },
		m_back{ m_buffer.get() }
	{}

	circular_buffer(const class_type&) = delete;
	circular_buffer(class_type&&) = default;
	class_type& operator=(const class_type &) = delete;
	class_type& operator=(class_type &&) = default;
	size_type capacity() const { return m_capacity; }

	size_type size() const
	{
		return (m_back >= m_front ? m_back : m_back + m_capacity) - m_front;
	}

	size_type max_size() const
	{
		const auto count = std::numeric_limits<size_type>::max() /
			sizeof(value_type);
		return std::max(count, size_type{ 1 });
	}

	bool empty() const
	{
		return m_front == m_back;
	}

private:

	const size_type m_capacity;
	std::unique_ptr<value_type[]> m_buffer;
	value_type* m_front;
	value_type* m_back;
};