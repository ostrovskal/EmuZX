
#pragma once

#include "ssh_xml.h"
#include "ssh_file.h"

// опции
#define SC_BIN			0x0001// данные в bin
#define SC_OCT			0x0002// данные в oct
#define SC_HEX			0x0003// данные в hex
#define SC_FLGS			0x0004// флаги
// управляющие
#define SC_NODE			0x0008// узел
#define SC_PTR			0x0010// указатель
#define SC_FLT			0x0020// float, double
#define SC_BOOL			0x0040// bool
#define SC_OBJ			0x0080// со схемой
#define SC_STR			0x0100// строка
#define SC_LIT			0x0200// литеральный
#define SC_CV			0x0400// константный
#define SC_BASE64		0x0800// данные кодируются в base64

#define SCHEME_SPEC(cls, var, flg)	(std::is_pointer<SSH_PURE_TYPE1_D(cls::var)>() * SC_PTR) | (std::is_floating_point<SSH_PURE_TYPE2_D(cls::var)>() * SC_FLT) | \
									(SSH_IS_STRING_D(cls::var) * SC_STR) | (SSH_IS_BOOL_D(cls::var) * SC_BOOL) | (SSH_IS_LITERAL_D(cls::var) * SC_LIT) |  \
									(std::is_const<SSH_PURE_TYPE2_D(cls::var)>() * SC_CV) | (std::is_base_of<Serialize, SSH_PURE_TYPE2_D(cls::var)>() * SC_OBJ) | flg, \
									ssh_count_array<decltype(cls::var)>(), sizeof(decltype(cls::var))

#define SCHEME_BEGIN(cls)			static SCHEME cls##_scheme[] = { {L#cls, nullptr, nullptr, 0, 0, 0, 0 },
#define SCHEME_END(cls)				{nullptr, nullptr, nullptr, 0, 0, 0, 0}}; return cls##_scheme;

#define SCHEME_VAR(cls, var, def, flgs, stk) {L#var, def, stk, offsetof(cls, var), SCHEME_SPEC(cls, var, flgs) },

#define SCHEME_NODE_BEGIN(cls, var) {L#var, nullptr, nullptr, offsetof(cls, var), SCHEME_SPEC(cls, var, SC_NODE) },
#define SCHEME_NODE_END()			{nullptr, nullptr, nullptr, 0, 0, 0, 0},

#include "ssh_enums.h"

namespace ssh
{
	template<class _Ty> struct ssh_count_array : std::integral_constant<int, 1> {};
	template<class _Ty, size_t _Nx> struct ssh_count_array<_Ty[_Nx]> : std::integral_constant<int, _Nx> {};

	class SSH Serialize
	{
	public:
		struct SCHEME
		{
			// имя (для простых - атрибут, для вложенных - узел)
			ssh_cws name;
			// значение по умолчанию
			ssh_cws def;
			// структуры для преобразования флагов и перечислений
			const EnumReflector* stk;
			// смещение переменной относительно родителя
			ssh_u offs;
			// опции типа
			ssh_w opt;
			// количество элементов в переменной
			ssh_w count;
			// размер типа в байтах
			ssh_w width;
		};

		Serialize() {}
		virtual ~Serialize() {}
		virtual SCHEME* get_scheme() const = 0;
		virtual void open(const Buffer& buf, bool is_xml);
		virtual void save(const String& path, bool is_xml);
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	protected:
		virtual void readXml(HXML hp, Xml* xml, ssh_l p_offs, ssh_l idx = 0);
		virtual void writeXml(HXML h, Xml* xml, ssh_l p_offs);
		virtual void writeBin(File* fl, ssh_l p_offs);
		virtual void readBin(ssh_b* buf, ssh_l p_offs);
	};
}