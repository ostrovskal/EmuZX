
#include "stdafx.h"
#include "ssh_srlz.h"

namespace ssh
{
	static Serialize::SCHEME* _sc(nullptr);

	void ssh::Serialize::open(const Buffer & buf, bool is_xml)
	{
		SSH_TRACE;
		_sc = get_scheme();
		if(is_xml)
		{
			Xml xml(buf);
			readXml(xml.root(), &xml, 0, 0);
		}
		else
		{
			readBin(buf.to<ssh_b*>(), 0);
		}
	}

	void ssh::Serialize::save(const String & path, bool is_xml)
	{
		SSH_TRACE;
		_sc = get_scheme();
		if(is_xml)
		{
			Xml xml;
			writeXml(xml.root(), &xml, 0);
			xml.save(path);
		}
		else
		{
			File f(path, File::create_write);
			writeBin(&f, 0);
		}
	}

	void ssh::Serialize::readXml(HXML hp, Xml* xml, ssh_l p_offs, ssh_l idx)
	{
		HXML h;
		String sval;
		// получить узел
		if(!(h = xml->node(hp, _sc->name, idx))) SSH_THROW(ssh_printf(L"Не найден узел <%s, индекс: %i> xml!", _sc->name, idx));
		_sc++;
		while(_sc->name)
		{
			SCHEME* sc(_sc);
			ssh_u offs(sc->offs + p_offs);
			ssh_u opt(sc->opt);
			ssh_u count(sc->count);
			ssh_u width(sc->width / count);
			String val, _val;
			ssh_ws* _tmp;
			if(!(opt & (SC_NODE | SC_CV | SC_OBJ | SC_PTR)))
			{
				val = xml->get_attr<String>(h, sc->name, sc->def);
				if(opt & SC_BASE64) val = ssh_base64(val).to<ssh_cws>();
				_tmp = val.buffer();
			}
			for(ssh_u _idx = 0; _idx < count; _idx++)
			{
				if(opt & SC_NODE)
				{
					if(opt & SC_OBJ)
					{
						Serialize* srlz((Serialize*)((ssh_b*)(this) + offs));
						_sc = srlz->get_scheme();
						srlz->readXml(h, xml, 0, _idx);
						// если последний индекс
						if((_idx + 1) < count) _sc = sc; else _sc = ++sc;
					}
					else
					{
						readXml(h, xml, offs, _idx);
						if((_idx + 1) < count) _sc = sc;
					}
				}
				else if(!(opt & (SC_CV | SC_PTR)))
				{
					ssh_b* obj((ssh_b*)(this) + offs);
					if(opt & SC_LIT)
					{
						ssh_memzero(obj, width * count);
						ssh_memcpy(obj, (width == 2 ? _tmp : Buffer(ssh_convert(cp_ansi, _tmp)).to<ssh_ws*>()), val.length() * width);
						break;
					}
					if(count > 1)
					{
						ssh_ws* tmp(_tmp);
						if((tmp = (ssh_ws*)ssh_wcschr(_tmp, L','))) *tmp = 0;
						_val = _tmp;
						_tmp = tmp + 1;
					}
					else _val = std::move(val);
					if(opt & SC_FLT)
					{
						if(width == 4)  *(float*)obj = (float)_val;
						else if(width == 8) *(double*)obj = (double)_val;
					}
					else if(opt & SC_BOOL) *(bool*)obj = (bool)_val;
					else if(opt & SC_STR) *(String*)obj = _val;
					else
					{
						ssh_u ret(0);
						if(sc->stk)
						{
							ssh_ws* tmp(_val.buffer()), *_ws(tmp);
							while(tmp)
							{
								if((tmp = (ssh_ws*)ssh_wcschr(_ws, L'|'))) *tmp = 0;
								ssh_u idx(sc->stk->find(_ws));
								if(idx == -1) { ret = (ssh_u)String(sc->def); break; }
								ret |= sc->stk->at(idx).value;
								_ws = tmp + 1;
							}
						}
						else ret = _val.to_num<ssh_u>(0, (Radix)(opt & 3));
						ssh_memcpy(obj, &ret, width);
					}
				}
				offs += width;
			}
			_sc++;
		}
	}

