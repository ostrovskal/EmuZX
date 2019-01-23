
#include "stdafx.h"
#include "ssh_xml.h"
#include "ssh_file.h"
#include "ssh_regx.h"

namespace ssh
{
	static ssh_ws* _xml(nullptr);
	static ssh_w vec[512];

	Xml::Xml(const Buffer& buf)
	{
		try
		{
			SSH_TRACE;
			_make(buf);
		}
		catch(const Exception& e) { e.out_log(L"ѕарсер XML!"); }
	}

	void Xml::open(const String& path)
	{
		try
		{
			SSH_TRACE;
			close();
			// 1. открываем файл
			File f(path, File::open_read);
			// 2. загружаем, декодируем и строим дерево
			_make(f.read(0));
		}
		catch(const Exception& e) { e.out_log(ssh_printf(L"ѕарсер XML <%s>!", path.str())); }
	}

	String Xml::encode(const Buffer& buf)
	{
		SSH_TRACE;
		String ret, charset, caption;
		// проверить на BOM
		ssh_b _0(buf[0]), _1(buf[1]), _2(buf[2]);
		ssh_l pos;
		bool bom16be(_0 == 0xfe && _1 == 0xff);
		bool bom16le(_0 == 0xff && _1 == 0xfe);
		bool bom8(_0 == 0xef && _1 == 0xbb && _2 == 0xbf);
		int width((bom16le || bom16be) + 1);
		// определить границы заголовка xml
		if((pos = (width == 1 ? (strstr(buf.to<ssh_ccs>(), "?>") - buf.to<ssh_ccs>()) : (ssh_wcsstr(buf.to<ssh_ws*>(), L"?>") - buf.to<ssh_cws>()))) < 0) SSH_THROW(L"Ќе удалось найти заголовок XML!");
		pos += 2;
		ssh_cs _cs(buf[pos * width]);
		buf[pos * width] = 0;
		caption = (width == 1 ? ssh_convert(L"utf-8", buf, bom8 * 3) : buf.to<ssh_cws>() + 1);
		buf[pos * width] = _cs;
		Regx rx;
		if(rx.match(caption, LR"((?im)<\?xml\s+version=.+encoding=["]?(.*?)["]?\s*\?>)", 0) > 0)
		{
			charset = rx.substr(1);
			charset.lower();
			if((bom16le && charset != L"utf-16le") || (bom16be && charset != L"utf-16be") || (bom8 && charset != L"utf-8")) charset.empty();
		}
		if(charset.is_empty()) SSH_THROW(L"Ќеизвестна€ кодирока.");
		return ssh_convert(charset, buf, pos * width);
	}

	void Xml::_make(const Buffer& buf)
	{
		SSH_TRACE;
		tree.reset();
		String tmp(encode(buf));
		_xml = tmp.buffer();
		// формирование
		make(root(), 0);
	}

	static int len_vec(int idx)
	{
		return vec[idx * 2 + 1] - vec[idx * 2];
	}

	// 0,1 - весь тег с значением
	// 2,3 - начало тега
	// 4,5 - им€ тега
	// 6,7 - конец тега
	// 8,9 - значение тега
	// 10... - им€ атрибута и его значение
	void Xml::make(HXML hp, ssh_u lev)
	{
		HXML h(nullptr);

		while(_xml)
		{
			ssh_l ret(asm_ssh_parse_xml(_xml, vec));
			if(ret == -1) SSH_THROW(L"ќшибка парсинга!");
			if(ret == 0)
			{
				if(lev) SSH_THROW(L"Ќеожиданный конец!");
				return;
			}
			if((len_vec(3) == 2 && len_vec(4) > 0) || (len_vec(1) == 2 && (len_vec(4) > 0 || ret > 5 || len_vec(3) == 2))) SSH_THROW(L"Ќекорректно!");
			ssh_ws* _x(_xml);
			_xml += vec[1];
			// это завершающий тег?
			if(len_vec(1) == 2)
			{
				_x[vec[5]] = 0;
				if(get_name(hp) != (ssh_cws)(_x + vec[4])) SSH_THROW(L"ќткрывающий и завершающий теги не совпадают!");
				return;
			}
			_x[vec[5]] = 0;
			// есть значение?
			if(vec[8]) _x[vec[9]] = 0;
			if(h && !hp) SSH_THROW(L"Ќесколько корневых элементов недопустимы!");
			h = add_node(hp, _x + vec[4], (vec[8] ? _x + vec[8] : L""));
			bool is_child(len_vec(3) == 1);
			// есть атрибуты?
			if(ret >= 5)
			{
				ssh_l attr(5);
				while(attr <= ret)
				{
					_x[vec[attr * 2 + 1]] = 0;
					_x[vec[attr * 2 + 3]] = 0;
					add_attr(h, _x + vec[attr * 2], _x + vec[attr * 2 + 2]);
					attr += 2;
				}
			}
			if(is_child) make(h, lev + 1);
		}
	}

	static String _save(HXML h, ssh_l level)
	{
		auto n(h->value);
		String s(L'\t', level);
		String str(s + L"<" + n->nm);
		// атрибуты узла
		for(auto a : n->attrs) str += L" " + a->nm + L"=\"" + a->val + L"\"";
		// значение узла
		auto ch(h->fchild);
		bool is_child(ch != 0);
		bool is_val(!n->val.is_empty());
		if(is_val) str += L">\"" + n->val + L"\"";
		else if(!is_child) str += L"/>\r\n";
		// начать обработку дочерних узлов
		if(is_child)
		{
			if(!is_val) str += L'>';
			str += "\r\n";
			do { str += _save(ch, level + 1); } while(ch = ch->next);
		}
		if((is_val || is_child))
		{
			if(is_child) str += s;
			str += L"</" + n->nm + L">\r\n";
		}
		return str;
	}

	void Xml::save(const String& path, ssh_cws code)
	{
		SSH_TRACE;
		String txt(ssh_printf(L"<?xml version=\"1.0\" encoding=\"%s\" ?>\r\n", code));
		txt += _save(tree.get_root(), 0);
		File f(path, File::create_write);
		ssh_u bom(0);
		if(!ssh_wcscmp(code, L"utf-16le")) bom = 0xfeff;
		else if(!ssh_wcscmp(code, L"utf-16be")) bom = 0xfffe;
		if(bom) f.write(&bom, 2);
		f.write(txt, code);
	}

}
