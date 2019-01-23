
#pragma once

namespace ssh
{
	template <typename T> class Stack
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 32);
			// ����������� ����
			Node(const T& t, Node* n) : next(n), value(t) {}
			// ����������
			~Node() { SSH_RELEASE_NODE(T, value); }
			// ��������� ����
			Node* next;
			// ��������
			T value;
		};
		// ������������
		Stack() { }
		// ����������� �����
		Stack(const Stack<T>& src) { *this = src; }
		// ���������������� �����������
		Stack(const std::initializer_list<T>& _list) { for(auto& t : _list) push(t); }
		// ����������� ��������
		Stack(Stack&& src) { root = src.root; src.free(); }
		// ����������
		~Stack() { reset(); }
		// �������������� ������������
		void free() { root = nullptr; Node::reset(); }
		// ���������� ������� �����
		const Stack& operator += (const Stack<T>& src) { auto n(src.root); while(n) { push(n->value); n = n->next; } return *this; }
		// ������������
		const Stack& operator = (const Stack<T>& src) { reset(); return operator += (src); }
		// �������� ��������
		const Stack& operator = (Stack<T>&& src) { reset(); root = src.root; src.free(); return *this; }
		// ���������� ��������
		void push(const T& t) { root = new Node(t, root); }
		// ����������
		T pop()
		{
			if(!root) return SSH_DUMMY(T);
			T t(root->value);
			auto n(root->next);
			delete root;
			root = n;
			return t;
		}
		// ������������ �����
		void reset() { auto n(root); while(root) { n = n->next; delete root; root = n; } free(); }
		// �������� �� ������� ���������
		bool is_empty() const {return (root == nullptr);}
	protected:
		// �������
		Node* root = nullptr;
	};
}
