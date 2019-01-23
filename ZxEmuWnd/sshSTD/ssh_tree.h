
#pragma once

namespace ssh
{
	template <typename T> class Tree
	{
	public:
		struct Node
		{
			SSH_NEW_DECL(Node, 128);
			// ����������� ����
			Node(const T& t, Node* p) : fchild(nullptr), lchild(nullptr), next(nullptr), prev(nullptr), parent(p), count(0), value(t) {}
			// ����������
			~Node() { SSH_RELEASE_NODE(T, value); }
			// ��������
			T value;
			// ������ ��������
			Node* fchild;
			// ��������� ��������
			Node* lchild;
			// ��������� �� ������
			Node* next;
			// ���������� �� ������
			Node* prev;
			// �������� (������� �������� -1 �� ��������)
			Node* parent;
			// ���������� ��������
			ssh_u count;
		};
		// �����������
		Tree() { }
		// ����������� �����
		Tree(const Tree<T>& src) { *this = src; }
		// ����������� ��������
		Tree(Tree<T>&& src) { root = src.root; src.free(); }
		// ����������
		~Tree() { reset(); }
		// �������������� ������������
		void free() { root = nullptr; Node::reset(); }
		// �����
		void reset() { reset(root); free(); }
		// ������������
		const Tree& operator = (const Tree<T>& src) { reset(); return operator += (src); }
		const Tree& operator = (Tree<T>&& src) { reset(); root = src.root; src.free(); return *this; }
		// ����������
		const Tree& operator += (const Tree<T>& src) { add(root, src); return *this; }
		// �������� ����
		void remove(Node* n) { remove(n, true); }
		// ���������� �������� ����
		void set(Node* n, const T& t) { SSH_RELEASE_NODE(T, n->value); n->value = t; }
		// �������� �� ����������� ����? � ������ ������ p = ������������ ����->fchild
		bool is_move(Node* p, Node* n) const
		{
			// �������� �� ���� �������� �� �������������
			while(p)
			{
				if(p == n) return false;
				if(p->fchild) {if(!is_move(p->fchild, n)) return false;}
				p = p->next;
			}
			return true;
		}
		// �����������
		Node* move(Node* n, bool is_last) {remove(n, false); return (is_last ? add(n->parent, n) : insert(n->parent, n)); }
		// �������� ��������
		Node* add(Node* n, const T& t) { return add(n, new Node(t, n)); }
		// �������� ������
		Node* add(Node* n, const Tree<T>& src) { return add(n, src.root); }
		// �������� ��������		
		Node* insert(Node* n, const T& t) { return insert(n, new Node(t, n)); }
		// �������� ������ ������
		Node* insert(Node* n, const Tree<T>& src) { return insert(n, src.get_root()); }
		// ������� ���� �� �������
		Node* get_node_index(Node* n, ssh_l idx) const
		{
			while(n)
			{
				if(--idx < 0) return n;
				n = n->next;
			}
			return nullptr;
		}
		// ����� �������� �� ��������
		Node* get_node_value(Node* n, const T& t) const
		{
			Node* nn;
			while(n)
			{
				if(n->value == t) return n;
				if(n->fchild) { if((nn = get_node_value(n->fchild, t))) return nn; }
				n = n->next;
			}
			return nullptr;
		}
		// ����� �������� �� �����
		Node* get_node_name(Node* n, const String& name) const
		{
			Node* nn;
			while(n)
			{
				if(n->value->name() == name) return n;
				if(n->fchild) { if((nn = get_node_name(n->fchild, name))) return nn; }
				n = n->next;
			}
			return nullptr;
		}
		// ������
		Node* get_root() const { return root; }
	protected:
		// �����
		void reset(Node* n)
		{
			while(n)
			{
				if(n->fchild) reset(n->fchild);
				auto nn(n->next);
				delete n;
				n = nn;
			}
		}
		void remove(Node* n, bool is_del)
		{
			Node* p(n->parent), *nn(n->next), *pn(n->prev);
			n->next = n->prev = nullptr;
			if(p) {p->count--; if(n == p->fchild) p->fchild = nn; if(n == p->lchild) p->lchild = pn;}
			if(is_del) delete n;
			if(nn) nn->prev = pn;
			if(pn) pn->next = nn;
			if(n == root) root = nullptr;
		}
		Node* add(Node* p, Node* n)
		{
			if(p)
			{
				if(p->fchild) {p->lchild->next = n; n->prev = p->lchild;} else p->fchild = n;
				p->count++;
				p->lchild = n;
				n->next = nullptr;
			} else root = n;
			return n;
		}
		Node* insert(Node* p, Node* n)
		{
			if(p)
			{
				if(p->fchild) {p->fchild->prev = n; n->next = p->fchild;} else p->lchild = n;
				p->count++;
				p->fchild = n;
				n->prev = nullptr;
			}
			else if(!root) root = n;
			return n;
		}
		// ������
		Node* root = nullptr;
	};
}
