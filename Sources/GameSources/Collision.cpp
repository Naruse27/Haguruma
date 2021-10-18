#include "Collision.h"
#include "../GameLibSource/Framework.h"
Collision collision;


//******************************************************
//球体　vs　球体
//******************************************************
bool Collision::SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2)
{
	if ((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) + (pos2.z - pos1.z) * (pos2.z - pos1.z) <= (r1 + r2) * (r1 + r2))
	{
		return true;
	}
	else return false;
}

//******************************************************
//球体　vs　カプセル
//******************************************************
bool Collision::SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsule_radius, DirectX::XMFLOAT3 sphere, float sphere_radius)
{
	// 球の中心とカプセルの線分の距離（の二乗）を計算
	DirectX::XMFLOAT3 dir;
	float dis = GetSqDistancePoint2Segment(sphere, capsule1, capsule2, dir);

	// 距離（の二乗）が半径の和（の二乗）より小さければ当たっている
	float radius = sphere_radius + capsule_radius;

	if (radius * radius < dis) return false;
	if (dis == 0.0f) return false;
	if (dir.x == 0.0f && dir.y == 0.0f && dir.z == 0.0f) return false;

	float length = sqrtf(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);

	dir.x /= length;
	dir.y /= length;
	dir.z /= length;

	capsule1.x = sphere.x + radius * dir.x;
	capsule1.y = sphere.y + radius * dir.y;
	capsule1.z = sphere.z + radius * dir.z;

	return true;
}
float Collision::GetSqDistancePoint2Segment(DirectX::XMFLOAT3 point, DirectX::XMFLOAT3 seg_start, DirectX::XMFLOAT3 seg_end, DirectX::XMFLOAT3& dir)
{
	const float eqsilon = 1.0e-5f;	// 誤差吸収用の微小な値

	// 線分の始点から終点へのベクトル
	DirectX::XMFLOAT3 segment_sub = DirectX::XMFLOAT3(seg_end.x - seg_start.x, seg_end.y - seg_start.y, seg_end.z - seg_start.z);

	// 線分の始点から点へのベクトル
	DirectX::XMFLOAT3 segment_point = DirectX::XMFLOAT3(point.x - seg_start.x, point.y - seg_start.y, point.z - seg_start.z);
	dir = segment_point;

	// 射影ベクトル
	DirectX::XMFLOAT3 cross_point;

	// 2ベクトルの内積
	float dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// 内積が負なら、線分の始点が最近傍
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// 点から線分の終点へのベクトル
	segment_point = DirectX::XMFLOAT3(seg_end.x - point.x, seg_end.y - point.y, seg_end.z - point.z);

	// 2ベクトルの内積
	dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// 内積が負なら、線分の終点が最近傍
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// 上記のどちらにも該当しない場合、線分上に落とした射影が最近傍
	// (本来ならサインで求めるが、外積の大きさ/線分のベクトルの大きさで求まる)
	cross_point.x = segment_sub.y * segment_point.z - segment_sub.z * segment_point.y;
	cross_point.y = segment_sub.z * segment_point.x - segment_sub.x * segment_point.z;
	cross_point.z = segment_sub.x * segment_point.y - segment_sub.y * segment_point.x;
	dir = segment_sub;
	return (((cross_point.x * cross_point.x) + (cross_point.y * cross_point.y) + (cross_point.z * cross_point.z)) 
		/ ((segment_sub.x * segment_sub.x) + (segment_sub.y * segment_sub.y) + (segment_sub.z * segment_sub.z)));
}

//******************************************************
//OBB　vs　OBB
//******************************************************

