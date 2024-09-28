#pragma once

struct EFF_PROPERTY;
struct EFF_PROPGROUP;
class DxEffSingleGroup;

#include <list>
#include <map>
#include "../../G-Logic/GLDefine.h"
#include "./DxEffSingle.h"

//	Note : "class DxEffSingleGroup"�ν��Ͻ����� (��, ���� �����ϰ� �ִ� "����Ʈ �׷�"��ü���� ���� ��Ƽ� �����ϰ�
//			�̸� ���۽�Ű�� ������ �Ѵ�. ( Update & Render ) ���� �̵��� �Ҹ���������� üũ���� �Ҹ�ǰ� �Ѵ�.
//
class DxEffGroupPlayer
{
protected:
	typedef std::multimap<STARGETID,SPDXEFFSINGLEGROUP>	EFFSGMAP;
	typedef EFFSGMAP::iterator											EFFSGMAP_ITER;
	typedef std::pair<EFFSGMAP_ITER,EFFSGMAP_ITER>			EFFSGMAP_PAIR;

	struct DXEFFSINGLEGROUP_DATA
	{
		TSTRING							m_strFileName;
		STARGETID						m_sTargetID;
		D3DXMATRIX						m_matTrans;
		DxEffSinglePropGroupResource	m_sPropGroupRes;
	};
	typedef std::list<DXEFFSINGLEGROUP_DATA>	LISTEFFSINGLEGROUP;
	typedef LISTEFFSINGLEGROUP::iterator		LISTEFFSINGLEGROUP_ITER;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFSGMAP			m_mapESG;
	LISTEFFSINGLEGROUP	m_listESG_Loading;

protected:
	void	NewAfterEff ( DxEffSingleGroup* pEffSG );

public:
	DWORD	GetAmount ()				{ return DWORD(m_mapESG.size()); }
	
	void	SetTime ( float fTimer )	{ m_fTimer = fTimer; }
	float	GetTime ()					{ return m_fTimer; }

public:
	D3DXVECTOR3 GetTargetIDPos ( STARGETID *pTargetID );

public:
	HRESULT PassiveEffect ( const char* szFileName, const D3DXMATRIX &matStart, const STARGETID &sTargetID, BOOL bEdit = FALSE );
	HRESULT DeletePassiveEffect ( const char* szFileName, const STARGETID &sTargetID );
	HRESULT DeletePassiveEffect ( const STARGETID &sTargetID );
	BOOL FindPassiveEffect ( const char* szFileName, const STARGETID &sTargetID );

public:
	SPDXEFFSINGLEGROUP NewEffGroup ( const char* szFileName, const D3DXMATRIX &matStart, const STARGETID* pTargetID, BOOL bThread, BOOL bBackUpCache );	// pTargetID=NULL
	SPDXEFFSINGLEGROUP NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, const D3DXMATRIX &matStart, const STARGETID* pTargetID=NULL );
	SPDXEFFSINGLEGROUP NewEffGroup ( EFF_PROPGROUP* pPropGroup, const D3DXMATRIX &matStart, const STARGETID* pTargetID=NULL );
	HRESULT NewEffBody ( const char* szFileName, const STARGETID* pTargetID, const D3DXVECTOR3 *pDir );

	//	Note : ���� ����Ʈ�� ȭ�鿡 ������ �ϴ� �޼ҵ�. ��� ����Ʈ ������ REND ����.
	HRESULT NewEffGroup ( DxEffSinglePropGroupResource spPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matStart );
	HRESULT NewEffGroup ( EFF_PROPGROUP* pPropGroup, EFF_PROPERTY* pTargetProp, const D3DXMATRIX &matStart );

	HRESULT RemoveAllEff ();
    HRESULT DeleteEff(const std::string& FileName);
    HRESULT DeleteAllEff(const std::string& FileName);

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime );
	HRESULT	Render ( LPDIRECT3DDEVICEQ pd3dDevice );

	void RenderEnable(const STARGETID& sID, const bool bRenderEnable = true);

private:
	void UpdateLoadingData();

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

protected:
	DxEffGroupPlayer (void);

public:
	~DxEffGroupPlayer (void);

public:
	static DxEffGroupPlayer& GetInstance();
};
