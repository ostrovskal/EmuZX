
#pragma once

ssh_u ssh_pow2(ssh_u val, bool nearest);

class StringZX {
	static const int SSH_BUFFER_LENGTH = 18;
public:
	// конструкторы
	StringZX() { init(); }
	StringZX(StringZX&& str) { _str = str._str; str.init(); }
	StringZX(ssh_cws cws, ssh_l len = -1);
	StringZX(ssh_ccs ccs, ssh_l len = -1);
	StringZX(ssh_ws* ws, ssh_l len = -1) : StringZX((ssh_cws)ws, len) {}
	StringZX(ssh_b* b, ssh_l len = -1) : StringZX((ssh_ccs)b, len) {}
	StringZX(const StringZX& str) { init(); *this = str; }
	StringZX(std::nullptr_t) {} //-V730
	StringZX(ssh_ws ws, ssh_l rep);
//	template <typename T> StringZX(T v, Radix r = Radix::_dec) { init(); num(v, r); }
	// деструктор
	~StringZX() { empty(); }
	// привидение типа
	operator ssh_cws() const { return str(); }

//	template <typename T, typename = std::enable_if_t < std::is_arithmetic<T>::value, T>>
//	T to_num(ssh_l idx, Radix R = Radix::_dec) const { return *(T*)asm_ssh_wton(str() + idx, (ssh_u)R, nullptr); }
//	template <typename T, typename = std::enable_if_t < std::is_arithmetic<T>::value, T>>
//	void num(T v, Radix R = Radix::_dec) { ssh_u tmp(0); *(T*)&tmp = v; *this = asm_ssh_ntow(&tmp, (ssh_u)R, nullptr); }

//	explicit operator double() const { return to_num<double>(0, Radix::_dbl); }
//	explicit operator float() const { return to_num<float>(0, Radix::_flt); }
//	explicit operator bool() const { return to_num<bool>(0, Radix::_bool); }
//	template<typename T> operator T() const { return to_num<T>(0, Radix::_dec); }
	// вернуть по индексу
	ssh_ws operator[](ssh_u idx) const { return buffer()[idx]; }
	// операторы сравнения
	friend bool operator == (const StringZX& str1, const StringZX& str2) { return (wcscmp(str1, str2) == 0); }
	friend bool operator == (const StringZX& str, ssh_cws wcs) { return (wcscmp(str, wcs) == 0); }
	friend bool operator == (ssh_cws wcs, const StringZX& str) { return (wcscmp(wcs, str) == 0); }
	friend bool operator < (const StringZX& str1, const StringZX& str2) { return wcscmp(str1, str2); }
	friend bool operator < (const StringZX& str, ssh_cws wcs) { return (wcscmp(str, wcs) < 0); }
	friend bool operator < (ssh_cws wcs, const StringZX& str) { return (wcscmp(wcs, str) < 0); }
	friend bool operator > (const StringZX& str1, const StringZX& str2) { return wcscmp(str1, str2); }
	friend bool operator > (const StringZX& str, ssh_cws wcs) { return (wcscmp(str, wcs) > 0); }
	friend bool operator > (ssh_cws wcs, const StringZX& str) { return (wcscmp(wcs, str) > 0); }
	friend bool operator != (const StringZX& str1, const StringZX& str2) { return !(operator == (str1, str2)); }
	friend bool operator != (const StringZX& str, ssh_cws wcs) { return !(operator == (str, wcs)); }
	friend bool operator != (ssh_cws wcs, const StringZX& str) { return !(operator == (wcs, str)); }
	// операторы присваивания
	const StringZX& operator = (const StringZX& str) { return make(str, str.length()); }
	const StringZX& operator = (StringZX&& str) { empty(); _str = str._str; str.init(); return *this; }
	const StringZX& operator = (ssh_ws ws) { return make((ssh_cws)&ws, 1); }
	const StringZX& operator = (ssh_cws wcs) { return make(wcs, wcslen(wcs)); }
	// операторы контакенции
	const StringZX& operator += (const StringZX& str) { return add(str, str.length()); }
	const StringZX& operator += (ssh_ws ws) { return add((ssh_cws)&ws, 1); }
	const StringZX& operator += (ssh_cws wcs) { return add(wcs, wcslen(wcs)); }
	// дружественные операторы
	friend StringZX operator + (ssh_ws ws, const StringZX& str) { return StringZX::add((ssh_cws)&ws, 1, str, str.length()); }
	friend StringZX operator + (ssh_cws wcs, const StringZX& str) { return StringZX::add(wcs, wcslen(wcs), str, str.length()); }
	friend StringZX operator + (const StringZX& str1, const StringZX& str2) { return StringZX::add(str1, str1.length(), str2, str2.length()); }
	friend StringZX operator + (const StringZX& str, ssh_ws ws) { return StringZX::add(str, str.length(), (ssh_cws)&ws, 1); }
	friend StringZX operator + (const StringZX& str, ssh_cws wcs) { return StringZX::add(str, str.length(), wcs, wcslen(wcs)); }
	// методы
	ssh_ws* buffer() const { return (ssh_ws*)str(); }
	ssh_u length() const { return _str.len; }
	ssh_ws at(ssh_u idx) const { return (idx < length() ? str()[idx] : 0); }
	void set(ssh_u idx, ssh_ws ws) { if(idx < length()) buffer()[idx] = ws; }
	void empty() { if(_str.len_buf > SSH_BUFFER_LENGTH) delete _str.ptr; init(); }
	bool is_empty() const { return !length(); }
	bool compare(ssh_cws wcs) const { return (_wcsicmp(str(), wcs) == 0); }
	// модификация
	const StringZX& load(ssh_u id);
	const StringZX& lower() { _wcslwr_s(buffer(), length() + 1); return *this; }
	const StringZX& upper() { _wcsupr_s(buffer(), length() + 1); return *this; }
	const StringZX& reverse() { _wcsrev(buffer()); return *this; }
	const StringZX& replace(ssh_cws _old, ssh_cws _new);
	const StringZX& replace(ssh_ws _old, ssh_ws _new);
	const StringZX& replace(ssh_cws* _old, ssh_cws _new);
	const StringZX& remove(ssh_cws wcs);
	const StringZX& remove(ssh_ws ws);
	const StringZX& remove(ssh_u idx, ssh_l len = -1);
	const StringZX& insert(ssh_u idx, ssh_cws wcs);
	const StringZX& insert(ssh_u idx, ssh_ws ws);
	const StringZX& trim() { return trim(L" "); }
	const StringZX& trim(ssh_cws wcs) { trim_left(wcs); return trim_right(wcs); }
	const StringZX& trim_left(ssh_cws wcs);
	const StringZX& trim_right(ssh_cws wcs);
	// поиск
	ssh_l find(ssh_cws wcs, ssh_u idx = 0) const { auto buf(buffer()); return (idx < length() ? (wcsstr(buf + idx, wcs) - buf) : -1); }
	ssh_l find(ssh_ws ws, ssh_u idx = 0) const { auto buf(str()); return (idx < length() ? (wcschr(buf + idx, ws) - buf) : -1); }
	ssh_l find_rev(ssh_ws ws) const { auto buf(str()); return (ssh_l)(wcsrchr(buf, ws) - buf); }
	StringZX substr(ssh_u idx, ssh_l len = -1) const;
	StringZX left(ssh_u idx) const { return substr(0, idx); }
	StringZX right(ssh_u idx) const { return substr(length() - idx); }
	ssh_cws str() const { return (_str.len_buf > SSH_BUFFER_LENGTH ? _str.ptr : _str.str); }
#ifdef _DEBUG
	// тест
	static void unit_test();
#endif
protected:
#pragma pack(push, 1)
	struct STRING_BUFFER {
		union {
			ssh_ws* ptr;
			ssh_ws str[SSH_BUFFER_LENGTH];
		};
		// длина данных
		int len;
		// длина буфера
		int len_buf;
	};
#pragma pack(pop)
	static StringZX add(ssh_cws wcs1, ssh_l len1, ssh_cws wcs2, ssh_l len2);
	void _replace(ssh_cws _old, ssh_cws _new);
	void init() { _str.len = 0; _str.len_buf = SSH_BUFFER_LENGTH; _str.ptr = nullptr; }
	ssh_ws* alloc(ssh_l size, bool is_copy);
	const StringZX& add(ssh_cws wcs, ssh_l len);
	const StringZX& make(ssh_cws wcs, ssh_l len);
private:
	alignas(16) STRING_BUFFER _str;
};