	void ssh::Serialize::writeXml(HXML hp, Xml* xml, ssh_l p_offs)
	{
		// создать узел
		HXML h(xml->add_node(hp, _sc->name, _sc->def));
		_sc++;
		while(_sc->name)
		{
			SCHEME* sc(_sc);
			ssh_u offs(sc->offs + p_offs);
			ssh_u opt(sc->opt);
			ssh_u count(sc->count);
			ssh_u width(sc->width / count);
			String val;
			while(count--)
			{
				if(opt & SC_NODE)
				{
					if(opt & SC_OBJ)
					{
						Serialize* srlz((Serialize*)((ssh_b*)(this) + offs));
						_sc = srlz->get_scheme();
						srlz->writeXml(h, xml, 0);
						if(count) _sc = sc; else _sc = ++sc;
					}
					else
					{
						writeXml(h, xml, offs);
						if(count) _sc = sc;
					}
				}
				else if(!(opt & (SC_CV | SC_PTR)))
				{
					if(!val.is_empty() && !(opt & SC_LIT)) val += L',';
					ssh_b* obj((ssh_b*)(this) + offs);

					if(opt & SC_FLT) val += String(*(ssh_u*)obj, (sc->width == 4 ? Radix::_flt : Radix::_dbl));
					else if(opt & SC_BOOL) val += String(*(bool*)obj, Radix::_bool);
					else if(opt & SC_STR) val += *(String*)obj;
					else if(opt & SC_LIT) { if(*obj) val += (width == 2 ? *(ssh_ws*)obj : *(ssh_cs*)obj); }
					else
					{
						ssh_u lval(0);
						ssh_memcpy(&lval, obj, width);
						val += (sc->stk ? ssh_implode2(lval, sc->stk, sc->def, !(opt & SC_FLGS)) : String(lval, (Radix)(opt & 3)));
					}
				}
				offs += width;
			}
			if(!(opt & (SC_CV | SC_PTR | SC_NODE))) xml->add_attr(h, sc->name, (opt & SC_BASE64) ? ssh_base64(cp_utf16, val) : val);
			_sc++;
		}
	}

	void ssh::Serialize::writeBin(File* f, ssh_l p_offs)
	{
		_sc++;
		while(_sc->name)
		{
			SCHEME* sc(_sc);
			ssh_u offs(sc->offs + p_offs);
			ssh_u opt(sc->opt);
			ssh_u count(sc->count);
			ssh_u width(sc->width / count);
			while(count--)
			{
				if(opt & SC_NODE)
				{
					if(opt & SC_OBJ)
					{
						Serialize* srlz((Serialize*)((ssh_b*)(this) + offs));
						_sc = srlz->get_scheme();
						srlz->writeBin(f, 0);
						if(count) _sc = sc - 1; else _sc = sc + 1;
					}
					else
					{
						writeBin(f, offs);
						if(count) _sc = sc;
					}
				}
				else
				{
					if(!(opt & (SC_CV | SC_PTR)))
					{
						ssh_b* obj((ssh_b*)(this) + offs);
						if(opt & SC_STR) f->write(((void*)*(ssh_u*)obj), ((*(String*)obj).length() + 1) * 2); else f->write(obj, width);
					}
				}
				offs += width;
			}
			_sc++;
		}
	}

	void ssh::Serialize::readBin(ssh_b* buf, ssh_l p_offs)
	{
		/*
		SCHEME* sc;
		ssh_u _ID(_sc++->ID);
		while((sc = _sc++))
		{
			if(!sc->name) break;
			ssh_u flg(sc->flags);
			ssh_l offs(sc->offs + p_offs);
			ssh_l count(sc->count);
			while(count--)
			{
				if(_ID)
				{
					if(!(flg & SC_OBJ) || sc->ID == -1) { if(sc->ID != -1) _sc--; return; }
					if(sc->ID != _ID) { _sc--; readBin(offs); if(count) _sc = sc + 1; }
				}
				else if((flg & SC_OBJ)) { _sc--; readBin(offs); if(count) _sc = sc + 1; }
				else if((flg & SC_NODE)) { Serialize* srlz((Serialize*)((ssh_b*)(this) + offs)); _sc = srlz->get_scheme(); srlz->readBin(0); _sc = sc + 1; }
				if((flg & SC_VAR))
				{
					ssh_b* obj((ssh_b*)(this) + offs);
					switch(sc->hash)
					{
						case _hash_wcs:
						case _hash_ccs: break;
						case _hash_string: *(String*)obj = (ssh_ws*)_buf; _buf += (ssh_wcslen((ssh_ws*)_buf) * 2 + 2); break;
						default: ssh_memcpy(obj, _buf, sc->width); _buf += sc->width; break;
					}
				}
				offs += sc->width;
			}
		}
		*/
	}
}
