#include "MXEZ/Vec/VecTransformer.h"

namespace MXEZ
{

#define PRECISION_ROTATION 100

	CosTable VecTransformer::_cosTable = CosTable(PRECISION_ROTATION);
	SinTable VecTransformer::_sinTable = SinTable(PRECISION_ROTATION);

// #include <iostream> // TMP

	void VecTransformer::Rotate(Vec2& vec, const float& angle)
	{
		if (angle < 0)
		{
			Rotate(vec, (360.f + angle));
			return;
		}
		unsigned int iangle = GetScaledAngle(angle);
		Vec2 tmp;
		tmp.x = vec.x;
		tmp.y = vec.y;
		vec.x = tmp.x * _cosTable.GetValue(iangle) - tmp.y * _sinTable.GetValue(iangle);
		vec.y = tmp.x * _sinTable.GetValue(iangle) + tmp.y * _cosTable.GetValue(iangle);
	}

	void VecTransformer::RotateInv(Vec2& vec, const float& angle)
	{
		unsigned int iangle = GetScaledAngle(angle);
		Vec2 tmp;
		tmp.x = vec.x;
		tmp.y = vec.y;
		vec.x = tmp.x * _cosTable.GetValue(iangle) + tmp.y * _sinTable.GetValue(iangle);
		vec.y = -tmp.x * _sinTable.GetValue(iangle) + tmp.y * _cosTable.GetValue(iangle);
	}

	void VecTransformer::RotateXYZ(Vec3& vec, const float& ax, const float& ay, const float& az)
	{
		if (ax != 0)
			RotateX(vec, ax);
		if (ay != 0)
			RotateY(vec, ay);
		if (az != 0)
			RotateZ(vec, az);
	}

	void VecTransformer::RotateX(Vec3& vec, const float& angle)
	{
		unsigned int iangle = GetScaledAngle(angle);
		float tmpy = vec.y;
		vec.y = tmpy * _cosTable.GetValue(iangle) - vec.z * _sinTable.GetValue(iangle);
		vec.z = tmpy * _sinTable.GetValue(iangle) + vec.z * _cosTable.GetValue(iangle);
	}

	void VecTransformer::RotateY(Vec3& vec, const float& angle)
	{
		unsigned int iangle = GetScaledAngle(angle);
		float tmpz = vec.z;
		vec.z = -vec.x * _sinTable.GetValue(iangle) + tmpz * _cosTable.GetValue(iangle);
		vec.x = vec.x * _cosTable.GetValue(iangle) + tmpz * _sinTable.GetValue(iangle);
	}

	void VecTransformer::RotateZ(Vec3& vec, const float& angle)
	{
		unsigned int iangle = GetScaledAngle(angle);
		float tmpx = vec.x;
		vec.x = tmpx * _cosTable.GetValue(iangle) - vec.y * _sinTable.GetValue(iangle);
		vec.y = tmpx * _sinTable.GetValue(iangle) + vec.y * _cosTable.GetValue(iangle);
	}

	void VecTransformer::RotateInvXYZ(Vec3& vec, const float& ax, const float& ay, const float& az)
	{
	}

	void VecTransformer::RotateInvX(Vec3& vec, const float& angle)
	{
	}

	void VecTransformer::RotateInvY(Vec3& vec, const float& angle)
	{
	}

	void VecTransformer::RotateInvZ(Vec3& vec, const float& angle)
	{
	}

	float VecTransformer::Distance(const Vec2& vecA, const Vec2& vecB)
	{
		static Vec2 bVec2;
		bVec2 = vecB;
		bVec2 -= vecA;
		return (ValueTools::FastSqrt((bVec2.x * bVec2.x) + (bVec2.y * bVec2.y)));
	}

	float VecTransformer::Length(const Vec2& vec)
	{
		return (ValueTools::FastSqrt((vec.x * vec.x) + (vec.y * vec.y)));
	}

	float VecTransformer::LengthSquare(const Vec2& vec)
	{
		return ((vec.x * vec.x) + (vec.y * vec.y));
	}

	void VecTransformer::Normalize(Vec2& vec)
	{
		float       coef = ValueTools::FastInvSqrt((vec.x * vec.x) + (vec.y * vec.y));

		vec.x *= coef;
		vec.y *= coef;
	}

	void VecTransformer::Normalize(Vec3& vec)
	{
		float       coef = ValueTools::FastInvSqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

		vec.x *= coef;
		vec.y *= coef;
		vec.z *= coef;
	}

	void VecTransformer::Normalize(const Vec2& vec, Vec2& res)
	{
		float       coef = ValueTools::FastInvSqrt((vec.x * vec.x) + (vec.y * vec.y));

		res.x = vec.x * coef;
		res.y = vec.y * coef;
	}

	void VecTransformer::Normalize(const Vec3& vec, Vec3& res)
	{
		float       coef = ValueTools::FastInvSqrt((vec.x * vec.x) + (vec.y * vec.y) + (vec.z * vec.z));

		res.x = vec.x * coef;
		res.y = vec.y * coef;
		res.z = vec.z * coef;
	}

	bool VecTransformer::AreEqual(const Vec2& a, const Vec2& b, const float& maxEpsilon)
	{
		float diff = fabs(a.x - b.x);
		diff += fabs(a.y - b.y);
		return (diff < maxEpsilon);
	}

	float VecTransformer::GetCos(const float& angle)
	{
		return (_cosTable.GetValue(GetScaledAngle(angle)));
	}

	float VecTransformer::GetSin(const float& angle)
	{
		return (_sinTable.GetValue(GetScaledAngle(angle)));
	}

	unsigned int VecTransformer::GetScaledAngle(const float& angle)
	{
		float cangle = angle;
		if (angle < 0)
		{
			unsigned int uangle = (unsigned int)-angle;
			float cuangle = (float)(uangle / 360);
			float rest_angle = (angle + (360.0f * cuangle));
			cangle = (360.0f + rest_angle);
		}
		else if (angle >= 360)
		{
			unsigned int uangle = (unsigned int)angle;
			float cuangle = (float)(uangle / 360);
			float rest_angle = (angle - (360.0f * cuangle));
			cangle = rest_angle;
		}
		unsigned int sAngle = (unsigned int)(cangle * (float)_cosTable.GetPrecision());

		return (sAngle);
	}
}