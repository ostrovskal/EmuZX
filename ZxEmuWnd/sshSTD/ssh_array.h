
#pragma once

namespace ssh
{
	template <typename T> class Array
	{
	public:
		// ����������� �� ���������
		Array() { }
		// ���������������� �����������
		Array(ssh_u _count) { alloc(_count); }
		// ����������� �� ������ �������������
		Array(const std::initializer_list<T>& _list) { insert(0, _list.begin(), _list.size()); }
		// ����������� �����
		Array(const Array<T>& src) { *this = src; }
		// ����������� ��������
		Array(Array<T>&& src) { data = src.data; count = src.count; max_count = src.max_count; src.init(); }
		// ����������
		~Array() { reset(); }
		// �����
		void reset() noexcept { remove(0, count); delete data; init(); }
		// ���������� ������
		void resize(ssh_u _count) noexcept { reset(); alloc(_count); }
		// �������� �������
		T& operator += (const T& elem) noexcept { return insert(count, elem); }
		// �������� ������
		const Array& operator += (const Array<T>& src) noexcept { return insert(count, src.data(), src.size()); }
		// ��������� ������
		const Array& operator = (const Array<T>& src) noexcept { reset(); return insert(0, src.get_data(), src.size()); }
		// �������� �������
		T& operator [] (ssh_u idx) const noexcept { return data[idx]; }
		// �������� ��������
		const Array& operator = (Array<T>&& src) noexcept { reset(); data = src.data; count = src.count; max_count = src.max_count; src.init(); return *this; }
		// ��������� �������� �� �������
		T& set(ssh_u idx, const T& elem) noexcept
		{
			if(idx < count)
			{
				SSH_RELEASE_NODE(T, data[idx]);
				data[idx] = elem;
			}
			return data[idx];
		}
		// ������� �������� �� �������
		T& insert(ssh_u idx, const T& elem) noexcept
		{
			if(idx <= count)
			{
				alloc(1);
				memmove(data + idx + 1, data + idx, (count - idx) * sizeof(T));
				data[idx] = elem;
				count++;
			}
			return data[idx];
		}
		// �������� ���������
		const Array& remove(ssh_u idx, ssh_u _count) noexcept
		{
			if(idx < count)
			{
				if((idx + _count) > count) _count = (count - idx);
				for(ssh_u i = 0; i < _count; i++) SSH_RELEASE_NODE(T, data[i + idx]);
				ssh_u ll(idx + _count);
				ssh_memcpy(data + idx, data + ll, (count - ll) * sizeof(T));
				count -= _count;
			}
			return *this;
		}
		// ����� �������
		ssh_l find(const T& t) const noexcept
		{
			for(ssh_u i = 0; i < count; i++)
			{
				if(data[i] == t) return i;
			}
			return -1;
		}
		// ����������� ����
		T* begin() const noexcept { return data; }
		T* end() const noexcept { return &data[count]; }
		// ������� ������
		ssh_u size() const noexcept { return count; }
		// ������� �� �������
		T& at(ssh_u idx) const noexcept { return data[idx]; }
		// ������� ��������� �� ������
		const T* get_data() const noexcept { return (const T*)data; }
		T* get_data() noexcept { return (T*)data; }
	protected:
		// ������� ���������� ���������
		const Array& insert(ssh_u idx, const T* elem, ssh_u _count)
		{
			alloc(_count);
			while(_count--) { insert(idx, elem[idx]); idx++; }
			return *this;
		}
		// ��������
		void init() { count = max_count = 0; data = nullptr; }
		// ��������� ������
		void alloc(ssh_u _count)
		{
			if((count + _count) >= max_count)
			{
				max_count += (_count + max_count);
				// �������� ����
				T* ptr((T*)new ssh_b[max_count * sizeof(T)]);
				// �������� ������ ������, ���� ����
				if(data)
				{
					ssh_memcpy(ptr, data, count * sizeof(T));
					delete data;
				}
				// �������������� �����
				for(ssh_u i = count; i < max_count; i++) ::new((void*)(ptr + i)) T();
				data = ptr;
			}
		}
		// ���������� ���������
		ssh_u count = 0;
		// �������� ���������
		ssh_u max_count = 0;
		// ������
		T* data = nullptr;
	};
}
