
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
		// инициализировать трассировщик
		void init(int _depth) { depth = _depth; }
		// добавить новый элемент
		void add(bool is, ssh_cws func);
		// вывести в лог
		void output();
		// запуск/остановка
		void start() { is_enabled = true; }
		void stop() { is_enabled = false; }
		// признак отключения
		bool is_started() const { return is_enabled; }
	protected:
		// признак активности
		bool is_enabled = false;
		// макс глубина
		int depth = 256;
		// отступ
		int indent = 0;
		// список
		List<String> elems;
	};

	// формат шаблонов:
	// $fn - функция
	// $ln - строка
	// $fl - файл
	// $ms - сообщение
	// $tm - время
	// $dt - дата длинная день месяц год день недели
	// $us - имя пользователя
	// $cm - имя компьютера
	// $nm - имя программы
	// $tp - тип сообщения
	// $DT - дата короткая день.месяц.год
	// $

	class SSH Log final
	{
		friend class Tracer;
//		friend void socket_receive(Socket* sock, Socket::SOCK* s, const Buffer& buf);
	public:
		enum TypeMessage
		{
			none,
			info,		// информация
			assert,		// проверка
			exception,	// исключение
			tracer		// трасировщик
		};
		enum class TypeOutput
		{
			null,		// не выводить
			screen,		// на экран
			file,		// в файл
			debug,		// в поток отладчика
			email,		// на почту
			net			// на хост
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
			// сообщение
			void message(const String& msg)
			{
				if(!is_blocked)
				{
					if(!file.is_close()) file.write(msg, charset);
					count_msgs++;
					if(count_msgs >= max_msgs) send(Log::apply_template(cont));
				}
			}
			// отправка
			void send(const String& ln);
			// адрес
			String address = L"ostrov_skal@mail.ru";
			// хост почты и порт
			String host = L"smtp.mail.ru:465";
			// шаблон
			String templ = L"[$tp] $DT-$tm\t$fn\t - \t($fl: $ln) - <$ms>\r\n";
			// тема
			String subj = L"Уведомление логгирования";
			// логин
			String login = L"ostrov_skal";
			// пароль
			String pass = MAIL_PASS;
			// продолжение
			String cont = L"\r\n------------------------------ Продолжение сеанса - $nm ($dt - $tm) ------------------------------\r\n\r\n";
			// путь до временного файла
			String path = ssh_system_paths(SystemInfo::TEMP_FOLDER) + ssh_gen_name(L"__MAIL__LOG__");
			// кодировка
			String charset = cp_utf16;
			// признак блокировки
			bool is_blocked = false;
			// флаги создания
			int flags = 0;// = Mail::stSSL;
			int flags_file = File::create_read_write | File::access_temp_remove;
			// максимальное количество сообщений
			int max_msgs = 200;
			// текущее количество сообщений
			int count_msgs = 0;
		protected:
			// временный файл
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
			// сообщение
			void message(const String& msg)
			{
//				if(sock.is_closed())
				{
//					sock.init(name, 0, flags, cert, pwd_cert);
					send(ssh_log->header);
				}
				send(msg);
			}
			// отправка сообщения серверу
			void send(const String& msg);
			// host:port для отправки по сети
			String name = L"localhost:11111";
			// сертификат
			String cert = L"e:\\SSL\\cer\\client.cer";
			// пароль к сертификату
			String pwd_cert;
			// шаблон для хоста
			String templ = L"[$tp] $fn - ($fl: $ln) - <$ms>";
			// флаги для создания сокета
			int flags = 0;
		protected:
			// сокет
			// Socket sock;
			// хэндл события
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
			// сообщение
			void message(const String& msg)
			{
				if(!file.is_close()) file.write(msg, L"utf-8");
			}
			// путь
			String path = ssh_system_paths(SystemInfo::PROG_FOLDER) + ssh_system_paths(SystemInfo::PROG_NAME) + L".log";
			// шаблон
			String templ = L"[$tp] $DT-$tm\t$fn  -  ($fl: $ln)\r\n$ms\r\n";;
			// флаги
			int flags = File::create_write;
		protected:
			// файл
			File file;
		};
		struct stk_common
		{
			// инициализация
			void init(TypeOutput _out);
			// завершение
			void shutdown();
			// сообщение
			void message(const String& msg, TypeMessage type = none, ssh_cws func = nullptr, ssh_cws file = nullptr, int line = 0);
			// шаблон для вывода на экран
			String screen = L"\r\n-------------------- [$tp] --------------------\r\nФункция: $fn\r\n\r\nФайл: $fl\r\n\r\nСтрока: $ln\r\n\r\nПричина: $ms\r\n\r\n\r\nПродолжить выполнение программы?\r\n\r\n\r\n-------------------- [$tp] --------------------\r\n";
			// шаблон для вывода в поток отладчика
			String debug = L"[$tp] $DT-$tm\t$fn\t - \t($fl: $ln) - [$ms]\r\n";
			// шаблон для трассировщика
			String trace = L"$ms\r\n";
			// структуры типа вывода
			stk_file* file = nullptr;
			stk_host* host = nullptr;
			stk_email* email = nullptr;
		protected:
			// тип вывода
			TypeOutput out = TypeOutput::null;
		};
		// доступ
		static Log* instance() { static Log log; return &log; }
		// добавить шаблонное сообщение
		void add(TypeMessage type, ssh_cws func, ssh_cws file, int line, ssh_cws msg, bool is_repl = true);
		// добавить простое сообщение
		void add_msg(ssh_cws msg, bool apply = true) { common.message(apply ? apply_template(common.trace, msg) : msg); }
		// инициализация
		void init(TypeOutput _out = TypeOutput::file);
		// вернуть базовую структуру
		const stk_common* get_common() const { return &common; }
		// заголовок
		String header = L"\r\n-------------------- Запуск сеанса [$us - $cm] - $nm ($dt - $tm) --------------------\r\n\r\n";
		// завершение
		String footer = L"\r\n------------------------------ Завершение сеанса - $nm ($dt - $tm) ------------------------------\r\n\r\n";
	protected:
		// деструктор
		~Log() { close(); }
		// приминение шаблона к сообщению
		static String apply_template(String templ, ssh_cws msg = nullptr, ssh_cws fn = nullptr, ssh_cws fl = nullptr, int ln = 0, int tp = 0);
		// закрыть
		void close();
		// сброс
		void shutdown();
		// основная структура
		stk_common common;
		// трассировщик стека
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
