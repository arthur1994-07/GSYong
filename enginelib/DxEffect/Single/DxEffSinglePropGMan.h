#pragma once

#include <boost/function.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/tr1/memory.hpp>
#include <string>

#include "../../../SigmaCore/CoreCommon.h"
#include "../../../InternalCommonLib/dxstdafx.h"

#include "../../DxSound/MovableSound.h"
//#include <hash_map>

namespace sc {
    class BaseStream;
    class SerialFile;
}

class DxEffSingle;
class DxEffSingleGroup;

#ifndef PDXEFFSINGLE
	typedef DxEffSingle* PDXEFFSINGLE;
#endif // PDXEFFSINGLE

#ifndef SPDXEFFSINGLEGROUP
	typedef boost::shared_ptr< DxEffSingleGroup > SPDXEFFSINGLEGROUP;
#endif // SPDXEFFSINGLEGROUP

//enum EFFECTFLAG		// 0x00000001 버젼
//{
//	EFF_PLY_PLAY		= (0x00010),	//	EFFECT는 Play 중.
//	EFF_PLY_DONE		= (0x00020),	//	EFFECT는 활동시간이 끝낫음.
//	EFF_PLY_END			= (0x00040),	//	EFFECT는 종료 할 준비가 됨.
//	EFF_PLY_RENDPASS	= (0x00100),	//	EFFECT는 그려지지 않고 지나감.
//
//	EFF_CFG_NEVERDIE	= (0x01000),			//	EFFECT는 죽지 않는 속성을 가진다.		// 체크 
//	EFF_CFG_TARGETPOS	= (0x02000),			//	EFFECT는 타겟 위치를 가진다.			// 라디오
//	EFF_CFG_TARGETID	= (0x04000),			//	EFFECT는 타겟 'ID'를 가진다.			// 라디오
//	EFF_CFG_MOVE		= (0x08000),			//	EFFECT는 실행물체를 따라 다닌다.		// 라디오
//	EFF_CFG_NONE		= (0x0f000),가 아닐때	//  EFFECT는 제자리에서 실행				// 라디오 ( 디폴트 )
//
//	EFF_CFG_EVENT		= (0x10000),	//	EFFECT는 event 가 발생시에 활성화 (EFF_SLEEP->EFF_BEGIN).
//
//	EFF_CFG_USEDTARGET	= (EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE),
//	EFF_CFG_NONE		= (EFF_CFG_NEVERDIE|EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE),
//};

enum EFFECTFLAG
{
	EFF_PLY_PLAY		= (0x0000010),	//	EFFECT는 Play 중.
	EFF_PLY_DONE		= (0x0000020),	//	EFFECT는 활동시간이 끝낫음.
	EFF_PLY_END			= (0x0000040),	//	EFFECT는 종료 할 준비가 됨.
	EFF_PLY_RENDPASS	= (0x0000100),	//	EFFECT는 그려지지 않고 지나감.

	EFF_CFG_TARGETLINE			= (0x0000200),	//  EFFECT는 타겟까지의 선
	EFF_CFG_TAGERT_CAMERA_MOVE	= (0x0000400),	//  EFFECT가 타겟 카메라까지 이동.

	EFF_CFG_NONE		= (0x0001000),	//  EFFECT는 제자리에서 실행				// 라디오 ( 디폴트 )
	EFF_CFG_TARGETPOS	= (0x0002000),	//	EFFECT는 타겟 위치를 가진다.			// 라디오
	EFF_CFG_TARGETID	= (0x0004000),	//	EFFECT는 타겟 'ID'를 가진다.			// 라디오
	EFF_CFG_MOVE		= (0x0008000),	//	EFFECT는 실행물체를 따라 다닌다.		// 라디오 (안쓰임)

	EFF_CFG_NEVERDIE	= (0x0010000),	//	EFFECT는 죽지 않는 속성을 가진다.		// 체크 

