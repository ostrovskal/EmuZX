
#include "stdafx.h"
#include "ssh_math.h"

namespace ssh
{
	Half::Half(float value)
	{
		if(ssh_cpu_caps(CpuCaps::HALF))
			val = _mm_cvtps_ph(_mm_set_ss(value), 0).m128i_u16[0];
		else
		{
			Bits v, s;
			v.f = value;
			unsigned long sign = v.si & signN;
			v.si ^= sign;
			sign >>= shiftSign; // logical shift
			s.si = mulN;
			s.si = (long)(s.f * v.f); // correct subnormals
			v.si ^= (s.si ^ v.si) & -(minN > v.si);
			v.si ^= (infN ^ v.si) & -((infN > v.si) & (v.si > maxN));
			v.si ^= (nanN ^ v.si) & -((nanN > v.si) & (v.si > infN));
			v.ui >>= shift; // logical shift
			v.si ^= ((v.si - maxD) ^ v.si) & -(v.si > maxC);
			v.si ^= ((v.si - minD) ^ v.si) & -(v.si > subC);
			val = (ssh_w)(v.ui | sign);
		}
	}

	Half::operator float() const
	{
		if(ssh_cpu_caps(CpuCaps::HALF))
			return _mm_cvtph_ps(_mm_set1_epi16(val)).m128_f32[0];
		Bits v;
		v.ui = val;
		long sign = v.si & signC;
		v.si ^= sign;
		sign <<= shiftSign;
		v.si ^= ((v.si + minD) ^ v.si) & -(v.si > subC);
		v.si ^= ((v.si + maxD) ^ v.si) & -(v.si > maxC);
		Bits s;
		s.si = mulC;
		s.f *= v.si;
		long mask = -(long)(norC > v.si);
		v.si <<= shift;
		v.si ^= (s.si ^ v.si) & mask;
		v.si |= sign;
		return v.f;
	}

	bool Sphere::intersects(const Bbox& box) const
	{
		const Vec3& mnn(box.minimum());
		const Vec3& mxx(box.maximum());
		return ((c.x >= mnn.x && mnn.x - c.x <= r) && (c.x <= mxx.x && c.x - mxx.x <= r) && (c.y >= mnn.y && mnn.y - c.y <= r) && (c.y <= mxx.y && c.y - mxx.y <= r) && (c.z >= mnn.z && mnn.z - c.z <= r) && (c.z <= mxx.z && c.z - mxx.z <= r));
	}

	Vec3 operator * (const Quat& q, const Vec3& v)
	{
		Vec3 qvec(q.x, q.y, q.z);
		Vec3 uv(qvec.cross(v));
		Vec3 uuv(qvec.cross(uv));

		uv *= (2.0f * q.w);
		uuv *= 2.0f;
		return v + uv + uuv;
	}

	Mtx::Mtx(float _f11, float _f12, float _f13, float _f14, float _f21, float _f22, float _f23, float _f24, float _f31, float _f32, float _f33, float _f34, float _f41, float _f42, float _f43, float _f44) :
				_11(_f11), _12(_f12), _13(_f13), _14(_f14), _21(_f21), _22(_f22), _23(_f23), _24(_f24),
				_31(_f31), _32(_f32), _33(_f33), _34(_f34), _41(_f41), _42(_f42), _43(_f43), _44(_f44) {}

