#include "Collision.h"
#include "../GameLibSource/Framework.h"
Collision collision;


//******************************************************
//���́@vs�@����
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
//���́@vs�@�J�v�Z��
//******************************************************
bool Collision::SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsule_radius, DirectX::XMFLOAT3 sphere, float sphere_radius)
{
	// ���̒��S�ƃJ�v�Z���̐����̋����i�̓��j���v�Z
	DirectX::XMFLOAT3 dir;
	float dis = GetSqDistancePoint2Segment(sphere, capsule1, capsule2, dir);

	// �����i�̓��j�����a�̘a�i�̓��j��菬������Γ������Ă���
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
	const float eqsilon = 1.0e-5f;	// �덷�z���p�̔����Ȓl

	// �����̎n�_����I�_�ւ̃x�N�g��
	DirectX::XMFLOAT3 segment_sub = DirectX::XMFLOAT3(seg_end.x - seg_start.x, seg_end.y - seg_start.y, seg_end.z - seg_start.z);

	// �����̎n�_����_�ւ̃x�N�g��
	DirectX::XMFLOAT3 segment_point = DirectX::XMFLOAT3(point.x - seg_start.x, point.y - seg_start.y, point.z - seg_start.z);
	dir = segment_point;

	// �ˉe�x�N�g��
	DirectX::XMFLOAT3 cross_point;

	// 2�x�N�g���̓���
	float dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// ���ς����Ȃ�A�����̎n�_���ŋߖT
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// �_��������̏I�_�ւ̃x�N�g��
	segment_point = DirectX::XMFLOAT3(seg_end.x - point.x, seg_end.y - point.y, seg_end.z - point.z);

	// 2�x�N�g���̓���
	dot = (segment_sub.x * segment_point.x) + (segment_sub.y * segment_point.y) + (segment_sub.z * segment_point.z);
	if (dot < eqsilon)
	{// ���ς����Ȃ�A�����̏I�_���ŋߖT
		return ((segment_point.x * segment_point.x) + (segment_point.y * segment_point.y) + (segment_point.z * segment_point.z));
	}

	// ��L�̂ǂ���ɂ��Y�����Ȃ��ꍇ�A������ɗ��Ƃ����ˉe���ŋߖT
	// (�{���Ȃ�T�C���ŋ��߂邪�A�O�ς̑傫��/�����̃x�N�g���̑傫���ŋ��܂�)
	cross_point.x = segment_sub.y * segment_point.z - segment_sub.z * segment_point.y;
	cross_point.y = segment_sub.z * segment_point.x - segment_sub.x * segment_point.z;
	cross_point.z = segment_sub.x * segment_point.y - segment_sub.y * segment_point.x;
	dir = segment_sub;
	return (((cross_point.x * cross_point.x) + (cross_point.y * cross_point.y) + (cross_point.z * cross_point.z)) 
		/ ((segment_sub.x * segment_sub.x) + (segment_sub.y * segment_sub.y) + (segment_sub.z * segment_sub.z)));
}