	EFF_CFG_AVERAGE		= (0x1000000),	//	EFFECT는 일반적인 속도
	EFF_CFG_SLOWFAST	= (0x2000000),	//	EFFECT는 점점 빨라진다.
	EFF_CFG_FASTSLOW	= (0x4000000),	//	EFFECT는 점점 느려진다.
	EFF_CFG_ACCEL		= (0xf000000),	//	속도 속성을 다 지워줄때 쓰인다.

	EFF_CFG_EVENT		= (0x0100000),	//	EFFECT는 event 가 발생시에 활성화 (EFF_SLEEP->EFF_BEGIN).

	EFF_CFG_AUTOAABBBOX	= (0x10000000),	//	자동적으로 AABBBOX를 만든다.

	EFF_CFG_USEDTARGET	= (EFF_CFG_TARGETPOS|EFF_CFG_TARGETID|EFF_CFG_MOVE|EFF_CFG_TARGETLINE),
};


//	Note : 개별 이펙트들의 설정값과 개별 리소스 자원들을 가짐
//			이 struct를 개별 이펙트의 제작시에 상속 받아서 이에 설정값 등을 넣어야 한다.
//			예를 들어 "struct PARTICLESYS_PROPERTY : public EFF_PROPERTY" 와 같이 사용될 것이다.
//
struct EFF_PROPERTY
{
	BOOL		m_bMoveObj;
	float		m_fGBeginTime;
	float		m_fGLifeTime;
	SMovSound	m_MovSound;	//	사운드	
	D3DXMATRIX	m_matLocal;

	DWORD	GetSizeBase ();

	EFF_PROPERTY *m_pChild;
	EFF_PROPERTY *m_pSibling;

	virtual DWORD GetTypeID () = 0;
	virtual const char* GetName () = 0;

	virtual DxEffSingle* NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup ) = 0;

	virtual void importMobileObjectFromXFile();

	virtual HRESULT LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );	//	(pChild,pSibling) 재귀 호출용.
	virtual HRESULT SaveFile ( sc::SerialFile &SFile );								//	(pChild,pSibling) 재귀 호출용.

	void	CloneEffProp	( LPDIRECT3DDEVICEQ pd3dDevice, EFF_PROPERTY* pSrcProp, BOOL bBackUpCache );

public:
	EFF_PROPERTY () :
		m_bMoveObj(FALSE),
		m_fGBeginTime(0.0f),
		m_fGLifeTime(4.0f),
		m_pChild(NULL),
		m_pSibling(NULL)
	{
		D3DXMatrixIdentity( &m_matLocal );
	}

	virtual ~EFF_PROPERTY()
	{
		CleanUp ();

		SAFE_DELETE(m_pChild);
		SAFE_DELETE(m_pSibling);
	}

	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache );
	//virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	//virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();

	virtual HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache )
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice, bBackUpCache )) )
			return E_FAIL;

		//if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
		//	return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		//InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

	//	Note : 정적 멤버.
	//
	static EFF_PROPERTY* NEW_PROP ( DWORD TypeID );
};
typedef EFF_PROPERTY* PEFF_PROPERTY;


//	Note : 이펙트의 설정값을 가지고 있는 "EFF_PROPERTY" 들을 트리 구조로 가지고 있음
//			이것이 하나의 파일 단위로 SAVE/LOAS 되며 이펙트 개체로 생성시에 하나의 단위로 설정됨
//
struct EFF_PROPGROUP
{
public:
	const static DWORD	VERSION;
	const static DWORD	FILEVERSION;

public:
	std::string			m_strFileName;
	BOOL				m_bBackUpCache;	// ResourceManager 에서 BackUpCache 로 작동되도록 한다.
	DWORD				m_dwFlag;		// EFFECTFLAG
	D3DXVECTOR3			m_vLocal;
	D3DXMATRIX			m_matWorld;
	EFF_PROPERTY*		m_pPropRoot;

public:
	D3DXVECTOR3 m_vMax;			//	AABB에 사용.
	D3DXVECTOR3 m_vMin;			//	AABB에 사용.

public:
	HRESULT DeleteProp ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp );
	HRESULT GetMatCombParent ( PEFF_PROPERTY &pPropHead, PEFF_PROPERTY pProp, D3DXMATRIX &matParent, D3DXMATRIX &matComb );
	BOOL	IsFlags( EFFECTFLAG emFlag );

