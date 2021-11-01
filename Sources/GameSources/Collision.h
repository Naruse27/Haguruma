#pragma once

#include <DirectXMath.h>
#include <memory>

//#include "OBJ3D.h"
#include "../GameLibSource/GeometricPrimitive.h"
#include "GameLibSource/Model.h"

// �q�b�g�v�Z
struct HitResult
{
	DirectX::XMFLOAT3 position = { 0, 0, 0 }; // ���C�ƃ|���S���̌�_
	DirectX::XMFLOAT3 normal = { 0, 0, 0 };   // �Փ˂����|���S���̖@���x�N�g��
	float distance = 0.0f;                    // ���C�̎n�_�����_�܂ł̋���
	int materialIndex = -1;                   // �Փ˂����|���S���̃}�e���A���ԍ�
	DirectX::XMFLOAT3 rotation = { 0, 0, 0 }; // ��]��
};

class Collision
{
public:
	static bool SphereVsSphere(DirectX::XMFLOAT3 pos1, DirectX::XMFLOAT3 pos2, float r1, float r2);
	static bool SphereVsCapsule(DirectX::XMFLOAT3& capsule1, DirectX::XMFLOAT3 capsule2, float capsule_radius, DirectX::XMFLOAT3 sphere, float sphere_radius);
	//bool OBBVsOBB(OBJ3D* obj1, OBJ3D* obj2);

	// 2�����ł̋�`�Ƌ�`�̏Փ˔��� & �����o�� �֐�
	static bool RectVsRectAndExtrusion(DirectX::XMFLOAT2& pos1, DirectX::XMFLOAT2 scale1, DirectX::XMFLOAT2 pos2, DirectX::XMFLOAT2 scale2);

	// 2�����ł̉~�Ɖ~�̏Փ˔��� & �����o�� �֐�
	static bool CircleVsCircleAndExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2, bool extrusion =  true);

	// 2�����ł̉~�̉����o�� �֐�
	static void CircleExtrusion(DirectX::XMFLOAT2& pos1, float radius1, DirectX::XMFLOAT2 pos2, float radius2);

	// �_�Ƌ�`�̓����蔻��
	static bool PointVsRect(const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2 pos2, const DirectX::XMFLOAT2 size2);


	// 2�����ł̃J�v�Z���Ɖ~�̏Փ˔��� & �����o�� �֐�
	// static bool CapsuleVsCircleAndExtrusion(DirectX::XMFLOAT3& nowPos1, DirectX::XMFLOAT3 oldPos1, float radius1, DirectX::XMFLOAT3 pos2, float radius2);
	
	// 3�������W��̐�����3�p�|���S�����������Ă邩�𔻒�
	bool detectIsIntersectedLineSegmentAndPolygon(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// �|���S����ɓ_���܂܂�邩�𔻒�
	bool detectPointIsEnclosedByPolygon(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3�������W��̐����ƕ��ʂ̌�_���W�����߂�
	DirectX::XMFLOAT3 calcIntersectionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ����_���畽�ʂ܂ł̋���
	float calcDistancePointAndPlane(DirectX::XMFLOAT3 p, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// 3�������W��̐����ƕ��ʂ��������Ă邩�𔻒�
	bool detectCollisionLineSegmentAndPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 v0, DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2);

	// ���C�ƃ��f���̌�������
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
	// ���̒��S�ƃJ�v�Z���̐����̋����i�̓��j���v�Z�p
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
		int type,						                             	// �Փ˔���̌`�̔ԍ��iCollisionPrimitiveType:: ���j
		bool isCreateBottom,				                // ���_�𑫌��ɂ��邩�t���O
		DirectX::XMFLOAT3 _collisionScale	// �Փ˔���̃X�P�[��
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