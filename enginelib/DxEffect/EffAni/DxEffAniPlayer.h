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

//	Note : "class DxEffSingleGroup"�ν��Ͻ����� (��, ���� �����ϰ� �ִ� "����Ʈ �׷�"��ü���� ���� ��Ƽ� �����ϰ�
//			�̸� ���۽�Ű�� ������ �Ѵ�. ( Update & Render ) ���� �̵��� �Ҹ���������� üũ���� �Ҹ�ǰ� �Ѵ�.
//
class DxEffAniPlayer
{
protected:
	typedef std::list<DxEffAni*>		EFFLIST;
	typedef EFFLIST::iterator			EFFLIST_ITER;

protected:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

	float				m_fTimer;
	EFFLIST				m_listAni;		// Ani Key�� ���� ȿ���� ����.
	EFFLIST				m_listOneSet;	// 1���� ȿ���� ����.

	// �ε��۾��� �ִϸ��̼�. - Effect�� �� �ε� ���� �ʱ� ���� �۾�
	typedef std::set<DxSkinAnimation*>		SET_PSSKINANIMATION;
	typedef SET_PSSKINANIMATION::iterator	SET_PSSKINANIMATION_ITER;
	SET_PSSKINANIMATION		m_setLoadAnimation;

	// ����Effect�� �����ϱ� ���� List
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
	void	DeleteEffSingleEffLoop( DxSkinAnimation* pAnimContainer );	// SingleEff Loop	// Loop �ɼ��� ���ֵ��� ��.

public:
	void	CheckCreateEff( DxSkinAnimation* pAnimContainer );										// Note : 1�� ������ �������� ����.
	void	CheckCreateEff( DxSkinAnimation* pAnimContainer, int dwPrevFrame, DWORD dwCurKeyTime );	// Note : �Ź� Ani Key�� �� �� Eff ����.
	void	ResetEff( DxSkinAnimation* pAnimContainer );
	void	DeleteEffSingleEffLoop();	// SingleEff Loop	// ��¥ ��������

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