private:
	HRESULT LoadFile( sc::BaseStream& stream, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bOldDecalUSE );

public:
	void importMobileObjectFromXFile();

    HRESULT LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, boost::function<void (const TCHAR*)> pArchive = 0);
	BOOL	LoadFile( const std::string& FileName, void* pBuf,UINT bufSize, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache );
	HRESULT SaveFile ( const char* szFileName, boost::function<void (const TCHAR*)> pArchive = 0 );

	HRESULT LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0101 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );
	HRESULT LoadFile_0106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bBackUpCache, BOOL bOldDecalUSE );	//	데이터 포맷 변경 진행


	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	//HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	//HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

	HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )
			return E_FAIL;

		//if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )
		//	return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		//InvalidateDeviceObjects();

		DeleteDeviceObjects();

		return S_OK;
	}

	EFF_PROPGROUP ()
		: m_pPropRoot(NULL)
		, m_vMax(15,30,15)
		, m_vMin(-15,0,-15)
		, m_vLocal(0.f,0.f,0.f)
		, m_bBackUpCache(FALSE)
	{
		m_dwFlag = 0L;
		m_dwFlag |= EFF_CFG_NONE;
		m_dwFlag |= EFF_CFG_AVERAGE;
		m_dwFlag |= EFF_CFG_AUTOAABBBOX;
		
		D3DXMatrixIdentity( &m_matWorld );
	}

	~EFF_PROPGROUP ()
	{
		CleanUp ();

		SAFE_DELETE(m_pPropRoot);
	}

public:
	BOOL RESET_MOVEOBJ_PROP ( PEFF_PROPERTY &pPropHead, EFF_PROPERTY &pCurEffProp );

protected:
	BOOL NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, SPDXEFFSINGLEGROUP pThisGroup );

	//	Note : 단일 이팩트만 화면에 나오게 하는 메소드. 모든 이펙트 생성후 REND 안함.
	BOOL NEWEFFSINGLE ( PEFF_PROPERTY &pCurEffProp, PDXEFFSINGLE &pCurEffSingle, SPDXEFFSINGLEGROUP pThisGroup, EFF_PROPERTY* pTargetProp );