	float Mtx::MINOR(const Mtx& m, int r0, int r1, int r2, int c0, int c1, int c2) const
	{
		float* f(m);
		return	f[r0 * 4 + c0] * (f[r1 * 4 + c1] * f[r2 * 4 + c2] - f[r2 * 4 + c1] * f[r1 * 4 + c2]) -
				f[r0 * 4 + c1] * (f[r1 * 4 + c0] * f[r2 * 4 + c2] - f[r2 * 4 + c0] * f[r1 * 4 + c2]) +
				f[r0 * 4 + c2] * (f[r1 * 4 + c0] * f[r2 * 4 + c1] - f[r2 * 4 + c0] * f[r1 * 4 + c1]);
	}
	float Mtx::determinant() const
	{
		return	_11 * MINOR( * this, 1, 2, 3, 1, 2, 3) - _12 * MINOR( * this, 1, 2, 3, 0, 2, 3) + _13 * MINOR( * this, 1, 2, 3, 0, 1, 3) - _14 * MINOR( * this, 1, 2, 3, 0, 1, 2);
	}
	Mtx Mtx::adjoint() const
	{
		return Mtx(	MINOR(*this, 1, 2, 3, 1, 2, 3),  -MINOR(*this, 0, 2, 3, 1, 2, 3), 
					MINOR(*this, 0, 1, 3, 1, 2, 3),  -MINOR(*this, 0, 1, 2, 1, 2, 3), 

					-MINOR(*this, 1, 2, 3, 0, 2, 3), MINOR(*this, 0, 2, 3, 0, 2, 3), 
					-MINOR(*this, 0, 1, 3, 0, 2, 3), MINOR(*this, 0, 1, 2, 0, 2, 3), 

					MINOR(*this, 1, 2, 3, 0, 1, 3), -MINOR(*this, 0, 2, 3, 0, 1, 3), 
					MINOR(*this, 0, 1, 3, 0, 1, 3), -MINOR(*this, 0, 1, 2, 0, 1, 3), 

					-MINOR(*this, 1, 2, 3, 0, 1, 2), MINOR(*this, 0, 2, 3, 0, 1, 2), 
					-MINOR(*this, 0, 1, 3, 0, 1, 2), MINOR(*this, 0, 1, 2, 0, 1, 2));
	}
	
	const Mtx& Mtx::view(const Vec3& pos, const Vec3& at, const Vec3& up)
	{
		Vec3 vLook(at - pos);
		vLook.normalize();

		Vec3 vRight(up.cross(vLook));
		vRight.normalize();

		Vec3 vUp(vLook.cross(vRight));
		vUp.normalize();

		_11 = vRight.x ; _12 = vUp.x ; _13 = vLook.x ; _14 = 0.0f;
		_21 = vRight.y ; _22 = vUp.y ; _23 = vLook.y ; _24 = 0.0f;
		_31 = vRight.z ; _32 = vUp.z ; _33 = vLook.z ; _34 = 0.0f;

		_41 = -vRight.dot(pos);
		_42 = -vUp.dot(pos);
		_43 = -vLook.dot(pos);
		_44 = 1.0f;

		return *this;
	}
	
	const Mtx& Mtx::shadow(const Vec4& l, const Plane& pln)
	{
		Plane p(pln);
		p.normalize();

		float d(p.dot(l));

		_11 = p.x * l.x + d;	_12 = p.x * l.y;	_13 = p.x * l.z;	_14 = p.x * l.w;
		_21 = p.y * l.x;		_22 = p.y * l.y + d;_23 = p.y * l.z;	_24 = p.y * l.w;
		_31 = p.z * l.x;		_32 = p.z * l.y;	_33 = p.z * l.z + d;_34 = p.z * l.w;
		_41 = p.d * l.x;		_42 = p.d * l.y;	_43 = p.d * l.z;	_44 = p.d * l.w + d;

		return *this;
	}
	
	const Mtx& Mtx::fromQuat(const Quat& q)
	{
		float fTx = 2 * q.x;
		float fTy = 2 * q.y;
		float fTz = 2 * q.z;

		float fTwx = fTx * q.w;
		float fTwy = fTy * q.w;
		float fTwz = fTz * q.w;

		float fTxx = fTx * q.x;
		float fTxy = fTy * q.x;
		float fTxz = fTz * q.x;
		float fTyy = fTy * q.y;
		float fTyz = fTz * q.y;
		float fTzz = fTz * q.z;

		_11 = 1 - (fTyy + fTzz);
		_21 = fTxy - fTwz;
		_31 = fTxz + fTwy;
		_12 = fTxy + fTwz;
		_22 = 1 - (fTxx + fTzz);
		_32 = fTyz - fTwx;
		_13 = fTxz - fTwy;
		_23 = fTyz + fTwx;
		_33 = 1 - (fTxx + fTyy);

		_14 = _24 = _34 = 0.0f;
		_41 = _42 = _43 = 0.0f;
		_44 = 1.0f;

		return  *this;

	}
	
	const Mtx& Mtx::world(const Vec3& position, const Vec3& scale, const Quat& orientation)
	{
		Mtx pos, sc, rot(orientation);
		pos.set_translate(position);
		sc.set_scale(scale);

		*this = sc * rot * pos;
		return *this;
	}

