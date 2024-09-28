#pragma once

#include <string>
#include <boost/tr1/unordered_map.hpp>

#include "./_new/DxSkinMesh9.h"
#include "DxSkinObject.h"
#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "DxAttBone.h"
#include "DxAttBoneData.h"
#include "DxSkinPieceBase.h"
#include "DxCharPart.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"

#include "../../DxEffect/EffAni/DxEffAniPlayer.h"
#include "../../DxEffect/EffAni/DxEffAni.h"
#include "../../DxEffect/EffKeep/DxEffKeepPlayer.h"

class DxClothColl;
struct DxAttBoneLinkThreadNeedData;


class DxAttBoneLink : public DxSkinPieceBase
{
protected:
	enum			{ FILE_LENGTH = 64 };
	static char		m_szPath[MAX_PATH];

public:
	static void		SetPath ( char* szPath );
	static char*	GetPath ()	{ return m_szPath; }

	enum { VERSION = 0x0102, };

	char			m_szFileName[FILE_LENGTH];	

public:
	
	DWORD			m_dwRef;
	std::string		m_strAttBoneName;	
	std::string		m_strPieceName;	
	std::string		m_strBaseBoneName;
    
	BOOL			m_bUseCPS;


	EMATTBONEPARRENTOBJTYPE m_emParentObjType;  // 부착될 Obj의 타입 ( Char, Vehicle 등 )
	DWORD					m_emPieceCharType;	// 헤드, 손, 몸, 오른손, 왼손 등....	// EMPIECECHAR, EMPIECEVEHICLE
	int						m_nOldAttSTEP;

	std::tr1::shared_ptr<DxAttBoneLinkThreadNeedData>	m_spThreadNeedData;

private:
	DxAttBone		m_AttBone;	
	BOOL			m_bRender;

	D3DXMATRIX		m_matWorld;

protected:
	BOOL LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	=false
    BOOL LoadABLFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags );
	BOOL SaveABLFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive = 0 );
	BOOL SetABFFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP = FALSE, int nLevel=0, EMSCD_FLAGS emFlags=EMSCD_ALL_THREAD, const char* pSelfBodyEffect=NULL );
	
public:
	const TCHAR* GetFileName() const	override	{ return m_szFileName; }
	DWORD GetSlot() const				override	{ return m_emPieceCharType; }

	const TCHAR* GetBaseBoneName() const			{ return m_strBaseBoneName.c_str(); }

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects();
	void ClearAll ();

	PDXATTBONE	 GetAttBone()			{ return &m_AttBone; }

	DWORD GetVertexNUM()				{ return m_AttBone.GetVertexNUM(); }

	void SetAttackMode ( BOOL bMode );
	void SetRender ( BOOL bRender )			{ m_bRender = bRender; }
	void SetLink ( DxAttBoneLink* pAttBoneLink, LPDIRECT3DDEVICEQ pd3dDevice, bool bAmbientUP, int nLevel=0, const char* pSelfBodyEffect=NULL );

	void SELECTANI ( EMANI_MAINTYPE MType );

public:
	HRESULT FrameMove ( const float fTime, 
						const float fElapsedTime, 
						BOOL bNoneOverFrame,
						std::tr1::shared_ptr<DxSkinAniControl>& spLinkSkinAniControl,	//const DxSkeletonMaintain *pLinkSkeleton,
						float fKeyJiggleWeight,
						const BOOL bContinue=TRUE, 
						BOOL const bFreeze=FALSE );

	HRESULT FrameMoveForTool ( const float fTime, 
								const float fElapsedTime, 
								const DxSkeletonMaintain *pLinkSkeleton );

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE
	HRESULT RenderFixed( const LPDIRECT3DDEVICEQ pd3dDevice, 
						float fAlpha,
						const BOOL bShadow, 
						const BOOL bEff, 
						const BOOL bReverse, 
						float fKeyJiggleWeight );

	HRESULT RenderShadow( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matRot, const D3DXMATRIX& matViewProj, const BOOL bDay );

	HRESULT RenderShadowForMaterialSystem( const LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX& matViewProj );

public:
	void SetPosOffset( const D3DXVECTOR3& vPosOffset );

	// const BOOL bShadow=FALSE, const BOOL bEff=TRUE, const BOOL bReverse = FALSE
	void RenderMaterialOpaque( LPDIRECT3DDEVICEQ pd3dDevice, 
								float fAlpha,
								const BOOL bShadow, 
								const BOOL bEff,
								const DxClothColl* pClothColl,
								const std::vector<physx::PxClothCollisionSphere>* pvecClothCollSpheres,
								float fAttParentBoneScale );

	void RenderMaterialHardAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );
	void RenderMaterialSoftAlpha( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );

	void RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha );

	void RenderMaterialAdd( LPDIRECT3DDEVICEQ pd3dDevice );
	void RenderMaterialGlow( LPDIRECT3DDEVICEQ pd3dDevice );

	BOOL RenderMaterialGhosting( LPDIRECT3DDEVICEQ pd3dDevice, float fAlpha, size_t unGhostingIndex );

	void RenderCpsForEdit( LPDIRECT3DDEVICEQ pd3dDevice, DxSkeleton* pAttBoneLinkSkeleton );

	void GhostingON( BOOL bGhosting, float fLifeTime );

	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

	//////////////////////////////////////////////////////////////////////////
	// 외부 옵션에 따른 ON/OFF 로 인한 내부 Data 변화시 호출 함수 및 변수
public:
	// 이곳은 매 프레임 들어오도록 한다.
	BOOL SetPhysX_Cloth( BOOL bOnOff );



public:
	DxAttBoneLink(void);
	virtual ~DxAttBoneLink(void);
};

typedef DxAttBoneLink*	PDXATTBONELINK;






class DxAttBoneLinkContainer
{
private:
    typedef std::tr1::unordered_map<std::string, DxAttBoneLink*> SKINATTBONELINKMAP;
	typedef SKINATTBONELINKMAP::iterator	                     SKINATTBONELINKMAP_ITER;
    typedef SKINATTBONELINKMAP::value_type	                     SKINATTBONELINKMAP_VALUE;

protected:
	char			m_szPath[MAX_PATH];

	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	SKINATTBONELINKMAP	m_mapAttBoneLink;

public:
	void			SetPath ( char* szPath ) { StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	char*			GetPath ()	{ return m_szPath; }

	void InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    DxAttBoneLink* FindLink(const std::string& FileName);
    DxAttBoneLink* LoadLink(const std::string& FileName, EMSCD_FLAGS emFlags );

	void ReleaseLink ( const char* szFile );
	BOOL DeleteLink ( const char* szFile );

	HRESULT CleanUp ();

protected:
	DxAttBoneLinkContainer ();

public:
	~DxAttBoneLinkContainer ();

public:
	static DxAttBoneLinkContainer& GetInstance();
};