public:
	SPDXEFFSINGLEGROUP NEWEFFGROUP ();
	
	//	Note : 단일 이팩트만 화면에 나오게 하는 메소드. 모든 이펙트 생성후 REND 안함.
	SPDXEFFSINGLEGROUP NEWEFFGROUP ( EFF_PROPERTY* pTargetProp );

	//	Note : 단일 이팩트만 화면에 나오게 하는 메소드.
	SPDXEFFSINGLEGROUP NEWEFFSINGLE ( EFF_PROPERTY* pTargetProp );
};
//
//
////! 싱글톤 패턴으로 되어 있다. (단일 개체로 존재한다.)
////! 이펙트 그룹을 파일에서 읽어서 리스트에 삽입하며 
////! 이 같은 이펙트 그룹들을 메모리에 올리고 내리는 것을 통제한다.
////! 원하는 이펙트 그룹을 찾아서 이를 원하는 곳에 연결해 준다.
//class DxEffSinglePropGMan
//{
//public:
//    typedef std::tr1::unordered_map<std::string, EFF_PROPGROUP*> MAPPROPGROUP;
//	typedef MAPPROPGROUP::iterator							     MAPPROPGROUP_ITER;
//
//protected:
//	char m_szPath[MAX_PATH];
//
//	LPDIRECT3DDEVICEQ	m_pd3dDevice;
//	MAPPROPGROUP		m_mapPROPGROUP;
//
//public:
//	void SetPath ( const char *szPath )
//	{
//		StringCchCopy( m_szPath, MAX_PATH, szPath );
//	}
//
//	const char* GetPath ()	{ return m_szPath; }
//
//public:
//	EFF_PROPGROUP* FindEffGProp ( const char* szFileName );
//	EFF_PROPGROUP* LoadEffectGProp ( const char* szFileName );
//	HRESULT DeleteEffectGProp ( const char* szFileName );
//
//public:
//	HRESULT CleanUp ()
//	{
//		InvalidateDeviceObjects();
//
//		DeleteDeviceObjects();
//
//		return S_OK;
//	}
//
//public:
//	HRESULT OneTimeSceneInit ( const char *szPath );
//	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
//	HRESULT RestoreDeviceObjects ();
//
//	HRESULT InvalidateDeviceObjects ();
//	HRESULT DeleteDeviceObjects ();
//
//protected:
//	DxEffSinglePropGMan (void);
//
//public:
//	~DxEffSinglePropGMan (void);
//
//public:
//	static DxEffSinglePropGMan& GetInstance();
//};


#include "../../ResourceManagerCacheAlgorithm.h"


//////////////////////////////////////////////////////////////////////////
//
// EFF_PROPGROUP 로드만 하고. 게임을 종료하기 전까지는 지워주지 않는다는 룰이 정해져있다.
// egp 파일 전체 파일 용량은 2015.08.18 일 현재 19.2MB 정도 된다.
// 이 룰이 깨지면 생각해야 할 것이 좀 있다.
// 일단 ParticlesForThread 쪽에서 EFF_PROPGROUP 값은 항상 존재한다라고 가정하고 사용한다.
// 
class DxEffSinglePropGMan : public OnceRefAndEnableAlgorithm<EFF_PROPGROUP,EFF_PROPGROUP_ADD_DATA,OnceRefNTime>
{
	DxEffSinglePropGMan();

public:
	struct Resource : public ResourceManager::Resource
	{
		explicit Resource(EFF_PROPGROUP* pEffPropGroup) {	m_spCompletedData = SP_COMPLETED_DATA( pEffPropGroup ); }
		Resource(const ResourceManager::Resource& res) : ResourceManager::Resource(res) {}
		Resource() {}

		EFF_PROPGROUP* get()  { return Update() ? m_spCompletedData.get() : NULL; }
	};

public:
	static DxEffSinglePropGMan& GetInstance();

	void			SetPath(const TSTRING& path)		{ m_path = path; }
	const TSTRING&	GetPath()							{ return m_path; }

	Resource LoadEffectGProp ( const TSTRING& strName, BOOL bThread, BOOL bBackUpCache );

public:
	HRESULT CleanUp ();

	// DeleteCacheDataAll 를 해도 m_spCompletedData 의 참조 값으로 인해 지워지는 것은 없을 것 이다.
	virtual void DeleteCacheDataAll() OVERRIDE;

private:
	virtual void ProcessCompleteData(
		SP_QUEUE_DATA_VECTOR& dataVec,
		NAME_N_SP_COMPLETED_DATA_VECTOR* pCompletedDataVec,
		DWORD dwPrevTick, 
		DWORD nWorkTime );

	virtual void ProcessCompleteDataBackUpCache( NAME_N_SP_COMPLETED_DATA_VECTOR& CompletedDataVec );

public:
	HRESULT OneTimeSceneInit ( const char *szPath );
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

public:
    void Archive(boost::function<void (const TCHAR*)> fun);

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;
	TSTRING				m_path;
};

typedef DxEffSinglePropGMan::Resource DxEffSinglePropGroupResource;