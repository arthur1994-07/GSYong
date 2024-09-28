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
	// 맵이동과 관련 이전 데이터 사라짐으로 인한 정리

	// 정리
	void DataCleanUp();




	//////////////////////////////////////////////////////////////////////////
	// Thread 생성과 삭제

	// 생성
	BOOL StartThread();

	// 삭제는 캐릭터 지우기 전에 해줘야한다.
	void EndThread();




	//////////////////////////////////////////////////////////////////////////
	// 동작을 위해 매 프레임 해줘야 하는 것들.

	// 모든 정보가 COMPLETE_CHAR_DATA 될 때 까지 기다리도록 한다.
	void Wait_CompleteSkinAniControl( float fElapsedTime, BOOL bNotRendering=FALSE );

	// CharEditer 에서만 쓰인다.
	// 이걸 사용하려면 LoadThread 동작되고 있으면 안된다.
	// EndThread() 를 꼭 해놔야한다.
	void CalcluateSkinAniControlForCharEditer( float fElapsedTime );




	//////////////////////////////////////////////////////////////////////////
	// SkinAniControl 등록

	// 등록만 한다.
	void AddAniControl( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
						std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
						const D3DXMATRIX& matLocalRot,
						BOOL bWait );

	// 등록만 한다.
	void AddAniControl_Vehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
								std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
								const D3DXMATRIX& matLocalRot,
								BOOL bWait );

	// 등록만 한다.
	void AddAniControl_AttBone( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
								std::tr1::shared_ptr<DxSkinAniControl>&	spLinkSkinAniControl, 
								std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>& spThreadNeedData,
								float fKeyJiggleWeight,
								float fScale,
								BOOL bWait );

	// 등록만 한다.
	void AddAniControl_CharOnVehicle( std::tr1::shared_ptr<DxSkinAniControl>&	spSkinAniControl, 
									std::tr1::shared_ptr<DxSkinAniControlThreadNeedData>& spThreadNeedData,
									std::tr1::shared_ptr<DxVehicleThreadNeedData>& spVehicleNeedData,
									DWORD dwPassengerID,
									BOOL bWait );




	//////////////////////////////////////////////////////////////////////////
	// CharPart 쪽 BoneMatrices 등록
	void AddCharPartBoneMatrices( std::tr1::shared_ptr<CharPartBoneMatrices>& spCharPartBoneMatrices, BOOL bWait );




	//////////////////////////////////////////////////////////////////////////
	// Name Collision

	// Thread 상으로 넘긴다.
	void AddNameCollision( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );
	void AddCollisionByObject( std::tr1::shared_ptr<ActorNameCollision>& spNameColl, const D3DXVECTOR3& vStart, const D3DXVECTOR3& vEnd );
	void AddActorWorldLight( std::tr1::shared_ptr<ActorWorldLight>& spActorWorldLight, const D3DXVECTOR3& vPos );
};