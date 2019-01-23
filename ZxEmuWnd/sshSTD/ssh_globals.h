
#pragma once

#include "ssh_inlines.h"
#include "ssh_buf.h"
#include "ssh_types.h"

namespace ssh
{
	class EnumReflector;

	struct DESC_WND
	{
		DESC_WND() : bkg(0), icon(0), iconSm(0), cursor(0), processingWnd(nullptr), stylesClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS), stylesWnd(WS_OVERLAPPEDWINDOW), hWndParent(nullptr), hInst(::GetModuleHandle(nullptr)), bar(0, 0, 256, 256) {}
		// заголовок
		String caption;
		// курсор
		HCURSOR cursor;
		// иконка
		HICON icon;
		// малая иконка
		HICON iconSm;
		// фон
		HBRUSH bkg;
		// имя класса
		mutable String className;
		// стили класса
		UINT stylesClass;
		// стили окна
		UINT stylesWnd;
		// хэндл родительского окна
		HWND hWndParent;
		// хэндл окна
		mutable HWND hWnd;
		// хэндл модуля
		HINSTANCE hInst;
		// габариты окна
		Bar<int> bar;
		// процедура обработки окна
		WNDPROC processingWnd;
	};

	// типы операционных систем 
	enum class WindowsTypes : ssh_w
	{
		_UNK, _NT, _2K, _XP, _VISTA, _7, _8, _8_1, _10
	};
	enum class SystemInfo : ssh_w
	{
		PROG_FOLDER, WORK_FOLDER, TEMP_FOLDER, USER_FOLDER, PROG_NAME, USER_NAME, COMP_NAME, CUSTOM, PLATFORM, TOTAL_MEMORY, PHYSICAL_MEMORY, CPU_SPEED
	};

	SSH float* ssh_vec3_mtx(float* dst, const float* v, const float* m);
	SSH float* ssh_vec4_mtx(float* dst, const float* v, const float* m);
	SSH float* ssh_mtx_vec3(const float* m, const float* v);
	SSH float* ssh_mtx_vec4(const float* m, const float* v);
	SSH float* ssh_mtx_mtx(const float* m1, const float* m2);
	SSH void* ssh_memset8(void* ptr, ssh_u set, ssh_u count);
	SSH void* ssh_memcpy8(void* dst, const void* src, ssh_u count);

	ssh_u SSH ssh_time_counter(ssh_u old_value);
	ssh_u SSH ssh_system_values(SystemInfo type);
	ssh_u SSH ssh_dll_proc(ssh_cws dll, ssh_ccs proc, ssh_cws suffix = L"d");
	ssh_u SSH ssh_offset_line(const String& text, ssh_l ln);
	ssh_u SSH ssh_count_lines(const String& text);
	ssh_u SSH ssh_split(ssh_ws split, const String& src, ssh_u* vec, ssh_u count_vec);
	ssh_u SSH ssh_dlg_save_or_open(bool bOpen, ssh_cws title, ssh_cws filter, ssh_cws ext, String& folder, HWND hWnd, String* arr = nullptr, ssh_u count = 0);
	String SSH ssh_printf(ssh_cws s, ...);
	String SSH ssh_system_paths(SystemInfo type, int csidl = CSIDL_LOCAL_APPDATA);
	String SSH ssh_slash_path(const String& path);
	String SSH ssh_file_ext(const String& path, bool is_pt = false);
	String SSH ssh_file_name(const String& path);
	String SSH ssh_file_title(const String& path);
	String SSH ssh_file_path(const String& path);
	String SSH ssh_file_path_title(const String& path);
	String SSH ssh_gen_name(ssh_cws nm, bool is_long = true);
	String SSH ssh_md5(const String& str);
	String SSH ssh_base64(ssh_cws to, const String& str);
	String SSH ssh_base64(const Buffer& buf);
	String SSH ssh_translate(String text, bool to_eng);
	String SSH ssh_num_volume(ssh_u num);
	String SSH ssh_path_in_range(const String& path, ssh_u range);
	String SSH ssh_make_guid(const GUID& guid);
	String SSH ssh_convert(ssh_cws charset, const Buffer& in, ssh_u offs);
	String SSH ssh_implode2(ssh_u val, const EnumReflector* stk, ssh_cws def, bool is_enum);
	GUID   SSH ssh_make_guid(ssh_cws src);
	Buffer SSH ssh_base64(const String& str);
	Buffer SSH ssh_convert(ssh_cws charset, const String& str);

	void SSH ssh_make_path(const String& path);
	void SSH ssh_remove_comments(String* lst, ssh_u count, bool is_simple);//