//bool Collision::OBBVsOBB(OBJ3D* obj1, OBJ3D* obj2)
//{
//	DirectX::XMFLOAT3 Nv1_1;
//	DirectX::XMFLOAT3 Nv2_1;
//	DirectX::XMFLOAT3 Nv3_1;
//
//	DirectX::XMFLOAT3 Nv1_2;
//	DirectX::XMFLOAT3 Nv2_2;
//	DirectX::XMFLOAT3 Nv3_2;
//
//
//	DirectX::XMFLOAT3 v1_1;
//	DirectX::XMFLOAT3 v2_1;
//	DirectX::XMFLOAT3 v3_1;
//
//	DirectX::XMFLOAT3 v1_2;
//	DirectX::XMFLOAT3 v2_2;
//	DirectX::XMFLOAT3 v3_2;
//
//
//	//	回転
//	DirectX::XMFLOAT4X4 rota;
//
//	DirectX::XMStoreFloat4x4(&rota, obj1->GetWorldMatrix());
//
//	Nv1_1.x = rota._11;
//	Nv1_1.y = rota._12;
//	Nv1_1.z = rota._13;
//
//	Nv2_1.x = rota._21;
//	Nv2_1.y = rota._22;
//	Nv2_1.z = rota._23;
//
//	Nv3_1.x = rota._31;
//	Nv3_1.y = rota._32;
//	Nv3_1.z = rota._33;
//
//	DirectX::XMStoreFloat4x4(&rota, obj2->GetWorldMatrix());
//
//	Nv1_2.x = rota._11;
//	Nv1_2.y = rota._12;
//	Nv1_2.z = rota._13;
//
//	Nv2_2.x = rota._21;
//	Nv2_2.y = rota._22;
//	Nv2_2.z = rota._23;
//
//	Nv3_2.x = rota._31;
//	Nv3_2.y = rota._32;
//	Nv3_2.z = rota._33;
//
//
//	DirectX::XMFLOAT3 scale = obj1->GetScale();
//
//	v1_1.x = Nv1_1.x * scale.x / 2.0f;
//	v1_1.y = Nv1_1.y * scale.x / 2.0f;
//	v1_1.z = Nv1_1.z * scale.x / 2.0f;
//
//	v2_1.x = Nv2_1.x * scale.y / 2.0f;
//	v2_1.y = Nv2_1.y * scale.y / 2.0f;
//	v2_1.z = Nv2_1.z * scale.y / 2.0f;
//
//	v3_1.x = Nv3_1.x * scale.z / 2.0f;
//	v3_1.y = Nv3_1.y * scale.z / 2.0f;
//	v3_1.z = Nv3_1.z * scale.z / 2.0f;
//
//	scale = obj2->GetScale();
//
//	v1_2.x = Nv1_2.x * scale.x / 2.0f;
//	v1_2.y = Nv1_2.y * scale.x / 2.0f;
//	v1_2.z = Nv1_2.z * scale.x / 2.0f;
//
//	v2_2.x = Nv2_2.x * scale.y / 2.0f;
//	v2_2.y = Nv2_2.y * scale.y / 2.0f;
//	v2_2.z = Nv2_2.z * scale.y / 2.0f;
//
//	v3_2.x = Nv3_2.x * scale.z / 2.0f;
//	v3_2.y = Nv3_2.y * scale.z / 2.0f;
//	v3_2.z = Nv3_2.z * scale.z / 2.0f;
//
//
//	DirectX::XMFLOAT3 interval;
//	DirectX::XMFLOAT3 pos1 = obj1->GetPos();
//	DirectX::XMFLOAT3 pos2 = obj2->GetPos();
//
//	interval.x = pos1.x - pos2.x;
//	interval.y = pos1.y - pos2.y;
//	interval.z = pos1.z - pos2.z;
//
//	float r1 = 0.0f;
//	float r2 = 0.0f;
//	float l = 0.0f;
//	float dot = 0.0f;
//
//
//	// 分離軸 v1_1
//	r1 = sqrtf((v1_1.x * v1_1.x) + (v1_1.y * v1_1.y) + (v1_1.z * v1_1.z));
//	r2 = LenSegOnSepAxis(&Nv1_1, &v1_2, &v2_2, &v3_2);
//	dot = ((interval.x * Nv1_1.x) + (interval.y * Nv1_1.y) + (interval.z * Nv1_1.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 v2_1
//	r1 = sqrtf((v2_1.x * v2_1.x) + (v2_1.y * v2_1.y) + (v2_1.z * v2_1.z));
//	r2 = LenSegOnSepAxis(&Nv2_1, &v1_2, &v2_2, &v3_2);
//	dot = ((interval.x * Nv2_1.x) + (interval.y * Nv2_1.y) + (interval.z * Nv2_1.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 v3_1
//	r1 = sqrtf((v3_1.x * v3_1.x) + (v3_1.y * v3_1.y) + (v3_1.z * v3_1.z));
//	r2 = LenSegOnSepAxis(&Nv3_1, &v1_2, &v2_2, &v3_2);
//	dot = ((interval.x * Nv3_1.x) + (interval.y * Nv3_1.y) + (interval.z * Nv3_1.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//
//	// 分離軸 v1_2
//	r1 = LenSegOnSepAxis(&Nv1_2, &v1_1, &v2_1, &v3_1);
//	r2 = sqrtf((v1_2.x * v1_2.x) + (v1_2.y * v1_2.y) + (v1_2.z * v1_2.z));
//	dot = ((interval.x * Nv1_2.x) + (interval.y * Nv1_2.y) + (interval.z * Nv1_2.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 v2_2
//	r1 = LenSegOnSepAxis(&Nv2_2, &v1_1, &v2_1, &v3_1);
//	r2 = sqrtf((v2_2.x * v2_2.x) + (v2_2.y * v2_2.y) + (v2_2.z * v2_2.z));
//	dot = ((interval.x * Nv2_2.x) + (interval.y * Nv2_2.y) + (interval.z * Nv2_2.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 v3_2
//	r1 = LenSegOnSepAxis(&Nv3_2, &v1_1, &v2_1, &v3_1);
//	r2 = sqrtf((v3_2.x * v3_2.x) + (v3_2.y * v3_2.y) + (v3_2.z * v3_2.z));
//	dot = ((interval.x * Nv3_2.x) + (interval.y * Nv3_2.y) + (interval.z * Nv3_2.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//
//	DirectX::XMFLOAT3 cross;
//
//	// 分離軸 cross v1_1 + v1_2
//	cross.x = v1_1.y * v1_2.z - v1_1.z * v1_2.y;
//	cross.y = v1_1.z * v1_2.x - v1_1.x * v1_2.z;
//	cross.z = v1_1.x * v1_2.y - v1_1.y * v1_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v1_1 + v2_2
//	cross.x = v1_1.y * v2_2.z - v1_1.z * v2_2.y;
//	cross.y = v1_1.z * v2_2.x - v1_1.x * v2_2.z;
//	cross.z = v1_1.x * v2_2.y - v1_1.y * v2_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v1_1 + v3_2
//	cross.x = v1_1.y * v3_2.z - v1_1.z * v3_2.y;
//	cross.y = v1_1.z * v3_2.x - v1_1.x * v3_2.z;
//	cross.z = v1_1.x * v3_2.y - v1_1.y * v3_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v2_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v2_1 + v1_2
//	cross.x = v2_1.y * v1_2.z - v2_1.z * v1_2.y;
//	cross.y = v2_1.z * v1_2.x - v2_1.x * v1_2.z;
//	cross.z = v2_1.x * v1_2.y - v2_1.y * v1_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v2_1 + v2_2
//	cross.x = v2_1.y * v2_2.z - v2_1.z * v2_2.y;
//	cross.y = v2_1.z * v2_2.x - v2_1.x * v2_2.z;
//	cross.z = v2_1.x * v2_2.y - v2_1.y * v2_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v2_1 + v3_2
//	cross.x = v2_1.y * v3_2.z - v2_1.z * v3_2.y;
//	cross.y = v2_1.z * v3_2.x - v2_1.x * v3_2.z;
//	cross.z = v2_1.x * v3_2.y - v2_1.y * v3_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v3_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v3_1 + v1_2
//	cross.x = v3_1.y * v1_2.z - v3_1.z * v1_2.y;
//	cross.y = v3_1.z * v1_2.x - v3_1.x * v1_2.z;
//	cross.z = v3_1.x * v1_2.y - v3_1.y * v1_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
//	r2 = LenSegOnSepAxis(&cross, &v2_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v3_1 + v2_2
//	cross.x = v3_1.y * v2_2.z - v3_1.z * v2_2.y;
//	cross.y = v3_1.z * v2_2.x - v3_1.x * v2_2.z;
//	cross.z = v3_1.x * v2_2.y - v3_1.y * v2_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v3_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// 分離軸 cross v3_1 + v3_2
//	cross.x = v3_1.y * v3_2.z - v3_1.z * v3_2.y;
//	cross.y = v3_1.z * v3_2.x - v3_1.x * v3_2.z;
//	cross.z = v3_1.x * v3_2.y - v3_1.y * v3_2.x;
//
//	r1 = LenSegOnSepAxis(&cross, &v1_1, &v2_1);
//	r2 = LenSegOnSepAxis(&cross, &v1_2, &v2_2);
//	dot = ((interval.x * cross.x) + (interval.y * cross.y) + (interval.z * cross.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//
//	return true;
//}

