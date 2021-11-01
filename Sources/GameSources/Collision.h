#pragma once

#include <DirectXMath.h>
#include <memory>

//#include "OBJ3D.h"
#include "../GameLibSource/GeometricPrimitive.h"
#include "GameLibSource/Model.h"

// ヒット計算
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0, 0, 0 }; // レイとポリゴンの交点
	DirectX::XMFLOAT3 normal = { 0, 0, 0 };   // 衝突したポリゴンの法線ベクトル
	float distance = 0.0f;                    // レイの始点から交点までの距離
	int materialIndex = -1;                   // 衝突したポリゴンのマテリアル番号
	DirectX::XMFLOAT3 rotation = { 0, 0, 0 }; // 回転量
};

class Collision
{
public:
	static bool SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2);
	static bool SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsule_radius, DirectX::XMFLOAT3 sphere, float sphere_radius);
	//bool OBBVsOBB(OBJ3D* obj1, OBJ3D* obj2);

	// 2次元での矩形と矩形の衝突判定 & 押し出し 関数
	static bool RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2);

	// 2次元での円と円の衝突判定 & 押し出し 関数
	static bool CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion =  true);

	// 2次元での円の押し出し 関数
	static void CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2);

	// 点と矩形の当たり判定
	static bool PointVsRect(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2 pos2, const DirectX::XMFLOAT2 size2);


	// 2次元でのカプセルと円の衝突判定 & 押し出し 関数
	// static bool CapsuleVsCircleAndExtrusion(DirectX::XMFLOAT3& nowPos1, DirectX::XMFLOAT3 oldPos1, float radius1, DirectX::XMFLOAT3 pos2, float radius2);
	
	// 3次元座標上の線分と3角ポリゴンが交差してるかを判定
	bool detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ポリゴン上に点が含まれるかを判定
	bool detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3次元座標上の線分と平面の交点座標を求める
	DirectX::XMFLOAT3 calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ある点から平面までの距離
	float calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3次元座標上の線分と平面が交差してるかを判定
	bool detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// レイとモデルの交差判定
	static bool IntersectRayVsModel(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		const Model* model,
		HitResult& result
	);

	static bool AabbVsAabb(
		const DirectX::XMFLOAT3& positionA,
		float widthA,
		float heightA,
		const DirectX::XMFLOAT3& positionB,
		float widthB,
		float heightB
	);

private:
	// 球の中心とカプセルの線分の距離（の二乗）を計算用
	static float GetSqDistancePoint2Segment(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 seg_start, DirectX::XMFLOAT3 seg_end, DirectX::XMFLOAT3& dir);
	float LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3 = 0);

	float Dot(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Cross(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1);
	DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3 v);
};

extern Collision collision;


class CollisionPrimitive// : public OBJ3D
{
private:
	std::unique_ptr<GeometricPrimitive> geometricPrimitive;

	int primitiveType;
	DirectX::XMFLOAT3 collisionScale;

public:
	CollisionPrimitive() : geometricPrimitive( nullptr ) {}

	enum CollisionPrimitiveType { CUBE, SPHERE, CYLINDER };
	CollisionPrimitive
	(
		ID3D11Device* device,
		int type,						                             	// 衝突判定の形の番号（CollisionPrimitiveType:: より）
		bool isCreateBottom,				                // 原点を足元にするかフラグ
		DirectX::XMFLOAT3 _collisionScale	// 衝突判定のスケール
	);

	~CollisionPrimitive()
	{
		if (geometricPrimitive)
		{
			geometricPrimitive = nullptr;
		}
	}

	void Render
	(
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& projection,
		const DirectX::XMFLOAT4& lightDirection,
		float elapsedTime,
		bool bSolid = false
	);

	DirectX::XMFLOAT3 GetCollisionScale() { return collisionScale; }
	void SetCollisionScale(DirectX::XMFLOAT3 _collisionScale) { collisionScale = _collisionScale; }

	//OBJ3D GetOBJ3D()
	//{
	//	OBJ3D allData;
	//
	//	allData.SetPos(pos);
	//	allData.SetSpeed(speed);
	//	allData.SetScale(scale);
	//	allData.SetAngle(angle);
	//	allData.SetColor(color);
	//	allData.SetAdjust(adjust);
	//	allData.SetExists(exists);
	//
	//	return allData;
	//}

private:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 speed    = { 0,0,0 };
	DirectX::XMFLOAT3 scale      = { 0,0,0 };
	float angle = 0;
	DirectX::XMFLOAT4 color      = { 1,1,1,1 };
};