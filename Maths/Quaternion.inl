#pragma once
#include "Quaternion.h"
#include "Vector3.h"

template<typename T>
Quaternion<T>::Quaternion()
{
	w = 1;
	x = y = z = 0;
}

template<typename T>
Quaternion<T>::Quaternion(const Quaternion<T>& q)
{
	x = q.x;
	y = q.y;
	z = q.z;
	w = q.w;
}

template<typename T>
Quaternion<T>::Quaternion(const Matrix3x3<T>& m)
{
	T t = 1 + m.x0 + m.y1 + m.z2;

	// large enough
	if (t > static_cast<T>(0.001))
	{
		T s = sqrt(t) * static_cast<T>(2.0);
		x = (m.y2 - m.z1) / s;
		y = (m.z0 - m.x2) / s;
		z = (m.x1 - m.y0) / s;
		w = static_cast<T>(0.25) * s;
	} // else we have to check several cases
	else if (m.x0 > m.y1 && m.x0 > m.z2)
	{
		// Column 0: 
		T s = sqrt(static_cast<T>(1.0) + m.x0 - m.y1 - m.z2) * static_cast<T>(2.0);
		x = static_cast<T>(0.25) * s;
		y = (m.x1 + m.y0) / s;
		z = (m.z0 + m.x2) / s;
		w = (m.y2 - m.z1) / s;
	}
	else if (m.y1 > m.z2)
	{
		// Column 1: 
		T s = sqrt(static_cast<T>(1.0) + m.y1 - m.x0 - m.z2) * static_cast<T>(2.0);
		x = (m.x1 + m.y0) / s;
		y = static_cast<T>(0.25) * s;
		z = (m.y2 + m.z1) / s;
		w = (m.z0 - m.x2) / s;
	}
	else
	{
		// Column 2:
		T s = sqrt(static_cast<T>(1.0) + m.z2 - m.x0 - m.y1) * static_cast<T>(2.0);
		x = (m.z0 + m.x2) / s;
		y = (m.y2 + m.z1) / s;
		z = static_cast<T>(0.25) * s;
		w = (m.x1 - m.y0) / s;
	}
}

template<typename T>
Quaternion<T>::Quaternion(T _x, T _y, T _z, T _w)
{
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}

template<typename T>
Quaternion<T>::Quaternion(const T* pData)
{
	x = pData[0];
	y = pData[1];
	z = pData[2];
	w = pData[3];
}

template<typename T>
Quaternion<T>::Quaternion(T _real, const Vector3<T>& _imag)
{
	imag = _imag;
	real = _real;
}

template<typename T>
Quaternion<T>::Quaternion(const Vector3<T>& v, T rotation)
{
	const T sin_a = sin(rotation / 2);
	const T cos_a = cos(rotation / 2);
	x = v.x * sin_a;
	y = v.y * sin_a;
	z = v.z * sin_a;
	w = cos_a;
}

template<typename T>
Quaternion<T>::Quaternion(const Vector3<T>& from, const Vector3<T>& to)
{
	Vector3<T> halfway = from + to;
	halfway.Normalize();

	if (from == to.Negative())
	{
		// rotates pi, cos(pi / 2) = 0
		w = 0;
		imag = from.Orthogonal();
	}
	else
	{
		w = halfway * from;
		if (w == static_cast<T>(1))
			imag = { 0, 0, 0 };
		else
		{
			imag = from ^ to;
			imag.Normalize();
			imag *= static_cast<T>(std::sqrt((double)(static_cast<T>(1) - w * w)));
		}
	}
}

template<typename T>
Matrix3x3<T> Quaternion<T>::Matrix() const
{
	Matrix3x3<T> resMatrix;
	resMatrix.x0 = static_cast<T>(1.0) - static_cast<T>(2.0) * (y * y + z * z);
	resMatrix.y0 = static_cast<T>(2.0) * (x * y - z * w);
	resMatrix.z0 = static_cast<T>(2.0) * (x * z + y * w);
	resMatrix.x1 = static_cast<T>(2.0) * (x * y + z * w);
	resMatrix.y1 = static_cast<T>(1.0) - static_cast<T>(2.0) * (x * x + z * z);
	resMatrix.z1 = static_cast<T>(2.0) * (y * z - x * w);
	resMatrix.x2 = static_cast<T>(2.0) * (x * z - y * w);
	resMatrix.y2 = static_cast<T>(2.0) * (y * z + x * w);
	resMatrix.z2 = static_cast<T>(1.0) - static_cast<T>(2.0) * (x * x + y * y);

	return resMatrix;
}

template<typename T>
bool Quaternion<T>::operator == (const Quaternion<T>& q) const
{
	// FIXME: wrong
	return w == q.w && x == q.x && y == q.y && z == q.z;
}

