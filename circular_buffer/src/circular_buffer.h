// circular_buffer.h
//
// Based off Pete Goodlife's articles from ~2008
// 
#include <cstddef>
#include <limits>
#include <memory>
#include <numeric>

template <typename T, typename A = std::allocator<T>>
class circular_buffer
{
public:
	using value_type = T;
	using allocator_type = A;
	using self_type = circular_buffer<T, A>;
	using size_type = std::size_t;
	using difference_type = typename allocator_type::difference_type;
	using reference = typename allocator_type::reference;
	using const_reference = typename allocator_type::const_reference;
	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;

	using class_type = circular_buffer;

	explicit circular_buffer(std::size_t capacity, const allocator_type& allocator = allocator_type())
		: m_capacity{ capacity },
		m_allocator{allocator},
		m_buffer{ m_allocator.allocate(capacity)},
		m_front{ nullptr },
		m_back{ m_buffer }
	{}

	~circular_buffer()
	{
		clear();
		m_allocator.deallocate(m_buffer, m_capacity);
	}

	circular_buffer(const class_type&) = delete;
	circular_buffer(class_type&&) = default;
	class_type& operator=(const class_type &) = delete;
	class_type& operator=(class_type &&) = default;
	size_type capacity() const { return m_capacity; }

	allocator_type get_allocator() const { return m_allocator; }

	size_type size() const
	{
		return empty() ? 0 :
			(m_back > m_front ? m_back : m_back + m_capacity) - m_front;
	}

	size_type max_size() const
	{
		return m_allocator.max_size();
	}

	bool empty() const
	{
		return !m_front;
	}

	// This version of push_back will construct and destroy objects in m_buffer
	// using the allocator's methods rather than traditional construction, copying
	// and assignment.
	bool push_back(const value_type &value)
	{
		// If the buffer is full, old data will be deleted. 
		if (m_front && m_front == m_back)
			m_allocator.destroy(m_back);

		m_allocator.construct(m_back, value);

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

	void pop_front()
	{
		assert(m_front);

		m_allocator.destroy(m_front);

		value_type* const next = wrap(m_front + 1);
		if (next == m_back)
			m_front = nullptr;
		else
			m_front = next;
	}

	void clear()
	{
		if (m_front) {
			do {
				m_allocator.destroy(m_front);
				m_front = wrap(m_front + 1);
			} while (m_front != m_back);
		}
		m_front = nullptr;
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
		assert(ptr < m_buffer + m_capacity * 2);
		if (ptr >= m_buffer + m_capacity)
			return ptr - m_capacity;
		else
			return ptr;
	}
	
	const size_type m_capacity;
	allocator_type m_allocator;
	pointer m_buffer;
	pointer m_front;
	pointer m_back;
};