float Collision::LenSegOnSepAxis(DirectX::XMFLOAT3* sep, DirectX::XMFLOAT3* v1, DirectX::XMFLOAT3* v2, DirectX::XMFLOAT3* v3)
{
	// 3つの内積の絶対値の和で投影線分長を計算
    // 分離軸Sepは標準化されていること
	float dot1 = ((sep->x * v1->x) + (sep->y * v1->y) + (sep->z * v1->z));
	float dot2 = ((sep->x * v2->x) + (sep->y * v2->y) + (sep->z * v2->z));
	float dot3 = 0.0f;
	if (!v3)
	{
		dot3 = 0.0f;
	}
	else
	{
		dot3 = ((sep->x * v3->x) + (sep->y * v3->y) + (sep->z * v3->z));
	}

	float r1 = fabs(dot1);
	float r2 = fabs(dot2);
	float r3 = v3 ? (fabs(dot3)) : 0;

	return r1 + r2 + r3;
}

//******************************************************
//矩形　vs　矩形
//******************************************************
bool Collision::RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2)
{
	bool isHit = false;

	if (pos1.x - scale1.x <= pos2.x - scale2.x)
	{
		pos1.x = pos2.x - scale2.x + scale1.x;
		isHit = true;
	}
	if (pos2.x + scale2.x <= pos1.x + scale1.x)
	{
		pos1.x = pos2.x + scale2.x - scale1.x;
		isHit = true;
	}
	if (pos1.y - scale1.y <= pos2.y - scale2.y)
	{
		pos1.y = pos2.y - scale2.y + scale1.y;
		isHit = true;
	}
	if (pos2.y + scale2.y <= pos1.y + scale1.y)
	{
		pos1.y = pos2.y + scale2.y - scale1.y;
		isHit = true;
	}

	return isHit;
}

