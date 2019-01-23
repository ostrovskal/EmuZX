
#pragma once

#include "ssh_tree.h"
#include "ssh_list.h"

namespace ssh
{
	struct XmlNode
	{
		// конструктор
		XmlNode(ssh_cws name, String v) : nm(name), val(std::move(v)) { }
		// вернуть атрибут по имени
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
		// вернуть имя
		String name() const { return nm; }
		// имя узла/атрибута
		String nm;
		// значение узла/атрибута
		String val;
		// список атрибутов
		//std::list<XmlNode*> attrs;
		List<XmlNode*> attrs;
	};

	typedef Tree <XmlNode*>::Node* HXML;

	class SSH Xml
	{
	public:
		// конструкторы
		Xml() { }
		// конструктор загрузки
		Xml(const String& path) { open(path); }
		// конструктор из памяти
		Xml(const Buffer& buf);
		// деструктор
		virtual ~Xml() { close(); }
		// открыть
		void open(const String& path);
		// закрыть
		void close() { tree.reset(); }
		// сохранить
		void save(const String& path, ssh_cws code = cp_utf8);
		// вернуть количество дочерних
		ssh_u count(HXML h) const { return h->count; }
		// установить имя узла
		void set_name(HXML h, ssh_cws name) { h->value->nm = name; }
		// вернуть имя узла
		String get_name(HXML h) const { return h->value->nm; }
		// получить родительский узел
		HXML parent(HXML h) const { return h->parent; }
		// получить корневой узел
		HXML root() const { return tree.get_root(); }
		// установить значение узла
		template <typename T> void set_val(HXML h, const T& val) { h->value->val = val; }
		// получить значение узла
		template <typename T> T get_val(HXML h) const { return (T)(h->value->val); }
		// добавить атрибут
		template<typename T> void add_attr(HXML h, ssh_cws name, const T& val) { auto n(h->value->attr(name)); if(n) n->val = val; else h->value->add_attr(new XmlNode(name, val)); }
		// получить значение атрибута
		template <typename T> T get_attr(HXML h, ssh_cws name, const T& def) const { auto n(h->value->attr(name)); return (n ? (T)n->val : def); }
		// вернуть признак наличия атрибута
		bool is_attr(HXML h, ssh_cws name) const { return (h->value->attr(name) != nullptr); }
		// добавить узел
		template <typename T> HXML add_node(HXML h, ssh_cws name, const T& val) { return tree.add(h, new XmlNode(name, val)); }
		// вернуть/добавить узел
		HXML node(HXML h, ssh_cws name, ssh_l index = 0) const
		{
			if(!index) return tree.get_node_name(h, name);
			return tree.get_node_index(h->fchild, index);
		}
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	protected:
		// декодировка
		String encode(const Buffer& buf);
		// формирование дерева узлов
		void make(HXML hp, ssh_u lev);
		void _make(const Buffer& buf);
		// дерево узлов
		Tree<XmlNode*> tree;
	};
}
