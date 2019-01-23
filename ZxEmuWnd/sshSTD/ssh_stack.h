
#pragma once

namespace ssh
{
	template <typename T> class Stack
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 32);
			// конструктор узла
			Node(const T& t, Node* n) : next(n), value(t) {}
			// деструктор
			~Node() { SSH_RELEASE_NODE(T, value); }
			// следующий узел
			Node* next;
			// значение
			T value;
		};
		// конструкторы
		Stack() { }
		// конструктор копии
		Stack(const Stack<T>& src) { *this = src; }
		// инициализирующий конструктор
		Stack(const std::initializer_list<T>& _list) { for(auto& t : _list) push(t); }
		// конструктор переноса
		Stack(Stack&& src) { root = src.root; src.free(); }
		// деструктор
		~Stack() { reset(); }
		// принудительное освобождение
		void free() { root = nullptr; Node::reset(); }
		// приращение другого стека
		const Stack& operator += (const Stack<T>& src) { auto n(src.root); while(n) { push(n->value); n = n->next; } return *this; }
		// присваивание
		const Stack& operator = (const Stack<T>& src) { reset(); return operator += (src); }
		// оператор переноса
		const Stack& operator = (Stack<T>&& src) { reset(); root = src.root; src.free(); return *this; }
		// добавление элемента
		void push(const T& t) { root = new Node(t, root); }
		// извлечение
		T pop()
		{
			if(!root) return SSH_DUMMY(T);
			T t(root->value);
			auto n(root->next);
			delete root;
			root = n;
			return t;
		}
		// освобождение стэка
		void reset() { auto n(root); while(root) { n = n->next; delete root; root = n; } free(); }
		// проверка на наличие элементов
		bool is_empty() const {return (root == nullptr);}
	protected:
		// вершина
		Node* root = nullptr;
	};
}
