
#pragma once

namespace ssh
{
	template <typename T> class List
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 64);
			// �����������
			Node(const T& t, Node* p, Node* n) : prev(p), next(n), value(t) {}
			// ����������
			~Node() { SSH_RELEASE_NODE(T, value); }
			// ���������
			Node* next;
			// ����������
			Node* prev;
			// ��������
			T value;
		};
		// �����������
		List() { }
		// ����������� �����
		List(const List<T>& src) { *this = src; }
		// ����������� �� ������ �������������
		List(const std::initializer_list<T>& _list) { for(auto& n : _list) add(n); }
		// ����������� ��������
		List(List<T>&& src) { nroot = src.nroot; nlast = src.nlast; src.free(); }
		// ����������
		~List() { reset(); }
		// �������������� ������������
		void free() noexcept { count = 0; nroot = nlast = nullptr; Node::reset(); }
		// ���������� �����
		int size() const noexcept { return count; }
		// ����������
		Node* operator += (const T& n) noexcept { return add(n); }
		const List& operator += (const List<T>& src) noexcept { for(auto n : src) add(n); return *this; }
		// ������������
		const List& operator = (const List<T>& src) noexcept { reset(); return operator += (src); }
		const List& operator = (List<T>&& src) noexcept { reset(); nroot = src.nroot; nlast = src.nlast; src.free(); return *this; }
		// �������(����� n)
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
		// �������(����� n)
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
		// �������
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
		// ����� �������� �� ��������
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
		// ����� �������� �� �����
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
		// �������� �� ������
		auto begin() const noexcept { return Iter<Node>(nroot); }
		auto end() const noexcept { return Iter<Node>(nullptr); }
		auto root() const noexcept { return nroot; }
		auto last() const noexcept { return nlast; }
		// �����
		void reset() noexcept { while(nroot) { auto n(nroot->next); delete nroot; nroot = n; } free(); }
		// ������� ������� �������
		bool is_empty() const noexcept { return (nroot == nullptr); }
	protected:
		// ���������� �����
		int count = 0;
		// ������
		Node* nroot = nullptr;
		// ���������
		Node* nlast = nullptr;
	};
}