	const Mtx& Mtx::reflect(const Plane& pl)
	{
		Plane p(pl);
		p.normalize();

		_11 = -2 * p.x * p.x + 1;	_12 = -2 * p.y * p.x;		_13 = -2 * p.z * p.x;		_14 = 0;
		_21 = -2 * p.x * p.y;		_22 = -2 * p.y * p.y + 1;	_23 = -2 * p.z * p.y;		_24 = 0;
		_31 = -2 * p.x * p.z;		_32 = -2 * p.y * p.z;		_33 = -2 * p.z * p.z + 1;	_34 = 0;
		_41 = -2 * p.x * p.d;		_42 = -2 * p.y * p.d;		_43 = -2 * p.z * p.d;		_44 = 1;

		return *this;
	}

	const Mtx& Mtx::ortho(float w, float h, float zn, float zf)
	{
		identity();
		_11 = 2.0f / w;
		_22 = 2.0f / h;
		_33 = 1.0f / (zf - zn);
		_43 = zn / (zn - zf);
		_44 = 1.0f;

		return *this;
	}

	const Mtx& Mtx::perspective(float w, float h, float zn, float zf)
	{
		SSH_MEMZERO(this, sizeof(Mtx));

		_11 = 2.0f * zn / w;
		_22 = 2.0f * zn / h;
		_33 = zf / (zf - zn);
		_34 = 1.0f;
		_43 = zn * zf / (zn - zf);

		return *this;
	}
	
	const Mtx& Mtx::perspectiveFov(float fovy, float aspect, float zn, float zf)
	{
		float h = (cosf(fovy / 2.0f) / sinf(fovy / 2.0f));
		float w = (h / aspect);
		float f = zf / (zf - zn);

		SSH_MEMZERO(this, sizeof(Mtx));
		
		_11 = w;
		_22 = h;
		_33 = f;
		_34 = 1.0f;
		_43 = -zn * f;
		
		return *this;
	}

	const Quat& Quat::operator *= (const Quat& q)
	{
		float xx(w * q.x + x * q.w + y * q.z - z * q.y);
		float yy(w * q.y + y * q.w + z * q.x - x * q.z);
		float zz(w * q.z + z * q.w + x * q.y - y * q.x);
		float ww(w * q.w - x * q.x - y * q.y - z * q.z);
		x = xx; y = yy; z = zz; w = ww;
		return *this;
	}

	Quat Quat::operator * (const Quat& q) const
	{
		return Quat(w * q.x + x * q.w + y * q.z - z * q.y,
					w * q.y + y * q.w + z * q.x - x * q.z,
					w * q.z + z * q.w + x * q.y - y * q.x,
					w * q.w - x * q.x - y * q.y - z * q.z);
	}

	void Quat::angleAxis(Vec3& axis, float& angle) const
	{
		float fSqrLength(x * x + y * y + z * z);

		if(fSqrLength > 0)
		{
			angle = 2 * acos(w);
			float fInvLength = sqrt(fSqrLength);

			axis.x = x / fInvLength;
			axis.y = y / fInvLength;
			axis.z = z / fInvLength;
		}
		else
		{
			angle = 0;
			axis.x = 1;
			axis.y = 0;
			axis.z = 0;
		}
	}
	Quat Quat::slerp(const Quat& q, float t, bool shortestPath) const
	{
		float fCos(dot(q));
		float fAngle(acos(fCos));

		if(fabs(fAngle) < SSH_EPSILON) return *this;
		float fSin(sinf(fAngle));
		float fInvSin(1.0f / fSin);
		float fCoeff0(sin((1.0f - t) * fAngle) * fInvSin);
		float fCoeff1(sin(t * fAngle) * fInvSin);

		if(fCos < 0 && shortestPath)
		{
			fCoeff0 =- fCoeff0;
			Quat res(*this * fCoeff0 + q * fCoeff1);
			res.normalize();
			return res;
		}
		return *this * fCoeff0 + q * fCoeff1;
	}

	const Quat& Quat::rotateAxis(const Vec3& v, float theta)
	{
		float fHalfAngle(theta / 2.0f);
		float fSin(sinf(fHalfAngle));

		w = cosf(fHalfAngle);
		x = fSin * v.x;
		y = fSin * v.y;
		z = fSin * v.z;

		return *this;
	}

