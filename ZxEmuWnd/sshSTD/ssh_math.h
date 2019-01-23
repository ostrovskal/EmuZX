
#pragma once

namespace ssh
{
	class Plane;
	class Color;
	class Mtx;
	class Vec2;
	class Vec3;
	class Vec4;
	class Quat;
	class Ray;
	class Sphere;
	class Bbox;
	class Obox;

	class SSH Vec2
	{
	public:
		// конструкторы
		Vec2() : x(0.0f), y(0.0f) {}
		Vec2(float x, float y) : x(x), y(y) {}
		Vec2(float* f) { x = f[0]; y = f[1]; }
		Vec2(const Vec2& v) : x(v.x), y(v.y) {}
		// операции
		// унарные
		const Vec2& operator + () const { return *this; }
		Vec2 operator - () const { return Vec2(-x, -y); }
		// бинарные
		Vec2 operator + (const Vec2& v) const { return Vec2(x + v.x, y + v.y); }
		Vec2 operator + (float f) const { return Vec2(x + f, y + f); }
		Vec2 operator - (const Vec2& v) const { return Vec2(x - v.x, y - v.y); }
		Vec2 operator - (float f) const { return Vec2(x - f, y - f); }
		Vec2 operator * (const Vec2& v) const { return Vec2(x * v.x, y * v.y); }
		Vec2 operator * (float f) const { return Vec2(x * f, y * f); }
		Vec2 operator / (const Vec2& v) const { return Vec2(x / v.x, y / v.y); }
		Vec2 operator / (float f) const { f = 1.0f / f; return Vec2(x * f, y * f); }
		auto operator += (const Vec2& v) { x += v.x; y += v.y; return *this; }
		auto operator += (float f) { x += f; y += f; return *this; }
		auto operator -= (const Vec2& v) { x -= v.x; y -= v.y; return *this; }
		auto operator -= (float f) { x -= f; y -= f; return *this; }
		auto operator *= (const Vec2& v) { x *= v.x; y *= v.y; return *this; }
		auto operator *= (float f) { x *= f; y *= f; return *this; }
		auto operator /= (const Vec2& v) { x /= v.x; y /= v.y; return *this; }
		auto operator /= (float f) { f = 1.0f / f; x *= f; y *= f; return *this; }
		friend Vec2 operator + (float f, const Vec2& v) { return Vec2(f + v.x, f + v.y); }
		friend Vec2 operator - (float f, const Vec2& v) { return Vec2(f - v.x, f - v.y); }
		friend Vec2 operator * (float f, const Vec2& v) { return Vec2(f * v.x, f * v.y); }
		friend Vec2 operator / (float f, const Vec2& v) { return Vec2(f / v.x, f / v.y); }
		// логические
		bool operator == (const Vec2& v) const { return ((fabs(x - v.x) + fabs(y - v.y)) < SSH_EPSILON); }
		bool operator != (const Vec2& v) const { return !(operator == (v)); }
		float operator[](ssh_u idx) const { return flt[idx]; }
		// присваивание
		const Vec2& operator = (const Vec2& v) { x = v.x; y = v.y; return *this; }
		const Vec2& operator = (float f) { x = f; y = f; return *this; }
		// специальные
		float length() const { return sqrt(x * x + y * y); }
		float lengthSq() const { return x * x + y * y; }
		float dot(const Vec2& v) const { return x * v.x + y * v.y; }
		float cross(const Vec2& v) const { return x * v.y - y * v.x; }
		bool is_identity() const { return (lengthSq() < SSH_EPSILON2); }
		auto normalize() { float l(length()); l = (l > SSH_EPSILON ? 1.0f / l : 0.0f); x *= l; y *= l; return *this; }
		auto ceil() { x = ssh_ceil(x); y = ssh_ceil(y); return *this; }
		auto floor() { x = ssh_floor(x); y = ssh_floor(y); return *this; }
		Vec2 middle(const Vec2& v) const { return Vec2((x + v.x) / 2.0f, (y + v.y) / 2.0f); }
		Vec2 reflect(const Vec2& v) const { return Vec2(*this - (2.0f * dot(v) * v)); }
		// приведение типа
		operator float*() const { return (float*)flt; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct
			{
				float x;
				float y;
			};
			float flt[2];
		};
	};

