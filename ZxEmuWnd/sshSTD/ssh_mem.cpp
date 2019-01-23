
#include "stdafx.h"
#include "ssh_mem.h"

namespace ssh
{
	static void __cdecl ssh_signal_handler(int numSignal)
	{
		SSH_FAULT((MemMgr::ExceptTypes)numSignal, (EXCEPTION_POINTERS*)_pxcptinfoptrs);
		exit(1);
	}

	static void __cdecl ssh_terminate_handler()
	{
		SSH_FAULT(MemMgr::ExceptTypes::TERMINATE_CALL, nullptr);
		exit(2);
	}

	static void __cdecl ssh_unexp_handler()
	{
		SSH_FAULT(MemMgr::ExceptTypes::UNEXPECTED_CALL, nullptr);
		exit(3);
	}

	static void __cdecl ssh_purecall_handler()
	{
		SSH_FAULT(MemMgr::ExceptTypes::PURE_CALL, nullptr);
		exit(4);
	}

	static void __cdecl ssh_security_handler(int code, void *x)
	{
		SSH_FAULT(MemMgr::ExceptTypes::SECURITY_ERROR, nullptr);
		exit(5);
	}

	static void __cdecl ssh_invalid_parameter_handler(ssh_cws expression, ssh_cws function, ssh_cws file, ssh_i line, uintptr_t pReserved)
	{
		ssh_mem->fault(MemMgr::ExceptTypes::INVALID_PARAMETER_ERROR, function, file, line, nullptr, expression);
		exit(6);
	}

	static int __cdecl ssh_new_handler(ssh_u size)
	{
		SSH_FAULT(MemMgr::ExceptTypes::NEW_OPERATOR_ERROR, nullptr);
		exit(7);
	}

	static LONG WINAPI Win32UnhandledExceptionFilter(EXCEPTION_POINTERS* except)
	{
		SSH_FAULT(MemMgr::ExceptTypes::UNHANDLED_EXCEPTION, except);
		exit(8);
	}

	void MemMgr::set_exceptionHandlers()
	{
		// установить фильтр исключений
		SetUnhandledExceptionFilter(Win32UnhandledExceptionFilter);
		// установить режимы отчета библиотеки времени выполнения
		//_CrtSetReportMode(_CRT_ERROR, 0);
		_CrtSetReportMode(_CRT_ASSERT, 0);

		set_terminate(ssh_terminate_handler);
		set_unexpected(ssh_unexp_handler);
		_set_purecall_handler(ssh_purecall_handler);
		_set_invalid_parameter_handler(ssh_invalid_parameter_handler);
		_set_new_handler(ssh_new_handler);
		//_set_security_error_handler(ssh_security_handler);
		signal(SIGABRT, ssh_signal_handler);
		signal(SIGINT, ssh_signal_handler);
		signal(SIGTERM, ssh_signal_handler);
		signal(SIGFPE, ssh_signal_handler);
		signal(SIGILL, ssh_signal_handler);
		signal(SIGSEGV, ssh_signal_handler);
	}

