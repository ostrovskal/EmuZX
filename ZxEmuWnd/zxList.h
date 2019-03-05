#pragma once

template <typename T> class List {
public:
	struct Node {
		Node() : next(nullptr) { }
		Node(T* t) : type(t), next(nullptr) { }
		~Node() { delete type; }
		T* type;
		Node* next;
	};
	List() : nroot(nullptr) { }
	~List() { free(); }
	// ��������
	void add(T* t) {
		auto n = new Node(t);
		if(!nroot) nroot = n;
		else {
			auto nn = nroot;
			while(nn->next) nn = nn->next;
			nn->next = n;
		}
	}
	// �������
	void remove(Node* n) {
		auto p = parent(n);
		auto nx = n->next;
		if(p) p->next = nx; else nroot = nx;
		if(n == ncur) n = p ? p : nx;
		delete n;
	}
	// ������� ������������ ����
	Node* parent(Node* n) const {
		auto nn = nroot;
		while(nn) {
			auto nx = nn->next;
			if(nx == n) return nn;
			nn = nx;
		}
		return nullptr;
	}
	// ������� �� �������
	Node* fromPos(int pos) const {
		auto n = nroot;
		while(n) {
			if(!pos) return n;
			n = n->next;
			pos--;
		}
		return nullptr;
	}
	Node* root() const { return nroot; }
	// ���������� ��� ����
	void free() {
		auto n = nroot;
		while(n) {
			auto nn = n->next;
			delete n;
			n = nn;
		}
		nroot = nullptr;
	}
protected:
	// ������������ ����
	Node* nroot;
};
