
#include "stdafx.h"
#include "ssh_compress.h"

namespace ssh
{
	Buffer RLE::compress(ssh_u size) noexcept
	{
		Buffer _out(size * 2); out = _out;
		ssh_b* _in(in + size); *_in = 0xff;
		// найти самый часто повторяющийся символ в последовательности
		int count[256];
		ssh_memzero(count, 256 * 4);
		int mx(0), idx(0);
		for(int i = 0; i < size - 2; i++)
		{
			auto c1(in[i]);
			auto c2(in[i + 1]);
			auto c3(in[i + 2]);
			if((c1 == c2) && (c2 == c3))
			{
				count[c1]++;
				int tmp(count[c1]);
				if(tmp > mx) mx = tmp, idx = c1;
				while(in[i++] == c1);
			}
		}
		while(in < _in)
		{
			auto c1(*in++);
			auto c2(*in);
			auto c3(*(in + 1));
			auto cc(c1);
			int is1((c1 == c2) &(c2 == c3));
			auto ll(0);
			auto tmp(out++);
			*out++ = c1;
			while(in < _in)
			{
				c1 = *in++;
				c2 = *in;
				c3 = *(in + 1);
				int is2((c1 == c2) & (c2 == c3));
				ll++;
				if(is1 != is2 || (ll > 63 && is1) || (!is1 && ll > 127))
				{
					ll--;
					if(is1)
					{
						in++; ll |= 0x80; out = tmp + 1;
						if(cc == idx) ll |= 0x40; else out++;
					}
					else in--;
					*tmp = ll;
					break;
				}
				*out++ = c1;
			}
			if(ll) { *tmp = ll; if(is1) *tmp |= 0x80; }
		}
		auto l(out - _out + 2);
		*out++ = idx;
		*out = (ssh_b)((size / l) + 1);
		return Buffer(_out, 0, l);
	}

	Buffer RLE::decompress(ssh_u size) noexcept
	{
		Buffer _out(size * in[size - 1]); out = _out;
		auto idx(in[size - 2]);
		auto _in(in + size - 2);
		while(in < _in)
		{
			auto l(*in++);
			if(l & 0x80)
			{
				auto cc((l & 0x40) ? idx : *in++);
				l = (l & 0x3f) + 3;
				out = (ssh_b*)ssh_memset(out, cc, l);
			}
			else
			{
				l ++;
				out = (ssh_b*)ssh_memcpy(out, in, l);
				in += l;
			}
		}
		return Buffer(_out, 0, out - _out);
	}
}
