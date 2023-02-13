#ifndef MODDERN_AND_EFFECTIVE_CPP_FORWARD_LIST_HPP
#define MODDERN_AND_EFFECTIVE_CPP_FORWARD_LIST_HPP

#include <functional>
#include <memory>

namespace my {

	template<class T>
	class forward_list_node {
	public:
		using value_type = T;
		using pointer = std::shared_ptr<forward_list_node<T>>;

	public:
		forward_list_node() = default;

		forward_list_node(value_type value)
			: value(value)
		{}

		forward_list_node(pointer next)
			: next(next)
		{}

		forward_list_node(value_type value, pointer next)
			: value(value), next(next)
		{}

	public:
		pointer next{ nullptr };
		value_type value{};
	};

	template<class T>
	class forward_list_iterator {
	public:
		using value_type		= T;
		using const_reference	= const forward_list_iterator<value_type>&;
		using node_pointer		= typename forward_list_node<T>::pointer;

		template<typename> friend class forward_list;

	public:

		forward_list_iterator(node_pointer ptr) : m_node(ptr) {}

		forward_list_iterator operator ++ (int) {
			node_pointer rtrn = m_node;
			m_node = m_node->next;
			return rtrn;
		}

		forward_list_iterator operator ++ () {
			m_node = m_node->next;
			return m_node;
		}

		bool operator == (const_reference it) const noexcept {
			return m_node.get() == it.m_node.get();
		}
		bool operator != (const_reference it) const noexcept {
			return m_node.get() != it.m_node.get();
		}

		const value_type& operator*() const {
			return m_node->value;
		}

		value_type& operator*() {
			return m_node->value;
		}

	private:
		node_pointer m_node;
	};

	template <class T>
	class forward_list {
	public:
		using value_type		= T;
		using size_type			= size_t;
		using reference			= value_type&;
		using const_reference	= const value_type&;
		using iterator			= forward_list_iterator<value_type>;
		using const_iterator	= const forward_list_iterator<value_type>;
		using node				= forward_list_node<value_type>;
		using node_pointer		= typename node::pointer;

	public:
		forward_list() = default;

		forward_list(std::initializer_list<value_type>&& iList) {
			for (auto i = std::rbegin(iList); i != std::rend(iList); ++i) {
				push_front(*i);
			}
		}

		forward_list(const forward_list& other) {
			if (!other.m_head) throw std::exception("list was empty");

			m_size = other.m_size;
			auto current_node = std::make_shared<node>(other.m_head->value);
			auto current_other_node = other.m_head;
			m_head = current_node;
			while (current_other_node->next != nullptr) {
				current_node->next = std::make_shared<node>(current_other_node->next->value);
				current_node = current_node->next;
				current_other_node = current_other_node->next;
			}
			m_tail = current_node;
			m_tail->next = nullptr;
			
		}

		~forward_list() {
			clear();
		}

	public:
		void push_front(const_reference value) {
			const auto current = std::make_shared<node>(value, m_head);
			m_head = current;
			++m_size;
		}

		void push_front(value_type&& value) {
			const auto current = std::make_shared<node>(std::move(value), m_head);
			m_head = current;
			++m_size;
		}

		void pop_front() {
			if (!m_head) throw std::exception("list was empty");
			auto head = m_head;
			m_head = head->next;
			head.reset();
			--m_size;
		}

		iterator emplace_after(iterator position, const_reference value) {
			const auto pos = position.m_node;
			const auto new_node = std::make_shared<node>(value);

			if (pos == m_tail) {
				m_tail->next = new_node;
				m_tail = m_tail->next;
			}
			else {
				const auto next_tmp = pos->next;
				pos->next = new_node;
				new_node->next = next_tmp;
			}
			++m_size;
			return new_node;
		}

		iterator erase_after(iterator position) {
			if (!m_head) throw std::exception("list was empty");
			const auto pos = position.m_node;
			auto next_pos = pos->next;

			if (pos && !next_pos->next) {
				pos->next = nullptr;
			}
			else if (pos) {
				pos->next = next_pos->next;
				next_pos.reset();
			}
			--m_size;
			return ++position;
		}

	public:
		void clear() {
			while (m_size) {
				pop_front();
			}
		}

		bool empty() const noexcept {
			return !m_head;
		}

		size_type size() const noexcept {
			return m_size;
		}

		const_reference front() const {
			return m_head->value;
		}

		reference front() {
			if(!m_head) throw std::exception("list was empty");
			return m_head->value;
		}

	public:
		iterator begin() noexcept {
			return m_head;
		}

		iterator end() noexcept {
			return m_tail ? m_tail->next : nullptr;
		}

		const_iterator begin() const noexcept {
			return m_head;
		}

		const_iterator end() const noexcept {
			return m_tail ? m_tail->next : nullptr;
		}

		const_iterator cbegin() const noexcept {
			return m_tail;
		}

		const_iterator cend() const noexcept {
			return m_tail ? m_tail->next : nullptr;
		}

	private:
		size_type m_size{ 0 };
		node_pointer m_head{ nullptr };
		node_pointer m_tail{ nullptr };
	};
}

#endif // !MODDERN_AND_EFFECTIVE_CPP_FORWARD_LIST_HPP