//******************************************************
//OBB�@vs�@OBB
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
//	//	��]
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
//	// ������ v1_1
//	r1 = sqrtf((v1_1.x * v1_1.x) + (v1_1.y * v1_1.y) + (v1_1.z * v1_1.z));
//	r2 = LenSegOnSepAxis(&Nv1_1, &v1_2, &v2_2, &v3_2);
//	dot = ((interval.x * Nv1_1.x) + (interval.y * Nv1_1.y) + (interval.z * Nv1_1.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// ������ v2_1
//	r1 = sqrtf((v2_1.x * v2_1.x) + (v2_1.y * v2_1.y) + (v2_1.z * v2_1.z));
//	r2 = LenSegOnSepAxis(&Nv2_1, &v1_2, &v2_2, &v3_2);
//	dot = ((interval.x * Nv2_1.x) + (interval.y * Nv2_1.y) + (interval.z * Nv2_1.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// ������ v3_1
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
//	// ������ v1_2
//	r1 = LenSegOnSepAxis(&Nv1_2, &v1_1, &v2_1, &v3_1);
//	r2 = sqrtf((v1_2.x * v1_2.x) + (v1_2.y * v1_2.y) + (v1_2.z * v1_2.z));
//	dot = ((interval.x * Nv1_2.x) + (interval.y * Nv1_2.y) + (interval.z * Nv1_2.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// ������ v2_2
//	r1 = LenSegOnSepAxis(&Nv2_2, &v1_1, &v2_1, &v3_1);
//	r2 = sqrtf((v2_2.x * v2_2.x) + (v2_2.y * v2_2.y) + (v2_2.z * v2_2.z));
//	dot = ((interval.x * Nv2_2.x) + (interval.y * Nv2_2.y) + (interval.z * Nv2_2.z));
//	l = fabs(dot);
//	if (l > r1 + r2)
//	{
//		return false;
//	}
//
//	// ������ v3_2
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
//	// ������ cross v1_1 + v1_2
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
//	// ������ cross v1_1 + v2_2
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
//	// ������ cross v1_1 + v3_2
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
//	// ������ cross v2_1 + v1_2
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
//	// ������ cross v2_1 + v2_2
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
//	// ������ cross v2_1 + v3_2
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
//	// ������ cross v3_1 + v1_2
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
//	// ������ cross v3_1 + v2_2
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
//	// ������ cross v3_1 + v3_2
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
	// 3�̓��ς̐�Βl�̘a�œ��e���������v�Z
    // ������Sep�͕W��������Ă��邱��
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
//��`�@vs�@��`
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
//�~�@vs�@�~
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
//�J�v�Z���@vs�@�~
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
/// 3�������W��̐�����3�p�|���S�����������Ă邩�𔻒�
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
/// �|���S����ɓ_���܂܂�邩�𔻒�
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
/// 3�������W��̐����ƕ��ʂ̌�_���W�����߂�
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
/// ����_���畽�ʂ܂ł̋���
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
/// 3�������W��̐����ƕ��ʂ��������Ă邩�𔻒�
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


