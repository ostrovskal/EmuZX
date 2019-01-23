
#include "stdafx.h"
#include "ssh_compress.h"

namespace ssh
{
	Buffer MTF::transform(ssh_u size) noexcept
	{
		int i;
		// создать буфер
		Buffer _out(size); out = _out;
		// закодировать
		for(i = 0; i < size; i++)
		{
			auto l(*in++);
			ssh_b idx(0);
			while(alphabit[idx] != l) { idx++; }
			if(idx) memmove(&alphabit[1], &alphabit[0], idx);
			alphabit[0] = l;
			*out++ = idx;
		}
		return _out;
	}

	Buffer MTF::untransform(ssh_u size) noexcept
	{
		// создать буфер
		Buffer _out(size); out = _out;
		// раскодировать
		while(size--)
		{
			auto idx(*in++);
			auto s(alphabit[idx]);
			memmove(alphabit + 1, alphabit, idx);
			*out++ = s;
			*alphabit = s;
		}
		return _out;
	}

	Buffer MTF::process(const Buffer& _in, bool is_transform) noexcept
	{
		for(int i = 0; i < 256; i++) alphabit[i] = i;
		in = _in;
		return is_transform ? transform(_in.size()) : untransform(_in.size());
	}
}
