#pragma once

#include <set>
#include <list>

class DxEffAni;
class DxSkinChar;
class DxEffAniData;
class DxAttBone;
class DxVehicle;
class DxSkinCharEffAniBase;
struct DxSkinAnimation;
struct SKINEFFDATA;

//	Note : "class DxEffSingleGroup"인스턴스들을 (즉, 지금 동작하고 있는 "이펙트 그룹"개체들을 같이 모아서 관리하고
//			이를 동작시키며 렌더링 한다. ( Update & Render ) 또한 이들이 소멸시점인지를 체크한후 소멸되게 한다.
//
class DxEffAniPlayer
{
protected:
	typedef std::list<DxEffAni*>		EFFLIST;
	typedef EFFLIST::iterator			EFFLIST_ITER;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFLIST				m_listAni;		// Ani Key에 따른 효과의 생성.
	EFFLIST				m_listOneSet;	// 1번만 효과를 생성.

	// 로딩작업한 애니메이션. - Effect를 재 로딩 하지 않기 위한 작업
	typedef std::set<DxSkinAnimation*>		SET_PSSKINANIMATION;
	typedef SET_PSSKINANIMATION::iterator	SET_PSSKINANIMATION_ITER;
	SET_PSSKINANIMATION		m_setLoadAnimation;

	// 지속Effect를 셋팅하기 위한 List
	struct NEVERDIE_EFFANI
	{
		DxSkinAnimation*	m_pSkinAnimation;
		DxEffAniData*		m_pEffData;
		DxEffAni*			m_pEffAni;
	};
	typedef std::vector<NEVERDIE_EFFANI>	VEC_EFFANI;
	typedef VEC_EFFANI::iterator			VEC_EFFANI_ITER;
	VEC_EFFANI		m_vecAniLoop;

protected:
	void	CreateEff ( DxEffAniData* pEff, DxSkinAnimation* pAnimContainer );
	void	CreateEffSingleEffLoop( DxEffAniData* pEff, DxSkinAnimation* pAnimContainer );	// SingleEff Loop
	void	DeleteEffSingleEffLoop( DxSkinAnimation* pAnimContainer );	// SingleEff Loop	// Loop 옵션을 없애도록 함.

public:
	void	CheckCreateEff( DxSkinAnimation* pAnimContainer );										// Note : 1번 설정후 다음부턴 안함.
	void	CheckCreateEff( DxSkinAnimation* pAnimContainer, int dwPrevFrame, DWORD dwCurKeyTime );	// Note : 매번 Ani Key를 본 후 Eff 셋팅.
	void	ResetEff( DxSkinAnimation* pAnimContainer );
	void	DeleteEffSingleEffLoop();	// SingleEff Loop	// 진짜 삭제해줌

public:
	DWORD	GetAmount ()				{ return DWORD(m_listAni.size()); }
	
	void	SetTime ( float fTimer )	{ m_fTimer = fTimer; }
	float	GetTime ()					{ return m_fTimer; }

	HRESULT RemoveAllEff ();
	HRESULT DeleteEff ( const TCHAR* szFile );

public:
	HRESULT FrameMove ( DxSkinCharEffAniBase* pSkinCharBase, const DxSkinAnimation* pAnimContainer, float fTime, float fElapsedTime, SKINEFFDATA& sSKINEFFDATA );
	HRESULT	Render ( const LPDIRECT3DDEVICEQ pd3dDevice, DxSkinCharEffAniBase* pSkinChar, const D3DXMATRIX& matCurPos, float fAlpha );

public:
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT RestoreDeviceObjects ();

	HRESULT InvalidateDeviceObjects ();
	HRESULT DeleteDeviceObjects ();

public:
	DxEffAniPlayer (void);

public:
	~DxEffAniPlayer (void);
};
