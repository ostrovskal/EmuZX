
#include "stdafx.h"
#include "ssh_compress.h"

namespace ssh
{
	void LZW::set_bits(ssh_d code) noexcept
	{
		ssh_d _mask(1L << (BITS - 1));
		while(_mask)
		{
			if(_mask & code) rack |= mask;
			mask >>= 1;
			if(!mask)
			{
				*out++ = rack;
				rack = 0;
				mask = 0x80;
			}
			_mask >>= 1;
		}
	}

	ssh_d LZW::get_bits(int& size)
	{
		ssh_d _mask(1L << (BITS - 1)), return_value;
		return_value = 0;
		while(_mask)
		{
			if(mask == 0x80)
			{
				rack = *in++;
				if(!--size) SSH_THROW(L"Некорректные входные данные при распаковке LZW!");
			}
			if(rack & mask) return_value |= _mask;
			_mask >>= 1;
			mask >>= 1;
			if(!mask) mask = 0x80;
		}
		return return_value;
	}

	Buffer LZW::compress(ssh_u size) noexcept
	{
		int next_code, character, string_code;
		ssh_i index, i;

		Buffer _out(size * 2 + sizeof(int)); out = _out; *(int*)out = (int)size;
		ssh_b* _out1(out); out += sizeof(int);

		// Инициализация
		next_code = FIRST_CODE;
		for(i = 0; i < TABLE_SIZE; i++) dict[i].code_value = UNUSED;
		// Считать первый символ
		string_code = *in++;
		// Пока не конец сообщения
		for(ssh_u i = 0; i < (size - 1); i++)
		{
			character = *in++;
			// Попытка найти в словаре пару <фраза, символ>
			index = find_dictionary_match(string_code, character);
			// Соответствие найдено
			if(dict[index].code_value != -1) string_code = dict[index].code_value;
			// Такой пары в словаре нет
			else
			{
				// Добавление в словарь
				if(next_code <= MAX_CODE)
				{
					dict[index].code_value = next_code++;
					dict[index].prefix_code = string_code;
					dict[index].character = (char)character;
				}
				// Выдача кода
				set_bits(string_code);
				string_code = character;
			}
		}
		// Завершение кодирования
		set_bits(string_code);
		set_bits(END_OF_STREAM);
		if(mask != 0x80) *out++ = rack;
		return Buffer(_out, 0, out - _out1);
	}

	Buffer LZW::decompress()
	{
		ssh_i count, next_code, new_code, old_code;
		int character;

		int size(*(int*)in);
		Buffer _out(size); in += sizeof(int); out = _out;

		next_code = FIRST_CODE;
		old_code = get_bits(size);
		if(old_code != END_OF_STREAM)
		{
			character = old_code;
			*out++ = old_code;
			while((new_code = get_bits(size)) != END_OF_STREAM)
			{
				// Обработка возможной исключительной ситуации
				if(new_code >= next_code)
				{
					decode_stack[0] = (char)character;
					count = decode_string(1, old_code);
				}
				else count = decode_string(0, new_code);
				character = decode_stack[count - 1];
				// Выдача раскодированной строки
				while(count > 0) *out++ = decode_stack[--count];
				// Обновление словаря
				if(next_code <= MAX_CODE)
				{
					dict[next_code].prefix_code = old_code;
					dict[next_code].character = (char)character;
					next_code++;
				}
				old_code = new_code;
			}
		}
		return _out;
	}

	ssh_i LZW::find_dictionary_match(int prefix_code, int character) noexcept
	{
		int index, offset;
		// Собственно получение значения хеш-функции
		index = (character << (BITS - 8)) ^ prefix_code;
		// Разрешение возможных коллизий
		if(index == 0) offset = 1; else offset = TABLE_SIZE - index;
		while(true)
		{
			// Эта ячейка словаря не использована
			if(dict[index].code_value == UNUSED) return index;
			// Найдено соответствие
			if(dict[index].prefix_code == prefix_code && dict[index].character == (char)character) return index;
			// Коллизия. Подготовка к следующей попытке ее разрешения
			index -= offset;
			if(index < 0) index += TABLE_SIZE;
		}
	}

	ssh_i LZW::decode_string(ssh_i count, ssh_i code) noexcept
	{
		while(code > 255)
		{
			decode_stack[count++] = dict[code].character;
			code = dict[code].prefix_code;
		}
		decode_stack[count++] = (char)code;
		return count;
	}
}
