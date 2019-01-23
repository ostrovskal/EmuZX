// stdafx.cpp: исходный файл, содержащий только стандартные включаемые модули
// ZxEmuWnd.pch будет использоваться в качестве предкомпилированного заголовка
// stdafx.obj будет содержать предварительно откомпилированные сведения о типе

#include "stdafx.h"

// преобразовать значение в ближайшую степень двойки
ssh_u ssh_pow2(ssh_u val, bool nearest) {
	ssh_d idx;
	_BitScanReverse64(&idx, val);
	ssh_u _val((1ULL << idx));
	return ((_val == val || nearest) ? _val : _val << 1ULL);
}

void* ssh_memset(void* ptr, int set, ssh_u count) {
	if(ptr) ptr = (ssh_b*)(memset((void*)ptr, set, count)) + count;
	return ptr;
}

void* ssh_memzero(void* ptr, ssh_u count) {
	return ssh_memset(ptr, 0, count);
}

void* ssh_memcpy(void* dst, const void* src, ssh_u count) {
	if(dst && src) dst = (ssh_b*)(memcpy((void*)dst, src, count)) + count;
	return dst;
}
