
#pragma once

#include "ssh_str.h"

namespace ssh
{
	class Vec2;
	template <typename T> class Bar;

	class SSH Lock
	{
	public:
		Lock() { EnterCriticalSection(get_section()); }
		~Lock() { LeaveCriticalSection(get_section()); }
	protected:
		static CRITICAL_SECTION* get_section()
		{
			static CRITICAL_SECTION section;
			static bool init(false);
			if(!init) { InitializeCriticalSection(&section); init = true; }
			return &section;
		}
	};

	class SSH Base
	{
		void* operator new(ssh_u sz);
	public:
		// оператор выделения памяти под объект
		void* Base::operator new(ssh_u sz, Base** obj, String name, String type)
		{
			// 1. определяем объект уже существует?
			Base* n(root);
			while(n)
			{
				if(n->name() == name && n->type() == type)
				{
					// существует, тогда увеличиваем счётчик
					n->ref++;
					*obj = n;
					return nullptr;
				}
				n = n->next;
			}
			// не существует, создаем новый
			*obj = (Base*)::operator new(sz);
			(*obj)->ref = 1;
			(*obj)->next = root;
			//(*obj)->nm.init(); 
			(*obj)->nm = std::move(name);
			//(*obj)->tp.init(); 
			(*obj)->tp = std::move(type);
			root = *obj;
			return (void*)*obj;
		}
		// реализовать объект
		void release()
		{
			if(--ref == 0)
			{
				Base* p(nullptr);
				auto n(root);
				while(n && n != this) p = n, n = n->next;
				if(p) p->next = n->next; else root = root->next;
				delete this;
			}
		}
		// увеличить счётчик
		void add_ref() { ref++; }
		// вернуть имя
		String name() const { return nm; }
		// вернуть тип
		String type() const { return tp; }
	protected:
		Base() : nm(nullptr), tp(nullptr) {}
		// деструктор
		virtual ~Base() {  }
		// число ссылок
		ssh_l ref;
		// имя
		String nm;
	private:
		// имя типа
		String tp;
		// корень списка
		static Base* root;
		// следующий в списке
		Base* next;
	};

	class SSH Bits
	{
	public:
		// конструктор
		constexpr Bits() : value(0) {}
		// конструктор по значению
		Bits(ssh_l v) : value(v) {}
		// конструктор копии
		Bits(const Bits& src) : value(src.value) {}
		// операции
		// присваивание
		auto operator = (const Bits& src) { value = src.value; return *this; }
		auto operator = (const ssh_l src) { value = src; return *this; }
		// приведение
		operator ssh_l() const { return value; }
		// логические
		bool operator == (const ssh_l v) const { return (value == v); }
		bool operator != (const ssh_l v) const { return (value != v); }
		// функции
		// установить
		void set(const ssh_l v) { value = v; }
		// очистить
		void clear() { value = 0; }
		// добавить
		void add_flags(const ssh_l v) { value |= v; }
		// исключить
		void del_flags(const ssh_l v) { value &= ~v; }
		// установить бит
		void set_bit(ssh_l bit) { _bittestandset64(&value, bit); }
		// очистить бит
		void clear_bit(ssh_l bit) { _bittestandreset64(&value, bit); }
		// установить по признаку
		void set_bit(ssh_l bit, bool is) { is ? set_bit(bit) : clear_bit(bit); }
		// пустой ?
		bool is_empty() const { return (value == 0); }
		// проверка на бит
		bool test_bit(ssh_l bit) const { return (_bittest64(&value, bit) != 0); }
		// проверка на несколько бит
		bool test_flags(const ssh_l test) const { return ((value & test) == test); }
		// проверка на наличие хоть одного установленного
		bool test_any(const ssh_l test) const { return ((value & test) != 0); }
		// длина
		ssh_u total_bits() const { return 64; }
		// количество установленных
		ssh_u total_set() const { return __popcnt64(value); }
		// "развернуть" биты
		ssh_u reverse() const { return ssh_rev_bits((int)(value >> 32)) | ((ssh_u)(ssh_rev_bits((int)value)) << 32); }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// значение
		ssh_l value;
	};

