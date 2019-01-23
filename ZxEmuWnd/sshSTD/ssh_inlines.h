#pragma once

extern "C"
{
	ssh_u	asm_ssh_capability();
	ssh_cws asm_ssh_to_base64(ssh_b* ptr, ssh_u count);
	ssh_b*	asm_ssh_from_base64(ssh_ws* str, ssh_u count, ssh_u* len_buf);
	void*	asm_ssh_wton(ssh_cws str, ssh_u radix, ssh_ws* end = nullptr);
	ssh_cws asm_ssh_ntow(const void* ptr, ssh_u radix, ssh_ws* end = nullptr);
	ssh_ws* asm_ssh_parse_spec(void* arg, ssh_cws* str);
	ssh_l	asm_ssh_parse_xml(ssh_ws* src, ssh_w* vec);
}

namespace ssh
{
	using	__regx_compile		= regex16* (CALLBACK*)(ssh_cws pattern, ssh_l options);
	using	__regx_exec			= ssh_l(CALLBACK*)(const void* re, ssh_cws subj, ssh_l len_subj, ssh_l idx, ssh_l options, ssh_l* vec, ssh_l count_vec);
	using	__regx_free			= ssh_l(CALLBACK*)(void* p);

	using	__xin_xenable		= void (CALLBACK*)(int is);
	using	__xin_xgstate		= ssh_d(CALLBACK*)(ssh_d idx, XINPUT_STATE* state);
	using	__xin_xsstate		= ssh_d(CALLBACK*)(ssh_d idx, XINPUT_VIBRATION* state);
	using	__xin_xcaps			= ssh_d(CALLBACK*)(ssh_d idx, ssh_d flags, XINPUT_CAPABILITIES* caps);

	// процедуры из библиотеки sshREGX
	extern	__regx_compile		ssh_regx_compile;
	extern	__regx_exec			ssh_regx_exec;
	extern	__regx_free			ssh_regx_free;

	// процедуры из библиотеки x_input1_3
	extern __xin_xenable		ssh_xin_enable;
	extern __xin_xgstate		ssh_xin_gstate;
	extern __xin_xsstate		ssh_xin_sstate;
	extern __xin_xcaps			ssh_xin_caps;

	// определени€ дл€ процессорно - зависимых функций
	using	__asm_ssh_wcsstr	= ssh_ws*(*)(ssh_cws _1, ssh_cws _2);
	using	__asm_ssh_wcscmp	= int(*)(ssh_cws _1, ssh_cws _2);
	using	__ssh_rand			= ssh_u(*)(ssh_u begin, ssh_u end);
	using	__ssh_hash			= int(*)(ssh_cws _cws);

	// указатели на процессорно - зависимые функции
	extern __ssh_rand			SSH ssh_rand;
	extern __asm_ssh_wcsstr		SSH ssh_wcsstr;
	extern __asm_ssh_wcscmp		SSH ssh_wcscmp;
	extern	__ssh_hash			SSH ssh_hash;

	enum class Radix { _dec, _bin, _oct, _hex, _dbl, _flt, _bool };

	// возможности процессора
	enum class CpuCaps : ssh_u
	{
		NONE, MMX, SSE, SSE2, SSE3, SSSE3, SSE4_1, SSE4_2, PCLMULQDQ, FMA, CMPXCHG16B, MOVBE, POPCNT, AES, AVX, RDRAND, CMOV, BMI1, AVX2, BMI2, AVX512F, RDSEED, AVX512PF, AVX512ER, AVX512CD, HALF
	};

	static lconv* ssh_lc(nullptr);
	static CpuCaps cpuCaps((CpuCaps)asm_ssh_capability());

	// глобальные встроенные функции
	inline bool ssh_is_null(ssh_cws str) { return (!str || !str[0]); }
	inline bool ssh_is_digit(ssh_cws str) { return (*str >= L'0' && *str <= L'9'); }
	inline ssh_b ssh_cpu_caps(CpuCaps caps) { return _bittest64((const ssh_l*)&cpuCaps, (ssh_u)caps); }
	inline auto ssh_set_locale(ssh_ccs loc = "") { return setlocale(LC_ALL, loc); }
	inline auto ssh_locale() { if(!ssh_lc) ssh_lc = localeconv(); return ssh_lc; }
	
