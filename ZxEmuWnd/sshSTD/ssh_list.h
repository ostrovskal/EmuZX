
#pragma once

namespace ssh
{
	template <typename T> class List
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 64);
			// конструктор
			Node(const T& t, Node* p, Node* n) : prev(p), next(n), value(t) {}
			// деструктор
			~Node() { SSH_RELEASE_NODE(T, value); }
			// следующий
			Node* next;
			// предыдущий
			Node* prev;
			// значение
			T value;
		};
		// конструктор
		List() { }
		// конструктор копии
		List(const List<T>& src) { *this = src; }
		// конструктор из списка инициализации
		List(const std::initializer_list<T>& _list) { for(auto& n : _list) add(n); }
		// конструктор переноса
		List(List<T>&& src) { nroot = src.nroot; nlast = src.nlast; src.free(); }
		// деструктор
		~List() { reset(); }
		// принудительное освобождение
		void free() noexcept { count = 0; nroot = nlast = nullptr; Node::reset(); }
		// количество узлов
		int size() const noexcept { return count; }
		// приращение
		Node* operator += (const T& n) noexcept { return add(n); }
		const List& operator += (const List<T>& src) noexcept { for(auto n : src) add(n); return *this; }
		// присваивание
		const List& operator = (const List<T>& src) noexcept { reset(); return operator += (src); }
		const List& operator = (List<T>&& src) noexcept { reset(); nroot = src.nroot; nlast = src.nlast; src.free(); return *this; }
		// вставка(после n)
		auto add(const T& t, Node* n = nullptr) noexcept
		{
			if(!n) n = nlast;
			auto nn(n ? n->next : nullptr);
			auto nd(new Node(t, n, nn));
			if(nn) nn->prev = nd;
			if(n) n->next = nd;
			if(n == nroot) nroot = nd;
			if(!nlast) nlast = nd;
			count++;
			return nd;
		}
		// вставка(перед n)
		auto insert(const T& t, Node* n = nullptr) noexcept
		{
			if(!n) n = nroot;
			auto np(n ? n->prev : nullptr);
			auto nd(new Node(t, np, n));
			if(np) np->next = nd;
			if(n) n->prev = nd;
			if(n == nroot) nroot = nd;
			if(!nlast) nlast = nd;
			count++;
			return nd;
		}
		// удалить
		void remove(Node* nd) noexcept
		{
			if(nd)
			{
				auto n(nd->next);
				auto p(nd->prev);
				if(nd == nroot) nroot = n;
				if(nd == nlast) nlast = p;
				if(n) n->prev = p;
				if(p) p->next = n;
				delete nd;
				count--;
			}
		}
		// найти дочерний по значению
		auto get_node_value(const T& t) const noexcept
		{
			auto n(root());
			while(n)
			{
				if(n->value == t) break;
				n = n->next;
			}
			return n;
		}
		// найти дочерний по имени
		auto get_node_name(const String& name) const noexcept
		{
			auto n(root());
			while(n)
			{
				if(n->value->name() == name) break;
				n = n->next;
			}
			return n;
		}
		// итерация по списку
		auto begin() const noexcept { return Iter<Node>(nroot); }
		auto end() const noexcept { return Iter<Node>(nullptr); }
		auto root() const noexcept { return nroot; }
		auto last() const noexcept { return nlast; }
		// сброс
		void reset() noexcept { while(nroot) { auto n(nroot->next); delete nroot; nroot = n; } free(); }
		// вернуть признак пустого
		bool is_empty() const noexcept { return (nroot == nullptr); }
	protected:
		// количество узлов
		int count = 0;
		// корень
		Node* nroot = nullptr;
		// последний
		Node* nlast = nullptr;
	};
}
