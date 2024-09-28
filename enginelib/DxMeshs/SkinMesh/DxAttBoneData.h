#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <map>
#include <string>
#include <vector>

#include "DxSkinAnimationManager.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkinAniControlData.h"

#include "../../../SigmaCore/File/BaseStream.h"

#define FG_BONEFILE_CHECK	(0x0001)

// abl 로드 후 abf 로드 그다음 abl 셋팅하려고 하면 crash 생기는 문제 수정함.
// 새로이 로드 되는 것은 다시 생각 해봐야 할 듯하다.
#define FG_MUSTNEWLOAD		(0x0002)

class DxAttBoneData
{
public:
	DxAttBoneData(void);
	~DxAttBoneData(void);
public:
	enum { VERSION = 0x0102, };

private:
	enum		{ FILE_LENGTH = 64 };
	char		m_szFileName[FILE_LENGTH];

	std::string			m_strATTBONE[ATTBONE_SIZE];
	//PDXSKINPIECE		m_pAttBoneArray[ATTBONE_SIZE];

	VEC_TSTRING			m_vecANIFILE;	

	boost::shared_ptr<DxSkinAniControlData>	m_spSkinAniControlData;

public:
	float				m_fScale;
	float				m_fHeight;
	float				m_fRadius;
	
	DWORD				m_dwRef;
	D3DXVECTOR3			m_vMax, m_vMin;

	BOOL				m_bWorldObj;

public:
	const char* GetFileName() const { return m_szFileName; }
    void SetAttBonePiece(int nType, const TCHAR* pPieceName) { m_strATTBONE[nType] = pPieceName; }
    const char* GetAttBonePiece(int nType) { return m_strATTBONE[nType].c_str(); }

public:
	boost::shared_ptr<DxSkinAniControlData>	GetSkinAniControlData()	const;

protected:
	BOOL LOAD_Ver100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_Ver101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// Jiggle Bone 기능 추가;
	BOOL LOAD_Ver102( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	// bone setting file 분리

public:
	//	=false
    BOOL LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlags, boost::function<void (const TCHAR*)> pArchive = 0 );
	BOOL SaveFile ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive = 0 );

//protected:
//	HRESULT CalculateBoundingBox ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );

public:
	HRESULT SetAttBonePiece ( const char* szFile, LPDIRECT3DDEVICEQ pd3dDevice, EMSCD_FLAGS emFlags, DWORD dwFlag );
	void ResetAttBonePiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i );
	BOOL LoadAnimationFromTool ( const TCHAR* szNameCHF, const TCHAR* szNameCFG, const TCHAR* szNameXFile, LPDIRECT3DDEVICEQ pd3dDevice, bool bREPRESH=false );
	void ReleaseAnimationFromTool ( LPDIRECT3DDEVICEQ pd3dDevice, const TCHAR* szAnim );

	DxSkinPiece* GetAttBonePiece ( LPDIRECT3DDEVICEQ pd3dDevice, int i, EMSCD_FLAGS emFlags=EMSCD_ALL_THREAD );	//		{ return m_pAttBoneArray[i]; }
	const TSTRING& GetAttBoneName( int i );
	void SetRadiusHeight();
	void RenderAABBBox( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT ClearAll ();
};


class DxAttBoneDataContainer
{
private:
    typedef std::tr1::unordered_map<std::string, DxAttBoneData*> ATTBONEDATAMAP;
	typedef ATTBONEDATAMAP::iterator	                         ATTBONEDATAMAP_ITER;
    typedef ATTBONEDATAMAP::value_type	                         ATTBONEDATAMAP_VALUE;

protected:
	char				m_szPath[MAX_PATH];

	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	ATTBONEDATAMAP		m_mapAttBoneData;

public:
	void SetPath(char* szPath) { StringCchCopy(m_szPath, MAX_PATH, szPath); }
	char* GetPath() { return m_szPath; }

	void InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
    DxAttBoneData* FindAttBoneData(const std::string& FileName);
    DxAttBoneData* LoadAttBoneData(const std::string& FileName, EMSCD_FLAGS emFlags );

	void ReleaseAttBoneData ( const char* szFile );
	BOOL DeleteAttBoneData ( const char* szFile );

	HRESULT CleanUp ();

	// 로딩을 전부 해보도록 한다.
	void OnFindDontHaveTexture();

protected:
	DxAttBoneDataContainer ();

public:
	~DxAttBoneDataContainer ();

public:
	static DxAttBoneDataContainer& GetInstance();
};