	inline ssh_u ssh_sse_wcslen(ssh_cws _wcs)
	{
		int res(0), ret(0);
		if(_wcs)
		{
			do
			{
				__m128i _1(_mm_lddqu_si128((__m128i*)(_wcs)));
				res = _mm_cmpistri(_1, _1, 0b00010001);
				ret += res;
				_wcs += res;
			} while(res == 8);
		}
		return ret;
	}

	inline ssh_u ssh_wcslen(ssh_cws _wcs)
	{
		if(_wcs)
		{
			if(!ssh_cpu_caps(CpuCaps::SSE4_2)) return wcslen(_wcs);
			return ssh_sse_wcslen(_wcs);
		}
		return 0;
	}
	
	inline ssh_ws* ssh_wcschr(ssh_cws _wcs, ssh_ws _ws)
	{
		int res, is(true);
		if(_wcs)
		{
			if(!ssh_cpu_caps(CpuCaps::SSE4_2)) return (ssh_ws*)wcschr(_wcs, _ws);
			__m128i _1(_mm_cvtsi32_si128((int)_ws)), _2;
			do
			{
				_2 = _mm_lddqu_si128((__m128i*)_wcs);
				res = _mm_cmpistri(_1, _2, 0b00000001);
				is = _mm_cmpistrz(_1, _2, 0b00000001);
				_wcs += res;
			} while(res == 8 && !is);
		}
		return (ssh_ws*)(is ? nullptr : _wcs);
	}

	inline void* ssh_memset(void* ptr, int set, ssh_u count)
	{
		if(ptr) ptr = (ssh_b*)(memset((void*)ptr, set, count)) + count;
		return ptr;
	}

	inline void* ssh_memzero(void* ptr, ssh_u count)
	{
		return ssh_memset(ptr, 0, count);
	}

	inline void* ssh_memcpy(void* dst, const void* src, ssh_u count)
	{
		if(dst && src) dst = (ssh_b*)(memcpy((void*)dst, src, count)) + count;
		return dst;
	}
	// реверс бит
	inline int ssh_rev_bits(int x)
	{
		x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
		x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
		x = ((x & 0x0f0f0f0f) << 4) | ((x >> 4) & 0x0f0f0f0f);
		return (_rotr((x & 0x00ff00ff), 8) | (_rotl(x, 8) & 0x00ff00ff));
	}
	// остаток от делени€ дл€ вещественных чисел
	inline float ssh_modf(float x, float y)
	{
		return (x - y *(int)(x / y));
	}
	// преобразовать значение в ближайшую степень двойки
	inline ssh_u ssh_pow2(ssh_u val, bool nearest)
	{
		ssh_d idx;
		_BitScanReverse64(&idx, val);
		ssh_u _val((1ULL << idx));
		return ((_val == val || nearest) ? _val : _val << 1ULL);
	}
	
	inline long ssh_loop(long x, long y)
	{
		long z(y / (x * 2));
		long y1 = z * x;
		y -= ((y - y1) >= x) ? y1 * 2 : 0;
		return x - (y < x ? x - y : y - x);
	}
	// логарифм по основанию 2
	inline int ssh_log2(int i)
	{
		int value = 0;
		while(i >>= 1) { value++; }
		return value;
	}
	// линейна€ интерпол€ци€
	inline float ssh_lerp(float f0, float f1, float t)
	{
		float s = 1.0f - t;
		return f0 * s + f1 * t;
	}
	// наибольшее целое
	inline float ssh_floor(float f)
	{
		return _mm_floor_ss(_mm_set_ss(f), _mm_set_ss(f)).m128_f32[0];
	}
	// наименьшее целое
	inline float ssh_ceil(float f)
	{
		return _mm_ceil_ss(_mm_set_ss(f), _mm_set_ss(f)).m128_f32[0];
	}
	// отбросить дробную часть
	inline int ssh_trunc(float f)
	{
		return (int)_mm_round_ss(_mm_set_ss(f), _mm_set_ss(f), _MM_FROUND_TO_ZERO).m128_f32[0];
	}
	// вернуть дробну часть
	inline float ssh_frac(float f)
	{
		return f - ssh_trunc(f);
	}
	// остаток от целочисленного делени€
	inline ssh_i ssh_mod(ssh_i y, ssh_i z)
	{
		ssh_i x = 0;
		for(int i = 0; i < 32; i++)
		{
			x = (x << 1) | (y >> 31);
			y = y << 1;
			if((x) >= z)
			{
				x = x - z;
				y = y + 1;
			}
		}
		return x;
	}

