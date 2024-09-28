#pragma once

//#include "../Meshs/DxBoneTransOptimize.h"
#include "../DxMeshs/SkinMesh/DxJointData.h"
#include "../DxMeshs/SkinMesh/DxJiggleBone.h"
#include "./NSPhysX_Def.h"

class DxClothColl;
struct SMATERIAL_PIECE;

namespace physx
{
	class PxShape;
	class PxCloth;
	struct PxClothCollisionSphere;
}

namespace NSPhysX
{
	BOOL IsPhysX();

	void InitNx_AvoidUnknownError( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* pAppName );
	void InitNx( LPDIRECT3DDEVICEQ pd3dDevice );
	void ReleaseNx();
	void Render( float fElapsedTime );

	BOOL CreateActorBox( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vHalfDir );
	BOOL CreateActorSphere( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, float fRadius );
	BOOL CreateActorCapsule( physx::PxRigidActor** ppActor, const D3DXVECTOR3& vPos, float fRadius, float fHalfHeight );
	void DeleteActor( physx::PxRigidActor* pNxActor );
	BOOL CreateJoint( const VEC_BONE_TRANS_JOINT_DATA& vecBoneTrans, const JiggleBoneData::VEC_JOINTDATA& vecJointData, VEC_NXACTOR& vecNxActor, VEC_NXJOINT& vecNxJoint, float fScale, BOOL bCollision );
	void DeleteJoint( physx::PxJoint* pNxJoint );

	//////////////////////////////////////////////////////////////////////////
	void GetMatrix( D3DXMATRIX& matOUT, physx::PxRigidActor* pActor );
	void GetMatrix( D3DXMATRIX& matOUT, physx::PxRigidActor* pTrans, physx::PxRigidActor* pRotate );

	//////////////////////////////////////////////////////////////////////////
	physx::PxCloth* CreateCloth
	( 
		LPD3DXMESH pMesh, 
		const D3DXMATRIX* arrayBoneMatrices, 
		DWORD dwNumInfl,
		DWORD dwNumPaletteEntries,
		const SMATERIAL_PIECE& sMaterialSystem,
		CONST D3DVERTEXELEMENT9 *pDeclaration,
		float fAttParentBoneScale, 
		const D3DXMATRIX& matWorld, 
		const float* arrayInvWeight,
		std::vector<LockParticle>& vecLockParticle,
		const DxClothColl* rClothColl
	);

	void DeleteCloth( physx::PxCloth** ppNxCloth );

	void UpdateCloth
	(
		LPD3DXMESH pMesh, 
		CONST D3DVERTEXELEMENT9 *pDeclaration, 
		DWORD dwNormalOffset,
		const WORD* pIndices, 
		physx::PxCloth* pCloth, 
		const D3DXMATRIX& matWorld,
		const float* arrayInvWeight,
		const std::vector<LockParticle>& vecLockParticle,
		const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres
	);

	//////////////////////////////////////////////////////////////////////////
	void DrawBox( physx::PxShape* box, const D3DXMATRIX& matWorld, DWORD dwColor );
	void DrawSphere( physx::PxShape* sphere, const D3DXMATRIX& matWorld, DWORD dwColor );
	void DrawCapsule( physx::PxShape* Capsule, const D3DXMATRIX& matWorld, DWORD dwColor );
}