	template <typename T = ssh_u> class Pts
	{
	public:
		// конструкторы
		constexpr Pts() : x(0), y(0) {}
		Pts(const T* ptr) { x = ptr[0]; y = ptr[1]; }
		Pts(const T& X, const T& Y) : x(X), y(Y) {}
		Pts(const Pts& pt) : x(pt.x), y(pt.y) {}
		Pts(const POINT& pt) : x(pt.x), y(pt.y) {}
		Pts(const Bar<T> b) : x(b.x), y(b.y) {}
		// Операции
		auto operator - (const Pts& p) const { return Pts(x - p.x, y - p.y); }
		auto operator - (const T& i) const { return Pts(x - i, y - i); }
		auto operator + (const Pts& p) const { return Pts(x + p.x, y + p.y); }
		auto operator + (const T& i) const { return Pts(x + i, y + i); }
		auto operator * (const Pts& p) const { return Pts(x * p.x, y * p.y); }
		auto operator * (const T& i) const { return Pts(x * i, y * i); }
		auto operator / (const Pts& p) const { return Pts(x / p.x, y / p.y); }
		auto operator / (const T& i) const { return Pts(x / i, y / i); }
		auto operator -= (const Pts& p) { x -= p.x; y -= p.y; return *this; }
		auto operator -= (const T& i) { x -= i; y -= i; return *this; }
		auto operator += (const Pts& p) { x += p.x; y += p.y; return *this; }
		auto operator += (const T& i) { x += i; y += i; return *this; }
		auto operator /= (const Pts& p) { x /= p.x; y /= p.y; return *this; }
		auto operator /= (const T& i) { x /= i; y /= i; return *this; }
		auto operator *= (const Pts& p) { x *= p.x; y *= p.y; return *this; }
		auto operator *= (const T& i) { x *= i; y *= i; return *this; }
		// присваивание
		auto operator = (const Bar<T>& r) { x = r.x; y = r.y; return *this; }
		// внешние
		friend auto operator - (const T& i, const Pts& p) { return Pts(i - p.x, i - p.y); }
		friend auto operator + (const T& i, const Pts& p) { return Pts(i + p.x, i + p.y); }
		friend auto operator / (const T& i, const Pts& p) { return Pts(i / p.x, i / p.y); }
		friend auto operator * (const T& i, const Pts& p) { return Pts(i * p.x, i * p.y); }
		// Сравнение
		bool operator == (const Pts& p) const { return (x == p.x && y == p.y); }
		bool operator != (const Pts& p) const { return (x != p.x || y != p.y); }
		bool operator == (const Bar<T>& r) const { return (x == r.x && y == r.y); }
		bool operator != (const Bar<T>& r) const { return (x != r.x || y != r.y); }
		bool is_empty() const { return (x == 0 && y == 0); }
		// Приведение типов
		operator T*() const { return (T*)&x; }
		operator POINT() const { return POINT{(int)x, (int)y}; }
		operator POINT*() const { static POINT pt; pt = {(int)x, (int)y}; return &pt; }
		// Специальные
		auto empty() { x = y = 0; return *this; }
		auto set(const T& X, const T& Y) { x = X; y = Y; return *this; }
#ifdef _DEBUG
		// тест
		static void unit_test()
		{
			Pts<> test1;
			Pts<> test2;
			Pts<> test3;
			Pts<> test4;
			Pts<> test5;
		}
#endif
		T x, y;
	};