//******************************************************
//円　vs　円
//******************************************************
bool Collision::CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion)
{
	if ((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y) <= (radius1 + radius2) * (radius1 + radius2))
	{
		if (extrusion) CircleExtrusion(pos1, radius1, pos2, radius2);

		return true;
	}
	else return false;
}

void Collision::CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2)
{
	DirectX::XMFLOAT2 dir = { pos1.x - pos2.x, pos1.y - pos2.y };
	float length = sqrtf(dir.x * dir.x + dir.y * dir.y);

	dir.x /= length;
	dir.y /= length;

	pos1.x = pos2.x + (radius1 + radius2) * dir.x;
	pos1.y = pos2.y + (radius1 + radius2) * dir.y;
}

//******************************************************
//カプセル　vs　円
//******************************************************
//bool Collision::CapsuleVsCircleAndExtrusion(DirectX::XMFLOAT3& nowPos1, DirectX::XMFLOAT3 oldPos1, float radius1, DirectX::XMFLOAT3 pos2, float radius2)
//{
//	/*if ((pos2.x - nowPos1.x) * (pos2.x - nowPos1.x) + (pos2.z - nowPos1.z) * (pos2.z - nowPos1.z) <= (radius1 + radius2) * (radius1 + radius2))
//	{
//		DirectX::XMFLOAT2 dir = { oldPos1.x - pos2.x, oldPos1.z - pos2.z };
//		float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
//
//		dir.x /= length;
//		dir.y /= length;
//
//		nowPos1.x = pos2.x + (radius1 + radius2) * dir.x;
//		nowPos1.z = pos2.z + (radius1 + radius2) * dir.y;
//
//		return true;
//	}*/
//	if ((pos2.x - oldPos1.x) * (pos2.x - oldPos1.x) + (pos2.z - oldPos1.z) * (pos2.z - oldPos1.z) <= (radius1 + radius2) * (radius1 + radius2))
//	{
//		DirectX::XMFLOAT2 dir = { oldPos1.x - pos2.x, oldPos1.z - pos2.z };
//		float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
//
//		dir.x /= length;
//		dir.y /= length;
//
//		nowPos1.x = pos2.x + (radius1 + radius2) * dir.x;
//		nowPos1.z = pos2.z + (radius1 + radius2) * dir.y;
//
//		return true;
//	}
//	else
//	{
//		DirectX::XMFLOAT2 dir = { nowPos1.x - oldPos1.x, nowPos1.z - oldPos1.z };
//		float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
//
//		if (length <= 0.0f) return false;
//
//		DirectX::XMFLOAT3 frontVec = { dir.x / length, 0.0f, dir.y / length };
//		DirectX::XMFLOAT3 upVec = { 0.0f, 1.0f, 0.0f };
//		DirectX::XMFLOAT3 rightVec;
//		rightVec.x = upVec.y * frontVec.z - upVec.z * frontVec.y;
//		rightVec.y = upVec.z * frontVec.x - upVec.x * frontVec.z;
//		rightVec.z = upVec.x * frontVec.y - upVec.y * frontVec.x;
//
//		DirectX::XMFLOAT3 rey = { dir.x, 0.0f, dir.y };
//		DirectX::XMFLOAT3 rightPoint = { rightVec.x * radius1, 0.0f, rightVec.z * radius1 };
//		DirectX::XMFLOAT3 leftPoint = { rightVec.x * radius1 * -1, 0.0f, rightVec.z * radius1 * -1 };
//
//
//	}
//}

