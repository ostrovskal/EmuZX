
#pragma once

namespace ssh
{
	class SSH Exception
	{
	public:
		Exception(ssh_cws fn, ssh_cws fl, int ln, ssh_cws msg) : func(fn), file(fl), line(ln), message(msg) {}
		virtual void out_log(String msg) const;
		virtual ~Exception() {}
	protected:
		Exception() : line(0), func(nullptr), file(nullptr) {}
		int line;
		ssh_cws func, file;
		String message;
	};
}

#include "ssh_list.h"
#include "ssh_file.h"
//#include "ssh_email.h"
//#include "ssh_sock.h"

namespace ssh
{
	class SSH StackTrace final
	{
	public:
		// ���������������� ������������
		void init(int _depth) { depth = _depth; }
		// �������� ����� �������
		void add(bool is, ssh_cws func);
		// ������� � ���
		void output();
		// ������/���������
		void start() { is_enabled = true; }
		void stop() { is_enabled = false; }
		// ������� ����������
		bool is_started() const { return is_enabled; }
	protected:
		// ������� ����������
		bool is_enabled = false;
		// ���� �������
		int depth = 256;
		// ������
		int indent = 0;
		// ������
		List<String> elems;
	};

	// ������ ��������:
	// $fn - �������
	// $ln - ������
	// $fl - ����
	// $ms - ���������
	// $tm - �����
	// $dt - ���� ������� ���� ����� ��� ���� ������
	// $us - ��� ������������
	// $cm - ��� ����������
	// $nm - ��� ���������
	// $tp - ��� ���������
	// $DT - ���� �������� ����.�����.���
	// $