	template <typename T = ssh_u> class Range
	{
	public:
		// Конструкторы
		Range() : w(0), h(0) {}
		Range(const T* ptr) { w = ptr[0]; h = ptr[1]; }
		Range(const T& W, const T& H) : w(W), h(H) {}
		Range(const Range& s) : w(s.w), h(s.h) {}
		Range(const SIZE& s) : w(s.cx), h(s.cy) {}
		Range(const Bar<T>& r) : w(r.w), h(r.h) {}
		// Операции
		auto operator - (const Range& s) const { return Range(w - s.w, h - s.h); }
		auto operator - (const T& i) const { return Range(w - i, h - i); }
		auto operator + (const Range& s) const { return Range(w + s.w, h + s.h); }
		auto operator + (const T& i) const { return Range(w + i, h + i); }
		auto operator * (const Range& s) const { return Range(w * s.w, h * s.h); }
		auto operator * (const T& i) const { return Range(w * i, h * i); }
		auto operator / (const Range& s) const { return Range(w / s.w, h / s.h); }
		auto operator / (const T& i) const { return Range(w / i, h / i); }
		auto operator += (const Range& s) { w += s.w; h += s.h; return *this; }
		auto operator += (const T& s) { w += s; h += s; return *this; }
		auto operator -= (const Range& s) { w -= s.w; h -= s.h; return *this; }
		auto operator -= (const T& s) { w -= s; h -= s; return *this; }
		auto operator /= (const Range& s) { w /= s.w; h /= s.h; return *this; }
		auto operator /= (const T& s) { w /= s; h /= s; return *this; }
		auto operator *= (const Range& s) { w *= s.w; h *= s.h; return *this; }
		auto operator *= (const T& s) { w *= s; h *= s; return *this; }
		// Присваивание
		auto operator = (const Bar<T>& r) { w = r.w; h = r.h; return *this; }
		// внешние
		friend auto operator - (const T& i, const Range& p) { return Range(i - p.w, i - p.h); }
		friend auto operator + (const T& i, const Range& p) { return Range(i + p.w, i + p.h); }
		friend auto operator * (const T& i, const Range& p) { return Range(i * p.w, i * p.h); }
		friend auto operator / (const T& i, const Range& p) { return Range(i / p.w, i / p.h); }
		// Операции
		bool operator == (const Range& s) const { return (w == s.w && h == s.h); }
		bool operator == (const Bar<T>& r) const { return (w == r.w && h == r.h); }
		bool operator != (const Range& s) const { return (w != s.w || h != s.h); }
		bool operator != (const Bar<T>& r) const { return (w != r.w || h != r.h); }
		// Приведение типов
		operator T*() const { return (T*)&w; }
		operator SIZE() const { return SIZE{(int)w, (int)h}; }
		operator SIZE*() const { static SIZE sz; sz = {(int)w, (int)h}; return &sz; }
		// Атрибуты
		bool is_empty() const { return (w == 0 && h == 0); }
		auto empty() { w = h = 0; return *this; }
		auto set(const T& W, const T& H) { w = W; h = H; return *this; }
#ifdef _DEBUG
		// тест
		static void unit_test()
		{
			Range test1;
			Range test2;
			Range test3;
			Range test4;
			Range test5;
		}
#endif
		T w, h;
	};

