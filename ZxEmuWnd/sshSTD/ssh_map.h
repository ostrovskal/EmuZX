
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
				// ключь
				K key;
				// значение
				T val;
			};
			// конструктор
			Node(const K& k, const T& t, Node* n) : next(n) { value.key = k; value.val = t; }
			// деструктор
			~Node() { SSH_RELEASE_NODE(T, value.val); SSH_RELEASE_NODE(K, value.key); }
			// следующий узел
			Node* next;
			// значение
			Value value;
		};
	protected:
		class Cursor
		{
		public:
			// добавление нового
			Cursor(Map<T, K>* arr, const K& k) : node(new Node(k, T(), arr->cells)) { arr->cells = node; }
			// возврат
			Cursor(Node* n) : node(n){}
			Cursor& operator = (const T& value) { SSH_RELEASE_NODE(T, node->value.val); node->value.val = value; return *this; }
			operator T() const { return node->value.val; }
			T operator->() const { return node->value.val; }
		protected:
			// узел
			Node* node;
		};
	public:
		// конструктор по умолчанию
		Map() { }
		// конструктор копии
		Map(const Map<T, K>& src) { *this = src; }
		// конструктор переноса
		Map(Map<T, K>&& src) { cells = src.cells; src.free(); }
		// деструктор
		~Map() { reset(); }
		// принудительное освобождение
		void free() { cells = nullptr; Node::reset(); }
		// присваивание
		const Map& operator = (const Map<T, K>& src) { reset(); return operator += (src); }
		const Map& operator = (Map<T, K>&& src) { reset(); cells = src.cells; src.free(); return *this; }
		// приращение
		const Map& operator += (const Map<T, K>& src) { for(auto n : src) operator[](n.key) = n.val; return *this; }
		// количество элементов
		ssh_u count() const { ssh_u c(0); auto n(cells); while(n) n = n->next, c++; return c; }
		// установка/возврат
		Cursor operator[](const K& key) { auto n(cells); while(n) { if(n->value.key == key) return Cursor(n); n = n->next; } return Cursor(this, key); }
		// вернуть все ключи
		Map<K, ssh_u> keys() const { auto n(cells); Map<K, ssh_u> keys; ssh_u i(0); while(n) { keys[i++] = n->value.key; n = n->next; } return keys; }
		// вернуть ключь по значению
		K get_key(const T& value) const { for(auto n : this) if(n.val == value) return n.key; return SSH_DUMMY(K); }
		// проверка - такой ключ существует?
		bool is_key(const K& key) const { for(auto n : this) if(n.key == key) return true; return false; }
		// удаление элемента
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
		// итерация по карте
		Iter<Node> begin() const { return Iter<Node>(cells); }
		Iter<Node> end() const { return Iter<Node>(nullptr); }
		// вернуть корень
		bool is_empty() const { return (cells == nullptr); }
		// удаление всего
		void reset() { auto n(cells); while(cells) { auto n(cells->next); delete cells; cells = n; } free(); }
	protected:
		// вернуть узел по ключу
		Node* get_key(const K& key, Node** p) const { auto n(cells); while(n) {if(n->value.key == key) return n; if(p) *p = n; n = n->next;} return nullptr; }
		// корневой элемент
		Node* cells = nullptr;
	};
}
