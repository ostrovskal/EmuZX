
#pragma once

namespace ssh
{
	template<typename T> class Range;

	template <typename T> class _Buffer
	{
	public:
		// �� ���������
		_Buffer() : data(nullptr), sz(0), is_owner(false) { }
		// ����������� �����
		_Buffer(const _Buffer& buf, ssh_u offs = 0, ssh_u count = -1) : sz(count == -1 ? buf.size() : count), is_owner(true)
		{
			const_cast<_Buffer*>(&buf)->is_owner = false;
			data = (sz ? buf.data + offs : nullptr);
		}
		// ����������� ��������
		_Buffer(_Buffer&& buf) : data(buf.data), sz(buf.sz), is_owner(buf.is_owner)
		{
			buf.data = nullptr;
			buf.sz = 0;
			buf.is_owner = false;
		}
		// ������� ����� ������������ �������
		_Buffer(ssh_u count) : data(new T[count]), sz(count * sizeof(T)), is_owner(true) { }
		// ������� �� ���������
		_Buffer(const Range<int>& wh, int bpp) : sz(wh.w * wh.h * bpp), is_owner(true) { data = new T[sz]; }
		// ������� �� ������
		_Buffer(ssh_cws p, bool is_own = false) : data((T*)p), sz(ssh_wcslen(p) * 2), is_owner(is_own) { }
		// ������� �� ������������� �������������� ������
		_Buffer(T* p, ssh_u count, bool is_own = true) : data(p), sz(count * sizeof(T)), is_owner(is_own) {}
		// ����������
		~_Buffer() { release(); }
		// �������� ������������
		const _Buffer& operator = (const _Buffer& buf) noexcept
		{
			release();
			is_owner = true;
			sz = buf.sz;
			const_cast<_Buffer*>(&buf)->is_owner = false;
			data = buf.data;
			return *this;
		}
		// �������� ��������
		const _Buffer& operator = (_Buffer&& buf) noexcept
		{
			release();
			data = buf.data;
			sz = buf.sz;
			is_owner = buf.is_owner;
			buf.data = nullptr;
			buf.sz = 0;
			return *this;
		}
		// ������������ ������
		void reset() { release(); }
		// ������� ������
		ssh_u size() const { return sz; }
		// ������� ������
		ssh_u count() const { return sz / sizeof(T); }
		// ���������� ����
		operator T*() const { return data; }
		// ������������� ����������� ������
		template<typename TYPE> TYPE to() const { return (TYPE)data; }
#ifdef _DEBUG
		// ����
		static void unit_test()
		{

		}
#endif
	protected:
		// ����������
		void release() noexcept { if(is_owner) SSH_DEL(data); }
		// ��������� �� ������
		T* data;
		// ������ ������
		ssh_u sz;
		// ������� ���������
		bool is_owner;
	};

	typedef _Buffer<ssh_b> Buffer;
	typedef _Buffer<ssh_w> BufferW;
	typedef _Buffer<ssh_d> BufferD;
}