	//Преобразование сферических координат в кватернион
	const Quat& Quat::fromSpherical(float latitude, float longitude, float angle)
	{
		float sin_a(sin(angle / 2.0f));
		float cos_a(cos(angle / 2.0f));
		float sin_lat(sin(latitude));
		float cos_lat(cos(latitude));
		float sin_long(sin(longitude));
		float cos_long(cos(longitude));

		x = sin_a * cos_lat * sin_long;
		y = sin_a * sin_lat;
		z = sin_a * sin_lat * cos_long;
		w = cos_a;

		return *this;
	}

	const Quat& Quat::fromMatrix(const Mtx& mm)
	{
		float* f(mm);
		float tr(f[0] + f[5] + f[10]), s;

		if(tr > 0.0f)
		{
			s = sqrt (tr + 1.0f);
			w = s / 2.0f;
			s = 0.5f / s;
			x = (f[6] - f[9]) * s;
			y = (f[8] - f[2]) * s;
			z = (f[1] - f[4]) * s;
		}
		else
		{
			int nxt[3] = {1, 2, 0};
			int i(0);
			float q[4];

			if(f[5] > f[0]) i = 1;
			if(f[10] > f[i * 4 + i]) i = 2;

			int j(nxt[i]);
			int k(nxt[j]);
			s = sqrt((f[i * 4 + i] - (f[j * 4 + j] + f[k * 4 + k])) + 1.0f);

			q[i] = s * 0.5f;

			if(s > SSH_EPSILON) s = 0.5f / s;

			q[3] = (f[j * 4 + k] - f[k * 4 + j]) * s;
			q[j] = (f[i * 4 + j] + f[j * 4 + i]) * s;
			q[k] = (f[i * 4 + k] + f[k * 4 + i]) * s;

			x = q[0];
			y = q[1];
			z = q[2];
			w = q[3];
		}

		return *this;
	}
	Vec3 Quat::xAxis() const
	{
		float fTy(2 * y);
		float fTz(2 * z);
		return Vec3(1 - ((fTy * y) + (fTz * z)), (fTy * x) + (fTz * w), (fTz * x) - (fTy * w));
	}
	Vec3 Quat::yAxis() const
	{
		float fTx(2 * x);
		float fTy(2 * y);
		float fTz(2 * z);
		return Vec3((fTy * x) - (fTz * w), 1 - ((fTx * x) + (fTz * z)), (fTz * y) + (fTx * w));
	}
	Vec3 Quat::zAxis() const
	{
		float fTx(2.0f * x);
		float fTy(2.0f * y);
		float fTz(2.0f * z);
		return Vec3((fTz * x) + (fTy * w), (fTz * y) - (fTx * w), 1 - ((fTx * x) + (fTy * y)));
	}
	const Quat& Quat::inverse()
	{
		float f(x * x + y * y + z * z + w * w);
		if(f <= SSH_EPSILON)
		{
			x = y = z = 0.0f; w = 1.0f;
		}
		else
		{
			f = 1.0f / f;
			x *= -f; y *= -f; z *= -f; w *= f;
		}

		return *this;
	}
	const Quat& Quat::exp()
	{
		float fAngle(sqrt(x * x + y * y + z * z));
		float fSin(sin(fAngle));

		if(fabs(fSin) >= SSH_EPSILON)
		{
			float fCoeff(fSin / fAngle);
			x = fCoeff * x; y = fCoeff * y; z = fCoeff * z;
		}
		w = cos(fAngle);
		return *this;
	}

	const Quat& Quat::ln()
	{
		if(fabs(w) < 1.0f)
		{
			float fAngle(acos(w));
			float fSin(sin(fAngle));

			if(fabs(fSin) >= SSH_EPSILON)
			{
				float fCoeff(fAngle / fSin);
				x = fCoeff * x; y = fCoeff * y; z = fCoeff * z;
			}
		}
		w = 0.0f;
		return *this;
	}

	const Quat& Quat::angles(float yaw, float pitch, float roll)
	{
		float fSinYaw(sin(yaw / 2.0f));
		float fSinPitch(sin(pitch / 2.0f));
		float fSinRoll(sin(roll / 2.0f));
		float fCosYaw(cos(yaw / 2.0f));
		float fCosPitch(cos(pitch / 2.0f));
		float fCosRoll(cos(roll / 2.0f));

		x = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
		y = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
		z = fSinRoll * fCosPitch * fCosYaw - fCosRoll * fSinPitch * fSinYaw;
		w = fCosRoll * fCosPitch * fCosYaw + fSinRoll * fSinPitch * fSinYaw;

		return *this;
	}

