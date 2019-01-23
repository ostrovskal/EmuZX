
#pragma once

#include "ssh_tree.h"
#include "ssh_list.h"

namespace ssh
{
	struct XmlNode
	{
		// �����������
		XmlNode(ssh_cws name, String v) : nm(name), val(std::move(v)) { }
		// ������� ������� �� �����
		XmlNode* attr(ssh_cws name) const
		{
			auto h(ssh_hash(name));
			for(auto n : attrs)
			{
				if(n->nm.hash() == h) return n;
			}
			return nullptr;
		}
		void add_attr(XmlNode* n) { attrs += n; }
		// ������� ���
		String name() const { return nm; }
		// ��� ����/��������
		String nm;
		// �������� ����/��������
		String val;
		// ������ ���������
		//std::list<XmlNode*> attrs;
		List<XmlNode*> attrs;
	};

	typedef Tree <XmlNode*>::Node* HXML;

	class SSH Xml
	{
	public:
		// ������������
		Xml() { }
		// ����������� ��������
		Xml(const String& path) { open(path); }
		// ����������� �� ������
		Xml(const Buffer& buf);
		// ����������
		virtual ~Xml() { close(); }
		// �������
		void open(const String& path);
		// �������
		void close() { tree.reset(); }
		// ���������
		void save(const String& path, ssh_cws code = cp_utf8);
		// ������� ���������� ��������
		ssh_u count(HXML h) const { return h->count; }
		// ���������� ��� ����
		void set_name(HXML h, ssh_cws name) { h->value->nm = name; }
		// ������� ��� ����
		String get_name(HXML h) const { return h->value->nm; }
		// �������� ������������ ����
		HXML parent(HXML h) const { return h->parent; }
		// �������� �������� ����
		HXML root() const { return tree.get_root(); }
		// ���������� �������� ����
		template <typename T> void set_val(HXML h, const T& val) { h->value->val = val; }
		// �������� �������� ����
		template <typename T> T get_val(HXML h) const { return (T)(h->value->val); }
		// �������� �������
		template<typename T> void add_attr(HXML h, ssh_cws name, const T& val) { auto n(h->value->attr(name)); if(n) n->val = val; else h->value->add_attr(new XmlNode(name, val)); }
		// �������� �������� ��������
		template <typename T> T get_attr(HXML h, ssh_cws name, const T& def) const { auto n(h->value->attr(name)); return (n ? (T)n->val : def); }
		// ������� ������� ������� ��������
		bool is_attr(HXML h, ssh_cws name) const { return (h->value->attr(name) != nullptr); }
		// �������� ����
		template <typename T> HXML add_node(HXML h, ssh_cws name, const T& val) { return tree.add(h, new XmlNode(name, val)); }
		// �������/�������� ����
		HXML node(HXML h, ssh_cws name, ssh_l index = 0) const
		{
			if(!index) return tree.get_node_name(h, name);
			return tree.get_node_index(h->fchild, index);
		}
#ifdef _DEBUG
		// ����
		static void unit_test();
#endif
	protected:
		// �����������
		String encode(const Buffer& buf);
		// ������������ ������ �����
		void make(HXML hp, ssh_u lev);
		void _make(const Buffer& buf);
		// ������ �����
		Tree<XmlNode*> tree;
	};
}
