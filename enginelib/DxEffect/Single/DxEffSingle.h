#pragma once

#include <string>
#include <list>

#include "../../../SigmaCore/Memory/ObjectPool.h"
#include "../../DxTools/Collision.h"
#include "../../DxSound/MovableSound.h"
#include "../../G-Logic/GLDefine.h"
#include "./DxEffectSingleType.h"
#include "./DxEffSinglePropGMan.h"

class SingleForThread;
class DxEffSingleGroup;

//	Note : "EFF_PROPERTY"�� �ϴ��� �����ȴ�. "EFF_PROPERTY"���� �����͸� �ްų� �����Ͽ� ����Ʈ�� ������Ų��.
//			�̸� ��� �޾Ƽ� �������� ����Ʈ ���� Ŭ������ ���۵ȴ�.
//			"class DxEffectPaticleSys : public DxEffSingle"�� ���� ��ӵǾ ���ȴ�.
//
class DxEffSingle : public sc::PoolAllocMgr		// : public DXAFFINEMATRIX
{
public:
	DWORD				m_dwRunFlag;
	D3DXMATRIX			m_matLocal;
//	DXAFFINEPARTS		m_AffineParts;
	DxEffSingleGroup*	m_rThisGroup;

public:
	BOOL		m_bMoveObj;		//	
	float		m_fGAge;		//	���� Eff�� ����ð�.
	float		m_fGBeginTime;	//	Ȱ��ȭ ���� �ð�.
	float		m_fGLifeTime;	//	Eff ���� �Ⱓ.

	SMovSound	m_MovSound;	

	DxEffSingle	*m_pChild;		//	�ڽ� Eff
	DxEffSingle	*m_pSibling;	//	���� Eff



protected:
	boost::shared_ptr<SingleForThread> m_spSingleForThread;

public:
	void CreateSingleForThread( boost::shared_ptr<SingleForThread> spSingle )
	{
		m_spSingleForThread = spSingle;
	}



public:
	virtual void ReStartEff ();
	virtual void CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin )
	{
		if( m_pChild )		m_pChild->CheckAABBBox( vMax, vMin );
		if( m_pSibling )	m_pSibling->CheckAABBBox( vMax, vMin );
	}

public:
	//! �¸��� Ŭ���� ���⸦ �ʿ� �����Ѵ�.
	//virtual void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	virtual void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

protected:
	void PutSleepMemory( PDXEFFSINGLE& pSingle );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime );
	virtual HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha );

	virtual HRESULT OneTimeSceneInit ();
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup ();

	virtual HRESULT Create ( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	virtual HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

	//	Note : ����Ʈ Ÿ�� ����.
	//
public:
	virtual DWORD GetTypeID () = 0;
	virtual DWORD GetFlag () = 0;
	virtual const char* GetName () = 0;

public:
	BOOL IsReadyToDie ();

public:
	const D3DXMATRIX& GetLocalMatrix() const { return m_matLocal; }

public:
	DxEffSingle(void) 
		: m_rThisGroup(NULL)
		, m_dwRunFlag(NULL),
		m_fGAge(0.0f),
		m_fGBeginTime(0.0f),
		m_fGLifeTime(0.0f),
		m_pChild(NULL),
		m_pSibling(NULL)
	{
		D3DXMatrixIdentity( &m_matLocal );
	//	UseAffineMatrix ();
	}

	virtual ~DxEffSingle(void)
	{
		CleanUp ();
	}
};
typedef DxEffSingle* PDXEFFSINGLE;

enum EMAFTERTYPE { EFFABODY = 0, EFFASINGLE = 1 };
struct SEFFSINGLE_AFTER
{
	EMAFTERTYPE		m_emType;
	std::string		m_strEffect;

	SEFFSINGLE_AFTER () :
		m_emType(EFFABODY)
	{
	}

