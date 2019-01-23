
#pragma once

namespace ssh
{
	template <typename T> class Array
	{
	public:
		// конструктор по умолчанию
		Array() { }
		// инициализирующий конструктор
		Array(ssh_u _count) { alloc(_count); }
		// конструктор из списка инициализации
		Array(const std::initializer_list<T>& _list) { insert(0, _list.begin(), _list.size()); }
		// конструктор копии
		Array(const Array<T>& src) { *this = src; }
		// конструктор переноса
		Array(Array<T>&& src) { data = src.data; count = src.count; max_count = src.max_count; src.init(); }
		// деструктор
		~Array() { reset(); }
		// сброс
		void reset() noexcept { remove(0, count); delete data; init(); }
		// установить размер
		void resize(ssh_u _count) noexcept { reset(); alloc(_count); }
		// добавить элемент
		T& operator += (const T& elem) noexcept { return insert(count, elem); }
		// добавить массив
		const Array& operator += (const Array<T>& src) noexcept { return insert(count, src.data(), src.size()); }
		// заместить массив
		const Array& operator = (const Array<T>& src) noexcept { reset(); return insert(0, src.get_data(), src.size()); }
		// оператор индекса
		T& operator [] (ssh_u idx) const noexcept { return data[idx]; }
		// оператор переноса
		const Array& operator = (Array<T>&& src) noexcept { reset(); data = src.data; count = src.count; max_count = src.max_count; src.init(); return *this; }
		// установка элемента по индексу
		T& set(ssh_u idx, const T& elem) noexcept
		{
			if(idx < count)
			{
				SSH_RELEASE_NODE(T, data[idx]);
				data[idx] = elem;
			}
			return data[idx];
		}
		// вставка элемента по индексу
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
		// удаление элементов
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
		// найти элемент
		ssh_l find(const T& t) const noexcept
		{
			for(ssh_u i = 0; i < count; i++)
			{
				if(data[i] == t) return i;
			}
			return -1;
		}
		// диапазонный цикл
		T* begin() const noexcept { return data; }
		T* end() const noexcept { return &data[count]; }
		// вернуть размер
		ssh_u size() const noexcept { return count; }
		// вернуть по индексу
		T& at(ssh_u idx) const noexcept { return data[idx]; }
		// вернуть указатель на данные
		const T* get_data() const noexcept { return (const T*)data; }
		T* get_data() noexcept { return (T*)data; }
	protected:
		// вставка нескольких элементов
		const Array& insert(ssh_u idx, const T* elem, ssh_u _count)
		{
			alloc(_count);
			while(_count--) { insert(idx, elem[idx]); idx++; }
			return *this;
		}
		// очистить
		void init() { count = max_count = 0; data = nullptr; }
		// выделение памяти
		void alloc(ssh_u _count)
		{
			if((count + _count) >= max_count)
			{
				max_count += (_count + max_count);
				// выделяем блок
				T* ptr((T*)new ssh_b[max_count * sizeof(T)]);
				// копируем старые данные, если есть
				if(data)
				{
					ssh_memcpy(ptr, data, count * sizeof(T));
					delete data;
				}
				// инициализируем новые
				for(ssh_u i = count; i < max_count; i++) ::new((void*)(ptr + i)) T();
				data = ptr;
			}
		}
		// количество элементов
		ssh_u count = 0;
		// выделено элементов
		ssh_u max_count = 0;
		// данные
		T* data = nullptr;
	};
}