#ifdef _DEBUG
   // тест
	void SSH ssh_unit_test();
#endif
	bool SSH ssh_is_wrong_lex(const String& str, ssh_cws errLexs = nullptr);
	bool SSH ssh_dlg_sel_folder(ssh_cws title, String& folder, HWND hWnd);
	bool SSH ssh_make_wnd(const DESC_WND& desc, bool is_show_wnd);//
	// разбить строку на элементы
	template <typename T, typename = std::enable_if_t < std::is_arithmetic<T>::value, T>>
	void ssh_explode(ssh_cws split, const String& src, T* dst, ssh_u count, const T& def, const EnumReflector* stk = nullptr, Radix R = Radix::_dec)
	{
		ssh_ws* _wcs(src.buffer()), *t(_wcs);
		ssh_u j(ssh_wcslen(split));
		T tmp;
		while(count && t)
		{
			if((t = ssh_wcsstr(_wcs, split))) *t = 0;
			if(stk)
			{
				ssh_u val(0);
				ssh_ws* tt;
				while(true)
				{
					if((tt = (ssh_ws*)ssh_wcschr(_wcs, L'|'))) *tt = 0;
					ssh_u idx(stk->find(_wcs));
					if(idx == -1) { val = (ssh_u)def; break; }
					val |= stk->at(idx).value;
					if(!tt) break;
					*tt = L'|';
					_wcs = ++tt;
				}
				tmp = (T)val;
			}
			else tmp = String(_wcs).to_num<T>(0, R);
			*dst++ = (T)tmp;
			if(t) { *t = *split; _wcs = t + j; }
			count--;
		}
		// заполняем значениями по умолчанию
		while(count--) *dst++ = def;
	}
	// соединить элементы в строку
	template <typename T, typename = std::enable_if_t < std::is_arithmetic<T>::value, T>>
	String ssh_implode(ssh_cws split, T* src, ssh_u count, ssh_cws def, const EnumReflector* stk = nullptr, bool is_enum = false, Radix R = Radix::_dec)
	{
		String ret, _tmp;

		while(count--)
		{
			T tmp(*src++);
			if(stk)
			{
				_tmp.empty();
				_tmp = ssh_implode2((int)tmp, def, is_enum);
			}
			else _tmp = String(tmp, R);
			if(!ret.is_empty()) ret += split;
			ret += _tmp;
		}
		return ret;
	}
	// сформировать строку байт в hex счислении
	template <typename T, typename = std::enable_if_t<std::is_integral<T>::value, T>>
	String ssh_make_hex_string(T* p, ssh_u count, String& txt, bool is_w, bool is_cont)
	{
		String bytes(L'\0', count * (sizeof(T) * 2 + 1)), _spc(L" ");
		ssh_ws* _ws(bytes.buffer());
		String gran(ssh_printf(L"%%0%iI64x ", sizeof(T) * 2));
		for(ssh_u i = 0; i < count; i++)
		{
			swprintf(_ws, gran, p[i]);
			_ws += (sizeof(T) * 2) + 1;
		}
		ssh_cs* _cs((ssh_cs*)p);
		txt.empty();
		count *= sizeof(T);
		while(count--)
		{
			ssh_ws val(is_w ? *(ssh_ws*)_cs : *_cs);
			_cs += 1 + is_w;
			count -= is_w;
			if(val < 33) val = L'.';
			txt += val + _spc;
		}
		if(is_cont) bytes += L". . .";
		return bytes;
	}
}