float Collision::Dot(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1)
{
	float dot;

	dot = (v0.x * v1.x) + (v0.y * v1.y) + (v0.z * v1.z);

	return dot;
}
DirectX::XMFLOAT3 Collision::Cross(DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1)
{
	DirectX::XMFLOAT3 cross;
	cross.x = v0.y * v1.z - v0.z * v1.y;
	cross.y = v0.z * v1.x - v0.x * v1.z;
	cross.z = v0.x * v1.y - v0.y * v1.x;
	return cross;
}
DirectX::XMFLOAT3 Collision::Normalize(DirectX::XMFLOAT3 v)
{
	DirectX::XMFLOAT3 _v = v;
	float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	
	_v.x /= length;
	_v.y /= length;
	_v.z /= length;

	return _v;
}

// -------------------------------------------------------------
/// <summary>
/// 3次元座標上の線分と3角ポリゴンが交差してるかを判定
/// </summary>
bool Collision::detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	bool bCollision = detectCollisionLineSegmentAndPlane(a, b, v0, v1, v2);

	if (bCollision)
	{
		DirectX::XMFLOAT3 p = calcIntersectionLineSegmentAndPlane(a, b, v0, v1, v2);
		if (detectPointIsEnclosedByPolygon(p, v0, v1, v2))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

// -------------------------------------------------------------
/// <summary>
/// ポリゴン上に点が含まれるかを判定
/// </summary>
bool Collision::detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };
	DirectX::XMFLOAT3 vec3 = { v0.x - v2.x, v0.y - v2.y, v0.z - v2.z };

	DirectX::XMFLOAT3 vec4 = { p.x - v1.x, p.y - v1.y, p.z - v1.z };
	DirectX::XMFLOAT3 vec5 = { p.x - v2.x, p.y - v2.y, p.z - v2.z };
	DirectX::XMFLOAT3 vec6 = { p.x - v0.x, p.y - v0.y, p.z - v0.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));

	DirectX::XMFLOAT3 n0 = Normalize(Cross(vec1, vec4));
	DirectX::XMFLOAT3 n1 = Normalize(Cross(vec2, vec5));
	DirectX::XMFLOAT3 n2 = Normalize(Cross(vec3, vec6));

	if ((1.0f - Dot(n, n0)) > 0.001f) return false;
	if ((1.0f - Dot(n, n1)) > 0.001f) return false;
	if ((1.0f - Dot(n, n2)) > 0.001f) return false;

	return true;
}

// -------------------------------------------------------------
/// <summary>
/// 3次元座標上の線分と平面の交点座標を求める
/// </summary>
DirectX::XMFLOAT3 Collision::calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	float distAP = calcDistancePointAndPlane(a, v0, v1, v2);
	float distBP = calcDistancePointAndPlane(b, v0, v1, v2);

	float t = distAP / (distAP + distBP);

	DirectX::XMFLOAT3 result;
	result.x = (b.x - a.x) * t + a.x;
	result.y = (b.y - a.y) * t + a.y;
	result.z = (b.z - a.z) * t + a.z;

	return result;
}

