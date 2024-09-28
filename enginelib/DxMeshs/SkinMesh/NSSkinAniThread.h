#pragma once

class DxLandMan;
class CharPartBoneMatrices;
class DxSkinAniControl;
class DxSkinAniControlThreadNeedData;
struct ActorNameCollision;
struct ActorWorldLight;
struct DxAttBoneLinkThreadNeedData;
struct DxVehicleThreadNeedData;


namespace NSSkinAniThread
{
	//////////////////////////////////////////////////////////////////////////
	// ���̵��� ���� ���� ������ ��������� ���� ����

	// ����
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread ������ ����

	// ����
	BOOL StartThread();

	// ������ ĳ���� ����� ���� ������Ѵ�.
	void EndThread();




	//////////////////////////////////////////////////////////////////////////
	// ������ ���� �� ������ ����� �ϴ� �͵�.

	// ��� ������ COMPLETE_CHAR_DATA �� �� ���� ��ٸ����� �Ѵ�.
	void Wait_CompleteSkinAniControl( float fElapsedTime, BOOL bNotRendering=FALSE );

	// CharEditer ������ ���δ�.
	// �̰� ����Ϸ��� LoadThread ���۵ǰ� ������ �ȵȴ�.
	// EndThread() �� �� �س����Ѵ�.
	void CalcluateSkinAniControlForCharEditer( float fElapsedTime );




	//////////////////////////////////////////////////////////////////////////
	// SkinAniControl ���

	// ��ϸ� �Ѵ�.
	void AddAniControl( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
						std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
						const D3DXMATRIX& matLocalRot,
						BOOL bWait );

	// ��ϸ� �Ѵ�.
	void AddAniControl_Vehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
								std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
								const D3DXMATRIX& matLocalRot,
								BOOL bWait );

	// ��ϸ� �Ѵ�.
	void AddAniControl_AttBone( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControl>&	spLinkSkinAniControl, 
								std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData,
								float fKeyJiggleWeight,
								float fScale,
								BOOL bWait );

	// ��ϸ� �Ѵ�.
	void AddAniControl_CharOnVehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
									std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
									std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
									DWORD dwPassengerID,
									BOOL bWait );




	//////////////////////////////////////////////////////////////////////////
	// CharPart �� BoneMatrices ���
	void AddCharPartBoneMatrices( std::tr1::shared_ptr<CharPartBoneMatrices>& spCharPartBoneMatrices, BOOL bWait );




	//////////////////////////////////////////////////////////////////////////
	// Name Collision

	// Thread ������ �ѱ��.
	void AddNameCollision( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );
	void AddCollisionByObject( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );
	void AddActorWorldLight( std::tr1::shared_ptr<ActorWorldLight>& spActorWorldLight, const D3DXVECTOR3& vPos );
};