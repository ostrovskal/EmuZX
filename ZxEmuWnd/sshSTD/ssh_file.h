
#pragma once

#include <fcntl.h>

#include "ssh_time.h"
#include "ssh_buf.h"

namespace ssh
{
	class SSH File
	{
	public:
		enum Seek : int
		{
			begin	= SEEK_SET,
			current = SEEK_CUR,
			end		= SEEK_END
		};
		enum OpenFlags : int
		{
			append_read			= 0,
			append_read_write	= 1,
			create_write		= 2,
			create_read_write	= 3,
			open_read			= 4,
			open_write			= 5,
			open_read_write		= 6,
			access_temp_remove	= _O_TEMPORARY,
			access_temp			= _O_SHORT_LIVED,
			access_random		= _O_RANDOM,
			access_sequential	= _O_SEQUENTIAL
		};
		struct MODE_FILE_OPEN
		{
			int flag;
			int mode;
		};
		// конструкторы
		File() : h(0) {}
		File(ssh_cws name, int flags) : h(0) { open(name, flags); }
		// деструктор
		virtual ~File() { close(); }
		// открыть
		void open(ssh_cws name, int flags)
		{
			static MODE_FILE_OPEN opens[] = { {_O_APPEND | _O_RDONLY, 0}, {_O_APPEND | _O_RDWR, 0}, {_O_CREAT | _O_TRUNC | _O_WRONLY, _S_IWRITE}, {_O_CREAT | _O_TRUNC | _O_RDWR, _S_IWRITE | _S_IREAD}, {_O_RDONLY, 0}, {_O_WRONLY, 0}, {_O_RDWR, 0} };
			if((h = _wopen(name, opens[flags & 7].flag | (flags & (~7) | _O_BINARY), opens[flags & 7].mode)) == -1) SSH_THROW(ssh_printf(L"Ќе удалось открыть файл %s!", name));
			path = name;
		}
		// закрыть
		void close() { if(h) _close(h); h = 0; }
		// чтение в предоставленный буфер определенного размера
		Buffer read(const Buffer& buf, ssh_u size) const
		{
			if(size > buf.size())  SSH_THROW(ssh_printf(L"–азмер буфера %i слишком мал дл€ чтени€ файла %s!", buf.size(), path.str()));
			if(_read(h, buf, (ssh_i)size) != size) SSH_THROW(ssh_printf(L"ќшибка чтени€ файла %s!", path.str()));
			return buf;
		}
		// чтение определенного размера
		Buffer read(ssh_u size = 0) const
		{
			if(!size) size = (length() - get_pos());
			return read(Buffer(size), size);
		}
		// чтение определенного размера с определенной позиции
		Buffer read(ssh_u size, ssh_u pos, int flags) const
		{
			set_pos(pos, flags);
			return read(size);
		}
		// чтение в предоставленный буфер определенного размера с определенной позиции
		Buffer read(const Buffer& buf, ssh_u size, ssh_u pos, int flags) const
		{
			set_pos(pos, flags);
			return read(buf, size);
		}
		// чтение строки с преобразованием кодировки
		String read(ssh_u size, ssh_cws cnv) const
		{
			return ssh_convert(cnv, read(size), 0);
		}
		// запись области пам€ти
		void write(void* ptr, ssh_u size) const
		{
			if(_write(h, ptr, (ssh_i)size) != size) SSH_THROW(ssh_printf(L"ќшибка записи файла %s!", path.str()));
		}
		// запись буфера определенного размера
		void write(const Buffer& buf, ssh_u size = 0) const
		{
			if(!size) size = buf.size();
			write((ssh_b*)buf, (ssh_i)size);
		}
		// запись буфера определенного размера в определенную позицию
		void write(const Buffer& buf, ssh_u size, ssh_u pos, int flags) const
		{
			set_pos(pos, flags);
			return write(buf, size);
		}
		// запись строки с преобразованием кодировки
		void write(const String& str, ssh_cws cnv) const
		{
			write(ssh_convert(cnv, str));
		}
		// сервис
		void get_time(Time* create, Time* access, Time* write) const
		{
			struct _stat64 stat;
			if(_wstat64(path, &stat)) SSH_THROW(ssh_printf(L"Ќе удалось получить врем€ файла %s!", path.str()));
			if(access) *access = stat.st_atime;
			if(create) *create = stat.st_ctime;
			if(write) *write = stat.st_mtime;
		}
		ssh_u get_pos() const { return _telli64(h); }
		ssh_u length() const { return _filelengthi64(h); }
		ssh_u set_pos(ssh_u pos, int flags) const { return _lseeki64(h, pos, flags); }
		bool set_length(ssh_u size) const { return (_chsize_s(h, size) == 0); }
		bool is_close() const { return (h == 0); }
		static bool rename(ssh_cws _old, ssh_cws _new) { return (_wrename(_old, _new) == 0); }
		static bool remove(ssh_cws name) { return (_wunlink(name) == 0); }
		static bool find(ssh_cws path, _wfinddata64_t* data, ssh_l* handle, bool is_begin)
		{
			if(is_begin) return ((*handle = _wfindfirst64(path, data)) != 0);
			return (_wfindnext64(*handle, data) == 0);
		}
		static bool is_exist(ssh_cws path)
		{
			int h(_wopen(path, _O_RDONLY));
			if(h != -1) _close(h);
			return (h != -1);
		}
		String get_path() const { return path; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	protected:
		int h;
		String path;
	};
}