template<typename T>
bool Quaternion<T>::operator != (const Quaternion<T>& q) const
{
	return !(*this == q);
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator *= (const Quaternion<T>& q)
{
	T _x = w * q.x + x * q.w + y * q.z - z * q.y;
	T _y = w * q.y + y * q.w + z * q.x - x * q.z;
	T _z = w * q.z + z * q.w + x * q.y - y * q.x;
	T _w = w * q.w - x * q.x - y * q.y - z * q.z;

	x = _x;
	y = _y;
	z = _z;
	w = _w;

	return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator += (const Quaternion<T>& q)
{
	x += q.x;
	y += q.y;
	z += q.z;
	w += q.w;

	return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator -= (const Quaternion<T>& q)
{
	x -= q.x;
	y -= q.y;
	z -= q.z;
	w -= q.w;

	return *this;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator * (const Quaternion<T>& q) const
{
	Quaternion<T> ret = *this;
	ret *= q;
	return ret;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator + (const Quaternion<T>& q) const
{
	Quaternion<T> ret = *this;
	ret += q;
	return ret;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator - (const Quaternion<T>& q) const
{
	Quaternion<T> ret = *this;
	ret -= q;
	return ret;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator *= (T s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;

	return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator += (T s)
{
	x += s;
	y += s;
	z += s;
	w += s;

	return *this;
}

template<typename T>
Quaternion<T>& Quaternion<T>::operator -= (T s)
{
	x -= s;
	y -= s;
	z -= s;
	w -= s;

	return *this;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator * (T s) const
{
	Quaternion<T> ret = *this;
	ret *= s;
	return ret;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator + (T s) const
{
	Quaternion<T> ret = *this;
	ret += s;
	return ret;
}

template<typename T>
const Quaternion<T> Quaternion<T>::operator - (T s) const
{
	Quaternion<T> ret = *this;
	ret -= s;
	return ret;
}

template<typename T>
T Quaternion<T>::Mag() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

template<typename T>
Quaternion<T>& Quaternion<T>::Normalize()
{
	const T mag = Mag();
	if (mag)
	{
		const T invMag = static_cast<T>(1.0) / mag;
		x *= invMag;
		y *= invMag;
		z *= invMag;
		w *= invMag;
	}
	return *this;
}

template<typename T>
Quaternion<T> Quaternion<T>::Normal()
{
	Quaternion<T> ret;
	const T mag = ret.Mag();
	if (mag)
	{
		const T invMag = static_cast<T>(1.0) / mag;
		ret.x *= invMag;
		ret.y *= invMag;
		ret.z *= invMag;
		ret.w *= invMag;
	}
	return ret;
}

template<typename T>
T Quaternion<T>::Dot(const Quaternion<T>& q)
{
	return x * q.x + y * q.y + z * q.z + w * q.w;
}

template<typename T>
Quaternion<T>& Quaternion<T>::Conjugate()
{
	ret.x = -ret.x;
	ret.y = -ret.y;
	ret.z = -ret.z;

	return *this;
}

template<typename T>
Quaternion<T> Quaternion<T>::GetConjugate() const
{
	Quaternion<T> ret = *this;
	ret.x = -ret.x;
	ret.y = -ret.y;
	ret.z = -ret.z;
	return ret;
}

template<typename T>
Vector3<T> Quaternion<T>::Rotate(const Vector3<T>& v)
{
	return v + (imag ^ (v * real + (imag ^ v))) * static_cast<T>(2.0);
}

template<typename T>
Quaternion<T> Quaternion<T>::SLerp(const Quaternion<T>& from, const Quaternion<T>& to, T factor)
{
	Quaternion<T> out = to;

	// calc cosine theta
	T cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;

	// adjust signs (if necessary)
	if (cosom < static_cast<T>(0.0))
	{
		cosom = -cosom;
		out.x = -out.x;   // Reverse all signs
		out.y = -out.y;
		out.z = -out.z;
		out.w = -out.w;
	}

	// Calculate coefficients
	T sclp, sclq;
	if ((static_cast<T>(1.0) - cosom) > static_cast<T>(0.0001)) // 0.0001 -> some epsillon
	{
		// Standard case (slerp)
		T omega, sinom;
		omega = acos(cosom); // extract theta from dot product's cos theta
		sinom = sin(omega);
		sclp = sin((static_cast<T>(1.0) - factor) * omega) / sinom;
		sclq = sin(factor * omega) / sinom;
	}
	else
	{
		// Very close, do linear interp (because it's faster)
		sclp = static_cast<T>(1.0) - factor;
		sclq = factor;
	}

	out.x = sclp * from.x + sclq * out.x;
	out.y = sclp * from.y + sclq * out.y;
	out.z = sclp * from.z + sclq * out.z;
	out.w = sclp * from.w + sclq * out.w;

	return out;
}

template<typename T>
Quaternion<float> Quaternion<T>::SinglePrecision() const
{
	return { (float)x, (float)y, (float)z, (float)w };
}

template<typename T>
Quaternion<double> Quaternion<T>::DoublePrecision() const
{
	return { (double)x, (double)y, (double)z, (double)w };
}

template<typename T>
Quaternion<T> Quaternion<T>::NLerp(const Quaternion<T>& from, const Quaternion<T>& to, T factor)
{
	Quaternion<T> ret;

	ret.x = from.x * factor + to.x * (static_cast<T>(1.0) - factor);
	ret.y = from.y * factor + to.y * (static_cast<T>(1.0) - factor);
	ret.z = from.z * factor + to.z * (static_cast<T>(1.0) - factor);
	ret.w = from.w * factor + to.w * (static_cast<T>(1.0) - factor);

	return ret.Normalize();
}

template<typename T>
T Quaternion<T>::Dot(const Quaternion<T>& q0, const Quaternion<T>& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z *q1.z + q0.w * q1.w;
}