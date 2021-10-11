#ifndef VECTOR
#define VECTOR

#include <DirectXMath.h>

//==============================================================================
//
//      VECTOR2クラス
//
//==============================================================================
class Vector2 : public DirectX::XMFLOAT2
{
public:
	Vector2() : DirectX::XMFLOAT2(0, 0) {}
	Vector2(float x, float y) : DirectX::XMFLOAT2(x, y) {}
	Vector2(const Vector2& v) : DirectX::XMFLOAT2(v.x, v.y) {}
	~Vector2() {}

    const DirectX::XMFLOAT2& GetXmfloat2() const { return DirectX::XMFLOAT2(this->x, this->y); }

	Vector2& operator=(const Vector2&);
    Vector2& operator+=(const Vector2&);
    Vector2& operator-=(const Vector2&);
    Vector2& operator*=(float);
    Vector2& operator/=(float);

    Vector2 operator+() const;
    Vector2 operator-() const;

    Vector2 operator+(const Vector2&) const;
    Vector2 operator-(const Vector2&) const;
    Vector2 operator*(float) const;
    friend Vector2 operator*(float, const Vector2&);
    Vector2 operator/(float) const;

    bool operator == (const Vector2&) const;
    bool operator != (const Vector2&) const;
};
//==============================================================================
//
//      VECTOR3クラス
//
//==============================================================================

class Vector3 : public DirectX::XMFLOAT3
{
public:
    Vector3() : DirectX::XMFLOAT3(0, 0, 0) {}
    Vector3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
    Vector3(const Vector3& v) : DirectX::XMFLOAT3(v.x, v.y, v.z) {}
    ~Vector3() {}

    const DirectX::XMFLOAT3& GetXmfloat3() const { return DirectX::XMFLOAT3(this->x, this->y, this->z); }

    Vector3& operator=(const Vector3&);
    Vector3& operator+=(const Vector3&);
    Vector3& operator-=(const Vector3&);
    Vector3& operator*=(float);
    Vector3& operator/=(float);

    Vector3 operator+() const;
    Vector3 operator-() const;

    Vector3 operator+(const Vector3&) const;
    Vector3 operator-(const Vector3&) const;
    Vector3 operator*(float) const;
    friend Vector3 operator*(float, const Vector3&);
    Vector3 operator/(float) const;

    bool operator == (const Vector3&) const;
    bool operator != (const Vector3&) const;

    Vector3(const Vector2& v);
    // VECTOR3にVECTOR2を代入
    Vector3& operator=(const Vector2&);
};

//==============================================================================
//
//      VECTOR4クラス
//
//==============================================================================

class Vector4 : public DirectX::XMFLOAT4
{
public:
    Vector4() : DirectX::XMFLOAT4(0, 0, 0, 0) {}
    Vector4(float x, float y, float z, float w) : DirectX::XMFLOAT4(x, y, z, w) {}
    ~Vector4() {}

    const DirectX::XMFLOAT4& GetXmfloat4() const { return DirectX::XMFLOAT4(this->x, this->y, this->z, this->w); }

};

#endif // !VECTOR

