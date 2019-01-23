
#pragma once

#include "ssh_array.h"

namespace ssh
{
	#define SSH_ENUM( enumName, ... ) SSH_ENUM_DETAIL_MAKE( class, enumName, __VA_ARGS__ )
	#define SSH_ENUM_NS( enumName, ... ) SSH_ENUM_DETAIL_MAKE( namespace, enumName, __VA_ARGS__ )
	#define SSH_ENUM_DETAIL_SPEC_namespace extern "C"{} inline
	#define SSH_ENUM_DETAIL_SPEC_class friend
	#define SSH_ENUM_DETAIL_STR(x) L#x
	#define SSH_ENUM_DETAIL_MAKE(spec, enumName, ...) enum enumName:ssh_u { __VA_ARGS__ };		\
    SSH_ENUM_DETAIL_SPEC_##spec const ::EnumReflector& _detail_reflector_(enumName)				\
    {																							\
        static const ::EnumReflector _reflector( []{											\
            static ssh_u _detail_sval;															\
            _detail_sval = 0;																	\
            struct _detail_val_t																\
            {																					\
                _detail_val_t(const _detail_val_t& rhs) : _val(rhs) { _detail_sval = _val + 1; }\
                _detail_val_t(int val)                  : _val(val) { _detail_sval = _val + 1; }\
                _detail_val_t()                 : _val(_detail_sval){ _detail_sval = _val + 1; }\
																								\
                _detail_val_t& operator=(const _detail_val_t&) { return *this; }				\
                _detail_val_t& operator=(ssh_u) { return *this; }								\
                operator ssh_u() const { return _val; }											\
                ssh_u _val;																		\
            } __VA_ARGS__;																		\
            const ssh_u _detail_vals[] = { __VA_ARGS__ };										\
            return ::EnumReflector( _detail_vals, sizeof(_detail_vals)/sizeof(ssh_u), L#enumName, \
								SSH_ENUM_DETAIL_STR((__VA_ARGS__))  );							\
        }() );																					\
        return _reflector;																		\
    }


	class EnumReflector
	{
	public:
		template<typename EnumType> static const EnumReflector& get(EnumType val = EnumType()) { return _detail_reflector_(val); }
		struct Enumerator
		{
			String name;
			ssh_u value;
		};
		// конструктор
		EnumReflector(const ssh_u*, ssh_u, ssh_cws, ssh_cws);
		// количество элементов
		ssh_u count() const { return values.size(); }
		// найти по имени
		ssh_u find(const String& name) const
		{
			for(ssh_u i = 0; i < count(); ++i)
				if(values[i].name == name) return i;
			return -1;
		}
		// найти по значению
		ssh_u find(ssh_u value) const
		{
			for(ssh_u i = 0; i < count(); ++i)
				if(values[i].value == value) return i;
			return -1;
		}
		// вернуть по индексу
		Enumerator at(ssh_u index) const { return values[index]; }
		Enumerator operator[](ssh_u index) const { return at(index); }
	private:
		// массив перечислений
		Array<Enumerator> values;
		// имя перечисления
		String enumName;
	};

	static bool IsIdentChar(ssh_ws c)
	{
		return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || (c >= L'0' && c <= L'9') || (c == L'_');
	}

	inline EnumReflector::EnumReflector(const ssh_u* vals, ssh_u count, ssh_cws name, ssh_cws body)
	{
		enumName = name;
		values.resize(count);
		enum states { state_start, state_ident, state_skip, } state = state_start;
		++body;
		ssh_cws ident_start(nullptr);
		int value_index = 0, level = 0;
		for(;;)
		{
			switch(state)
			{
				case state_start:
					if(IsIdentChar(*body)) { state = state_ident; ident_start = body; }
					++body;
					break;
				case state_ident:
					if(!IsIdentChar(*body))
					{
						state = state_skip;
						values[value_index].name = String(ident_start, body - ident_start);
						values[value_index].value = vals[value_index];
						++value_index;
					}
					else ++body;
					break;
				case state_skip:
					if(*body == L'(') level++;
					else if(*body == L')')
					{
						if(level == 0) return;
						--level;
					}
					else if(level == 0 && *body == L',') state = state_start;
					++body;
			}
		}
	}
}