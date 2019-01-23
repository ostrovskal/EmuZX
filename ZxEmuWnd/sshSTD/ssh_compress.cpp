
#include "stdafx.h"
#include "ssh_compress.h"

namespace ssh
{
	Buffer ssh_compress(const Buffer& in)
	{
		BWT bwt;
		MTF mtf;
		RLE rle;
		Arith ari;
		ssh_u val(ssh_time_counter(0));
		for(int i = 0; i < 1000; i++)
		{
			Buffer _bwt(bwt.process(in, true));
		}
		val = ssh_time_counter(val) / 1000;
		SSH_MSG(String(val));
		return Buffer();
		/*
		Buffer _mtf(mtf.process(_bwt, true));
		Buffer _rle(rle.process(_mtf, true));// rle
		Buffer _ari1(ari.process(_rle, true));// rle|ari
		Buffer _ari2(ari.process(_mtf, true));// ari
		ssh_b* out(_rle);
		int opt(SSH_COMPRESS_RLE);
		auto sz(_rle.size());
		if(sz > _ari1.size())
		{
			out = _ari1;
			opt = SSH_COMPRESS_RLE | SSH_COMPRESS_ARI;
			sz = _ari1.size();
		}
		if(sz > _ari2.size())
		{
			out = _ari2;
			opt = SSH_COMPRESS_ARI;
			sz = _ari2.size();
		}
		Buffer buf(sz + 1);
		ssh_b* _buf(buf);
		_buf[sz] = opt;
		ssh_memcpy(_buf, out, sz);
		return buf;
		*/
	}
	
	Buffer ssh_decompress(const Buffer& in)
	{
		BWT bwt;
		MTF mtf;
		RLE rle;
		Arith ari;

		auto size(in.size() - 1);
		Buffer out(in, 0, size);

		int opt(in[size]);
		if(opt & SSH_COMPRESS_ARI) out = ari.process(out, false);
		if(opt & SSH_COMPRESS_RLE) out = rle.process(out, false);
		out = mtf.process(out, false);
		return bwt.process(out, false);
	}

}