	Quat Quat::nlerp(const Quat& q, float t, bool shortestPath) const
	{
		float fCos(dot(q));
		Quat result((fCos < 0 && shortestPath) ? *this + t * ((-q) - *this) : *this + t * (q - *this));
		return result.normalize();
	}
	
	Quat Quat::squad(const Quat& q1, const Quat& q2, const Quat& q3, float t, bool shortestPath) const
	{
		float slerpT(2.0f * t * (1.0f - t));
		Quat slerpP(slerp(q3, t, shortestPath));
		Quat slerpQ(q1.slerp(q2, t, false));
		return slerpP.slerp(slerpQ, slerpT, shortestPath);
	}

	Plane::Plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
	{
		Vec3 edge1(v2 - v1);

		normal = edge1.cross(v3 - v1);
		normal.normalize();
		d = -normal.dot(v1);
	}

	Plane Plane::operator * (const Mtx& m) const
	{
		Mtx inv(m);
		Plane pp;

		inv.inverse();
		inv.transpose();
		pp.normal *= inv;
		Vec3 pt(pp.normal);

		pt *= -d;
		pt *= m;

		pp.d = -pt.dot(pp.normal);

		return pp;
	}

	Plane::Side Plane::side(const Vec3& v) const
	{
		float fDistance(distance(v));
		if(fDistance < 0) return Plane::NEGATIVE_SIDE;
		if(fDistance > 0) return Plane::POSITIVE_SIDE;
		return Plane::NO_SIDE;
	}

	Plane operator * (const Mtx& mm, const Plane& p)
	{
		Plane pp;
		Mtx inv(mm);
		Vec3 pt(p.normal);

		inv.inverse();
		inv.transpose();

		float* f(inv);

		pp.x = f[0] * p.x + f[1] * p.y + f[2] * p.z;
		pp.y = f[4] * p.x + f[5] * p.y + f[6] * p.z;
		pp.z = f[8] * p.x + f[9] * p.y + f[10] * p.z;
		pt *= -p.d;
		pt *= mm;
		pp.d = -pt.dot(pp.normal);
		return pp;
	}

	void Color::HSB(float hue, float saturation, float brightness)
	{
		if(hue > 1) hue -= (int)hue;
		else if(hue < 0) hue += (int)(hue + 1);

		saturation = min(saturation, 1.0f);
		saturation = max(saturation, 0.0f);
		brightness = min(brightness, 1.0f);
		brightness = max(brightness, 0.0f);

		if(brightness < SSH_EPSILON)
		{
			r = g = b = 0;
			return;
		}
		if(saturation < SSH_EPSILON)
		{
			r = g = b = brightness;
			return;
		}

		float hueDomain(hue * 6);
		if(hueDomain >= 6) hueDomain = 0;

		unsigned short domain = (unsigned short)hueDomain;

		float f1(brightness * (1 - saturation));
		float f2(brightness * (1 - saturation * (hueDomain - domain)));
		float f3(brightness * (1 - saturation * (1 - (hueDomain - domain))));

		switch(domain)
		{
			case 0: r = brightness; g = f3; b = f1; break;
			case 1: r = f2; g = brightness; b = f1; break;
			case 2: r = f1; g = brightness; b = f3; break;
			case 3: r = f1; g = f2; b = brightness; break;
			case 4: r = f3; g = f1; b = brightness; break;
			case 5: r = brightness; g = f1; b = f2; break;
		}
	}

	void Color::RGBA(long val)
	{
		long val32(val);

		a = (ssh_b)(val32 >> 24) / 255.0f;
		b = (ssh_b)(val32 >> 16) / 255.0f;
		g = (ssh_b)(val32 >> 8) / 255.0f;
		r = (ssh_b)(val32) / 255.0f;
	}

	void Color::BGRA(long val)
	{
		long val32(val);

		a = (ssh_b)(val32 >> 24) / 255.0f;
		r = (ssh_b)(val32 >> 16) / 255.0f;
		g = (ssh_b)(val32 >> 8) / 255.0f;
		b = (ssh_b)(val32) / 255.0f;
	}