	bool MemMgr::fault(ExceptTypes type, ssh_cws fn, ssh_cws fl, int ln, EXCEPTION_POINTERS* exc, ssh_cws msg_ex)
	{
		CONTEXT ContextRecord;
		EXCEPTION_RECORD ExceptionRecord;
		ssh_memzero(&ContextRecord, sizeof(CONTEXT));
		ssh_memzero(&ExceptionRecord, sizeof(EXCEPTION_RECORD));

		RtlCaptureContext(&ContextRecord);

		ExceptionRecord.ExceptionCode = 0;
		ExceptionRecord.ExceptionAddress = _ReturnAddress();

		exc = new EXCEPTION_POINTERS;
		exc->ContextRecord = new CONTEXT;
		exc->ExceptionRecord = new EXCEPTION_RECORD;

		ssh_memcpy(exc->ContextRecord, &ContextRecord, sizeof(CONTEXT));
		ssh_memcpy(exc->ExceptionRecord, &ExceptionRecord, sizeof(EXCEPTION_RECORD));
	
		String caption;
		switch(type)
		{
			case ExceptTypes::SIGNAL_INTERRUPT: caption = L"Прерывание. "; break;
			case ExceptTypes::SIGNAL_INSTRUCTION: caption = L"Недопустимая инструкция. "; break;
			case ExceptTypes::SIGNAL_FLOATING: caption = L"Недопустимая операция с вещественными числами. "; break;
			case ExceptTypes::SIGNAL_FAULT: caption = L"Недопустимый указатель. "; break;
			case ExceptTypes::SIGNAL_TERMINATE: caption = L"SIGNAL TERMINATE. "; break;
			case ExceptTypes::SIGNAL_ABORT: caption = L"SIGNAL ABORT. "; break;
			case ExceptTypes::UNHANDLED_EXCEPTION: caption = L"Необработанное исключение. "; break;
			case ExceptTypes::TERMINATE_CALL: caption = L"Вызов функции terminate(). "; break;
			case ExceptTypes::UNEXPECTED_CALL: caption = L"Неожиданное исключение. "; break;
			case ExceptTypes::PURE_CALL: caption = L"Вызов чистой виртуальной функции. "; break;
			case ExceptTypes::SECURITY_ERROR: caption = L"Переполнение буфера. "; break;
			case ExceptTypes::NEW_OPERATOR_ERROR: caption = L"Не удалось выделить память оператором new. "; break;
			case ExceptTypes::INVALID_PARAMETER_ERROR: caption = L"Недопустимый параметр CRT функции, при выполнении операции " + String(msg_ex); caption += L". "; break;
		}
		String msg(ssh_printf(	L"\r\nКонтекст на момент возбуждения исключения: \r\n\r\nrip: %016I64x\tflags: %08b\r\n"
								L"rax: %016I64x\trcx: %016I64x\trdx: %016I64x\trbx: %016I64x\trbp: %016I64x\trsp: %016I64x\trsi: %016I64x\trdi: %016I64x\r\n"
								L"r8:  %016I64x\tr9:  %016I64x\tr10: %016I64x\tr11: %016I64x\tr12: %016I64x\tr13: %016I64x\tr14: %016I64x\tr15: %016I64x\r\n"
								L"xmm0:  %016I64x%016I64x\txmm1:  %016I64x%016I64x\txmm2:  %016I64x%016I64x\txmm3:  %016I64x%016I64x\r\n"
								L"xmm4:  %016I64x%016I64x\txmm5:  %016I64x%016I64x\txmm6:  %016I64x%016I64x\txmm7:  %016I64x%016I64x\r\n"
								L"xmm8:  %016I64x%016I64x\txmm9:  %016I64x%016I64x\txmm10: %016I64x%016I64x\txmm11: %016I64x%016I64x\r\n"
								L"xmm12: %016I64x%016I64x\txmm13: %016I64x%016I64x\txmm14: %016I64x%016I64x\txmm15: %016I64x%016I64x\r\n",
				exc->ExceptionRecord->ExceptionAddress, exc->ContextRecord->EFlags,
				exc->ContextRecord->Rax, exc->ContextRecord->Rcx, exc->ContextRecord->Rdx, exc->ContextRecord->Rbx,
				exc->ContextRecord->Rbp, exc->ContextRecord->Rsp, exc->ContextRecord->Rsi, exc->ContextRecord->Rdi,
				exc->ContextRecord->R8, exc->ContextRecord->R9, exc->ContextRecord->R10, exc->ContextRecord->R11,
				exc->ContextRecord->R12, exc->ContextRecord->R13, exc->ContextRecord->R14, exc->ContextRecord->R15,
				exc->ContextRecord->Xmm0.Low, exc->ContextRecord->Xmm0.High,
				exc->ContextRecord->Xmm1.Low, exc->ContextRecord->Xmm1.High,
				exc->ContextRecord->Xmm2.Low, exc->ContextRecord->Xmm2.High,
				exc->ContextRecord->Xmm3.Low, exc->ContextRecord->Xmm3.High,
				exc->ContextRecord->Xmm4.Low, exc->ContextRecord->Xmm4.High,
				exc->ContextRecord->Xmm5.Low, exc->ContextRecord->Xmm5.High,
				exc->ContextRecord->Xmm6.Low, exc->ContextRecord->Xmm6.High,
				exc->ContextRecord->Xmm7.Low, exc->ContextRecord->Xmm7.High,
				exc->ContextRecord->Xmm8.Low, exc->ContextRecord->Xmm8.High,
				exc->ContextRecord->Xmm9.Low, exc->ContextRecord->Xmm9.High,
				exc->ContextRecord->Xmm10.Low, exc->ContextRecord->Xmm10.High,
				exc->ContextRecord->Xmm11.Low, exc->ContextRecord->Xmm11.High,
				exc->ContextRecord->Xmm12.Low, exc->ContextRecord->Xmm12.High,
				exc->ContextRecord->Xmm13.Low, exc->ContextRecord->Xmm13.High,
				exc->ContextRecord->Xmm14.Low, exc->ContextRecord->Xmm14.High,
				exc->ContextRecord->Xmm15.Low, exc->ContextRecord->Xmm15.High));
		ssh_log->add(Log::exception, fn, fl, ln, L"\r\n\r\n" + caption + msg, false);
		return true;
	}