// -------------------------------------------------------------
/// <summary>
/// ある点から平面までの距離
/// </summary>
float Collision::calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));
	DirectX::XMFLOAT3 g;
	g.x = (v0.x + v1.x + v2.x) / 3.0f;
	g.y = (v0.y + v1.y + v2.y) / 3.0f;
	g.z = (v0.z + v1.z + v2.z) / 3.0f;

	DirectX::XMFLOAT3 vec3;
	vec3.x = p.x - g.x;
	vec3.y = p.y - g.y;
	vec3.z = p.z - g.z;

	return abs(Dot(n, vec3));
}

// -------------------------------------------------------------
/// <summary>
/// 3次元座標上の線分と平面が交差してるかを判定
/// </summary>
bool Collision::detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2)
{
	DirectX::XMFLOAT3 vec1 = { v1.x - v0.x, v1.y - v0.y, v1.z - v0.z };
	DirectX::XMFLOAT3 vec2 = { v2.x - v1.x, v2.y - v1.y, v2.z - v1.z };

	DirectX::XMFLOAT3 n = Normalize(Cross(vec1, vec2));
	DirectX::XMFLOAT3 g;
	g.x = (v0.x + v1.x + v2.x) / 3.0f;
	g.y = (v0.y + v1.y + v2.y) / 3.0f;
	g.z = (v0.z + v1.z + v2.z) / 3.0f;

	DirectX::XMFLOAT3 vec3 = { a.x - g.x, a.y - g.y, a.z - g.z };
	DirectX::XMFLOAT3 vec4 = { b.x - g.x, b.y - g.y, b.z - g.z };

	if (Dot(vec3, n) * Dot(vec4, n) <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}


// レイとモデルの交差判定 
bool Collision::IntersectRayVsModel(
	const DirectX::XMFLOAT3& start,
	const DirectX::XMFLOAT3& end,
	const Model* model,
	HitResult& result)
{
	DirectX::XMVECTOR WorldStart = DirectX::XMLoadFloat3(&start);
	DirectX::XMVECTOR WorldEnd = DirectX::XMLoadFloat3(&end);
	DirectX::XMVECTOR WorldRayVec = DirectX::XMVectorSubtract(WorldEnd, WorldStart);
	DirectX::XMVECTOR WorldRayLength = DirectX::XMVector3Length(WorldRayVec);
	// ワールド空間のレイの長さ 
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);
	bool hit = false;

	//const ModelResource* resource = model->GetResource();
	for (const SkinnedMesh::Mesh& mesh : model->GetMeshs())
	{
		// メッシュノード取得 
		//const SkinnedMesh::Mesh& mesh = a.GetKeyframe().nodes.at(mesh.nodeIndex);//model->GetNodes().at(mesh.nodeIndex);
		//mesh.defaultGlobalTransform
		// レイをワールド空間からローカル空間へ変換 
	
		//DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node.worldTransform);
		//DirectX::XMMATRIX InverseWorldTransform = DirectX::XMMatrixInverse(nullptr, WorldTransform);
		DirectX::XMMATRIX GlobalTransform = DirectX::XMLoadFloat4x4(&mesh.defaultGlobalTransform);
		DirectX::XMMATRIX WorldTransform = DirectX::XMMatrixInverse(nullptr, GlobalTransform);
		//DirectX::XMMATRIX InverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, GlobalTransform);

		DirectX::XMVECTOR Start = DirectX::XMVector3TransformCoord(WorldStart, GlobalTransform);
		DirectX::XMVECTOR End = DirectX::XMVector3TransformCoord(WorldEnd, GlobalTransform);
		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);
		DirectX::XMVECTOR Dir = DirectX::XMVector3Normalize(Vec);
		DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);
	
		// レイの長さ 
		float neart;
		DirectX::XMStoreFloat(&neart, Length);
	
		// 三角形（面）との交差判定 
		const std::vector<SkinnedMesh::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;
		int materialIndex = -1;
	
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const SkinnedMesh::Mesh::Subset& subset : mesh.subsets)
			// 三角形を抽出する
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndexLocation + i;
				// 三角形の頂点を抽出 
				const SkinnedMesh::Vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::Vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::Vertex& c = vertices.at(indices.at(index + 2));
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
				// 三角形の三辺ベクトルを算出 
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
				// 三角形の法線ベクトルを算出 
				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);
				// 内積の結果がプラスならば裏向き 
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
				float dot;
				DirectX::XMStoreFloat(&dot, Dot);
				if (dot >= 0) continue;
				// レイと平面の交点を算出 
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, Start);
				// 割り算                                                D2         D1
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, SA), Dot);
				float t;
				DirectX::XMStoreFloat(&t, X);
				if (t < .0f || t > neart) continue;
				// 交点までの距離が今までに計算した最近距離より 
				// 大きい時はスキップ 
				DirectX::XMVECTOR Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, X), Start);
	
				// 交点が三角形の内側にあるか判定 
			   //  １つめ 
				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position); DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal); float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f) continue;
				// ２つめ 
				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position); DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal); float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f) continue;
				// ３つめ 
				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position); DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal); float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f) continue;
				// 最近距離を更新
				neart = t;
				// 交点と法線を更新 
				HitPosition = Position;
				HitNormal = Normal;
				materialIndex = subset.materialUniqueId;
			}
		}
		if (materialIndex >= 0)
		{
			// ローカル空間からワールド空間へ変換 
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);
			// ヒット情報保存 
			if (result.distance > distance)
			{
				DirectX::XMVECTOR WorldNormal = DirectX::XMVector3TransformNormal(HitNormal, WorldTransform);
				result.distance = distance;
				result.materialIndex = materialIndex;
				DirectX::XMStoreFloat3(&result.position, WorldPosition);
				DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(WorldNormal));
				hit = true;
			}
		}
	}
	return hit;
    //return true;
}

