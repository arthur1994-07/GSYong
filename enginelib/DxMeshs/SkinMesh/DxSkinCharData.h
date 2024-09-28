//
//	[class DxSkinCharData], (2002.12.09), JDH.
//
//
#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <vector>

#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "DxSkinAnimationManager.h"
#include "DxAttBone.h"
#include "DxAttBoneData.h"
#include "DxAttBoneLink.h"
#include "./DxSkinAniControlData.h"

#include "../../../SigmaCore/File/BaseStream.h"

class DxFootColl;
class DxJiggleBone;
class DxJiggleBoneColl;

#define FG_BONEFILE_CHECK	(0x0001)

// abl 로드 후 abf 로드 그다음 abl 셋팅하려고 하면 crash 생기는 문제 수정함.
// 새로이 로드 되는 것은 다시 생각 해봐야 할 듯하다.
#define FG_MUSTNEWLOAD		(0x0002)

enum EMSCD2_FLAGS
{
	EMSCD2_WORLD_COLOR = 0x01,	// 월드컬러를 사용하도록 한다.
};

class DxSkinCharData
{
public:
	enum 
	{ 
		VERSION = 0x0108, 
	};

private:
	enum		{ FILE_LENGTH = 64 };
    std::string	m_strFileName;

	std::string			m_strPIECE[PIECE_SIZE];
	//PDXATTBONELINK		m_pAttBoneArray[PIECE_SIZE];

	VEC_TSTRING			m_vecANIFILE;

	boost::shared_ptr<DxSkinAniControlData>	m_spSkinAniControlData;

public:
	float				m_fScale;
	float				m_fHeight;
	float				m_fRadius;
	float				m_fHeightYPos;
	
	DWORD				m_dwRef;
	D3DXVECTOR3			m_vMax, m_vMin;

	BOOL				m_bWorldObj;
	DWORD				m_dwFlags;		// EMSCD2_FLAGS

	// 무기 scale
	float				m_arrayfWeaponScale[EMPEACE_WEAPON_SIZE];

public:
    const std::string& GetFileName() const { return m_strFileName; }
    __forceinline void AddRef() { ++m_dwRef; }
    __forceinline void ReleaseRef() { --m_dwRef; }
    __forceinline DWORD GetRef() const { return m_dwRef; }
    void SetPiece(EMPIECECHAR eType, const TCHAR* pPieceName) { m_strPIECE[eType] = pPieceName; }
    const char* GetPiece(EMPIECECHAR eType) { return m_strPIECE[eType].c_str(); }

public:
	boost::shared_ptr<DxSkinAniControlData>	GetSkinAniControlData()	const;
	const TCHAR*							GetSkeletonName() const;
	std::tr1::shared_ptr< DxFootColl >		GetFootColl() const;

protected:
	BOOL LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver103( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver104( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행
	BOOL LOAD_Ver105( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver106( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
    BOOL LOAD_Ver107( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver108( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
	//	=false
    BOOL LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, bool bAniDirectLoad=false, boost::function<void (const TCHAR*)> pArchive = 0);
	BOOL SaveFile(const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive = 0);

//protected:
//	 HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	// [shhan][2014.12.08] chf 에 있는 cfg 에 들어있는 bin 파일들을 Thread 로 하든 아니든 파일 로딩을 하도록 한다.
	//						목적 : 자신의 캐릭터가 애니메이션이 없어서 알맞은 동작을 못하는 문제 해결.
	void LoadBIN_HaveToAnimationForMyChar( BOOL bAniThread );
	void LoadBIN_HaveToAnimationForMyChar( EMANI_MAINTYPE emANIMTYPE, EMANI_SUBTYPE emANISTYPE, BOOL bAniThread );

public:
	//HRESULT SetPiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, const BOOL bThread, DWORD dwFlag = 0x0, BOOL bAttachSub = FALSE );
	//void ResetPiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i );
	DxSkinPieceBase* GetPiece( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags=EMSCD_ALL_THREAD );
	//PDXATTBONELINK  GetAttachBone ( int i )	{ return m_pAttBoneArray[i]; }
	void SaveBinFile( const TSTRING& strNameCFG );
	void SaveBinFileAll();
	void SetRadiusHeight();
	void RenderAABBBox( LPDIRECT3DDEVICEQ pd3dDevice );
	void CleanUp();
	HRESULT ClearAll();

	// Edit
public:
	HRESULT SetPieceEDIT( const TCHAR* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag );
	void ResetPieceEDIT ( LPDIRECT3DDEVICEQ pd3dDevice, int i );
	BOOL LoadAnimationFromTool ( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	void ReleaseAnimationFromTool ( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim );

public:
	DxSkinCharData ();
	~DxSkinCharData ();
};


class DxSkinCharDataContainer
{
private:
    typedef std::tr1::unordered_map<std::string, DxSkinCharData*> SKINCHARDATAMAP;
	typedef SKINCHARDATAMAP::iterator	                          SKINCHARDATAMAP_ITER;
    typedef SKINCHARDATAMAP::value_type	                          SKINCHARDATAMAP_VALUE;

protected:
	char			m_szPath[MAX_PATH];

	SKINCHARDATAMAP	m_mapSkinCharData;

public:
	void  SetPath(char* szPath) { StringCchCopy( m_szPath, MAX_PATH, szPath ); }
	char* GetPath() { return m_szPath; }

public:
    DxSkinCharData* FindData(const std::string& FileName);	
    DxSkinCharData* LoadData(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice);

    void ReleaseData(const std::string& FileName);
    BOOL DeleteData(const std::string& FileName);

	HRESULT CleanUp ();

	void CreateChfBillboard( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* FileName );

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

protected:
	DxSkinCharDataContainer ();

public:
	~DxSkinCharDataContainer ();

public:
	static DxSkinCharDataContainer& GetInstance();
};

