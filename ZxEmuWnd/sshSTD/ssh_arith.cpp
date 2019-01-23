
#include "stdafx.h"
#include "ssh_compress.h"

extern "C"
{
	void ssh_asm_encode_ari(void* _this, ssh_u size);
}
namespace ssh
{
	void Arith::update(int symbol) noexcept
	{
		int i;
		// проверка на переполнение счетчика частоты
		if(cum_freq[0] == MAX_FREQUENCY)
		{
			int cum(0);
			// масштабирование частот при переполнении
			for(i = 257; i >= 0; i--)
			{
				freq[i] = (freq[i] + 1) / 2;
				cum_freq[i] = cum;
				cum += freq[i];
			}
		}
		for(i = symbol; freq[i] == freq[i - 1]; i--);
		if(i < symbol)
		{
			int ch_i(index_to_char[i]);
			int ch_symbol(index_to_char[symbol]);

			index_to_char[i] = ch_symbol;
			index_to_char[symbol] = ch_i;

			char_to_index[ch_i] = symbol;
			char_to_index[ch_symbol] = i;
		}
		// обновление значений в таблицах частот
		freq[i]++;
		while(i-- > 0) cum_freq[i]++;
	}

	static ssh_b buffer, bits_to_go(0x80);

	void Arith::output(ssh_b bit) noexcept
	{
		buffer = (buffer >> 1) | bit;
		if((bits_to_go = _rotr8(bits_to_go, 1)) == 0x80) *out++ = buffer;
	}

	void Arith::output_plus(ssh_b bit) noexcept
	{
		output(bit); bit ^= 0x80;
		while(bits_to_follow > 0) output(bit), bits_to_follow--;
	}

	int Arith::input() noexcept
	{
		if((bits_to_go = _rotl8(bits_to_go, 1)) == 1) buffer = *in++;
		return ((buffer & bits_to_go) == bits_to_go);
	}

	void Arith::encode_symbol(int symbol) noexcept
	{
		// пересчет значений границ
		auto range((high - low) + 1);
		high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
		low = low + (range * cum_freq[symbol]) / cum_freq[0];
		// выдвигание очередных битов
		while(true)
		{
			if(high < HALF) output_plus(0x0);
			else if(low >= HALF)
			{
				output_plus(0x80);
				low -= HALF;
				high -= HALF;
			}
			else if(low >= FIRST_QTR && high < THIRD_QTR)
			{
				bits_to_follow++;
				low -= FIRST_QTR;
				high -= FIRST_QTR;
			}
			else break;
			// сдвиг влево с вт€гиванием очередного бита
			low += low;
			high += high + 1;
		}
	}

	int Arith::decode_symbol() noexcept
	{
		int symbol;
		// определение текущего масштаба частот
		auto range((high - low) + 1);
		// масштабирование значени€ в регистре кода
		int cum((int)((((long)(value - low) + 1) * cum_freq[0] - 1) / range));
		// поиск соответствующего символа в таблице частот
		for(symbol = 1; cum_freq[symbol] > cum; symbol++);
		// пересчет границ
		high = low + (range * cum_freq[symbol - 1]) / cum_freq[0] - 1;
		low = low + (range * cum_freq[symbol]) / cum_freq[0];
		// удаление очередного символа из входного потока
		while(true)
		{
			if(high < HALF) {}
			else if(low >= HALF)
			{
				value -= HALF;
				low -= HALF;
				high -= HALF;
			}
			else if(low >= FIRST_QTR && high < THIRD_QTR)
			{
				value -= FIRST_QTR;
				low -= FIRST_QTR;
				high -= FIRST_QTR;
			}
			else break;
			// сдвиг влево с вт€гиванием очередного бита
			low += low;
			high += high + 1;
			value += value + input();
		}
		return symbol;
	}

	Buffer Arith::compress(ssh_u size) noexcept
	{
		Buffer _out(size * 2 + 1); out = _out;

		for(ssh_u i = 0 ; i < size; i++)
		{
			// Ќаходим индекс символа
			int symbol(char_to_index[*in++]);
			encode_symbol(symbol);
			update(symbol);
		}
		encode_symbol(EOF_SYMBOL);
		// ќчистка побитового вывода
		bits_to_follow++;
		output_plus(low >= FIRST_QTR);
		*out++ = (buffer & bits_to_go);
		// расчитать соотношение размеров
		auto l((out + 1) - _out);
		*out = (ssh_b)((size / l) + 1);
		// вернуть буффер
		return Buffer(_out, 0, l);
	}

	Buffer Arith::decompress(ssh_u size) noexcept
	{
		int symbol;

		size--;
		Buffer _out(size * in[size]); out = _out;

		for(int i = 0; i < BITS_IN_REGISTER; i++) value += value + input();
		while((symbol = decode_symbol()) != EOF_SYMBOL)
		{
			*out++ = index_to_char[symbol];
			update(symbol);
		}
		// вернуть буффер
		return Buffer(_out, 0, out - _out);
	}

	Buffer Arith::process(const Buffer& _in, bool is_compress) noexcept
	{
		low = value = bits_to_follow = 0;
		bits_to_go = 0x80; buffer = 0;
		high = TOP_VALUE;

		int i;
		for(i = 0; i < 256; i++)
		{
			char_to_index[i] = i + 1;
			index_to_char[i + 1] = i;
		}
		for(i = 0; i <= 257; i++)
		{
			freq[i] = 1;
			cum_freq[i] = 257 - i;
		}
		freq[0] = 0;
		in = _in;
		return is_compress ? compress(_in.size()) : decompress(_in.size());
	}
}