	const Color& Color::saturate()
	{
		r = SSH_CLAMP(r, 0.0f, 1.0f);
		b = SSH_CLAMP(b, 0.0f, 1.0f);
		g = SSH_CLAMP(g, 0.0f, 1.0f);
		a = SSH_CLAMP(a, 0.0f, 1.0f);
		return *this;
	}
	
	long Color::RGBA() const
	{
		ssh_b val8;
		long val32;

		val8 = (ssh_b)(a * 255); val32 = val8 << 24;
		val8 = (ssh_b)(b * 255); val32 |= val8 << 16;
		val8 = (ssh_b)(g * 255); val32 |= val8 << 8;
		val8 = (ssh_b)(r * 255); val32 |= val8;

		return val32;
	}
	
	long Color::BGRA() const
	{
		ssh_b val8;
		long val32;

		val8 = (ssh_b)(a * 255); val32 = val8 << 24;
		val8 = (ssh_b)(r * 255); val32 |= val8 << 16;
		val8 = (ssh_b)(g * 255); val32 |= val8 << 8;
		val8 = (ssh_b)(b * 255); val32 |= val8;

		return val32;
	}

	Sphere Sphere::operator + (const Sphere& s) const
	{
		Vec3 v;
		float f;
		contactSphere(s, v, f);
		return Sphere(v, f);
	}

	void Sphere::contactSphere(const Sphere& s, Vec3& v, float& f) const
	{
		Vec3 mn1(c - r), mx1(c + r);
		Vec3 mn2(s.c - s.r), mx2(s.c + s.r);

		if(mn2 < mn1) mn1 = mn2;
		if(mx2 > mx1) mx1 = mx2;

		v = Vec3((mx1 + mn1) / 2.0f);
		mn1 = (mx1 - mn1) / 2.0f;

		f = max(mn1.z, max(mn1.x, mn1.y));
	}

	bool Bbox::intersects(const Bbox& b2) const
	{
		return ((mx.x >= b2.mn.x) && (mx.y >= b2.mn.y) && (mx.z >= b2.mn.z) && (mn.x <= b2.mx.x) && (mn.y <= b2.mx.y) && (mn.z > b2.mx.z));
	}

	bool Bbox::intersects(const Sphere& s) const
	{
		const Vec3& center(s.center());
		float radius(s.radius());

		return ((center.x >= mn.x && (mn.x - center.x) <= radius) &&
			(center.x <= mx.x && (center.x - mx.x) <= radius) &&
			(center.y >= mn.y && (mn.y - center.y) <= radius) &&
			(center.y <= mx.y && (center.y - mx.y) <= radius) &&
			(center.z >= mn.z && (mn.z - center.z) <= radius) &&
			(center.z <= mx.z && (center.z - mx.z) <= radius));
	}

	Bbox Bbox::intersection(const Bbox& b2) const
	{
		if(!intersects(b2)) return Bbox();

		Vec3 intMin, intMax;
		const Vec3& b2max(b2.maximum());
		const Vec3& b2min(b2.minimum());

		if(b2max.x > mx.x && mx.x > b2min.x) intMax.x = mx.x; else intMax.x = b2max.x;
		if(b2max.y > mx.y && mx.y > b2min.y) intMax.y = mx.y; else intMax.y = b2max.y;
		if(b2max.z > mx.z && mx.z > b2min.z) intMax.z = mx.z; else intMax.z = b2max.z;
		if(b2min.x < mn.x && mn.x < b2max.x) intMin.x = mn.x; else intMin.x = b2min.x;
		if(b2min.y < mn.y && mn.y < b2max.y) intMin.y = mn.y; else intMin.y = b2min.y;
		if(b2min.z < mn.z && mn.z < b2max.z) intMin.z = mn.z; else intMin.z = b2min.z;

		return Bbox(intMin, intMax);
	}
	
	void Bbox::transform(const Mtx& matrix)
	{
		Vec3 min, max, temp;

		bool first = true;

		for(int i = 0 ; i < 8 ; ++i)
		{
			temp = matrix * corners[i];
			if(first || temp.x > max.x) max.x = temp.x;
			if(first || temp.y > max.y) max.y = temp.y;
			if(first || temp.z > max.z) max.z = temp.z;
			if(first || temp.x < min.x) min.x = temp.x;
			if(first || temp.y < min.y) min.y = temp.y;
			if(first || temp.z < min.z) min.z = temp.z;

			first = false;
		}
		setExtents(min, max);
	}
	