bool Collision::AabbVsAabb(
	const DirectX::XMFLOAT3& positionA,
	float widthA,
	float heightA,
	const DirectX::XMFLOAT3& positionB,
	float widthB,
	float heightB)
{
	if (positionA.x + widthA > positionB.x - widthB) return true;
	if (positionB.x + widthB > positionA.x - widthA) return true;

	if (positionA.y < positionB.y + heightB) return true;
	if (positionB.y < positionA.y + heightA) return true;

	if (positionA.z + widthA > positionB.z - widthB) return true;
	if (positionB.z + widthB > positionA.z - widthA) return true;

	return false;
}


CollisionPrimitive::CollisionPrimitive(ID3D11Device* device, int type, bool isCreateBottom, DirectX::XMFLOAT3 _collisionScale)
{
	//Microsoft::WRL::ComPtr<ID3D11Device> device = Framework::GetInstance().GetDevice();

	primitiveType = type;
	geometricPrimitive = std::make_unique<GeometricPrimitive>( device, type, isCreateBottom, &_collisionScale );

	collisionScale = _collisionScale;
	if (type == CollisionPrimitiveType::CYLINDER || type == CollisionPrimitiveType::SPHERE)
	{
		scale.x = _collisionScale.x;
		scale.z = _collisionScale.x;
		scale.y = _collisionScale.y;
	}
	color = DirectX::XMFLOAT4(0.2f, 0.8f, 0.45f, 0.9f);
}

void CollisionPrimitive::Render
 (  const DirectX::XMMATRIX& view,
	const DirectX::XMMATRIX& projection,
	const DirectX::XMFLOAT4& lightDirection,
	float elapsedTime,
	bool bSolid )
{
#ifdef DEBUG_MODE
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediateContext = FrameWork::GetInstance().GetContext();

	//	ワールド変換行列取得
	DirectX::XMMATRIX worldM;
	worldM = GetWorldMatrix();

	/*if (bSolid) color.w = 0.5f;
	else color.w = 1.0f;*/

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 wvp;
	DirectX::XMFLOAT4X4 world;

	DirectX::XMStoreFloat4x4(&wvp, worldM * view * projection);
	DirectX::XMStoreFloat4x4(&world, worldM);

	//	描画
	if (geometricPrimitive)
	{
		geometricPrimitive->Render(immediateContext.Get(), wvp, world, lightDirection, color, bSolid);
	}
#endif
}