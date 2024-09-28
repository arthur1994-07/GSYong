#pragma once

#include "../../enginelib/DxMeshs/DxAniKeys.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../RanLogic/Item/GLItemDef.h"
#include "../../RanLogic/Item/GLItemCustom.h"
#include "./GLWidgetTexture.h"

class DxSkinChar;
struct GLCHARLOGIC;

// 캐릭터 텍스처
class GLWidgetTextureChar : public GLWidgetTexture
{
public :
	GLWidgetTextureChar( LPDIRECT3DDEVICEQ pDevice );
	virtual ~GLWidgetTextureChar(void);

public :
	virtual bool OnCreate();
	virtual void OnResetDevice();
	virtual void OnLostDevice();
	virtual void OnFrameMove( float fElapsedTime );
	virtual void OnRender();

	virtual void CleanUp();

public :
	enum EMPREVIEWSLOT
	{
		EMPREVIEWSLOT_HEAD,
		EMPREVIEWSLOT_UPPER,
		EMPREVIEWSLOT_LOWER,
		EMPREVIEWSLOT_HAND,
		EMPREVIEWSLOT_FOOT,
		EMPREVIEWSLOT_WEAPON,

		// 기획팀에서 셋팅을 코스츔이 되도록 해버렸다. 지원해줘야한다. (EMPREVIEWSLOT_DECORATION,EMPREVIEWSLOT_EARRINGS)
		EMPREVIEWSLOT_DECORATION,
		EMPREVIEWSLOT_EARRINGS,

		EMPREVIEWSLOT_SIZE,
	};

	struct SPIECEINFO
	{
		std::string szFILENAME;
		std::string szSELFBODYEFF;
		BYTE        cGRADE;
		DWORD       dwMAINCOLOR;
		DWORD       dwSUBCOLOR;
		BOOL        bNONCHARDATA;

		SPIECEINFO() 
			: cGRADE(0)
			, dwMAINCOLOR(0)
			, dwSUBCOLOR(0)
			, bNONCHARDATA(FALSE)
		{
		}

		bool operator==(const SPIECEINFO& other) const
		{
			return szFILENAME == other.szFILENAME &&
				szSELFBODYEFF == other.szSELFBODYEFF &&
				cGRADE == other.cGRADE &&
				dwMAINCOLOR == other.dwMAINCOLOR &&
				dwSUBCOLOR == other.dwSUBCOLOR &&
				bNONCHARDATA == other.bNONCHARDATA;
		}

		bool operator!=(const SPIECEINFO& other) const
		{
			return !operator==(other);
		}

		void RESET()
		{
			szFILENAME.clear();
			szSELFBODYEFF.clear();
			cGRADE       = 0;
			dwMAINCOLOR  = 0;
			dwSUBCOLOR   = 0;
			bNONCHARDATA = FALSE;
		}
	};

protected :
	D3DXMATRIX m_matWorld;
	D3DXMATRIX m_matView;
	D3DXMATRIX m_matProj;

	D3DXVECTOR3 m_vViewPos;
	float		m_fRotate;

	DxSkinChar*		m_pSkinChar;
	EMANI_MAINTYPE	m_emAniMainType;
	EMANI_SUBTYPE	m_emAniSubType;

	float			m_fTime;

	EMCHARINDEX		m_emCharIndex;

protected :
	void SelectAni( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType = AN_SUB_NONE, DWORD dwFlags = NULL, DWORD dwSelect = 0 );

	DWORD ConvertColor( const WORD wColor );
	EMPREVIEWSLOT Slot_2_PreviewSlot( const EMSLOT emSlot );
	BOOL IsPreviewItem( const SITEMCUSTOM& sItemCustom, const EMCHARINDEX emCharIndex, const EMSLOT emSlot );

public :
	void SetSkinChar( GLCHARLOGIC* pChar );
	void SetPiece( const SPIECEINFO& sInfo );
	void SetItem( const SITEMCUSTOM& sItemCustom, const EMSLOT emSlot );

	void SetAni( EMANI_MAINTYPE MType, EMANI_SUBTYPE SType = AN_SUB_NONE );

	void SetViewPosition( const D3DXVECTOR3& vPos ) { m_vViewPos = vPos; }
	D3DXVECTOR3 GetViewPosition() { return m_vViewPos; }
	void SetRotate( float fRotate ) { m_fRotate = fRotate; }

	const float GetHeight();
};