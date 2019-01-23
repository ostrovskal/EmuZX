
#pragma once

#include <signal.h>

namespace ssh
{
	class SSH MemMgr final
	{
	public:
		enum class ExceptTypes : int
		{
			SIGNAL_INTERRUPT		= SIGINT,
			SIGNAL_INSTRUCTION		= SIGILL,
			SIGNAL_FLOATING			= SIGFPE,
			SIGNAL_FAULT			= SIGSEGV,
			SIGNAL_TERMINATE		= SIGTERM,
			SIGNAL_ABORT			= SIGABRT,
			UNHANDLED_EXCEPTION		= 0x8000,
			TERMINATE_CALL			= 0x4000,
			UNEXPECTED_CALL			= 0x2000,
			PURE_CALL				= 0x1000,
			SECURITY_ERROR			= 0x0800,
			NEW_OPERATOR_ERROR		= 0x0400,
			INVALID_PARAMETER_ERROR = 0x0200
		};
		struct NodeMem
		{
			// признак использованного блока
			int	use;
			// размер блока
			int sz;
#ifdef _DEBUG
			// конструктор
			NodeMem(int _sz, NodeMem* nn) : ptr((ssh_b*)this + sizeof(NodeMem)), next(nn), prev(nullptr), sz(_sz), use(1) {}
			// указатель на данные (только для отладки
			ssh_b* ptr;
#else
			// конструктор
			NodeMem(int _sz, NodeMem* nn) : next(nn), prev(nullptr), sz(_sz), use(1) {}
#endif // DEBUG
			// следующий
			NodeMem* next;
			// предыдущий
			NodeMem* prev;
		};
		static MemMgr* instance() { static MemMgr mem; return &mem; }
		// обработка внешнего исключения
		bool fault(ExceptTypes type, ssh_cws fn, ssh_cws fl, int ln, EXCEPTION_POINTERS* except = nullptr, ssh_cws msg_ex = nullptr);
		// вернуть информацию об сеансе работы менеджера
		void output();
		// выделение памяти
		void* alloc(ssh_u sz);
		// освобождение
		void free(ssh_b* p);
		// вернуть признак блокировки
		bool is_started() const { return is_enabled; }
		// установить признак блокировки
		void start() { is_enabled = true; }
		void stop() { is_enabled = false; }
		// обработчики нестандартных исключительных ситуаций
		void set_exceptionHandlers();
	protected:
		// вернуть не освобожденные блоки
		void leaks();
		// количество блоков
		ssh_u count_alloc = 0, max_alloc = 0;
		// всего выделено
		ssh_u total_alloc = 0;
		// всего было освобождено
		ssh_u total_free = 0;
		// максимум единовременно выделенной памяти
		ssh_u use_mem = 0, use_max_mem = 0;
		// признак блокировки
		bool is_enabled = false;
		// корневой блок
		NodeMem* root = nullptr;
	};
}


inline void* operator new(ssh_u sz){return ssh::MemMgr::instance()->alloc(sz);}
inline void operator delete(void* p) { ssh::MemMgr::instance()->free((ssh_b*)p); }
inline void* operator new[](ssh_u sz) { return ssh::MemMgr::instance()->alloc(sz); }
inline void operator delete[](void* p) { ssh::MemMgr::instance()->free((ssh_b*)p); }