	SEFFSINGLE_AFTER ( const SEFFSINGLE_AFTER &value )
	{
		m_emType = value.m_emType;
		m_strEffect = value.m_strEffect;
	}

	SEFFSINGLE_AFTER& operator= ( const SEFFSINGLE_AFTER &value )
	{
		m_emType = value.m_emType;
		m_strEffect = value.m_strEffect;
		return *this;
	}
	
	SEFFSINGLE_AFTER ( const EMAFTERTYPE emType, const std::string& strEffect ) :
		m_emType(emType),
		m_strEffect(strEffect)
	{
	}
};

//	Note : "EFF_PROPGROUP"�� �ϴ��� �����ȴ�. "DxEffSing"���� Ʈ�� ������� ������ ������ �̸� �����Ѵ�.
//			"OBJAABB"�� �� ����Ʈ �׷��� ���ӻ󿡼� ���� �����ɶ� ī�޶� �������ø��� ����� �� �ִ� ȯ���� �����ϴµ��� ���δ�.
//
class DxEffSingleGroup : public OBJAABB
{
public:
	typedef std::list<SEFFSINGLE_AFTER>		AFTERLIST;
	typedef AFTERLIST::iterator				AFTERLIST_ITER;

	static int			g_nEFF_COUNT;

public:
	std::string			m_strFileName;

	int					m_nCount;		// DXLANDEFF���� ȿ���� �ߺ� FrameMove ���ϱ� ����.

	DWORD				m_dwFlag;		// EFFECTFLAG
	STARGETID			m_TargetID;
	STARGETID			m_ResultTargetID;
	D3DXMATRIX			m_matWorld;
	D3DXMATRIX			m_matTarget;

	float				m_fElapsedTime;
	float				m_fGAge;
	float				m_fGBeginTime;	// ȿ�� �̵��ϴ� �ð�
	float				m_fGLifeTime;	// ȿ�� �̵� �ð�
	D3DXVECTOR3			m_vGDelta;		// �ð��� �̵��ϴ� �Ÿ�
	D3DXVECTOR3			m_vGNowPos;		// ���� ��ü ��ġ

	D3DXVECTOR3			m_vStartPos;		// ó�� �����ߴ� ��ġ.
	D3DXVECTOR3			m_vLocal;			// ���鶧�� Y �� ���̸� ����ؼ� �ѷ��ش�.
	D3DXVECTOR3			m_vDirection;		// �����Ǵ� ��ƼŬ���� ó�� �̵� ���� * �� �� ��Ÿ��

	BOOL				m_bOut_Particle;	// ���ο� ��ƼŬ�� �����Ѵ�.
	float				m_fOut_Scale;		// �ܺο��� �������� ��
	BOOL				m_bOut_Direct;		// �ܺο��� ������ ��
	BOOL				m_bAutoMove;		// �ܺο��� ������ ��
	D3DXVECTOR3			m_vTargetPos;		// ��ǥ ��ġ.		�ܺο��� ���� �ִ� ����
	D3DXVECTOR3			m_vDirect;			// ���� ����...		�ܺο��� ���� �ִ� ����
	D3DXMATRIX			m_matOutRotate;		// �ܺ��� ȸ�� ��Ʈ����

	DxEffSingle*	m_pEffSingleRoot;
	DWORD			m_dwAliveCount;

	AFTERLIST		m_listEffSAfter;

	bool				m_bEnableRender;