	void Bbox::merge(const Bbox& b2)
	{
		Vec3 mnn(mn), mxx(mx);
		mxx.ceil(b2.mx);
		mnn.floor(b2.mn);
		setExtents(mnn, mxx);
	}

	void Bbox::updateCorners()
	{
		corners[0] = mn;
		corners[4] = mx;
		corners[1].x = mn.x ; corners[1].y = mx.y ; corners[1].z = mn.z;
		corners[2].x = mx.x ; corners[2].y = mx.y ; corners[2].z = mn.z;
		corners[3].x = mx.x ; corners[3].y = mn.y ; corners[3].z = mn.z;
		corners[5].x = mn.x ; corners[5].y = mx.y ; corners[5].z = mx.z;
		corners[6].x = mn.x ; corners[6].y = mn.y ; corners[6].z = mx.z;
		corners[7].x = mx.x ; corners[7].y = mn.y ; corners[7].z = mx.z;
	}

	Obox::Obox(const Vec3& _x1y1z1, const Vec3& _x2y1z1, const Vec3& _x1y2z1, const Vec3& _x2y2z1, const Vec3& _x1y1z2, const Vec3& _x2y1z2,
			   const Vec3& _x1y2z2, const Vec3& _x2y2z2)
	{
		corners[X1Y1Z1] = _x1y1z1;
		corners[X2Y1Z1] = _x2y1z1;
		corners[X1Y2Z1] = _x1y2z1;
		corners[X2Y2Z1] = _x2y2z1;
		corners[X1Y1Z2] = _x1y1z2;
		corners[X2Y1Z2] = _x2y1z2;
		corners[X1Y2Z2] = _x1y2z2;
		corners[X2Y2Z2] = _x2y2z2;
	}
	Obox::Obox(const Bbox& Bbox)
	{
		Vec3 mn(Bbox.minimum());
		Vec3 mx(Bbox.maximum());

		corners[X1Y1Z1] = mn;
		corners[X2Y1Z1] = Vec3(mx.x, mn.y, mn.z);
		corners[X1Y2Z1] = Vec3(mn.x, mx.y, mn.z);
		corners[X2Y2Z1] = Vec3(mx.x, mx.y, mn.z);
		corners[X1Y1Z2] = Vec3(mn.x, mn.y, mx.z);
		corners[X2Y1Z2] = Vec3(mx.x, mn.y, mx.z);
		corners[X1Y2Z2] = Vec3(mn.x, mx.y, mx.z);
		corners[X2Y2Z2] = mx;
	}
	Obox::Obox(float* b)
	{
		corners[X1Y1Z1] = Vec3(b[0]);
		corners[X2Y1Z1] = Vec3(b[3]);
		corners[X1Y2Z1] = Vec3(b[6]);
		corners[X2Y2Z1] = Vec3(b[9]);
		corners[X1Y1Z2] = Vec3(b[12]);
		corners[X2Y1Z2] = Vec3(b[15]);
		corners[X1Y2Z2] = Vec3(b[18]);
		corners[X2Y2Z2] = Vec3(b[21]);
	}

	Obox Obox::transform(const Mtx& m) const
	{
		return Obox(corners[X1Y1Z1] * m, corners[X2Y1Z1] * m, corners[X1Y2Z1] * m, corners[X2Y2Z1] * m,
					corners[X1Y1Z2] * m, corners[X2Y1Z2] * m, corners[X1Y2Z2] * m, corners[X2Y2Z2] * m);
	}
	
	bool Obox::intersects(const Bbox& b) const
	{
		return false;
	}
	
	bool Obox::intersects(const Obox& b) const
	{
		return false;
	}
	
	bool Obox::intersects(const Sphere& s) const
	{
		return false;
	}
	
	bool Obox::intersects(const Vec3& v) const
	{
		return false;
	}
	
	Vec3 Obox::center() const
	{
		Vec3 _1(corners[X1Y2Z1] + (corners[X2Y1Z2] - corners[X1Y2Z1]) / 2.0f);
		Vec3 _2(corners[X2Y2Z1] + (corners[X1Y1Z2] - corners[X2Y2Z1]) / 2.0f);
		Vec3 _3(corners[X2Y1Z1] + (corners[X1Y2Z2] - corners[X2Y1Z1]) / 2.0f);
		return Vec3((_1 + _2 + _3) / 3.0f);
	}

