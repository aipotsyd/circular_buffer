// circular_buffer.h
//
// Based off Pete Goodlife's articles from ~2008
// 
#include <cstddef>
#include <memory>

template <typename T>
class circular_buffer
{
public:
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = int;
	using refernce = value_type&;
	using const_reference = const value_type&;

	explicit circular_buffer(std::size_t capacity) 
		: m_capacity{ capacity },
		m_buffer{std::make_unique<value_type[]>(capacity)},
		m_front{m_buffer.get()},
		m_back{m_buffer.get()}
	{}

	const size_type capacity() const { return m_capacity; }

private:
	const size_type m_capacity;
	std::unique_ptr<value_type[]> m_buffer;
	value_type* m_front;
	value_type* m_back;
};