	DxEffSinglePropGroupResource	m_spEffGroupProp;	// source

public:
	DxEffSingleGroup	*m_pNext;

public:
	D3DXVECTOR3* m_pMax;			//	AABB�� ���.	// EFF_PROPGROUP �� m_vMax ������
	D3DXVECTOR3* m_pMin;			//	AABB�� ���.	// EFF_PROPGROUP �� m_vMin ������

public:
	float GetAppElapsedTime ()		{ return m_fElapsedTime; }
	virtual void GetAABBSize ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin );
	virtual BOOL IsDetectDivision ( D3DXVECTOR3 &vDivMax, D3DXVECTOR3 &vDivMin );
	virtual OBJAABB* GetNext ()	{ return m_pNext; }

	BOOL IsBeginAfterEffect ()
	{
		if ( m_listEffSAfter.empty() )		return FALSE;
		return ( m_fGAge >= (m_fGBeginTime+m_fGLifeTime) );
	}

	const bool isEnableRender(void) const { return m_bEnableRender; }

	void MovePos( const D3DXVECTOR3 &vTarget );

	BOOL IsFlag( EFFECTFLAG emEffectFlag )
	{
		return (m_dwFlag & emEffectFlag) ? TRUE : FALSE;
	}

public:
	void SetTargetID ( const STARGETID* pTargetID );
	void SetEnableRender(const bool bEnableRender) { m_bEnableRender = bEnableRender; }
	void AddEffAfter ( const EMAFTERTYPE emType, const std::string& strEffSingle )
	{
		m_listEffSAfter.push_back ( SEFFSINGLE_AFTER(emType,strEffSingle) );
	}

	int GetCount()					{ return m_nCount; }
	void SetCount( int nCount )		{ m_nCount = nCount; }

public:
	const D3DXVECTOR3& GetNowPos() const	{ return m_vGNowPos; }
	const D3DXVECTOR3& GetTargetPos() const	{ return m_vTargetPos; }

public:
	//! �¸��� Ŭ���� ���⸦ �ʿ� �����Ѵ�.
	//void SetFlagWinnerClubCountry( const TSTRING& stTextureName, const D3DXVECTOR4& vFlagConvert );

	void SetWorldBattleFlag( const TCHAR* pWorldBattleFlag );

public:
	void	ReStartEff ();

protected:
	void PutSleepMemory( PDXEFFSINGLE& pSingle );

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR3& vScale, float fAlpha );

public:
	HRESULT OneTimeSceneInit ();
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup ();

public:
	HRESULT Create(LPDIRECT3DDEVICEQ pd3dDevice)
	{
		if ( FAILED(OneTimeSceneInit()) )				return E_FAIL;
		if ( FAILED(InitDeviceObjects(pd3dDevice)) )	return E_FAIL;
		if ( FAILED(RestoreDeviceObjects(pd3dDevice)) )	return E_FAIL;

		return S_OK;
	}

	HRESULT CleanUp ()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
		FinalCleanup();

		return S_OK;
	}

public:
	DxEffSingleGroup ()
	  	: m_nCount(0)
		, m_dwFlag(NULL),
		m_dwAliveCount(0xFFFFFFFF),
		m_pEffSingleRoot(NULL),
		m_fElapsedTime(0.0f),
		m_fGAge(0.0f),
		m_fGBeginTime(0.f),
		m_fGLifeTime(0.f),
		m_vGDelta(D3DXVECTOR3(0.f,0.f,0.f)),
		m_vGNowPos(D3DXVECTOR3(0.f,0.f,0.f)),
		m_pMax(NULL),
		m_pMin(NULL),
		m_vLocal(0.f,0.f,0.f),
		m_vDirection(0.f,0.f,0.f),

		m_bOut_Particle(TRUE),
		m_fOut_Scale(1.f),
		m_bOut_Direct(FALSE),
		m_bAutoMove(FALSE),

		m_vTargetPos(0.f,0.f,0.f),
		m_vDirect(0.f,0.f,0.f),
		m_pNext(NULL)
		, m_bEnableRender(true)
	{
		D3DXMatrixIdentity ( &m_matOutRotate );
		D3DXMatrixIdentity ( &m_matWorld );
		D3DXMatrixIdentity ( &m_matTarget );
	}

	virtual ~DxEffSingleGroup ()
	{
		CleanUp ();
	}
};

typedef boost::shared_ptr< DxEffSingleGroup > SPDXEFFSINGLEGROUP;