// ���C�ƃ��f���̌������� 
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
	// ���[���h��Ԃ̃��C�̒��� 
	DirectX::XMStoreFloat(&result.distance, WorldRayLength);
	bool hit = false;

	//const ModelResource* resource = model->GetResource();
	for (const SkinnedMesh::Mesh& mesh : model->GetMeshs())
	{
		// ���b�V���m�[�h�擾 
		//const SkinnedMesh::Mesh& mesh = a.GetKeyframe().nodes.at(mesh.nodeIndex);//model->GetNodes().at(mesh.nodeIndex);
		//mesh.defaultGlobalTransform
		// ���C�����[���h��Ԃ��烍�[�J����Ԃ֕ϊ� 
	
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
	
		// ���C�̒��� 
		float neart;
		DirectX::XMStoreFloat(&neart, Length);
	
		// �O�p�`�i�ʁj�Ƃ̌������� 
		const std::vector<SkinnedMesh::Vertex>& vertices = mesh.vertices;
		const std::vector<UINT> indices = mesh.indices;
		int materialIndex = -1;
	
		DirectX::XMVECTOR HitPosition;
		DirectX::XMVECTOR HitNormal;
		for (const SkinnedMesh::Mesh::Subset& subset : mesh.subsets)
			// �O�p�`�𒊏o����
		{
			for (UINT i = 0; i < subset.indexCount; i += 3)
			{
				UINT index = subset.startIndexLocation + i;
				// �O�p�`�̒��_�𒊏o 
				const SkinnedMesh::Vertex& a = vertices.at(indices.at(index));
				const SkinnedMesh::Vertex& b = vertices.at(indices.at(index + 1));
				const SkinnedMesh::Vertex& c = vertices.at(indices.at(index + 2));
				DirectX::XMVECTOR A = DirectX::XMLoadFloat3(&a.position);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&b.position);
				DirectX::XMVECTOR C = DirectX::XMLoadFloat3(&c.position);
				// �O�p�`�̎O�Ӄx�N�g�����Z�o 
				DirectX::XMVECTOR AB = DirectX::XMVectorSubtract(B, A);
				DirectX::XMVECTOR BC = DirectX::XMVectorSubtract(C, B);
				DirectX::XMVECTOR CA = DirectX::XMVectorSubtract(A, C);
				// �O�p�`�̖@���x�N�g�����Z�o 
				DirectX::XMVECTOR Normal = DirectX::XMVector3Cross(AB, BC);
				// ���ς̌��ʂ��v���X�Ȃ�Η����� 
				DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Dir, Normal);
				float dot;
				DirectX::XMStoreFloat(&dot, Dot);
				if (dot >= 0) continue;
				// ���C�ƕ��ʂ̌�_���Z�o 
				DirectX::XMVECTOR SA = DirectX::XMVectorSubtract(A, Start);
				// ����Z                                                D2         D1
				DirectX::XMVECTOR X = DirectX::XMVectorDivide(DirectX::XMVector3Dot(Normal, SA), Dot);
				float t;
				DirectX::XMStoreFloat(&t, X);
				if (t < .0f || t > neart) continue;
				// ��_�܂ł̋��������܂łɌv�Z�����ŋߋ������ 
				// �傫�����̓X�L�b�v 
				DirectX::XMVECTOR Position = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(Dir, X), Start);
	
				// ��_���O�p�`�̓����ɂ��邩���� 
			   //  �P�� 
				DirectX::XMVECTOR V1 = DirectX::XMVectorSubtract(A, Position); DirectX::XMVECTOR Cross1 = DirectX::XMVector3Cross(V1, AB);
				DirectX::XMVECTOR Dot1 = DirectX::XMVector3Dot(Cross1, Normal); float dot1;
				DirectX::XMStoreFloat(&dot1, Dot1);
				if (dot1 < 0.0f) continue;
				// �Q�� 
				DirectX::XMVECTOR V2 = DirectX::XMVectorSubtract(B, Position); DirectX::XMVECTOR Cross2 = DirectX::XMVector3Cross(V2, BC);
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(Cross2, Normal); float dot2;
				DirectX::XMStoreFloat(&dot2, Dot2);
				if (dot2 < 0.0f) continue;
				// �R�� 
				DirectX::XMVECTOR V3 = DirectX::XMVectorSubtract(C, Position); DirectX::XMVECTOR Cross3 = DirectX::XMVector3Cross(V3, CA);
				DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(Cross3, Normal); float dot3;
				DirectX::XMStoreFloat(&dot3, Dot3);
				if (dot3 < 0.0f) continue;
				// �ŋߋ������X�V
				neart = t;
				// ��_�Ɩ@�����X�V 
				HitPosition = Position;
				HitNormal = Normal;
				materialIndex = subset.materialUniqueId;
			}
		}
		if (materialIndex >= 0)
		{
			// ���[�J����Ԃ��烏�[���h��Ԃ֕ϊ� 
			DirectX::XMVECTOR WorldPosition = DirectX::XMVector3TransformCoord(HitPosition, WorldTransform);
			DirectX::XMVECTOR WorldCrossVec = DirectX::XMVectorSubtract(WorldPosition, WorldStart);
			DirectX::XMVECTOR WorldCrossLength = DirectX::XMVector3Length(WorldCrossVec);
			float distance;
			DirectX::XMStoreFloat(&distance, WorldCrossLength);
			// �q�b�g���ۑ� 
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

	//	���[���h�ϊ��s��擾
	DirectX::XMMATRIX worldM;
	worldM = GetWorldMatrix();

	/*if (bSolid) color.w = 0.5f;
	else color.w = 1.0f;*/

	//	Matrix -> Float4x4 �ϊ�
	DirectX::XMFLOAT4X4 wvp;
	DirectX::XMFLOAT4X4 world;

	DirectX::XMStoreFloat4x4(&wvp, worldM * view * projection);
	DirectX::XMStoreFloat4x4(&world, worldM);

	//	�`��
	if (geometricPrimitive)
	{
		geometricPrimitive->Render(immediateContext.Get(), wvp, world, lightDirection, color, bSolid);
	}
#endif
}