	class SSH Vec3
	{
	public:
		// конструкторы
		Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
		Vec3(float f) : x(f), y(f), z(f) {}
		Vec3(float X, float Y, float Z) : x(X), y(Y), z(Z) {}
		Vec3(float* f) { x = f[0]; y = f[1]; z = f[2]; }
		Vec3(const Vec2& v) : x(v.x), y(v.y), z(0.0f) {}
		Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) {}
		// операции
		// унарные
		auto operator + () const { return *this; }
		Vec3 operator - () const { return Vec3(-x, -y, -z); }
		// бинарные
		Vec3 operator + (const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
		Vec3 operator + (float f) const { return Vec3(x + f, y + f, z + f); }
		Vec3 operator - (const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
		Vec3 operator - (float f) const { return Vec3(x - f, y - f, z - f); }
		Vec3 operator * (const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }
		Vec3 operator * (const Mtx& m) const;
		Vec3 operator * (float f) const { return Vec3(x * f, y * f, z * f); }
		Vec3 operator / (const Vec3& v) const { return Vec3(x / v.x, y / v.y, z / v.z); }
		Vec3 operator / (float f) const { f = 1.0f / f; return Vec3(x * f, y * f, z * f); }
		auto operator += (const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
		auto operator += (float f) { x += f; y += f; z += f; return *this; }
		auto operator -= (const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
		auto operator -= (float f) { x -= f; y -= f; z -= f; return *this; }
		auto operator *= (const Vec3& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
		const Vec3& operator *= (const Mtx& m);
		auto operator *= (float f) { x *= f; y *= f; z *= f; return *this; }
		auto operator /= (const Vec3& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
		auto operator /= (float f) { f = 1.0f / f; x *= f; y *= f; z *= f; return *this; }
		friend Vec3 operator + (float f, const Vec3& v) { return Vec3(f + v.x, f + v.y, f + v.z); }
		friend Vec3 operator - (float f, const Vec3& v) { return Vec3(f - v.x, f - v.y, f - v.z); }
		friend Vec3 operator * (float f, const Vec3& v) { return Vec3(f * v.x, f * v.y, f * v.z); }
		friend Vec3 operator / (float f, const Vec3& v) { return Vec3(f / v.x, f / v.y, f / v.z); }
		friend Vec3 operator * (const Mtx& m, const Vec3& v);
		friend Vec3 operator * (const Quat& q, const Vec3& v);
		// логические
		bool operator == (const Vec3& v) const { return ((fabs(x - v.x) + fabs(y - v.y) + fabs(z - v.z)) < SSH_EPSILON); }
		bool operator != (const Vec3& v) const { return (!operator == (v)); }
		bool operator > (const Vec3& v) const { return (x > v.x && y > v.y && z > v.z); }
		bool operator < (const Vec3& v) const { return (x < v.x && y < v.y && z < v.z); }
		// присваивание
		auto operator = (const Vec3& v) { x = v.x; y = v.y; z = v.z; return *this; }
		auto operator = (float f) { x = f; y = f; z = f; return *this; }
		float operator[](ssh_u idx) const { return flt[idx]; }
		// специальные
		float length() const { return sqrt(x * x + y * y + z * z); }
		float lengthSq() const { return x * x + y * y + z * z; }
		float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
		bool is_identity() const { return (lengthSq() < SSH_EPSILON2); }
		auto normalize() { float l(length()); l = (l > SSH_EPSILON ? 1.0f / l : 0.0f); x *= l; y *= l; z *= l; return *this; }
		auto floor(const Vec3& v) { if(v.x < x) x = v.x; if(v.y < y) y = v.y; if(v.z < z) z = v.z; return *this; }
		auto ceil(const Vec3& v) { if(v.x > x) x = v.x; if(v.y > y) y = v.y; if(v.z > z) z = v.z; return *this; }
		auto set(float X, float Y, float Z) { x = X; y = Y; z = Z; return *this; }
		auto floor() { x = ssh_floor(x); y = ssh_floor(y); z = ssh_floor(z); return *this; }
		Vec3 cross(const Vec3& v) const { return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		Vec3 middle(const Vec3& v) const { return Vec3((x + v.x) / 2.0f, (y + v.y) / 2.0f, (z + v.z) / 2.0f); }
		Vec3 reflect(const Vec3& v) const { return Vec3(*this - (2.0f * dot(v) * v)); }
		Vec3 lerp(const Vec3& v, float t) const { float s = 1.0f - t; return Vec3(x * s + t * v.x, y * s + t * v.y, z * s + t * v.z); }
		// приведение типа
		operator float*() const { return (float*)flt; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct { float x, y, z; };
			float flt[3];
		};
	};

	__declspec(align(16)) class SSH Vec4
	{
	public:
		// конструкторы
		Vec4() { identity(); }
		Vec4(float f) : x(f), y(f), z(f), w(f) {}
		Vec4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {}// 0 - vector 1 - point
		Vec4(float* f) { x = f[0]; y = f[1]; z = f[2]; w = f[3]; }
		Vec4(const Vec2& v) : x(v.x), y(v.y), z(1.0f), w(0.0f) {}
		Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0.0f) {}
		Vec4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
		// операции
		// унарные
		const Vec4& operator + () const { return *this; }
		Vec4 operator - () const { return Vec4(-x, -y, -z, -w); }
		// бинарные
		Vec4 operator + (const Vec4& v) const { return Vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
		Vec4 operator + (float f) const { return Vec4(x + f, y + f, z + f, w + f); }
		Vec4 operator - (const Vec4& v) const { return Vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
		Vec4 operator - (float f) const { return Vec4(x - f, y - f, z - f, w - f); }
		Vec4 operator * (const Vec4& v) const { return Vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
		Vec4 operator * (const Mtx& m) const;
		Vec4 operator * (float f) const { return Vec4(x * f, y * f, z * f, w * f); }
		Vec4 operator / (const Vec4& v) const { return Vec4(x / v.x, y / v.y, z / v.z, w / v.w); }
		Vec4 operator / (float f) const { f = 1.0f / f; return Vec4(x * f, y * f, z * f, w * f); }
		auto operator += (const Vec4& v) { x += v.x; y += v.y; z += v.z; w += v.w; return *this; }
		auto operator += (float f) { x += f; y += f; z += f; w += f; return *this; }
		auto operator -= (const Vec4& v) { x -= v.x; y -= v.y; z -= v.z; w -= v.w; return *this; }
		auto operator -= (float f) { x -= f; y -= f; z -= f; w -= f; return *this; }
		auto operator *= (const Vec4& v) { x *= v.x; y *= v.y; z *= v.z; w *= v.w; return *this; }
		const Vec4& operator *= (const Mtx& m);
		auto operator *= (float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		auto operator /= (const Vec4& v) { x /= v.x; y /= v.y; z /= v.z; w /= v.w; return *this; }
		auto operator /= (float f) { f = 1.0f / f; x *= f; y *= f; z *= f; w *= f; return *this; }
		friend Vec4 operator + (float f, const Vec4& v) { return Vec4(f + v.x, f + v.y, f + v.z, f + v.w); }
		friend Vec4 operator - (float f, const Vec4& v) { return Vec4(f - v.x, f - v.y, f - v.z, f - v.w); }
		friend Vec4 operator * (float f, const Vec4& v) { return Vec4(f * v.x, f * v.y, f * v.z, f * v.w); }
		friend Vec4 operator * (const Mtx& m, const Vec4& v);
		friend Vec4 operator / (float f, const Vec4& v) { return Vec4(f / v.x, f / v.y, f / v.z, f / v.w); }
		// логические
		bool operator == (const Vec4& v) const { return ((fabs(x - v.x) + fabs(y - v.y) + fabs(z - v.z) + fabs(w - v.w)) < SSH_EPSILON); }
		bool operator != (const Vec4& v) const { return !(operator == (v)); }
		// присваивание
		auto operator = (const Vec4& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; }
		auto operator = (float f) { x = f; y = f; z = f; w = 0.0f; return *this; }
		float operator[](ssh_u idx) const { return flt[idx]; }
		// специальные
		void identity() { x = y = z = w = 0.0f; }
		float length() const { return sqrt(x * x + y * y + z * z + w * w); }
		float lengthSq() const { return x * x + y * y + z * z + w * w; }
		float dot(const Vec4& v) const { return x * v.x + y * v.y + z * v.z + w * v.w; }
		bool is_identity() const { return(lengthSq() < SSH_EPSILON2); }
		auto normalize() { float l(length()); l = (l > SSH_EPSILON ? 1.0f / l : 0.0f); x *= l; y *= l; z *= l; w *= l; return *this; }
		auto floor(const Vec4& v) { if(v.x < x) x = v.x; if(v.y < y) y = v.y; if(v.z < z) z = v.z; w = 0.0f; return *this; }
		auto ceil(const Vec4& v) { if(v.x > x) x = v.x; if(v.y > y) y = v.y; if(v.z > z) z = v.z; w = 0.0f; return *this; }
		auto set(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; return *this; }
		Vec4 cross(const Vec4& v) const { return Vec4(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x, w); }
		Vec4 middle(const Vec4& v) const { return Vec4((x + v.x) / 2.0f, (y + v.y) / 2.0f, (z + v.z) / 2.0f, w); }
		Vec4 reflect(const Vec4& v) const { return Vec4(*this - (2.0f * dot(v) * v)); }
		// приведение типа
		operator float*() const { return (float*)flt; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct { float x, y, z, w; };
			__m128 xmm;
			float flt[4];
		};
	};

	__declspec(align(16)) class SSH Mtx
	{
	public:
		// конструкторы
		Mtx() { identity(); }
		Mtx(float _f11, float _f12, float _f13, float _f14, float _f21, float _f22, float _f23, float _f24, float _f31, float _f32, float _f33, float _f34, float _f41, float _f42, float _f43, float _f44);
		Mtx(__m128 _1, __m128 _2, __m128 _3, __m128 _4) { xmm[0] = _1; xmm[1] = _2; xmm[2] = _3; xmm[3] = _4; }
		Mtx(float* mm) { ssh_memcpy(*this, mm, sizeof(Mtx)); }
		Mtx(const Mtx& mm) { *this = mm; }
		Mtx(const Mtx& trans, const Mtx& rotx, const Mtx& roty, const Mtx& rotz) { *this = rotx * roty; *this *= rotz * trans; }
		Mtx(const Quat& q) { fromQuat(q); }
		// операции
		const Mtx& operator + () const { return *this; }
		// бинарные
		Mtx operator - () const { return Mtx(-_11, -_12, -_13, -_14, -_21, -_22, -_23, -_24, -_31, -_32, -_33, -_34, -_41, -_42, -_43, -_44); }
		Mtx operator + (float f) const
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_add_ps(xmm[0], a), _mm_add_ps(xmm[1], a), _mm_add_ps(xmm[2], a), _mm_add_ps(xmm[3], a));
		}
		Mtx operator - (float f) const
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_sub_ps(xmm[0], a), _mm_sub_ps(xmm[1], a), _mm_sub_ps(xmm[2], a), _mm_sub_ps(xmm[3], a));
		}
		Mtx operator * (float f) const
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_mul_ps(xmm[0], a), _mm_mul_ps(xmm[1], a), _mm_mul_ps(xmm[2], a), _mm_mul_ps(xmm[3], a));
		}
		Mtx operator / (float f) const
		{
			__m128 a(_mm_rcp_ss(_mm_set_ss(f))); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_mul_ps(xmm[0], a), _mm_mul_ps(xmm[1], a), _mm_mul_ps(xmm[2], a), _mm_mul_ps(xmm[3], a));
		}
		Mtx operator + (const Mtx& m) const
		{
			return Mtx(_mm_add_ps(xmm[0], m.xmm[0]), _mm_add_ps(xmm[1], m.xmm[1]), _mm_add_ps(xmm[2], m.xmm[2]), _mm_add_ps(xmm[3], m.xmm[3]));
		}
		Mtx operator - (const Mtx& m) const
		{
			return Mtx(_mm_sub_ps(xmm[0], m.xmm[0]), _mm_sub_ps(xmm[1], m.xmm[1]), _mm_sub_ps(xmm[2], m.xmm[2]), _mm_sub_ps(xmm[3], m.xmm[3]));
		}
		Mtx operator / (const Mtx& m) const
		{
			return Mtx(_mm_div_ps(xmm[0], m.xmm[0]), _mm_div_ps(xmm[1], m.xmm[1]), _mm_div_ps(xmm[2], m.xmm[2]), _mm_div_ps(xmm[3], m.xmm[3]));
		}
		auto operator += (float f)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			xmm[0] = _mm_add_ps(xmm[0], a); xmm[1] = _mm_add_ps(xmm[1], a);
			xmm[2] = _mm_add_ps(xmm[2], a); xmm[3] = _mm_add_ps(xmm[3], a);
			return *this;
		}
		auto operator -= (float f)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			xmm[0] = _mm_sub_ps(xmm[0], a); xmm[1] = _mm_sub_ps(xmm[1], a);
			xmm[2] = _mm_sub_ps(xmm[2], a); xmm[3] = _mm_sub_ps(xmm[3], a);
			return *this;
		}
		auto operator *= (float f)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			xmm[0] = _mm_mul_ps(xmm[0], a); xmm[1] = _mm_mul_ps(xmm[1], a);
			xmm[2] = _mm_mul_ps(xmm[2], a); xmm[3] = _mm_mul_ps(xmm[3], a);
			return *this;
		}
		auto operator /= (float f)
		{
			__m128 a(_mm_rcp_ss(_mm_set_ss(f))); a = _mm_shuffle_ps(a, a, 0);
			xmm[0] = _mm_mul_ps(xmm[0], a); xmm[1] = _mm_mul_ps(xmm[1], a);
			xmm[2] = _mm_mul_ps(xmm[2], a); xmm[3] = _mm_mul_ps(xmm[3], a);
			return *this;
		}
		auto operator += (const Mtx& m)
		{
			xmm[0] = _mm_add_ps(xmm[0], m.xmm[0]); xmm[1] = _mm_add_ps(xmm[1], m.xmm[1]);
			xmm[2] = _mm_add_ps(xmm[2], m.xmm[2]); xmm[3] = _mm_add_ps(xmm[3], m.xmm[3]);
			return *this;
		}
		auto operator -= (const Mtx& m)
		{
			xmm[0] = _mm_sub_ps(xmm[0], m.xmm[0]); xmm[1] = _mm_sub_ps(xmm[1], m.xmm[1]);
			xmm[2] = _mm_sub_ps(xmm[2], m.xmm[2]); xmm[3] = _mm_sub_ps(xmm[3], m.xmm[3]);
			return *this;
		}
		auto operator /= (const Mtx& m)
		{
			xmm[0] = _mm_div_ps(xmm[0], m.xmm[0]); xmm[1] = _mm_div_ps(xmm[1], m.xmm[1]);
			xmm[2] = _mm_div_ps(xmm[2], m.xmm[2]); xmm[3] = _mm_div_ps(xmm[3], m.xmm[3]);
			return *this;
		}
		Mtx operator * (const Mtx& m) const
		{
			return Mtx(ssh_mtx_mtx(*this, m));
		}
		const Mtx& operator *= (const Mtx& m)
		{
			return (*this = ssh_mtx_mtx(*this, m));
		}
		friend Mtx operator + (float f, const Mtx& m)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_add_ps(a, m.xmm[0]), _mm_add_ps(a, m.xmm[1]), _mm_add_ps(a, m.xmm[2]), _mm_add_ps(a, m.xmm[3]));
		}
		friend Mtx operator - (float f, const Mtx& m)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_sub_ps(a, m.xmm[0]), _mm_sub_ps(a, m.xmm[1]), _mm_sub_ps(a, m.xmm[2]), _mm_sub_ps(a, m.xmm[3]));
		}
		friend Mtx operator * (float f, const Mtx& m)
		{
			__m128 a(_mm_set_ss(f)); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_mul_ps(a, m.xmm[0]), _mm_mul_ps(a, m.xmm[1]), _mm_mul_ps(a, m.xmm[2]), _mm_mul_ps(a, m.xmm[3]));
		}
		friend Mtx operator / (float f, const Mtx& m)
		{
			__m128 a(_mm_rcp_ss(_mm_set_ss(f))); a = _mm_shuffle_ps(a, a, 0);
			return Mtx(_mm_mul_ps(a, m.xmm[0]), _mm_mul_ps(a, m.xmm[1]), _mm_mul_ps(a, m.xmm[2]), _mm_mul_ps(a, m.xmm[3]));
		}
		const Mtx& operator = (const Mtx& m) { ssh_memcpy((void*)this, (void*)&m, sizeof(Mtx)); return *this; }
		float operator [] (ssh_u idx) const { return m[idx]; }
		bool operator == (const Mtx& m) { return (memcmp(*this, &m, sizeof(Mtx)) == 0); }
		bool operator != (const Mtx& m) { return !(operator == (m)); }
		float MINOR(const Mtx& m, int r0, int r1, int r2, int c0, int c1, int c2) const;
		// определитель
		float determinant() const;
		Mtx adjoint() const;
		// инвертировать
		auto inverse() { *this = Mtx(adjoint() * (1.0f / determinant())); return *this; }
		// матрица вида
		const Mtx& view(const Vec3& pos, const Vec3& at, const Vec3& up);
		// теневая
		const Mtx& shadow(const Vec4& l, const Plane& pln);
		// из кватерниона
		const Mtx& fromQuat(const Quat& q);
		// мировая матрица
		const Mtx& world(const Vec3& position, const Vec3& scale, const Quat& orientation);
		// отражения
		const Mtx& reflect(const Plane& pl);
		// ортографическая матрица
		const Mtx& ortho(float w, float h, float zn, float zf);
		// перспективная матрица
		const Mtx& perspective(float w, float h, float zn, float zf);
		// перспективная матрица из угла камеры
		const Mtx& perspectiveFov(float fovy, float aspect, float zn, float zf);
		// перевернуть
		const Mtx& transpose() { *this = Mtx(_11, _21, _31, _41, _12, _22, _32, _42, _13, _23, _33, _43, _14, _24, _34, _44); return *this; }
		// единичная
		const Mtx& identity() { ssh_memzero(this, sizeof(Mtx)); _11 = 1.0f; _22 = 1.0f; _33 = 1.0f; _44 = 1.0f; return *this; }
		// матрица позиции
		auto make_translate(const Vec3& v) { identity(); _41 = v.x; _42 = v.y; _43 = v.z; return *this; }
		// матрица масштабирования
		auto make_scale(const Vec3& v) { identity(); _11 = v.x; _22 = v.y; 	_33 = v.z; return *this; }
		// создать матрицу поворота из углов Эйлера
		const Mtx& set_rotateEuler(float yaw, float pitch, float roll);
		// из произвольной оси и угла
		const Mtx& set_rotateAngleAxis(const Vec3& v, float angle);
		// установить позицию
		auto set_translate(const Vec3& v) { _41 = v.x; _42 = v.y; _43 = v.z; return *this; }
		// установить масштаб
		auto set_scale(const Vec3& v) { _11 = v.x; _22 = v.y; _33 = v.z; return *this; }
		// убрать позицию
		auto no_trans() { _41 = 0.0f; _42 = 0.0f; _43 = 0.0f; return *this; }
		// вернуть смещение
		Vec3 get_translate() const { return Vec3(_41, _42, _43); }
		// вернуть масштаб
		Vec3 get_scale() const { return Vec3(_11, _22, _33); }
		// вернуть углы Эйлера
		Vec3 get_rotateEuler() const;
		const Mtx& from3dsMax(float* f);
		// приведение типа
		operator float*() const { return (float*)m; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			__m128 xmm[4];
			float m[16];
		};
	};

	__declspec(align(16)) class SSH Quat
	{
	public:
		// конструкторы
		Quat() { identity(); }
		Quat(float X, float Y, float Z, float W = 1.0f) : x(X), y(Y), z(Z), w(W) {}
		Quat(float* f) { x = f[0]; y = f[1]; z = f[2]; w = f[3]; }
		Quat(const Vec3& v, float f) { rotateAxis(v, f); }
		Quat(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
		Quat(const Quat& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
		Quat(const Mtx& m) { fromMatrix(m); }
		// операции
		auto operator + () const { return *this; }
		Quat operator - () const { return Quat(-x, -y, -z, -w); }
		// бинарные
		Quat operator + (const Quat& q) const { return Quat(x + q.x, y + q.y, z + q.z, w + q.w); }
		Quat operator + (float f) const { return Quat(x + f, y + f, z + f, w + f); }
		Quat operator - (const Quat& q) const { return Quat(x - q.x, y - q.y, z - q.z, w - q.w); }
		Quat operator - (float f) const { return Quat(x - f, y - f, z - f, w - f); }
		Quat operator * (const Quat& q) const;
		Quat operator * (float f) const { return Quat(x * f, y * f, z * f, w * f); }
		Quat operator / (const Quat& q) const { return Quat(x / q.x, y / q.y, z / q.z, w / q.w); }
		Quat operator / (float f) const { f = 1.0f / f; return Quat(x * f, y * f, z * f, w * f); }
		auto operator += (const Quat& q) { x += q.x; y += q.y; z += q.z; w += q.w; return *this; }
		auto operator += (float f) { x += f; y += f; z += f; w += f; return *this; }
		auto operator -= (const Quat& q) { x -= q.x; y -= q.y; z -= q.z; w -= q.w; return *this; }
		auto operator -= (float f) { x -= f; y -= f; z -= f; w -= f; return *this; }
		const Quat& operator *= (const Quat& q);
		auto operator *= (float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
		auto operator /= (const Quat& q) { x /= q.x; y /= q.y; z /= q.z; w /= q.w; return *this; }
		auto operator /= (float f) { f = 1.0f / f; x *= f; y *= f; z *= f; w *= f; return *this; }
		friend Quat operator + (float f, const Quat& q) { return Quat(f + q.x, f + q.y, f + q.z, f + q.w); }
		friend Quat operator - (float f, const Quat& q) { return Quat(f - q.x, f - q.y, f - q.z, f - q.w); }
		friend Quat operator * (float f, const Quat& q) { return Quat(f * q.x, f * q.y, f * q.z, f * q.w); }
		friend Quat operator / (float f, const Quat& q) { return Quat(f / q.x, f / q.y, f / q.z, f / q.w); }
		// логические
		bool operator == (const Quat& q) const { return ((fabs(x - q.x) + fabs(y - q.y) + fabs(z - q.z) + fabs(w - q.w)) < SSH_EPSILON); }
		bool operator != (const Quat& q) const { return !(operator == (q)); }
		// присваивание
		auto operator = (const Quat& q) { x = q.x; y = q.y; z = q.z; w = q.w; return *this; }
		// специальные
		bool is_identity() const { return (fabs(x - y - z) < SSH_EPSILON2 && fabs(w - 1) < SSH_EPSILON2); }
		float dot(const Quat& q) const { return w * q.w + x * q.x + y * q.y + z * q.z; }
		float length() const { return sqrtf(x * x + y * y + z * z + w * w); }
		float lengthSq() const { return (x * x + y * y + z * z + w * w); }
		float roll() const { return atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z); }
		float pitch() const { return atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z); }
		float yaw() const { return asin(-2 * (x * z - w * y)); }
		auto set(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; return *this; }
		const Quat& identity() { x = 0.0f; y = 0.0f; z = 0.0f; w = 1.0f; return *this; }
		Vec3 get_rotate() const { return Vec3(yaw(), pitch(), roll()); }
		void angleAxis(Vec3& axis, float& angle) const;
		Quat slerp(const Quat& q, float t, bool shortestPath) const;
		const Quat& rotateAxis(const Vec3& v, float theta);
		//Преобразование сферических координат в кватернион
		const Quat& fromSpherical(float latitude, float longitude, float angle);
		const Quat& fromMatrix(const Mtx& mm);
		Vec3 xAxis() const;
		Vec3 yAxis() const;
		Vec3 zAxis() const;
		const Quat& inverse();
		const Quat& exp();
		const Quat& ln();
		const Quat& angles(float yaw, float pitch, float roll);
		Quat nlerp(const Quat& q, float t, bool shortestPath) const;
		Quat squad(const Quat& q1, const Quat& q2, const Quat& q3, float t, bool shortestPath) const;
		auto normalize() { float factor(1.0f / sqrt(lengthSq())); *this *= factor; return *this; }
		// приведение типа
		operator float*() const { return (float*)&flt; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct { float x, y, z, w; };
			__m128 xmm;
			float flt[4];
		};
	};

	__declspec(align(16)) class SSH Dual_quat
	{
	public:
		// конструкторы
		Dual_quat() { make(Quat(), Vec3()); }
		Dual_quat(float* f) { make(f, &f[4]); }
		Dual_quat(float* q, float* v) { make(q, v); }
		Dual_quat(const Quat& q) { make(q, Vec3(0, 0, 0)); }
		Dual_quat(const Dual_quat& qq) { ssh_memcpy(this, &qq, sizeof(Dual_quat)); }
		Dual_quat(const Quat& q, const Vec3& v) { make(q, v); }
		Dual_quat(const Mtx& m) { make(Quat(m), m.get_translate()); }
		// формирование
		const Dual_quat& make(float* qq, float* t)
		{
			flt[0] = qq[0]; flt[1] = qq[1]; flt[2] = qq[2]; flt[3] = qq[3];
			flt[4] = -0.5f * (t[0] * qq[1] + t[1] * qq[2] + t[2] * qq[3]);
			flt[5] = 0.5f  * (t[0] * qq[0] + t[1] * qq[3] - t[2] * qq[2]);
			flt[6] = 0.5f  * (-t[0] * qq[3] + t[1] * qq[0] + t[2] * qq[1]);
			flt[7] = 0.5f  * (t[0] * qq[2] - t[1] * qq[1] + t[2] * qq[0]);

			return *this;
		}
		// логические
		bool operator == (const Dual_quat& qq) const
		{
			return ((fabsf(flt[0] - qq.flt[0]) + fabsf(flt[1] - qq.flt[1]) + fabsf(flt[2] - qq.flt[2]) +
					 fabsf(flt[3] - qq.flt[3]) + fabsf(flt[4] - qq.flt[4]) + fabsf(flt[5] - qq.flt[5]) +
					 fabsf(flt[6] - qq.flt[6]) + fabsf(flt[7] - qq.flt[7])) < SSH_EPSILON);
		}
		bool operator != (const Dual_quat& qq) const { return !(operator == (qq)); }
		// приведение типа
		operator const Quat() const { return Quat((float*)flt); }
		operator const Vec3() const
		{
			return Vec3(2.0f * (-flt[4] * flt[1] + flt[5] * flt[0] - flt[6] * flt[3] + flt[7] * flt[2]),
						2.0f * (-flt[4] * flt[2] + flt[5] * flt[3] + flt[6] * flt[0] - flt[7] * flt[1]),
						2.0f * (-flt[4] * flt[3] - flt[5] * flt[2] + flt[6] * flt[1] + flt[7] * flt[0]));
		}
		operator float*() const { return (float*)flt; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			float flt[8];
			__m128 xmm[2];
		};
	};

	class SSH Plane
	{
	public:
		enum Side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE
		};
		// коснтрукторы
		Plane() : x(0), y(0), z(0), d(0) {}
		Plane(float X, float Y, float Z, float D) : x(X), y(Y), z(Z), d(D) {}
		Plane(float* f) { x = f[0]; y = f[1]; z = f[2]; d = f[3]; }
		Plane(const Plane& p) : x(p.x), y(p.y), z(p.z), d(p.d) {}
		Plane(const Vec4& v) : x(v.x), y(v.y), z(v.z), d(v.w) {}
		Plane(const Vec3& v, float dist) : x(v.x), y(v.y), z(0.0f), d(dist) {}
		Plane(const Vec3& point, const Vec3& normal) : x(normal.x), y(normal.y), z(normal.z), d(point.x * normal.x + point.y * normal.y + point.z * normal.z) {}
		Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3);
		Plane operator * (const Mtx& mm) const;
		auto normalize() { vec.normalize(); return *this; }
		Side side(const Vec3& v) const;
		// операции
		// унарные
		auto operator + () const { return *this; }
		Plane operator - () const { return Plane(x, y, z, -d); }
		// бинарные
		friend Plane operator * (const Mtx& mm, const Plane& p);
		// логические
		bool operator == (const Plane& p) const { return ((fabs(x - p.x) + fabs(y - p.y) + fabs(z - p.z) + fabs(d - p.d)) < SSH_EPSILON); }
		bool operator != (const Plane& p) const { return !(operator == (p)); }
		// присваивание
		auto operator = (const Plane& p) { x = p.x; y = p.y; z = p.z; d = p.d; return *this; }
		// приведение типов
		operator float*() const { return (float*)flt; }
		operator Vec3() const { return normal; }
		// специальные
		float dot(const Vec4& v) const { return x * v.x + y * v.y + z * v.z + d * v.w; }
		float dotNormal(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
		float distance(const Vec3& v) const { return normal.dot(v) + scalar; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		union
		{
			struct { Vec3 normal; float scalar; };
			struct { Vec4 vec; };
			struct { float x, y, z, d; };
			float flt[4];
		};
	};

	class SSH Color
	{
	public:
		// конструкторы
		Color() : r(0), g(0), b(0), a(0) {}
		Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha) { saturate(); }
		Color(long rgba) { BGRA(rgba); }
		Color(const Color& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
		Color(float* f) { r = f[0]; g = f[1]; b = f[2]; a = f[3]; }
		// операции
		// арифметические
		Color operator + (const Color& c) const { return Color(r + c.r, g + c.g, b + c.b, a + c.a); }
		Color operator - (const Color& c) const { return Color(r - c.r, g - c.g, b - c.b, a - c.a); }
		Color operator * (float f) const { return Color(r * f, g * f, b * f, a * f); }
		Color operator * (const Color& c) const { return Color(r * c.r, g * c.g, b * c.b, a * c.a); }
		Color operator / (const Color& c) const { return Color(r / c.r, g / c.g, b / c.b, a / c.a); }
		Color operator / (float f) const { f = 1.0f / f; return Color(r * f, g * f, b * f, a * f); }
		auto operator += (const Color& c) { r += c.r; g += c.g; b += c.b; a += c.a; return *this; }
		auto operator -= (const Color& c) { r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this; }
		auto operator *= (float f) { r *= f; g *= f; b *= f; a *= f; return *this; }
		auto operator /= (float f) { f = 1.0f / f; r *= f; g *= f; b *= f; a *= f; return *this; }
		// логические
		float operator [] (ssh_u idx) const { return (idx < 4 ? flt[idx] : 0.0f); }
		bool operator == (const Color& c) const { return ((fabs(r - c.r) + fabs(g - c.g) + fabs(b - c.b) + fabs(a - c.a)) < SSH_EPSILON); }
		bool operator != (const Color& c) const { return !(*this == c); }
		friend Color operator * (float f, const Color& c) { return Color(f * c.r, f * c.g, f * c.b, f * c.a); }
		// приведение
		operator float*() const { return (float*)flt; }
		// специальные
		void HSB(float hue, float saturation, float brightness);
		void RGBA(long val);
		void BGRA(long val);
		long RGBA() const;
		long BGRA() const;
		const Color& saturate();
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		// члены
		union
		{
			struct { float r, g, b, a; };
			float flt[4];
		};
	};

	class SSH Sphere
	{
	public:
		Sphere() { identity(); }
		Sphere(const Vec3& v, float f) : c(v), r(f) {}
		Sphere(const Sphere& s) : c(s.c), r(s.r) {}
		Sphere(float* f) { ssh_memcpy(flt, f, sizeof(Sphere)); }
		Sphere operator + (const Sphere& s) const;
		const Sphere& operator += (const Sphere& s) { contactSphere(s, c, r); return *this; }
		void identity() { r = 0.0f; c.x = 0.0f; c.y = 0.0f; c.z = 0.0f; }
		bool is_identity() const { return (r < SSH_EPSILON && fabs(c.x - c.y - c.z) < SSH_EPSILON); }
		// установить радиус
		void setRadius(float f) { r = f; }
		// установить центр
		void setCenter(const Vec3& v) { c = v; }
		// вернуть радиус
		float radius() const { return r; }
		// вернуть центр
		const Vec3& center() const { return c; }
		// проверки на пересечения
		bool intersects(const Bbox& box) const;
		bool intersects(const Sphere& s) const { float rr(s.r + r); rr *= rr; return ((s.c - c).lengthSq() <= rr); }
		bool intersects(const Plane& plane) const { return (fabs(plane.dotNormal(c)) <= r); }
		bool intersects(const Vec3& v) const { return ((v - c).lengthSq() <= (r * r)); }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		union
		{
			struct { Vec3 c; float r; };
			float flt[4];
		};
	protected:
		void contactSphere(const Sphere& s, Vec3& v, float& f) const;
	};

	class SSH Bbox
	{
	public:
		enum Angle
		{
			X1Y1Z1, X2Y1Z1, X1Y2Z1, X2Y2Z1, X1Y1Z2, X2Y1Z2, X1Y2Z2, X2Y2Z2
		};
		// сделать проверку на то, что минимум реально меньше максимума
		Bbox() { setMinimum(-1, -1, -1); setMaximum(1, 1, 1); }
		Bbox(const Vec3& min, const Vec3& max) { setExtents(min, max); }
		Bbox(float x1, float y1, float z1, float x2, float y2, float z2) { setExtents(x1, y1, z1, x2, y2, z2); }
		Bbox(const Bbox& box) { *this = box; }
		Bbox(float* box) { setExtents(box[0], box[1], box[2], box[3], box[4], box[5]); }
		void setMinimum(const Vec3& vec) { mn = vec; updateCorners(); }
		void setMinimum(float x, float y, float z) { mn.set(x, y, z); updateCorners(); }
		void setMaximum(const Vec3& vec) { mx = vec; updateCorners(); }
		void setMaximum(float x, float y, float z) { mx.set(x, y, z); updateCorners(); }
		void setExtents(const Vec3& min, const Vec3& max) { mn = min; mx = max; updateCorners(); }
		void setExtents(float x1, float y1, float z1, float x2, float y2, float z2) { mn.x = x1; mn.y = y1; mn.z = z1; mx.x = x2; mx.y = y2; mx.z = z2; updateCorners(); }
		void scale(const Vec3& s) { setExtents(mn * s, mx * s); }
		bool intersects(const Vec3& v) const { return(v.x >= mn.x && v.x <= mx.x && v.y >= mn.y && v.y <= mx.y && v.z >= mn.z && v.z <= mx.z); }
		const Bbox& operator = (const Bbox& box) { setExtents(box.mn, box.mx); return *this; }
		auto minimum() const { return mn; }
		auto maximum() const { return mx; }
		bool intersects(const Bbox& b2) const;
		bool intersects(const Sphere& s) const;
		Bbox intersection(const Bbox& b2) const;
		void transform(const Mtx& matrix);
		void merge(const Bbox& b2);
		void merge(const Vec3& point);
		float volume() const { Vec3 diff(mx - mn); return diff.x * diff.y * diff.z; }
		Vec3 center() const { return Vec3((mx + mn) / 2.0f); }
		// вернуть массив углов бокса
		//			       5-----4
		//			      /|    /|
		//			     / |   / |
		//			    1-----2  |
		//			    |  6--|--7
		//			    | /   | /
		//			    |/    |/
		//			    0-----3
		auto getAllCorners() const { return (const Vec3*)corners; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	protected:
		void updateCorners();
		Vec3 mn;
		Vec3 mx;
		Vec3 corners[8];
	};

	class SSH Obox
	{
	public:
		enum Angle
		{
			X1Y1Z1, X2Y1Z1, X1Y2Z1, X2Y2Z1, X1Y1Z2, X2Y1Z2, X1Y2Z2, X2Y2Z2
		};
		Obox() { identity(); }
		Obox(const Obox& b) { ssh_memcpy(this, &b, sizeof(Obox)); }
		Obox(const Vec3& _x1y1z1, const Vec3& _x2y1z1, const Vec3& _x1y2z1, const Vec3& _x2y2z1, const Vec3& _x1y1z2, const Vec3& _x2y1z2, const Vec3& _x1y2z2, const Vec3& _x2y2z2);
		Obox(const Bbox& Bbox);
		Obox(float* b);
		void identity() { ssh_memzero(this, sizeof(Obox)); }
		bool intersects(const Bbox& b) const;
		bool intersects(const Obox& b) const;
		bool intersects(const Sphere& s) const;
		bool intersects(const Vec3& v) const;
		Obox transform(const Mtx& m) const;;
		Vec3 center() const;
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		//			       5-----6
		//			      /|    /|
		//			     / |   / |
		//			    1-----2  |
		//			    |  7--|--8
		//			    | /   | /
		//			    |/    |/
		//			    3-----4
		Vec3 corners[8];
	};

	class SSH Ray
	{
	public:
		Ray() {}
		Ray(const Vec3& position, const Vec3& direction) : pos(position), dir(direction) {}
		Ray(const Ray& r) : pos(r.pos), dir(r.dir) {}
		Vec3 operator * (float t) const { return point(t); }
		void setPosition(const Vec3& position) { pos = position; }
		void setDirection(const Vec3& direction) { dir = direction; }
		bool intersects(const Sphere& s, float* f) const;
		bool intersects(const Bbox& box, float* f) const;
		bool intersects(const Plane& p, float* f) const;
		auto position() const { return pos; }
		auto direction() const { return dir; }
		Vec3 point(float t) const { return Vec3(pos + (dir * t)); }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		Vec3 pos;
		Vec3 dir;
	};

	class SSH Angle
	{
	public:
		Angle() : angle(0.0f) {}
		Angle(float v) : angle(v) {}
		float RadianToDegree() const { return (float)(angle * 180.0f / SSH_PI); }
		float DegreeToRadian() const { return (float)(angle * SSH_PI / 180.0f); }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
		float angle;
	};

	class SSH Half
	{
		union Bits
		{
			float			f;
			long			si;
			unsigned long	ui;
		};

		static int const shift = 13;
		static int const shiftSign = 16;
		static ssh_d const infN = 0x7F800000; // flt32 infinity
		static ssh_d const maxN = 0x477FE000; // max flt16 normal as a flt32
		static ssh_d const minN = 0x38800000; // min flt16 normal as a flt32
		static ssh_d const signN = 0x80000000; // flt32 sign bit

		static ssh_d const infC = infN >> shift;
		static ssh_d const nanN = (infC + 1) << shift; // minimum flt16 nan as a flt32
		static ssh_d const maxC = maxN >> shift;
		static ssh_d const minC = minN >> shift;
		static ssh_d const signC = signN >> shiftSign; // flt16 sign bit

		static ssh_d const mulN = 0x52000000; // (1 << 23) / minN
		static ssh_d const mulC = 0x33800000; // minN / (1 << (23 - shift))

		static ssh_d const subC = 0x003FF; // max flt32 subnormal down shifted
		static ssh_d const norC = 0x00400; // min flt32 normal down shifted

		static ssh_d const maxD = infC - maxC - 1;
		static ssh_d const minD = minC - subC - 1;

		ssh_w val;
	public:
		Half() : val(0) {}
		Half(float value);
		Half(double value) : Half((float)value) {}
		Half(long double value) : Half((float)value) {}
		operator float() const;
		operator ssh_w() const { return val; }
#ifdef _DEBUG
		// тест
		static void unit_test();
#endif
	};

	inline Half operator ""_half(long double f)
	{
		return Half(f);
	}

	inline Vec3 Vec3::operator * (const Mtx& m) const
	{
		Vec3 v;
		ssh_vec3_mtx(v, *this, m);
		return v;
	}

	inline const Vec3& Vec3::operator *= (const Mtx& m)
	{
		ssh_vec3_mtx(*this, *this, m);
		return *this;
	}

	inline Vec3 operator * (const Mtx& m, const Vec3& v)
	{
		return Vec3(ssh_mtx_vec3(m, v));
	}

	inline Vec4 Vec4::operator * (const Mtx& m) const
	{
		Vec4 dst;
		ssh_vec4_mtx(dst, *this, m);
		return dst;
	}

	inline const Vec4& Vec4::operator *= (const Mtx& m)
	{
		ssh_vec4_mtx(*this, *this, m);
		return *this;
	}

	inline Vec4 operator * (const Mtx& m, const Vec4& v)
	{
		return Vec4(ssh_mtx_vec4(m, v));
	}

	inline const Mtx& Mtx::set_rotateEuler(float yaw, float pitch, float roll)
	{
		Quat q;
		return fromQuat(q.angles(yaw, pitch, roll));
	}

	inline const Mtx& Mtx::set_rotateAngleAxis(const Vec3& v, float angle)
	{
		Quat q;
		return fromQuat(q.rotateAxis(v, angle));
	}

	inline Vec3 Mtx::get_rotateEuler() const
	{
		Quat q(*this);
		return q.get_rotate();
	}
}