	bool Ray::intersects(const Sphere& s, float* f) const
	{
		const Vec3& rayorig(pos - s.center());
		float radius(s.radius());

		if(f) *f = 0.0f;
		if(rayorig.lengthSq() <= (radius * radius))
			return true;
		float a(dir.dot(dir));
		float b(2.0f * rayorig.dot(dir));
		float c(rayorig.dot(rayorig) - radius * radius);
		float d((b * b) - (4.0f * a * c));
		if(d < SSH_EPSILON) return false;
		if(f)
		{
			*f = (-b - sqrtf(d)) / (2.0f * a);
			if(*f < 0.0f) *f = (-b + sqrtf(d)) / (2.0f * a);
		}

		return true;
	}
	
	bool Ray::intersects(const Bbox& box, float* f) const
	{
		Vec3 hitpoint;
		const Vec3& mnn(box.minimum());
		const Vec3& mxx(box.maximum());

		float lowt = 0.0f, t;
		bool hit = false;

		if(f) *f = 0.0f;
		
		if(pos > mnn && pos < mxx)
			return true;
		if(pos.x < mnn.x && dir.x > 0)
		{
			t = (mnn.x - pos.x) / dir.x;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.y >= mnn.y && hitpoint.y <= mxx.y && hitpoint.z >= mnn.z && hitpoint.z <= mxx.z && t < lowt))
					lowt = t;
			}
		}
		if(pos.x > mxx.x && dir.x < 0 && !hit)
		{
			t = (mxx.x - pos.x) / dir.x;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.y >= mnn.y && hitpoint.y <= mxx.y && hitpoint.z >= mnn.z && hitpoint.z <= mxx.z && t < lowt))
					lowt = t;
			}
		}
		if(pos.y < mnn.y && dir.y > 0 && !hit)
		{
			t = (mnn.y - pos.y) / dir.y;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.x >= mnn.x && hitpoint.x <= mxx.x && hitpoint.z >= mnn.z && hitpoint.z <= mxx.z && t < lowt))
					lowt = t;
			}
		}
		if(pos.y > mxx.y && dir.y < 0 && !hit)
		{
			t = (mxx.y - pos.y) / dir.y;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.x >= mnn.x && hitpoint.x <= mxx.x && hitpoint.z >= mnn.z && hitpoint.z <= mxx.z && t < lowt))
					lowt = t;
			}
		}
		if(pos.z < mnn.z && dir.z > 0 && !hit)
		{
			t = (mnn.z - pos.z) / dir.z;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.x >= mnn.x && hitpoint.x <= mxx.x && hitpoint.y >= mnn.y && hitpoint.y <= mxx.y && t < lowt))
					lowt = t;
			}
		}
		if(pos.z > mxx.z && dir.z < 0 && !hit)
		{
			t = (mxx.z - pos.z) / dir.z;
			if(t > 0)
			{
				hitpoint = pos + dir * t;
				if((hit = hitpoint.x >= mnn.x && hitpoint.x <= mxx.x && hitpoint.y >= mnn.y && hitpoint.y <= mxx.y && t < lowt))
					lowt = t;
			}
		}
		if(f) *f = lowt;

		return hit;
	}
	
	bool Ray::intersects(const Plane& p, float* f) const
	{
		Vec3 v((float*)p);

		float denom(v.dot(dir));

		if(fabs(denom) < SSH_EPSILON)
			return false;

		float nom(v.dot(pos) + p.d);
		float t(-(nom / denom));

		if(f) *f = t;
		return (t >= 0);
	}

	const Mtx& Mtx::from3dsMax(float* f)
	{
		_11 = f[0] ; _12 = f[2] ; _13 = f[1] ; _14 = 0.0f;
		_21 = f[3] ; _22 = f[4] ; _23 = f[5] ; _24 = 0.0f;
		_31 = f[6] ; _32 = f[7] ; _33 = f[8] ; _34 = 0.0f;
		_41 = 0.0f ; _42 = 0.0f ; _43 = 0.0f ; _44 = 1.0f;

		return *this;
	}
}
