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
	using reference = value_type&;
	using const_reference = const value_type&;
	using class_type = circular_buffer;

	explicit circular_buffer(std::size_t capacity)
		: m_capacity{ capacity },
		m_buffer{ std::make_unique<value_type[]>(capacity) },
		m_front{ nullptr },
		m_back{ m_buffer.get() }
	{}

	circular_buffer(const class_type&) = delete;
	circular_buffer(class_type&&) = default;
	class_type& operator=(const class_type &) = delete;
	class_type& operator=(class_type &&) = default;
	size_type capacity() const { return m_capacity; }

	size_type size() const
	{
		return empty() ? 0 :
			(m_back > m_front ? m_back : m_back + m_capacity) - m_front;
	}

	size_type max_size() const
	{
		const auto count = std::numeric_limits<size_type>::max() /
			sizeof(value_type);
		return std::max(count, size_type{ 1 });
	}

	bool empty() const
	{
		return !m_front;
	}

	bool push_back(const value_type &value)
	{
		*m_back = value; 

		value_type* const next = wrap(m_back + 1);
		if (empty()) {
			m_front = m_back;
			m_back = next;
			return true;
		}
		else if (m_front == m_back) {
			// full, so overwrite
			m_front = m_back = next;
			return false;
		}
		else {
			m_back = next;
			return true;
		}
	}

	reference front()
	{
		assert(m_front);
		return *m_front;
	}

	const_reference front() const
	{
		assert(m_front);
		return *m_front;
	}
	
private:
	value_type* wrap(value_type* ptr)
	{
		assert(ptr < m_buffer.get() + m_capacity * 2);
		if (ptr >= m_buffer.get() + m_capacity)
			return ptr - m_capacity;
		else
			return ptr;
	}

	const size_type m_capacity;
	std::unique_ptr<value_type[]> m_buffer;
	value_type* m_front;
	value_type* m_back;
};