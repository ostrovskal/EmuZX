
#pragma once

namespace ssh
{
	template <typename T, typename K> class Map
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 64);
			struct Value
			{
				// �����
				K key;
				// ��������
				T val;
			};
			// �����������
			Node(const K& k, const T& t, Node* n) : next(n) { value.key = k; value.val = t; }
			// ����������
			~Node() { SSH_RELEASE_NODE(T, value.val); SSH_RELEASE_NODE(K, value.key); }
			// ��������� ����
			Node* next;
			// ��������
			Value value;
		};
	protected:
		class Cursor
		{
		public:
			// ���������� ������
			Cursor(Map<T, K>* arr, const K& k) : node(new Node(k, T(), arr->cells)) { arr->cells = node; }
			// �������
			Cursor(Node* n) : node(n){}
			Cursor& operator = (const T& value) { SSH_RELEASE_NODE(T, node->value.val); node->value.val = value; return *this; }
			operator T() const { return node->value.val; }
			T operator->() const { return node->value.val; }
		protected:
			// ����
			Node* node;
		};
	public:
		// ����������� �� ���������
		Map() { }
		// ����������� �����
		Map(const Map<T, K>& src) { *this = src; }
		// ����������� ��������
		Map(Map<T, K>&& src) { cells = src.cells; src.free(); }
		// ����������
		~Map() { reset(); }
		// �������������� ������������
		void free() { cells = nullptr; Node::reset(); }
		// ������������
		const Map& operator = (const Map<T, K>& src) { reset(); return operator += (src); }
		const Map& operator = (Map<T, K>&& src) { reset(); cells = src.cells; src.free(); return *this; }
		// ����������
		const Map& operator += (const Map<T, K>& src) { for(auto n : src) operator[](n.key) = n.val; return *this; }
		// ���������� ���������
		ssh_u count() const { ssh_u c(0); auto n(cells); while(n) n = n->next, c++; return c; }
		// ���������/�������
		Cursor operator[](const K& key) { auto n(cells); while(n) { if(n->value.key == key) return Cursor(n); n = n->next; } return Cursor(this, key); }
		// ������� ��� �����
		Map<K, ssh_u> keys() const { auto n(cells); Map<K, ssh_u> keys; ssh_u i(0); while(n) { keys[i++] = n->value.key; n = n->next; } return keys; }
		// ������� ����� �� ��������
		K get_key(const T& value) const { for(auto n : this) if(n.val == value) return n.key; return SSH_DUMMY(K); }
		// �������� - ����� ���� ����������?
		bool is_key(const K& key) const { for(auto n : this) if(n.key == key) return true; return false; }
		// �������� ��������
		void remove(const K& key)
		{
			Node* n, *p;
			if((n = get_key(key, &p)))
			{
				auto nn(n->next); n->next = nullptr;
				cells == n ? cells = nn : p->next = nn;
				delete n;
			}
		}
		// �������� �� �����
		Iter<Node> begin() const { return Iter<Node>(cells); }
		Iter<Node> end() const { return Iter<Node>(nullptr); }
		// ������� ������
		bool is_empty() const { return (cells == nullptr); }
		// �������� �����
		void reset() { auto n(cells); while(cells) { auto n(cells->next); delete cells; cells = n; } free(); }
	protected:
		// ������� ���� �� �����
		Node* get_key(const K& key, Node** p) const { auto n(cells); while(n) {if(n->value.key == key) return n; if(p) *p = n; n = n->next;} return nullptr; }
		// �������� �������
		Node* cells = nullptr;
	};
}