	// провер€ет на кратность значени€ степени двойки
	template <typename T> bool ssh_is_pow2(const T& value)
	{
		return (value == ssh_pow2<T>(value, true));

	}
	// обмен€ть значени€
	template <typename T> void ssh_swap(T& val1, T& val2)
	{
		T tmp(std::move(val1));
		val1 = std::move(val2);
		val2 = std::move(tmp);
	}
	// минимум
	template <typename T> T ssh_min(const T& val1, const T& val2)
	{
		return (val1 < val2 ? val1 : val2);
	}
	// максимум
	template <typename T> T ssh_max(const T& val1, const T& val2)
	{
		return (val1 > val2 ? val1 : val2);
	}
	// итератор
	template <typename T> class Iter
	{
	public:
		// конструктор по значению
		Iter(T* n) : val(n) {}
		// оператор сравнени€
		bool operator != (const Iter& it) const { return (val != it.val); }
		// оператор приращени€
		Iter operator++() const { val = val->next; return *this; }
		// оператор извлечени€
		auto operator*() const { return val->value; }
	protected:
		// значение
		mutable T* val;
	};
	// менеджер пам€ти дл€ однотипных элементов
	template <typename T, ssh_u N = 128> class MemArray
	{
	public:
		struct Block
		{
			union
			{
				Block* next;
				ssh_b t[sizeof(T)];
			};
		};

		struct BlockFix
		{
			BlockFix() : next(nullptr) {}
			~BlockFix() { SSH_DEL(next); }
			BlockFix* next;
			Block arr[N];
		};
		void Reset() { if(!count) { SSH_DEL(arrs); free = nullptr; } }
		T* Alloc()
		{
			if(!free)
			{
				BlockFix* tmp(new BlockFix);
				ssh_memzero(tmp->arr, sizeof(Block) * N);
				tmp->next = arrs; arrs = tmp;
				for(ssh_u i = 0; i < N; i++)
				{
					arrs->arr[i].next = free;
					free = &(arrs->arr[i]);
				}
			}
			Block* b(free);
			free = free->next;
			count++;
			return (T*)(b->t);
		}
		void Free(T* t)
		{
			Block* b((Block*)t);
			b->next = free;
			free = b;
			count--;
		}
		Block* free = nullptr;
		BlockFix* arrs = nullptr;
		int count = 0;
	};

	template < typename T> class Allocator
	{
	public:
	};

	template <typename T, bool> struct release_node { static void release(const T& t) { static_assert(false, "release_node invalid!"); } };
	template <typename T> struct release_node < T, false > { static void release(const T& t) { t.~T(); } static T dummy() { return T(); } };
	template <typename T> struct release_node < T, true > { static void release(const T& t) { delete t; } static T dummy() { return nullptr; } };

	#define SSH_RELEASE_NODE(T, V)	release_node<T, SSH_IS_PTR(T)>::release(V)
	#define SSH_DUMMY(T)			release_node<T, SSH_IS_PTR(T)>::dummy()

	template <typename T> using ArraySTL= std::vector<T>;
	template <typename T> using MapSTL	= std::map<T, Allocator<T>>;
	template <typename T> using ListSTL	= std::list<T, Allocator<T>>;
	template <typename T> using TreeSTL	= std::multiset<T, Allocator<T>>;
}
