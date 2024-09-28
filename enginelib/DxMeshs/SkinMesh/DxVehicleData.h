#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <vector>

#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "DxAttBone.h"
#include "DxAttBoneData.h"
#include "DxAttBoneLink.h"

#include "../../../SigmaCore/File/BaseStream.h"

#define FG_BONEFILE_CHECK	(0x0001)

// abl 로드 후 abf 로드 그다음 abl 셋팅하려고 하면 crash 생기는 문제 수정함.
// 새로이 로드 되는 것은 다시 생각 해봐야 할 듯하다.
#define FG_MUSTNEWLOAD		(0x0002)

#define MAX_PASSENGER_COUNT 5
#define PASSENGER_COUNT		2

class DxFootColl;

struct SPassengerData
{
	D3DXMATRIX	matWorld;
	D3DXVECTOR3	vPos;
	D3DXVECTOR3	vRotate;
	std::string strBoneName;

	SPassengerData()
		: strBoneName("")
		, vPos( 0.0f, 0.0f, 0.0f)
		, vRotate( 0.0f, 0.0f, 0.0f)
	{
		D3DXMatrixIdentity( &matWorld );
	}

	void Init()
	{
		strBoneName = "";
		D3DXMatrixIdentity( &matWorld );
		vPos = vRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	}

	SPassengerData& operator = ( const SPassengerData &input )
	{
		matWorld	= input.matWorld;
		vPos		= input.vPos;
		vRotate		= input.vRotate;
		strBoneName = input.strBoneName;

		return *this;
	}
};

class DxVehicleData
{
public:
	DxVehicleData(void);
	~DxVehicleData(void);
public:
	enum { VERSION = 0x0103, };

private:
	enum		{ FILE_LENGTH = 64 };
	char		m_szFileName[FILE_LENGTH];

	std::string			m_strVehicle[PART_V_SIZE];
	//PDXSKINPIECE		m_V_PartsArray[PART_V_SIZE];
	//PDXATTBONELINK		m_V_AttBoneArray[PART_V_SIZE];

	VEC_TSTRING			m_vecANIFILE;	

	boost::shared_ptr<DxSkinAniControlData>	m_spSkinAniControlData;

public:
	float				m_fScale;
	float				m_fHeight;
	float				m_fRadius;

	DWORD				m_dwRef;
	D3DXVECTOR3			m_vMax, m_vMin;

	BOOL				m_bWorldObj;

	SPassengerData		m_sPassengerData[MAX_PASSENGER_COUNT];

public:
	const char* GetFileName()		{ return m_szFileName; }
    void SetVehiclePiece(int nType, const TCHAR* pPieceName) { m_strVehicle[nType] = pPieceName; }
    const char* GetVehiclePiece(int nType) { return m_strVehicle[nType].c_str(); }

public:
	boost::shared_ptr<DxSkinAniControlData>	GetSkinAniControlData()	const;
	std::tr1::shared_ptr< DxFootColl > GetFootColl() const;

protected:
	BOOL LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Foot Coll 기능 추가;
	BOOL LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Jiggle Bone 기능 추가;
	BOOL LOAD_Ver103( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// bone setting file 분리

public:
	//	=false
    BOOL LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, boost::function<void (const TCHAR*)> pArchive = 0 );
	BOOL SaveFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive = 0 );
//
//protected:
//	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT SetPieceEDIT( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag );
	void ResetPieceEDIT( int i );
	BOOL LoadAnimationFromTool ( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	void ReleaseAnimationFromTool ( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim );

	DxSkinPieceBase* GetVehiclePart ( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags=EMSCD_ALL_THREAD );
	const TSTRING& GetVehicleName( int i );
	//HRESULT SetVehiclePart ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bMeshThread, BOOL bAniThread, DWORD dwFlag = 0x0, BOOL bAttachSub = FALSE );
	//void ResetVehiclePart ( LPDIRECT3DDEVICEQ pd3dDevice, int i );
	//PDXSKINPIECE GetVehiclePart ( int i )		{ return m_V_PartsArray[i]; }
	void SetRadiusHeight();
	void RenderAABBBox( LPDIRECT3DDEVICEQ pd3dDevice );
	//PDXATTBONELINK  GetAttachBone ( int i )	{ return m_V_AttBoneArray[i]; }
	HRESULT ClearAll ();
};


class DxVehicleDataContainer
{
private:
    typedef std::tr1::unordered_map<std::string, DxVehicleData*> VEHICLEDATAMAP;
	typedef VEHICLEDATAMAP::iterator	                         VEHICLEDATAMAP_ITER;
    typedef VEHICLEDATAMAP::value_type	                         VEHICLEDATAMAP_VALUE;

protected:
	char				m_szPath[MAX_PATH];
	
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	VEHICLEDATAMAP		m_mapVehicleData;

public:
	void			SetPath ( char* szPath ) { StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	char*			GetPath ()	{ return m_szPath; }

	void InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	DxVehicleData* FindVehicleData ( const char* szFile );
	DxVehicleData* LoadVehicleData ( const char* szFile, EMSCD_FLAGS emFlags );

	void ReleaseVehicleData ( const char* szFile );
	BOOL DeleteVehicleData ( const char* szFile );

	HRESULT CleanUp ();

	// 로딩을 전부 해보도록 한다.
	void OnFindDontHaveTexture();

protected:
	DxVehicleDataContainer ();

public:
	~DxVehicleDataContainer ();

public:
	static DxVehicleDataContainer& GetInstance();
};