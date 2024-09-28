//	기본 정보 표시
//
//	최초 작성자 : 성기엽
//	이후 수정자 : 
//	로그
//		[2003.11.27]
//			@ 작성
//

#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"

struct SITEMCUSTOM;
class CBasicProgressBar;
class GLGaeaClient;

class CItemImage : public CUIGroup
{
private:
	enum {
		ITEM_IMAGE_SLOT = NO_ID + 1,
		ITEM_IMAGE_OVER,
	};
public:
	CItemImage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CItemImage ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual void CreateSubControl (const std::string& overImageKey = "" ,DWORD color = D3DCOLOR_XRGB(255,255,255), const char* szControlKeyword = "ITEM_IMAGE");
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void SetDiffuse(D3DCOLOR _diffuse);
	virtual void SetGlobalPos(const UIRECT& Pos);
	virtual void SetGlobalPos(const D3DXVECTOR2& vPos);

//	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );			

public:
    const bool	SetItem ( const SITEMCUSTOM& itemCustom );
	const bool	SetItem ( SNATIVEID sNativeID  );

	void	SetAnimatePosition(const float fGapX, const float fGapY, const float fGapWidth, const float fGapHeight);
	void	doAnimate(const float fT);

	void    SetVisibleOverImage(bool isVisible);
	bool    IsVisibleOverImage();

	SNATIVEID   GetItemID();
	SNATIVEID	GetItemImageSlotPos();
	const CString&	GetItemTextureName () const;

	void SetUseTwinkle( bool bUse ) { m_bTwinkle = bUse; }

	void SetNumber ( int nNumber, int nMaxNumber );
	void ResetNumber (  );

private:
    const bool	SetItem ( SNATIVEID sICONINDEX, const char* szTexture, SNATIVEID sNativeID );
    const bool	SetItem ( SNATIVEID sICONINDEX, const char* szTexture, SNATIVEID sNativeID, SNATIVEID sICONINDEX_Overlaped, const char* szTexture_Overlaped, SNATIVEID sNativeID_Overlaped );

public:
	virtual void	ResetItem ();
	virtual void	SetItemProcess( SNATIVEID sNativeID );
	virtual void	ReSetItemProcess();

private:
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

protected:
	SNATIVEID	m_sICONINDEX;
	SNATIVEID	m_sNativeID;
    SNATIVEID	m_sNativeID_Overlaped;
	CString		m_strTextureName;

	CBasicProgressBar*	m_pItemProcess;

	float m_TotElapsedTime;
	bool m_bItemUsed;
	bool m_bItemEnd;

	bool m_bTwinkle;

	UIRECT m_AnimateOriginalPosition;
	UIRECT m_AnmateBeginPosition;
	CUIControl*		m_pItemImage;
    CUIControl*		m_pItemImageOverlaped;

	CUIControl*		m_pOverImage;
	CBasicTextBox*  m_pNumberText;
	
	// Item의 GaeaID는 Item의 NativeID이다;
	// Character, CharClient는 GaeaID가 GaeaID임;
	STARGETID m_sTargetID;
};
