
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
			// ������� ��������������� �����
			int	use;
			// ������ �����
			int sz;
#ifdef _DEBUG
			// �����������
			NodeMem(int _sz, NodeMem* nn) : ptr((ssh_b*)this + sizeof(NodeMem)), next(nn), prev(nullptr), sz(_sz), use(1) {}
			// ��������� �� ������ (������ ��� �������
			ssh_b* ptr;
#else
			// �����������
			NodeMem(int _sz, NodeMem* nn) : next(nn), prev(nullptr), sz(_sz), use(1) {}
#endif // DEBUG
			// ���������
			NodeMem* next;
			// ����������
			NodeMem* prev;
		};
		static MemMgr* instance() { static MemMgr mem; return &mem; }
		// ��������� �������� ����������
		bool fault(ExceptTypes type, ssh_cws fn, ssh_cws fl, int ln, EXCEPTION_POINTERS* except = nullptr, ssh_cws msg_ex = nullptr);
		// ������� ���������� �� ������ ������ ���������
		void output();
		// ��������� ������
		void* alloc(ssh_u sz);
		// ������������
		void free(ssh_b* p);
		// ������� ������� ����������
		bool is_started() const { return is_enabled; }
		// ���������� ������� ����������
		void start() { is_enabled = true; }
		void stop() { is_enabled = false; }
		// ����������� ������������� �������������� ��������
		void set_exceptionHandlers();
	protected:
		// ������� �� ������������� �����
		void leaks();
		// ���������� ������
		ssh_u count_alloc = 0, max_alloc = 0;
		// ����� ��������
		ssh_u total_alloc = 0;
		// ����� ���� �����������
		ssh_u total_free = 0;
		// �������� ������������� ���������� ������
		ssh_u use_mem = 0, use_max_mem = 0;
		// ������� ����������
		bool is_enabled = false;
		// �������� ����
		NodeMem* root = nullptr;
	};
}


inline void* operator new(ssh_u sz){return ssh::MemMgr::instance()->alloc(sz);}
inline void operator delete(void* p) { ssh::MemMgr::instance()->free((ssh_b*)p); }
inline void* operator new[](ssh_u sz) { return ssh::MemMgr::instance()->alloc(sz); }
inline void operator delete[](void* p) { ssh::MemMgr::instance()->free((ssh_b*)p); }