	void MemMgr::leaks()
	{
		if(count_alloc)
		{
			Lock cs;
			String txt;
			ssh_log->add_msg(ssh_printf(L"Обнаружено %I64i потерянных блоков памяти...\r\n", count_alloc));
			auto n(root);
			while(n)
			{
				ssh_b* ptr((ssh_b*)(n) + sizeof(NodeMem));
				String bytes(ssh_make_hex_string(ptr, n->sz > 48 ? 48 : n->sz, txt, true, n->sz > 48));
				ssh_log->add_msg(ssh_printf(L"node <0x%I64x, %i,\t%s\t%s>", ptr, n->sz, bytes.str(), txt.str()));
				n = n->next;
			}
		}
	}

	void MemMgr::output()
	{
		ssh_log->add_msg(ssh_printf(L"\r\n------------------------------------------------------- Статистика ----------------------------------------------------------------\r\n"));
		leaks();
		ssh_log->add_msg(ssh_printf(L"\r\nЗа данный сеанс было выделено %i(~%s) байт памяти ..., освобождено %i(~%s) ...:%c, единовременно - %i(~%s), блоков - %i\r\n",
									total_alloc, ssh_num_volume(total_alloc), total_free, ssh_num_volume(total_free), (total_alloc != total_free ? L'(' : L')'),
									use_max_mem, ssh_num_volume(use_max_mem), max_alloc));
	}

	void* MemMgr::alloc(ssh_u sz)
	{
		Lock cs;

		ssh_b* p((ssh_b*)::_mm_malloc(sz + sizeof(NodeMem) + 4, 16));
		// создать узел
		NodeMem* nd(::new((NodeMem*)(p)) NodeMem((int)sz, root));
		if(is_enabled)
		{
#ifdef _DEBUG
			ssh_memset8(p + sizeof(NodeMem), 0x2052454755424544, sz);
#endif
			// добавить статистику
			count_alloc++;
			if(max_alloc < count_alloc) max_alloc = count_alloc;
			total_alloc += sz;
			use_mem += sz;
			if(use_max_mem < use_mem) use_max_mem = use_mem;
			if(root) root->prev = nd;
			root = nd;
		}
		else nd->use = 0;
		*(int*)(p + sz + sizeof(NodeMem)) = 0;
		return (void*)(p + sizeof(NodeMem));
	}

	// освобождение
	void MemMgr::free(ssh_b* p)
	{
		Lock cs;

		if(p)
		{
			p -= sizeof(NodeMem);
			NodeMem* nd((NodeMem*)p);

			if(nd->use)
			{
				ssh_u sz(nd->sz);
				// корректируем статистику
				total_free += sz;
				use_mem -= sz;
				count_alloc--;
				// удаляем узел из списка
				auto nn(nd->next);
				auto np(nd->prev);
				if(nn) nn->prev = np;
				if(np) np->next = nn;
				if(nd == root) root = nn;
#ifdef _DEBUG
				ssh_memset8(p + sizeof(NodeMem), 0x2020594547524553, sz);
#endif
			}
			// освобождаем память
			::_mm_free(p);
		}
	}
}
