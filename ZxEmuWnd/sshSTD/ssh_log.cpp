
#include "stdafx.h"
#include "ssh_log.h"
#include "ssh_mem.h"

namespace ssh
{
	void Log::stk_common::init(TypeOutput _out)
	{
		out = _out;
		ssh_log->header = Log::apply_template(ssh_log->header);
		switch(out) //-V719
		{
			case TypeOutput::file:
				file = new stk_file();
				break;
			case TypeOutput::debug:
				OutputDebugString(ssh_log->header);
				break;
			case TypeOutput::email:
				email = new stk_email();
				break;
			case TypeOutput::net:
				host = new stk_host();
				break;
		}
	}

	void Log::stk_common::shutdown()
	{
		ssh_log->footer = Log::apply_template(ssh_log->footer);
		switch(out) //-V719
		{
			case TypeOutput::net:
				host->shutdown();
				SSH_DEL(host);
				break;
			case TypeOutput::screen:
				break;
			case TypeOutput::file:
				file->shutdown();
				SSH_DEL(file);
				break;
			case TypeOutput::email:
				email->shutdown();
				SSH_DEL(email);
				break;
			case TypeOutput::debug:
				OutputDebugStringW(ssh_log->footer);
				break;
		}
		out = TypeOutput::null;
	}

	void Log::stk_common::message(const String & msg, TypeMessage type, ssh_cws fn, ssh_cws fl, int ln)
	{
		switch(out)
		{
			case TypeOutput::net:
				host->message(type != none ? Log::apply_template(host->templ, msg, fn, fl, ln, type) : msg);
				break;
			case TypeOutput::file:
				file->message(type != none ? Log::apply_template(file->templ, msg, fn, fl, ln, type) : msg);
				break;
			case TypeOutput::email:
				email->message(type != none ? Log::apply_template(email->templ, msg, fn, fl, ln, type) : msg);
				break;
			case TypeOutput::debug:
				OutputDebugStringW(type != none ? Log::apply_template(debug, msg, fn, fl, ln, type) : msg);
				break;
			case TypeOutput::screen:
				if(MessageBox(nullptr, msg, ssh_system_paths(SystemInfo::PROG_NAME), MB_ICONERROR | MB_YESNO) == IDNO) exit(4);
				break;
		}
	}

	void Log::init(TypeOutput _out)
	{
		try
		{
			common.init(_out);
			// установить обработчики исключений
			ssh_mem->set_exceptionHandlers();
			// старт трассировщика
			ssh_trc.start();
			// старт менеджера памяти
			ssh_mem->start();
		}
		catch(const Exception&)
		{
			shutdown();
		}
	}

	void Log::add(TypeMessage type, ssh_cws fn, ssh_cws fl, int ln, ssh_cws msg, bool is_repl)
	{
		if(ssh_trc.is_started())
		{
			ssh_trc.stop();
			String msgArgs(msg);
			if(is_repl)
			{
				msgArgs.replace(L'\r', L'.');
				msgArgs.replace(L'\n', L'.');
			}
			// формируем сообщение на основании шаблона
			common.message(msgArgs, type, fn, fl, ln);
			ssh_trc.start();
		}
	}

	String Log::apply_template(String tpl, ssh_cws msg, ssh_cws fn, ssh_cws fl, int ln, int tp)
	{
		static ssh_cws m_types[] = {L"NONE", L"INFO", L"ASSERT", L"EXCEPTION", L"TRACE"};
		static ssh_cws rpl[] = {L"$DT", L"$fn", L"$ln", L"$fl", L"$ms", L"$tm", L"$dt", L"$us", L"$cm", L"$nm", L"$tp", nullptr};
		Time tm(Time::current());
		String tmp(ssh_printf(L"%s;%s;%i;%s;%s;%s;%s;%s;%s;%s;%s", //-V510
							  tm.fmt(L"%x").str(), fn, ln, fl, msg, tm.fmt(L"%T").str(),
							  tm.fmt(L"%d %B %Y (%A)").str(), ssh_system_paths(SystemInfo::USER_NAME).str(),
							   ssh_system_paths(SystemInfo::COMP_NAME).str(), ssh_system_paths(SystemInfo::PROG_NAME).str(), m_types[tp]));
		tmp.replace(L';', L'\0');
		return tpl.replace(rpl, tmp);
	}

	void Log::close()
	{
		ssh_trc.stop();
		ssh_mem->stop();
		ssh_trc.output();
		ssh_mem->output();
		common.shutdown();
	}

	void Log::shutdown()
	{
		ssh_trc.stop();
		ssh_mem->stop();
		common.shutdown();
	}

	void Log::stk_email::send(const String & ln)
	{
		if(!file.is_close())
		{
			is_blocked = true;
			// читаем из файла
			file.set_pos(0, File::begin);
			String str(file.read(0, charset));
			file.close();
			// обнуляем файл
			file.open(path, flags_file);
			file.write(ln, charset);
			count_msgs = 0;
			// отправка
			/*
			Mail m(host, login, pass, flags), 1);
			m.set_charset(L"utf-8");
			m.add_recipient(L"", address);
			m.set_sender(ssh_system_paths(SystemInfo::PROG_NAME), address);
			m.set_message(L"Система логгирования для отладки программы.");
			m.smtp(subj, str);
			*/
			is_blocked = false;
		}
	}

//	static void socket_receive(Socket* sock, Socket::SOCK* s, const Buffer& buf)
//	{
//		SetEvent(hEventSocket);
//	}

	void Log::stk_host::send(const String & msg)
	{
//		if(!sock.is_closed())
		{
//			sock.send(0, msg);
			if(WaitForSingleObject(hEventSocket, 30000) != WAIT_OBJECT_0) {}
//				sock.close();
			ResetEvent(hEventSocket);
		}
	}

	void StackTrace::add(bool is, ssh_cws func)
	{
		stop();
		if(!is) indent--;
		if(elems.size() >= depth) elems.remove(elems.last());
		if(indent < 0) indent = 0;
		String _indent(L' ', indent * 2);
		ssh_ws _is(is ? L'+' : L'-');
		elems += ssh_printf(L"%s%c%s", _indent, _is, func); //-V510
		if(is) indent++;
		start();
	}

	void StackTrace::output()
	{
		if(elems.size())
		{
			ssh_log->add_msg(ssh_printf(L"\r\n\r\n-------------------------------------------------- Трассировка стека (%i вызовов) --------------------------------------------------\r\n\r\n", elems.size() / 2));
			for(auto n : elems) ssh_log->add_msg(n);
			//ssh_log->add_msg(L"\r\n\r\n--------------------------------------------------------- Трассировка стека -------------------------------------------------------\r\n\r\n");
		}
		elems.reset();
	}

	void Exception::out_log(String msg) const
	{
		if(!msg.is_empty()) msg += L", ";
		// добавляем в лог
		ssh_log->add(Log::exception, func, file, line, msg + message);
	}
}
