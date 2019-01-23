
#include "stdafx.h"
#include "StringZX.h"

StringZX::StringZX(ssh_cws cws, ssh_l len) {
	init();
	ssh_l t(wcslen(cws));
	make(cws, (len < 0 || len >= t) ? t : len);
}

StringZX::StringZX(ssh_ccs ccs, ssh_l len) {
	init();
	if(ccs) {
		size_t sz = 1000;
		ssh_l t(strlen(ccs));
		if(len < 0 || len >= t) len = t;
		mbstowcs_s(&sz, alloc(len, false), len + 1, ccs, len);
	}
}

StringZX::StringZX(ssh_ws ws, ssh_l rep) {
	init();
	auto buf(alloc(rep, false));
	while(rep-- > 0) { *buf++ = ws; }
}

const StringZX& StringZX::add(ssh_cws wcs, ssh_l len) {
	ssh_l l(length());
	ssh_memcpy(alloc(l + len, true) + l, wcs, len * 2);
	return *this;
}

const StringZX& StringZX::make(ssh_cws wcs, ssh_l len) {
	ssh_memcpy(alloc(len, false), wcs, len * 2);
	return *this;
}

ssh_ws* StringZX::alloc(ssh_l sz, bool is_copy) {
	if(sz) {
		ssh_ws* _new(buffer());
		ssh_l nsz(sz + 1);
		if(nsz > _str.len_buf) {
			if(nsz < 8192) nsz = ssh_pow2(nsz * sizeof(ssh_ws), false);
			// выделим память под новый буфер
			_new = new ssh_ws[nsz];
			// скопировать старый, если необходимо
			if(is_copy && !is_empty()) ssh_memcpy(_new, str(), length() * 2);
			// стереть старый
			empty();
			// инициализируем новый
			_str.len_buf = (int)nsz;
			_str.ptr = _new;
		}
		_str.len = (int)sz;
		_new[sz] = 0;
		return _new;
	}
	empty();
	return _str.str;
}

StringZX StringZX::substr(ssh_u idx, ssh_l len) const {
	auto l(length());
	if(idx <= l) {
		if(len < 0) len = l;
		if((idx + len) > l) len = (l - idx);
	} else idx = len = 0;
	return StringZX(str() + idx, len);
}

StringZX StringZX::add(ssh_cws wcs1, ssh_l len1, ssh_cws wcs2, ssh_l len2) {
	StringZX ret(L'\0', len1 + len2);
	ssh_memcpy(ssh_memcpy(ret.buffer(), wcs1, len1 * 2), wcs2, len2 * 2);
	return ret;
}

void StringZX::_replace(ssh_cws _old, ssh_cws _new) {
	auto l_old(wcslen(_old)), l_new(wcslen(_new)), nCount(0ULL);
	auto b_old(buffer()), b_new(b_old), buf(b_old);
	// расчитать новый размер
	while((buf = wcsstr(buf, _old))) nCount++, buf += l_old;
	if(nCount) {
		// выделяем память под новый буфер
		StringZX ret(L'\0', length() + (l_new - l_old) * nCount); b_new = ret.buffer();
		// непосредственно замена
		while((buf = wcsstr(b_old, _old))) {
			b_new = (ssh_ws*)ssh_memcpy(ssh_memcpy(b_new, b_old, (buf - b_old) * 2), _new, l_new * 2);
			b_old = buf + l_old;
		}
		wcscpy_s(b_new, l_new, b_old);
		*this = ret;
	}
}

const StringZX& StringZX::replace(ssh_cws _old, ssh_cws _new) {
	_replace(_old, _new);
	return *this;
}

const StringZX& StringZX::replace(ssh_ws _old, ssh_ws _new) {
	ssh_ws* ptr(buffer());
	while(*ptr) { if(*ptr == _old) *ptr = _new; ptr++; }
	return *this;
}

const StringZX& StringZX::remove(ssh_cws wcs) {
	auto nWcs(wcslen(wcs)), nLen(length());
	ssh_ws* f(buffer()), *_buf(f);
	while((f = wcsstr(f, wcs))) { nLen -= nWcs; ssh_memcpy(f, f + nWcs, ((nLen - (f - _buf)) + 1) * 2); }
	_str.len = (int)nLen;
	return *this;
}

const StringZX& StringZX::remove(ssh_ws ws) {
	ssh_ws* ptr(buffer()), *rem(ptr);
	while(*ptr) { if(*ptr != ws) *rem++ = *ptr; ptr++; }
	*rem = 0;
	_str.len -= (int)(ptr - rem);
	return *this;
}

const StringZX& StringZX::remove(ssh_u idx, ssh_l len) {
	auto l(length());
	if(idx < l) {
		if(len < 0) len = l;
		if((idx + len) > l) len = (l - idx);
		ssh_l ll(idx + len);
		ssh_ws* buf(buffer());
		ssh_memcpy(buf + idx, buf + ll, ((l - ll) + 1) * 2);
		_str.len -= (int)len;
	}
	return *this;
}

const StringZX& StringZX::insert(ssh_u idx, ssh_cws wcs) {
	auto len(length()), nWcs(wcslen(wcs));
	if(idx < len && alloc(len + nWcs, true)) {
		ssh_ws* _buf(buffer());
		memmove(_buf + idx + nWcs, _buf + idx, ((len - idx) + 1) * 2);
		ssh_memcpy(_buf + idx, wcs, nWcs * 2);
	}
	return *this;
}

const StringZX& StringZX::insert(ssh_u idx, ssh_ws ws) {
	auto len(length());
	if(idx < len && alloc(len + 1, true)) {
		ssh_ws* _buf(buffer());
		memmove(_buf + idx + 1, _buf + idx, ((len - idx) + 1) * 2);
		_buf[idx] = ws;
	}
	return *this;
}

const StringZX& StringZX::replace(ssh_cws* _old, ssh_cws _new) {
	ssh_l idx(0);
	while(_old[idx]) {
		_replace(_old[idx++], _new);
		_new += (wcslen(_new) + 1);
	}
	return *this;
}

const StringZX& StringZX::load(ssh_u id) {
	HINSTANCE hInst(::GetModuleHandle(nullptr));
	HRSRC h(::FindResourceW(hInst, MAKEINTRESOURCE((((id >> 4) + 1) & static_cast<ssh_w>(~0))), RT_STRING));
	int len(::SizeofResource(hInst, h) / sizeof(ssh_ws));
	if(alloc(len, false)) {
		::LoadString(hInst, (UINT)id, buffer(), len);
	}
	return *this;
}

const bool is_chars(ssh_ws* ws, ssh_cws wcs, ssh_l ln) {
	for(ssh_l i = 0; i < ln; i++) {
		if(*ws == wcs[i]) return true;
	}
	return (*ws == L' ');
}

const StringZX& StringZX::trim_left(ssh_cws wcs) {
	ssh_l len(length()), ln(wcslen(wcs));
	ssh_ws* _ws(buffer()), *_buf(_ws);
	while(is_chars(_ws, wcs, ln)) { len--; _ws++; }
	memcpy(_buf, _ws, (len + 1) * 2);
	_str.len = (int)len;
	return *this;
}

const StringZX& StringZX::trim_right(ssh_cws wcs) {
	ssh_l len(length()), ln(wcslen(wcs));
	ssh_ws* _ws(buffer() + len - 1);
	while(is_chars(_ws, wcs, ln)) { len--; _ws--; }
	buffer()[len] = 0;
	_str.len = (int)len;
	return *this;
}
