
#pragma once

namespace ssh
{
	class SSH RLE
	{
	public:
		// �����������
		RLE() : in(nullptr), out(nullptr) {}
		// ���������
		Buffer process(const Buffer& _in, bool is_compress) noexcept { in = _in; return (is_compress ? compress(_in.size()) : decompress(_in.size())); }
	protected:
		// ���������
		Buffer compress(ssh_u size) noexcept;
		// �����������
		Buffer decompress(ssh_u size) noexcept;
		// ������ ����� � ������
		ssh_b* in, *out;
	};

	class SSH BWT
	{
		static const int SSH_BWT_BLOCK_LENGHT = 8192;
	public:
		// ���������
		Buffer process(const Buffer& _in, bool is_transform) noexcept { in = _in; return (is_transform ? transform(_in.size()) : untransform(_in.size())); }
	protected:
		// ��������� �������� �������� ����� �������������
		void set_val(int idx) noexcept;
		// ��������� �������� �������� �� ����� �������������
		ssh_b get_val(int idx) noexcept;
		// ����������� ��������� ����������
		void sort() noexcept;
		// ������� ��������������
		void untransform_block(int size, ssh_w* vec) noexcept;
		// ������� �������������
		void transform_block(int size, bool is_txt = true) noexcept;
		// ��������� ������������� ������
		Buffer transform(ssh_u size) noexcept;
		// ��������� �������������� ������
		Buffer untransform(ssh_u size) noexcept;
		// ����� �������� ����
		ssh_w* RT;
		// ����� �������� ����
		ssh_w* LT;
		// ��������� �� ����� ��������
		ssh_b* _index;
		// ��������� �� ������� ���� �������������
		ssh_b* _result;
		// ������� �����
		ssh_b* in;
		// ���������� ��������
		ssh_w keys;
		// ������� ������ �����
		ssh_w idx_lit;
	};
	
	class SSH MTF
	{
	public:
		// ���������
		Buffer process(const Buffer& _in, bool is_transform) noexcept;
	protected:
		// ��������� ������������� ������
		Buffer transform(ssh_u size) noexcept;
		// ��������� �������������� ������
		Buffer untransform(ssh_u size) noexcept;
		// ������ ����� � ������
		ssh_b* in, *out;
		// �������
		ssh_b alphabit[256];
	};
	
	class SSH LZW
	{
	public:
		struct dictionary
		{
			int code_value;
			int prefix_code;
			char character;
		};
		// �����������
		LZW() : rack(0), mask(0x80), in(nullptr), out(nullptr) {}
		// ��������� ������
		Buffer process(const Buffer& _in, bool is_compress) { in = _in; return (is_compress ? compress(_in.size()) : decompress()); }
	protected:
		// ��������� ������ ������
		Buffer compress(ssh_u size) noexcept;
		// ��������� ������������� ������ ������
		Buffer decompress();
		// ��������� ������������� ������. ��������� ������� � �����, ���������� �� ����������.
		ssh_i decode_string(ssh_i count, ssh_i code) noexcept;
		// ��������� ������ � ������� ��������� ���� <��� �����, ������>. ��� ��������� ������ ������������ ���, ���������� �� ����������.
		ssh_i find_dictionary_match(int prefix_code, int character) noexcept;
		// ��������� ���
		void set_bits(ssh_d code) noexcept;
		// ��������� ���
		ssh_d get_bits(int& size);
		// ������ ����� � ������
		ssh_b* in, *out;
		// �����
		ssh_b mask;
		// ������� ������ ���������
		int rack;
	private:
		// ������ ������� � ���������
		static const int TABLE_SIZE = 5021;
		// ���������� ����� � ����
		static const int BITS = 12;
		// ������������ �������� ����
		static const int MAX_CODE = ((1 << BITS) - 1);
		// ����������� ��� ����� ������
		static const int END_OF_STREAM = 256;
		// �������� ����, ������� �������� ������ ����������� � ������� �����
		static const int FIRST_CODE = 257;
		// ������� ��������� ������ � �������
		static const int UNUSED = -1;
		// ��������� �������
		dictionary dict[TABLE_SIZE];
		// ���� ��� �������������
		char decode_stack[TABLE_SIZE];
	};

	class SSH Arith
	{
	public:
		// ���������
		Buffer process(const Buffer& _in, bool is_compress) noexcept;
	protected:
		// ��������
		Buffer compress(ssh_u size) noexcept;
		// ����������
		Buffer decompress(ssh_u size) noexcept;
		// ����������� ���������� �������
		void encode_symbol(int symbol) noexcept;
		// ����� ���������� ���� ������ ����������
		void output(ssh_b bit) noexcept;
		// ���������� ������ ��������� ��������
		void update(int symbol) noexcept;
		// ��������� �������� ��������� �����
		void output_plus(ssh_b bit) noexcept;
		// ���� ���������� ���� ������ ����������
		int input() noexcept;
		// ������������� ���������� �������
		int decode_symbol() noexcept;
		// ������� �������������
		ssh_b index_to_char[257];
		int char_to_index[256];
		// ������� ������
		int cum_freq[258];
		int freq[258];
		// �������� ������ � ����
		long low, high, value;
		// ��������� ��������� ��������
		long bits_to_follow;
		// ������ ����� � ������
		ssh_b* in, *out;
	private:
		// ���������� ����� � ��������
		static const int BITS_IN_REGISTER	= 16;
		// ����������� ��������� �������� � �������� (65535)
		static const int TOP_VALUE			= ((1 << BITS_IN_REGISTER) - 1);
		// ���������
		static const int FIRST_QTR			= (TOP_VALUE / 4 + 1);	// 16384
		static const int HALF				= (2 * FIRST_QTR);		// 32768
		static const int THIRD_QTR			= (3 * FIRST_QTR);		// 49152
		static const int NO_OF_CHARS		= 256;
		static const int EOF_SYMBOL			= (NO_OF_CHARS + 1);	// 257
		static const int NO_OF_SYMBOLS		= (NO_OF_CHARS + 1);	// 257
		// ����� ������� ��� ���������������
		static const int MAX_FREQUENCY		= 16383;
	};

	#define SSH_COMPRESS_RLE		0x01
	#define SSH_COMPRESS_LZW		0x02
	#define SSH_COMPRESS_ARI		0x04
	
	Buffer SSH ssh_compress(const Buffer& in);
	Buffer SSH ssh_decompress(const Buffer& in);
}
