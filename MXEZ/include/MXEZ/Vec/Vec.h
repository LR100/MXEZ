#ifndef VEC_H_
#define VEC_H_

#include <sstream>
#include <string>

namespace MXEZ
{

#pragma pack(1)
	template <typename T>
	struct Vec2T
	{
		T			x, y;

		Vec2T(T _x, T _y)
		{
			x = _x;
			y = _y;
		}
		Vec2T()
		{
			x = 0;
			y = 0;
		}

		virtual ~Vec2T() {};

		std::string	ToString() const
		{
			std::stringstream ss;
			ss << "X (" << x << ") Y (" << y << ")";
			return (ss.str());
		}

		Vec2T<T>	operator-() const
		{
			return Vec2T<T>(-x, -y);
		}

		Vec2T<T>& operator-=(const Vec2T<T>& other)
		{
			x -= other.x;
			y -= other.y;
			return (*this);
		}

		Vec2T<T>& operator-=(const T& value)
		{
			x -= value;
			y -= value;
			return (*this);
		}

		Vec2T<T>& operator+=(const Vec2T<T>& other)
		{
			x += other.x;
			y += other.y;
			return (*this);
		}

		Vec2T<T>& operator+=(const T& value)
		{
			x += value;
			y += value;
			return (*this);
		}

		Vec2T<T>& operator*=(const T& value)
		{
			x *= value;
			y *= value;
			return (*this);
		}

		Vec2T<T>& operator/=(const T& value)
		{
			x /= value;
			y /= value;
			return (*this);
		}

		// CONST Op
		bool		operator==(const Vec2T<T>& other) const
		{
			return (x == other.x && y == other.y);
		}

		Vec2T<T>	operator-(const Vec2T& other) const
		{
			return Vec2T<T>(x - other.x, y - other.y);
		}

		Vec2T<T>	operator+(const Vec2T<T>& other) const
		{
			return Vec2T<T>(x + other.x, y + other.y);
		}

		Vec2T<T>	operator*(const T& value) const
		{
			return Vec2T<T>(x * value, y * value);
		}

		Vec2T<T>	operator*(const Vec2T& other) const
		{
			return Vec2T<T>(x * other.x, y * other.y);
		}


		Vec2T<T>	operator/(const T& value) const
		{
			return Vec2T<T>(x / value, y / value);
		}


		T			Dot(const Vec2T<T>& other) const
		{
			return T((x * other.x) + (y * other.y));
		}

		T			Cross(const Vec2T<T>& other) const
		{
			return T((x * other.y) - (y * other.x));
		}

		void		Zero()
		{
			x = 0;
			y = 0;
		}




	};
#pragma pack()

	// Float 
	template struct Vec2T<float>;
	typedef Vec2T<float> Vec2;
	// Int
	template struct Vec2T<int>;
	typedef Vec2T<int> Vec2i;

	// UInt8
	template struct Vec2T<uint8_t>;
	typedef Vec2T<uint8_t> Vec2ui8;


#pragma pack(1)
	template <typename T>
	struct Vec3T
	{
		Vec3T(T _x, T _y, T _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		Vec3T()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		virtual ~Vec3T() {};

		std::string	ToString() const
		{
			std::stringstream ss;
			ss << "X (" << x << ") Y (" << y << ") Z (" << z << ")";
			return (ss.str());
		}

		Vec3T<T>	operator-() const
		{
			return Vec3T<T>(-x, -y, -z);
		}

		Vec3T<T>& operator-=(const Vec3T<T>& other)
		{
			x -= other.x;
			y -= other.y;
			z -= other.z;
			return (*this);
		}
		Vec3T<T>& operator+=(const Vec3T<T>& other)
		{
			x += other.x;
			y += other.y;
			z += other.z;
			return (*this);
		}

		Vec3T<T>& operator*=(const T& value)
		{
			x *= value;
			y *= value;
			z *= value;
			return (*this);
		}
		Vec3T<T>& operator/=(const T& value)
		{
			x /= value;
			y /= value;
			z /= value;
			return (*this);
		}

		// CONST Op
		bool		operator==(const Vec3T<T>& other) const
		{
			return (x == other.x && y == other.y && z == other.z);
		}

		Vec3T<T>	operator-(const Vec3T& other) const
		{
			return Vec3T<T>(x - other.x, y - other.y, z - other.z);
		}
		Vec3T<T>	operator+(const Vec3T<T>& other) const
		{
			return Vec3T<T>(x + other.x, y + other.y, z + other.z);
		}

		Vec3T<T>	operator*(const T& value) const
		{
			return Vec3T<T>(x * value, y * value, z * value);
		}
		Vec3T<T>	operator/(const T& value) const
		{
			return Vec3T<T>(x / value, y / value, z / value);
		}

		T			Dot(const Vec3T<T>& other) const
		{
			return T((x * other.x) + (y * other.y) + (z * other.z));
		}
		void		Zero()
		{
			x = 0;
			y = 0;
			z = 0;
		}

		T	x, y, z;
	};
#pragma pack()

	// Float 
	template struct Vec3T<float>;
	typedef Vec3T<float> Vec3;
	// Int
	template struct Vec3T<int>;
	typedef Vec3T<int> Vec3i;

}

#endif // VEC_H_