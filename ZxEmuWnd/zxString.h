
#pragma once

enum Radix { _dec, _hex, _bin, _oct, _dbl, _flt, _bool };

class zxString {
	static const int SSH_BUFFER_LENGTH = 18;
public:
	// конструкторы
	zxString() { init(); }
	zxString(zxString&& str) { _str = str._str; str.init(); }
	zxString(ssh_cws cws, ssh_l len = -1);
	zxString(ssh_ccs ccs, ssh_l len = -1);
	zxString(ssh_ws* ws, ssh_l len = -1) : zxString((ssh_cws)ws, len) {}
	zxString(ssh_b* b, ssh_l len = -1) : zxString((ssh_ccs)b, len) {}
	zxString(const zxString& str) { init(); *this = str; }
	zxString(ssh_ws ws, ssh_l rep);
	// деструктор
	~zxString() { empty(); }
	// привидение типа
	operator ssh_cws() const { return str(); }

	// вернуть по индексу
	ssh_ws operator[](ssh_u idx) const { return buffer()[idx]; }
	// операторы сравнения
	friend bool operator == (const zxString& str1, const zxString& str2) { return (wcscmp(str1, str2) == 0); }
	friend bool operator == (const zxString& str, ssh_cws wcs) { return wcscmp(str, wcs) == 0; }
	friend bool operator == (ssh_cws wcs, const zxString& str) { return wcscmp(wcs, str) == 0; }
	friend bool operator < (const zxString& str1, const zxString& str2) { return wcscmp(str1, str2) < 0; }
	friend bool operator < (const zxString& str, ssh_cws wcs) { return wcscmp(str, wcs) < 0; }
	friend bool operator < (ssh_cws wcs, const zxString& str) { return wcscmp(wcs, str) < 0; }
	friend bool operator > (const zxString& str1, const zxString& str2) { return wcscmp(str1, str2) > 0; }
	friend bool operator > (const zxString& str, ssh_cws wcs) { return wcscmp(str, wcs) > 0; }
	friend bool operator > (ssh_cws wcs, const zxString& str) { return wcscmp(wcs, str) > 0; }
	friend bool operator != (const zxString& str1, const zxString& str2) { return !(operator == (str1, str2)); }
	friend bool operator != (const zxString& str, ssh_cws wcs) { return !(operator == (str, wcs)); }
	friend bool operator != (ssh_cws wcs, const zxString& str) { return !(operator == (wcs, str)); }
	// операторы присваивания
	const zxString& operator = (const zxString& str) { return make(str, str.length()); }
	const zxString& operator = (zxString&& str) { empty(); _str = str._str; str.init(); return *this; }
	const zxString& operator = (ssh_ws ws) { return make((ssh_cws)&ws, 1); }
	const zxString& operator = (ssh_cws wcs) { return make(wcs, wcslen(wcs)); }
	// операторы контакенции
	const zxString& operator += (const zxString& str) { return add(str, str.length()); }
	const zxString& operator += (ssh_ws ws) { return add((ssh_cws)&ws, 1); }
	const zxString& operator += (ssh_cws wcs) { return add(wcs, wcslen(wcs)); }
	// дружественные операторы
	friend zxString operator + (ssh_ws ws, const zxString& str) { return zxString::add((ssh_cws)&ws, 1, str, str.length()); }
	friend zxString operator + (ssh_cws wcs, const zxString& str) { return zxString::add(wcs, wcslen(wcs), str, str.length()); }
	friend zxString operator + (const zxString& str1, const zxString& str2) { return zxString::add(str1, str1.length(), str2, str2.length()); }
	friend zxString operator + (const zxString& str, ssh_ws ws) { return zxString::add(str, str.length(), (ssh_cws)&ws, 1); }
	friend zxString operator + (const zxString& str, ssh_cws wcs) { return zxString::add(str, str.length(), wcs, wcslen(wcs)); }
	// методы
	ssh_ws* buffer() const { return (ssh_ws*)str(); }
	ssh_ws* reserved(ssh_u count) { alloc(count, false); return buffer(); }
	void releaseReserved() { _str.len = (int)wcslen(buffer()); }
	ssh_u length() const { return _str.len; }
	ssh_ws at(ssh_u idx) const { return (idx < length() ? str()[idx] : 0); }
	void set(ssh_u idx, ssh_ws ws) { if(idx < length()) buffer()[idx] = ws; }
	void empty() { if(_str.len_buf > SSH_BUFFER_LENGTH) delete _str.ptr; init(); }
	bool is_empty() const { return !length(); }
	bool compare(ssh_cws wcs) const { return (_wcsicmp(str(), wcs) == 0); }
	// модификация
	const zxString& load(ssh_u id);
	const zxString& lower() { _wcslwr_s(buffer(), length() + 1); return *this; }
	const zxString& upper() { _wcsupr_s(buffer(), length() + 1); return *this; }
	const zxString& reverse() { _wcsrev(buffer()); return *this; }
	const zxString& replace(ssh_cws _old, ssh_cws _new);
	const zxString& replace(ssh_ws _old, ssh_ws _new);
	const zxString& replace(ssh_cws* _old, ssh_cws _new);
	const zxString& remove(ssh_cws wcs);
	const zxString& remove(ssh_ws ws);
	const zxString& remove(ssh_u idx, ssh_l len = -1);
	const zxString& insert(ssh_u idx, ssh_cws wcs);
	const zxString& insert(ssh_u idx, ssh_ws ws);
	const zxString& trim() { return trim(L" "); }
	const zxString& trim(ssh_cws wcs) { trim_left(wcs); return trim_right(wcs); }
	const zxString& trim_left(ssh_cws wcs);
	const zxString& trim_right(ssh_cws wcs);
	static zxString fmt(ssh_cws fmt, ...);
	zxString* split(ssh_cws delim, int& count) const;
	// поиск
	ssh_l find(ssh_cws wcs, ssh_u idx = 0) const { auto buf(buffer()); return (idx < length() ? (wcsstr(buf + idx, wcs) - buf) : -1); }
	ssh_l find(ssh_ws ws, ssh_u idx = 0) const { auto buf(str()); return (idx < length() ? (wcschr(buf + idx, ws) - buf) : -1); }
	ssh_l find_rev(ssh_ws ws) const { auto buf(str()); return (ssh_l)(wcsrchr(buf, ws) - buf); }
	zxString substr(ssh_u idx, ssh_l len = -1) const;
	zxString left(ssh_u idx) const { return substr(0, idx); }
	zxString right(ssh_u idx) const { return substr(length() - idx); }
	ssh_cws str() const { return (_str.len_buf > SSH_BUFFER_LENGTH ? _str.ptr : _str.str); }
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
	static zxString add(ssh_cws wcs1, ssh_l len1, ssh_cws wcs2, ssh_l len2);
	void _replace(ssh_cws _old, ssh_cws _new);
	void init() { _str.len = 0; _str.len_buf = SSH_BUFFER_LENGTH; _str.ptr = nullptr; }
	ssh_ws* alloc(ssh_l size, bool is_copy);
	const zxString& add(ssh_cws wcs, ssh_l len);
	const zxString& make(ssh_cws wcs, ssh_l len);
private:
	alignas(16) STRING_BUFFER _str;
};