	class SSH Log final
	{
		friend class Tracer;
//		friend void socket_receive(Socket* sock, Socket::SOCK* s, const Buffer& buf);
	public:
		enum TypeMessage
		{
			none,
			info,		// ����������
			assert,		// ��������
			exception,	// ����������
			tracer		// �����������
		};
		enum class TypeOutput
		{
			null,		// �� ��������
			screen,		// �� �����
			file,		// � ����
			debug,		// � ����� ���������
			email,		// �� �����
			net			// �� ����
		};
		struct stk_email
		{
			stk_email()
			{
				file.open(path, flags_file);
				file.write(ssh_log->header, charset);
			}
			void shutdown()
			{
				if(!file.is_close()) file.write(ssh_log->footer, charset);
				send(L"");
			}
			// ���������
			void message(const String& msg)
			{
				if(!is_blocked)
				{
					if(!file.is_close()) file.write(msg, charset);
					count_msgs++;
					if(count_msgs >= max_msgs) send(Log::apply_template(cont));
				}
			}
			// ��������
			void send(const String& ln);
			// �����
			String address = L"ostrov_skal@mail.ru";
			// ���� ����� � ����
			String host = L"smtp.mail.ru:465";
			// ������
			String templ = L"[$tp] $DT-$tm\t$fn\t - \t($fl: $ln) - <$ms>\r\n";
			// ����
			String subj = L"����������� ������������";
			// �����
			String login = L"ostrov_skal";
			// ������
			String pass = MAIL_PASS;
			// �����������
			String cont = L"\r\n------------------------------ ����������� ������ - $nm ($dt - $tm) ------------------------------\r\n\r\n";
			// ���� �� ���������� �����
			String path = ssh_system_paths(SystemInfo::TEMP_FOLDER) + ssh_gen_name(L"__MAIL__LOG__");
			// ���������
			String charset = cp_utf16;
			// ������� ����������
			bool is_blocked = false;
			// ����� ��������
			int flags = 0;// = Mail::stSSL;
			int flags_file = File::create_read_write | File::access_temp_remove;
			// ������������ ���������� ���������
			int max_msgs = 200;
			// ������� ���������� ���������
			int count_msgs = 0;
		protected:
			// ��������� ����
			File file;
		};
		struct stk_host
		{
			stk_host()
			{
				if((hEventSocket = CreateEvent(nullptr, true, false, nullptr))) {}
//					sock.setCallbacks(socket_receive, nullptr, nullptr, nullptr, nullptr);
			}
			void shutdown()
			{
				send(ssh_log->footer);
				if(hEventSocket)
				{
					CloseHandle(hEventSocket);
					hEventSocket = nullptr;
				}
			}
			// ���������
			void message(const String& msg)
			{
//				if(sock.is_closed())
				{
//					sock.init(name, 0, flags, cert, pwd_cert);
					send(ssh_log->header);
				}
				send(msg);
			}
			// �������� ��������� �������
			void send(const String& msg);
			// host:port ��� �������� �� ����
			String name = L"localhost:11111";
			// ����������
			String cert = L"e:\\SSL\\cer\\client.cer";
			// ������ � �����������
			String pwd_cert;
			// ������ ��� �����
			String templ = L"[$tp] $fn - ($fl: $ln) - <$ms>";
			// ����� ��� �������� ������
			int flags = 0;
		protected:
			// �����
			// Socket sock;
			// ����� �������
			HANDLE hEventSocket = 0;
		};
		struct stk_file
		{
			stk_file()
			{
				file.open(path, flags);
				file.write(ssh_log->header, L"utf-8");
			}
			void shutdown()
			{
				if(!file.is_close()) file.write(ssh_log->footer, L"utf-8");
			}
			// ���������
			void message(const String& msg)
			{
				if(!file.is_close()) file.write(msg, L"utf-8");
			}
			// ����
			String path = ssh_system_paths(SystemInfo::PROG_FOLDER) + ssh_system_paths(SystemInfo::PROG_NAME) + L".log";
			// ������
			String templ = L"[$tp] $DT-$tm\t$fn  -  ($fl: $ln)\r\n$ms\r\n";;
			// �����
			int flags = File::create_write;
		protected:
			// ����
			File file;
		};
		struct stk_common
		{
			// �������������
			void init(TypeOutput _out);
			// ����������
			void shutdown();
			// ���������
			void message(const String& msg, TypeMessage type = none, ssh_cws func = nullptr, ssh_cws file = nullptr, int line = 0);
			// ������ ��� ������ �� �����
			String screen = L"\r\n-------------------- [$tp] --------------------\r\n�������: $fn\r\n\r\n����: $fl\r\n\r\n������: $ln\r\n\r\n�������: $ms\r\n\r\n\r\n���������� ���������� ���������?\r\n\r\n\r\n-------------------- [$tp] --------------------\r\n";
			// ������ ��� ������ � ����� ���������
			String debug = L"[$tp] $DT-$tm\t$fn\t - \t($fl: $ln) - [$ms]\r\n";
			// ������ ��� �������������
			String trace = L"$ms\r\n";
			// ��������� ���� ������
			stk_file* file = nullptr;
			stk_host* host = nullptr;
			stk_email* email = nullptr;
		protected:
			// ��� ������
			TypeOutput out = TypeOutput::null;
		};
		// ������
		static Log* instance() { static Log log; return &log; }
		// �������� ��������� ���������
		void add(TypeMessage type, ssh_cws func, ssh_cws file, int line, ssh_cws msg, bool is_repl = true);
		// �������� ������� ���������
		void add_msg(ssh_cws msg, bool apply = true) { common.message(apply ? apply_template(common.trace, msg) : msg); }
		// �������������
		void init(TypeOutput _out = TypeOutput::file);
		// ������� ������� ���������
		const stk_common* get_common() const { return &common; }
		// ���������
		String header = L"\r\n-------------------- ������ ������ [$us - $cm] - $nm ($dt - $tm) --------------------\r\n\r\n";
		// ����������
		String footer = L"\r\n------------------------------ ���������� ������ - $nm ($dt - $tm) ------------------------------\r\n\r\n";
	protected:
		// ����������
		~Log() { close(); }
		// ���������� ������� � ���������
		static String apply_template(String templ, ssh_cws msg = nullptr, ssh_cws fn = nullptr, ssh_cws fl = nullptr, int ln = 0, int tp = 0);
		// �������
		void close();
		// �����
		void shutdown();
		// �������� ���������
		stk_common common;
		// ������������ �����
		StackTrace _tracer;
	};

	class SSH Tracer
	{
	public:
		Tracer(ssh_cws _fn) : fn(_fn)
		{
			if(ssh_trc.is_started()) ssh_trc.add(true, _fn);
		}
		~Tracer()
		{
			if(ssh_trc.is_started()) ssh_trc.add(false, fn);
		}
	protected:
		ssh_cws fn;
	};
}
