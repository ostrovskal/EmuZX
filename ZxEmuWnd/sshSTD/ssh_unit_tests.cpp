
#include "stdafx.h"

#include "ssh_xml.h"
#include "ssh_io.h"
#include "ssh_regx.h"
#include "ssh_srlz.h"
#include "ssh_math.h"

namespace ssh
{
	void SSH ssh_unit_test()
	{
		Time::unit_test();
		String::unit_test();
		Xml::unit_test();
		Serialize::unit_test();
		Regx::unit_test();
		Mouse::unit_test();
		Keyboard::unit_test();
		Gamepad::unit_test();
		File::unit_test();

		_Buffer<ssh_b>::unit_test();
		Pts<>::unit_test();
		Range<>::unit_test();
		Box<>::unit_test();
		Bar<>::unit_test();

		Bits::unit_test();
		Vec2::unit_test();
		Vec3::unit_test();
		Vec4::unit_test();
		Mtx::unit_test();
		Quat::unit_test();
		Dual_quat::unit_test();
		Plane::unit_test();
		Color::unit_test();
		Sphere::unit_test();
		Angle::unit_test();
		Ray::unit_test();
		Half::unit_test();
		Bbox::unit_test();
		Obox::unit_test();
	}
	
	void Time::unit_test()
	{
		Time test1;
		Time test2;
		Time test3;
		Time test4;
		Time test5;
	}

	void String::unit_test()
	{
		String test1;
		String test2;
		String test3;
		String test4;
		String test5;
	}

	void Xml::unit_test()
	{
		Xml test1;
	}

	void Serialize::unit_test()
	{

	}
	
	void Regx::unit_test()
	{
		Regx test1;
	}
	
	void Mouse::unit_test()
	{
		Mouse test1;
	}
	
	void Keyboard::unit_test()
	{
		Keyboard test1;
	}
	
	void Gamepad::unit_test()
	{

	}
	
	void File::unit_test()
	{
		File test1;
		File test2;
		File test3;
	}

	void Bits::unit_test()
	{
		Bits test1;
		Bits test2;
		Bits test3;
		Bits test4;
	}

	void Vec2::unit_test()
	{
		Vec2 test1;
		Vec2 test2;
		Vec2 test3;
		Vec2 test4;
		Vec2 test5;
	}

	void Vec3::unit_test()
	{
		Vec3 test1;
		Vec3 test2;
		Vec3 test3;
		Vec3 test4;
		Vec3 test5;
	}

	void Vec4::unit_test()
	{
		Vec4 test1;
		Vec4 test2;
		Vec4 test3;
		Vec4 test4;
		Vec4 test5;
	}

	void Mtx::unit_test()
	{
		Mtx test1;
		Mtx test2;
		Mtx test3;
		Mtx test4;
		Mtx test5;
	}

	void Quat::unit_test()
	{
		Quat test1;
		Quat test2;
		Quat test3;
		Quat test4;
		Quat test5;
	}

	void Dual_quat::unit_test()
	{
		Dual_quat test1;
		Dual_quat test2;
		Dual_quat test3;
		Dual_quat test4;
		Dual_quat test5;
	}

	void Plane::unit_test()
	{
		Plane test1;
		Plane test2;
		Plane test3;
		Plane test4;
		Plane test5;
	}

	void Color::unit_test()
	{
		Color test1;
		Color test2;
		Color test3;
		Color test4;
		Color test5;
	}

	void Sphere::unit_test()
	{
		Sphere test1;
		Sphere test2;
		Sphere test3;
		Sphere test4;
		Sphere test5;
	}

	void Angle::unit_test()
	{
		Angle test1;
	}

	void Ray::unit_test()
	{
		Ray test1;
		Ray test2;
		Ray test3;
		Ray test4;
		Ray test5;
	}

	void Half::unit_test()
	{
		Half test1;
		Half test2;
		Half test3;
	}

	void Bbox::unit_test()
	{
		Bbox test1;
		Bbox test2;
		Bbox test3;
		Bbox test4;
		Bbox test5;
	}

	void Obox::unit_test()
	{
		Obox test1;
		Obox test2;
		Obox test3;
		Obox test4;
		Obox test5;
	}
}
