// circular_buffer.h
//
// Based off Pete Goodlife's articles from ~2008
// 

#include <cstddef>
#include <limits>
#include <stdexcept>

template <typename T, typename A = std::allocator<T>>
class circular_buffer
{
public:
	using value_type = T;
	using allocator_type = A;
	using self_type = circular_buffer<T, A>;
	using size_type = typename allocator_type::size_type;
	using difference_type = typename allocator_type::difference_type;
	using reference = typename allocator_type::reference;
	using const_reference = typename allocator_type::const_reference;
	using pointer = typename allocator_type::pointer;
	using const_pointer = typename allocator_type::const_pointer;
	using class_type = circular_buffer;

	class iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;

	explicit circular_buffer(std::size_t capacity, const allocator_type& allocator = allocator_type())
		: m_capacity{ capacity },
		m_allocator{allocator},
		m_buffer(m_allocator.allocate(capacity)),
		m_front{ nullptr },
		m_back{ m_buffer }
	{}

	~circular_buffer()
	{
		clear();
		m_allocator.deallocate(m_buffer, m_capacity);
	}

	circular_buffer(const class_type&) = default;
	circular_buffer(class_type&&) = default;
	class_type& operator=(const class_type &) = default;
	class_type& operator=(class_type &&) = default;
	allocator_type get_allocator() const { return m_allocator; }

	iterator begin()
	{
		return iterator(this, 0, size());
	}

	iterator end()
	{
		return iterator(this, size(), size());
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

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

	size_type capacity() const { return m_capacity; }

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

	reference back()
	{
		assert(m_front);
		return *wrap(m_back - 1);
	}

	const_reference back() const
	{
		assert(m_front);
		return *wrap(m_back - 1);
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

	reference operator[](std::size_t index)
	{
		return *wrap(m_front + index);
	}

	const_reference operator[](std::size_t index) const
	{
		return *wrap(m_front + index);
	}

	reference at(std::size_t index)
	{
		if (index >= size())
			throw std::out_of_range("Index out of range");
		return (*this)[index];
	}

	const_reference at(std::size_t index) const
	{
		if (index >= size())
			throw std::out_of_range("Index out of range");
		return (*this)[index];
	}

private:
	value_type* wrap(value_type* ptr) const
	{
		assert(ptr < m_buffer + m_capacity * 2);
		if (ptr >= m_buffer + m_capacity)
			return ptr - m_capacity;
		
		if (ptr < m_buffer)
			return ptr + m_capacity;
		
		return ptr;
	}
	
	const size_type m_capacity;
	allocator_type m_allocator;
	pointer m_buffer;
	pointer m_front;
	pointer m_back;
};

template<typename T, typename A>
class circular_buffer<T, A>::iterator {
public:
	using parent_type = circular_buffer<T>;
	using self_type = typename parent_type::iterator;
	using difference_type = typename parent_type::difference_type;
	using value_type = typename parent_type::value_type;
	using pointer = typename parent_type::pointer;
	using reference = typename parent_type::reference;
	using iterator_category = typename std::random_access_iterator_tag;

	iterator(parent_type* parent, size_type index, size_type wrap)
		: parent(parent), index_(index), wrap(wrap) {}

	iterator(const iterator&) = default;
	iterator(iterator&&) = default;
	iterator& operator=(const iterator&) = default;
	iterator& operator=(iterator&&) = default;
	~iterator() {}

	self_type& operator++()
	{
		++index_;
		//if (index_ == wrap)
		//	index_ = 0;
		return *this;
	}

	self_type operator++(int)
	{
		self_type old{ *this };
		operator++();
		return old;
	}

	self_type& operator--()
	{
		//if (index_ = 0)
		//	index_ = wrap;
		--index_;
		return *this;
	}

	self_type operator--(int)
	{
		self_type old{ *this };
		operator--();
		return old;
	}

	self_type& operator+=(difference_type delta)
	{
		//index_ = normalize(index_ + delta);
		index_ += delta;
		return *this;
	}

	self_type operator+(difference_type delta) const
	{
		self_type tmp{ *this };
		tmp += delta;
		return tmp;
	}

	self_type& operator-=(difference_type delta)
	{
		//if (delta > wrap)
		//	delta %= wrap;
		//index_ = normalize(index_ + wrap - delta);
		index_ -= delta;
		return *this;
	}

	self_type operator-(difference_type delta) const
	{
		self_type tmp{ *this };
		tmp -= delta;
		return tmp;
	}

	difference_type operator-(const self_type &c) const
	{
		return index_ - c.index_;
	}

	
	reference operator*() { return (*parent)[index_]; }
	pointer operator->() { return &(operator*()); }

	bool operator==(const self_type &other) const
	{
		return parent == other.parent && index_ == other.index_ && wrap == other.wrap;
	}

	bool operator!=(const self_type &other) const
	{
		return !(other == *this);
	}

	bool operator>(const self_type &other) const
	{
		return index_ > other.index_;
	}

	bool operator>=(const self_type &other) const
	{
		return index_ >= other.index_;
	}

	bool operator<(const self_type &other) const
	{
		return index_ < other.index_;
	}

	bool operator<=(const self_type &other) const
	{
		return index_ <= other.index_;
	}

private:
	size_type normalize(size_type index) const
	{
		return index % wrap;
	}

	parent_type* parent;
	size_type index_;
	size_type wrap;
};

//template <typename circular_buffer_iterator_t>
//circular_buffer_iterator_t operator+
//(const typename circular_buffer_iterator_t::difference_type &a,
//	const circular_buffer_iterator_t                           &b)
//{
//	return circular_buffer_iterator_t(nullptr, a) + b;
//}
//
//template <typename circular_buffer_iterator_t>
//circular_buffer_iterator_t operator-
//(const typename circular_buffer_iterator_t::difference_type &a,
//	const circular_buffer_iterator_t                           &b)
//{
//	return circular_buffer_iterator_t(a) - b;
//}