	template <typename T = ssh_u> class Bar
	{
	public:
		// Конструкторы
		Bar() : x(0), y(0), w(0), h(0) {}
		Bar(const T* ptr) { x = ptr[0]; y = ptr[1]; w = ptr[2]; h = ptr[3]; }
		Bar(const T& X, const T& Y, const T& W, const T& H) : x(X), y(Y), w(W), h(H) {}
		Bar(const RECT& r) : x(r.left), y(r.top), w(r.right - x), h(r.bottom - y) {}
		Bar(const Bar& r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
		Bar(const Pts<T>& pt, const Range<T>& s) : x(pt.x), y(pt.y), w(s.w), h(s.h) {}
		Bar(const Range<T>& sz) : x(0), y(0), w(sz.w), h(sz.h) {}
		Bar(const Pts<T>& pt) : x(pt.x), y(pt.y), w(0), h(0) {}
		// Операции
		auto operator - (const Bar& r) const { return Bar(x - r.x, y - r.y, w - r.w, h - r.h); }
		auto operator - (const T& i) const { return Bar(x - i, y - i, w - i, h - i); }
		auto operator - (const Pts<T>& p) const { return Bar(x - p.x, y - p.y, w, h); }
		auto operator - (const Range<T>& s) const { return Bar(x, y, w - s.w, h - s.h); }
		auto operator + (const Bar& r) const { return Bar(x + r.x, y + r.y, w + r.w, h + r.h); }
		auto operator + (const T& i) const { return Bar(x + i, y + i, w + i, h + i); }
		auto operator + (const Pts<T>& p) const { return Bar(x + p.x, y + p.y, w, h); }
		auto operator + (const Range<T>& s) const { return Bar(x, y, w + s.w, h + s.h); }
		auto operator / (const Bar& r) const { return Bar(x / r.x, y / r.y, w / r.w, h / r.h); }
		auto operator / (const T& i) const { return Bar(x / i, y / i, w / i, h / i); }
		auto operator / (const Pts<T>& p) const { return Bar(x / p.x, y / p.y, w, h); }
		auto operator / (const Range<T>& s) const { return Bar(x, y, w / s.w, h / s.h); }
		auto operator * (const Bar& r) const { return Bar(x * r.x, y * r.y, w * r.w, h * r.h); }
		auto operator * (const T& i) const { return Bar(x * i, y * i, w * i, h * i); }
		auto operator * (const Pts<T>& p) const { return Bar(x * p.x, y * p.y, w, h); }
		auto operator * (const Range<T>& s) const { return Bar(x, y, w * s.w, h * s.h); }
		auto operator -= (const Bar& r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; return *this; }
		auto operator -= (const Pts<T>& p) { x -= p.x; y -= p.y; return *this; }
		auto operator -= (const T& i) { x -= i; y -= i; w -= i; h -= i; return *this; }
		auto operator -= (const Range<T>& s) { w -= s.w; h -= s.h; return *this; }
		auto operator += (const Bar& r) { x += r.x; y += r.y; w += r.w; h += r.h; return *this; }
		auto operator += (const Pts<T>& p) { x += p.x; y += p.y; return *this; }
		auto operator += (const T& i) { x += i; y += i; w += i; h += i; return *this; }
		auto operator += (const Range<T>& s) { w += s.w, h += s.h; return *this; }
		auto operator /= (const Bar& r) { x /= r.x; y /= r.y; w /= r.w; h /= r.h; return *this; }
		auto operator /= (const Pts<T>& p) { x /= p.x; y /= p.y; return *this; }
		auto operator /= (const T& i) { x /= i; y /= i; w /= i; h /= i; return *this; }
		auto operator /= (const Range<T>& s) { w /= s.w; h /= s.h; return *this; }
		auto operator *= (const Bar& r) { x *= r.x; y *= r.y; w *= r.w; h *= r.h; return *this; }
		auto operator *= (const Pts<T>& p) { x *= p.x; y *= p.y; return *this; }
		auto operator *= (const T& i) { x *= i; y *= i; w *= i; h *= i; return *this; }
		auto operator *= (const Range<T>& s) { w *= s.w; h *= s.h; return *this; }
		// внешние
		friend Bar operator - (const Pts<T>& p, const Bar& r) { return Bar(p.x - r.x, p.y - r.y, r.w, r.h); }
		friend Bar operator - (const T& i, const Bar& r) { return Bar(i - r.x, i - r.y, i - r.w, i - r.h); }
		friend Bar operator - (const Range<T>& s, const Bar& r) { return Bar(r.x, r.y, s.w - r.w, s.h - r.h); }
		friend Bar operator + (const Pts<T>& p, const Bar& r) { return Bar(p.x + r.x, p.y + r.y, r.w, r.h); }
		friend Bar operator + (const T& i, const Bar& r) { return Bar(i + r.x, i + r.y, i + r.w, i + r.h); }
		friend Bar operator + (const Range<T>& s, const Bar& r) { return Bar(r.x, r.y, s.w + r.w, s.h + r.h); }
		friend Bar operator / (const Pts<T>& p, const Bar& r) { return Bar(p.x / r.x, p.y / r.y, r.w, r.h); }
		friend Bar operator / (const T& i, const Bar& r) { return Bar(i / r.x, i / r.y, i / r.w, i / r.h); }
		friend Bar operator / (const Range<T>& s, const Bar& r) { return Bar(r.x, r.y, s.w / r.w, s.h / r.h); }
		friend Bar operator * (const Pts<T>& p, const Bar& r) { return Bar(p.x * r.x, p.y * r.y, r.w, r.h); }
		friend Bar operator * (const T& i, const Bar& r) { return Bar(i * r.x, i * r.y, i * r.w, i * r.h); }
		friend Bar operator * (const Range<T>& s, const Bar& r) { return Bar(r.x, r.y, s.w * r.w, s.h * r.h); }
		// Сравнение
		bool operator == (const Pts<T>& p) const { return (x == p.x && y == p.y); }
		bool operator == (const Bar& r) const { return (x == r.x && y == r.y && w == r.w && h == r.h); }
		bool operator == (const Range<T>& s) const { return (w == s.w && h == s.h); }
		bool operator != (const Pts<T>& p) const { return (x != p.x || y != p.y); }
		bool operator != (const Bar& r) const { return (x != r.x || y != r.y || w != r.w || h != r.h); }
		bool operator != (const Range<T>& s) const { return (w != s.w || h != s.h); }
		// Присваивание
		auto operator = (const Bar& r) { x = r.x; y = r.y; w = r.w; h = r.h; return *this; }
		auto operator = (const RECT& r) { x = r.left; y = r.top; w = r.right - x; h = r.bottom - y; return *this; }
		auto operator = (const Range<T>& s) { w = s.w; h = s.h; return *this; }
		auto operator = (const Pts<T>& p) { x = p.x; y = p.y; return *this; }
		// Приведение типов
		operator T*() const { return (T*)&x; }
		operator RECT() const { return RECT{(int)x, (int)y, (int)(y + h), (int)(x + w)}; }
		operator RECT*() const { static RECT r; r.left = x; r.top = y; r.bottom = y + h; r.right = x + w; return &r; }
		operator Range<T>() const { return rg; }
		operator Pts<T>() const { return pt; }
		// Специальные
		auto set(const T& X, const T& Y, const T& W, const T& H) { x = X; y = Y; w = W; h = H; return *this; }
		auto set(const Pts<T>& pt, const Range<T>& sz) { x = pt.x; y = pt.y; w = sz.w; h = sz.h; return *this; }
		auto null() { x = y = w = h = 0; return *this; }
		auto empty() { w = h = 0; return *this; }
		auto inflate(const T& X, const T& Y, const T& W, const T& H) { x += X; y += Y; w += W; h += H; return *this; }
		bool is_null() const { return (x == 0 && y == 0 && (w <= 0 || h <= 0)); }
		bool is_empty() const { return (w <= 0 || h <= 0); }
		bool ptInRc(const Pts<T>& pt) const { return ((pt.x >= x && pt.x < (x + w)) && (pt.y >= y && pt.y < (y + h))); }
		bool rcInRc(const Bar& rc, RECT* dst) const { return (::IntersectRect(dst, *this, r2) != 0); }
		T right() const { return (x + w); }
		T bottom() const { return (y + h); }
		Pts<T> center() const { return Pts<T>(x + (w / 2), y + (h / 2)); }
#ifdef _DEBUG
		// тест
		static void unit_test()
		{
			Bar test1;
			Bar test2;
			Bar test3;
			Bar test4;
			Bar test5;
		}
#endif
		union
		{
			struct
			{
				Pts<T> pt;
				Range<T> rg;
			};
			struct
			{
				T x;
				T y;
				T w;
				T h;
			};
		};
	};

	template <typename T1 = long, typename T2 = float> class Box
	{
	public:
		// конструкторы
		Box() : x(0), y(0), w(0), h(0), n(0), f(0) {}
		Box(const Bar<T1>& r, const Range<T2>& s) : x(r.x), y(r.y), w(r.w), h(r.h), n(s.w), f(s.h) {}
		Box(const T1& X, const T1& Y, const T1& W, const T1& H, const T2& N, const T2& F) : x(X), y(Y), w(W), h(H), n(N), f(F) {}
		Box(const Box& b) : x(b.x), y(b.y), w(b.w), h(b.h), n(b.n), f(b.f) {}
		// операторы
		// логические
		bool operator == (const Box& b) const { return (x == b.x && y == b.y && w == b.w && h == b.h && n == b.n && f == b.f); }
		bool operator != (const Box& b) const { return (x != b.x || y != b.y || w != b.w || h != b.h || n != b.n || f != b.f); }
		// присваивания
		auto operator = (const Box& b) { ssh_memcpy(*this, &b, sizeof(Box)); return *this; }
		auto operator = (const Bar<T1>& r) { x = r.x; y = r.y; w = r.w; h = r.h; return *this; }
		auto operator = (const Range<T2>& s) { n = s.w; f = s.h; return *this; }
		// арифметические
		auto operator + (const Bar<T1>& r) const { return Box(x + r.x, y + r.y, w + r.w, h + r.h, n, f); }
		auto operator + (const Range<T2>& s) const { return Box(x, y, w, h, n + s.w, f + s.h); }
		auto operator - (const Bar<T1>& r) const { return Box(x - r.x, y - r.y, w - r.w, h - r.h, n, f); }
		auto operator - (const Range<T2>& s) const { return Box(x, y, w, h, n - s.w, f - s.h); }
		auto operator += (const Bar<T1>& r) { x += r.x; y += r.y; w += r.w; h += r.h; return this; }
		auto operator += (const Range<T2>& s) { n += s.w; f += s.h; return this; }
		auto operator -= (const Bar<T1>& r) { x -= r.x; y -= r.y; w -= r.w; h -= r.h; return this; }
		auto operator -= (const Range<T2>& s) { n -= s.w; f -= s.h; return this; }
		// приведение типа
		operator Bar<T1>() const { return Bar<T1>(x, y, w, h); }
		operator Range<T2>() const { return Range<T2>(n, f); }
		// специальные
		bool is_null() const { return (x == 0 && y == 0 && w == 0 && h == 0 && n == 0 && f == 0); }
		bool is_empty() const { return (w <= 0 && h <= 0 && n <= 0 && f <= 0); }
#ifdef _DEBUG
		// тест
		static void unit_test()
		{
			Box test1;
			Box test2;
			Box test3;
			Box test4;
			Box test5;
		}
#endif
		union
		{
			struct
			{
				Pts<T1> pt;
				Range<T1> rg;
				Vec2 depth;
			};
			struct
			{
				T1 x, y, w, h;
				T2 n, f;
			};
		};
	};
}
