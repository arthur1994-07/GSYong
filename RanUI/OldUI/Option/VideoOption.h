#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicButton;
class	CD3DFontPar;
class	CBasicComboBox;
class	CBasicComboBoxRollOver;
enum	EMSCREEN_FORMAT;
class GLGaeaClient;

#define DEFAULT_OPTION_SIZE 5
//#define TEXTURE_FILTER_SIZE 5	// Trilinear, Anisotropic 2X, Anisotropic 4X, Anisotropic 8X, Anisotropic 16X
//#define PHYSX_LEVEL_SIZE	3

class	CBasicVideoOption : public CUIGroup
{
	static const char cRESOLUTION_SEPERATOR;

protected:
	enum
	{
		HWOPTION_VIDEO_DEFAULT_COMBO_OPEN = NO_ID + 1,
		HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER,
		HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN,
		HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER,
		HWOPTION_VIDEO_SHADOW_COMBO_OPEN,
		HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER,
		HWOPTION_VIDEO_DETAIL_COMBO_OPEN,
		HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER,
		HWOPTION_VIDEO_SIGHT_COMBO_OPEN,
		HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER,
		HWOPTION_VIDEO_BUFF_BUTTON,
		HWOPTION_VIDEO_SHADOWLAND_BUTTON,
		HWOPTION_VIDEO_REFLECT_BUTTON,
		HWOPTION_VIDEO_REFRACT_BUTTON,
		HWOPTION_VIDEO_GLOW_BUTTON,
		HWOPTION_VIDEO_POSTPROCESSING_BUTTON,
		HWOPTION_VIDEO_FRAME_LIMIT_BUTTON,
		HWOPTION_VIDEO_TARGET_EFFECT_BUTTON,
		HWOPTION_VIDEO_MINE_EFFECT_BUTTON,
		HWOPTION_VIDEO_CLICK_EFFECT_BUTTON,
		HWOPTION_VIDEO_NORMALMAP_BUTTON,
		HWOPTION_VIDEO_TEXFILTER_COMBO_OPEN,
		HWOPTION_VIDEO_TEXFILTER_COMBO_ROLLOVER,
		HWOPTION_VIDEO_TEXQUALITY_COMBO_OPEN,
		HWOPTION_VIDEO_TEXQUALITY_COMBO_ROLLOVER,
		HWOPTION_VIDEO_PHYSXCLOTH_COMBO_OPEN,
		HWOPTION_VIDEO_PHYSXCLOTH_COMBO_ROLLOVER,
	};

public:
	CBasicVideoOption(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicVideoOption ();

public:
	void	CreateSubControl ();

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

private:
	void	LoadComboData ();
	void	SetTextSightCombo( DWORD dwIndex );
	void	SetTextSkinDetailCombo( DWORD dwIndex );
	void	SetTextShadowCharCombo( DWORD dwIndex );
	void	SetTextResolutionCombo( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFormat);
	void	SetTextDefaultOptionCombo( DWORD dwIndex );
	void	SetTextTexFilterCombo( DWORD dwIndex );
	void	SetTextTexQualityCombo( DWORD dwIndex );
	void	SetTextPhysXClothCombo( DWORD dwIndex );

public:
	void	LoadCurrentOption ();

private:
	void	LoadResolution ();
	void	LoadShadow ();
	void	LoadSkinDetail ();
	void	LoadSight ();
	void	LoadTexFilter ();
	void	LoadTexQuality ();
	void	LoadPhysXCloth ();
	void	LoadBuffButton();
	//void	LoadShadowlandButton ();
	void	LoadReflectButton ();
	void	LoadRefractButton ();
	void	LoadGlowButton ();
	void	LoadPostButton ();
	void	LoadFrameLimitButton ();

	void	LoadTargetEffectButton ();
	void	LoadClickEffectButton ();
	void	LoadMineEffectButton ();
	void	LoadUseNormalMapButton ();

	void	LoadDefaultOption();
	void	SetVideoLevel(int nIndex );
	void	SetLowLevel();
	void	SetMediumLevel();
	void	SetHighLevel();
	void	SetVeryHighLevel();

private:
	CD3DFontPar*	m_pFont9;

private:
	CBasicButton*		m_pBuffButton;
	CBasicButton*		m_pShadowlandButton;
	CBasicButton*		m_pReflectButton;
	CBasicButton*		m_pRefractButton;
	CBasicButton*		m_pGlowButton;
	CBasicButton*		m_pPostButton;
	CBasicButton*		m_pFrameLimitButton;
	CBasicButton*		m_pTargetEffectButton;
	CBasicButton*		m_pMineEffectButton;
	CBasicButton*		m_pClickEffectButton;
	CBasicButton*		m_pUseNormalMapButton;

private:
	CBasicComboBox*			m_pComboBoxResolutionOpen;
	CBasicComboBoxRollOver*	m_pComboBoxResolutionRollOver;

	CBasicComboBox*			m_pComboBoxShadowOpen;
	CBasicComboBoxRollOver*	m_pComboBoxShadowRollOver;

	CBasicComboBox*			m_pComboBoxCharDetailOpen;
	CBasicComboBoxRollOver*	m_pComboBoxCharDetailRollOver;

	CBasicComboBox*			m_pComboBoxSightOpen;
	CBasicComboBoxRollOver*	m_pComboBoxSightRollOver;
	
	CBasicComboBox*			m_pComboBoxDefaultOpen;
	CBasicComboBoxRollOver*	m_pComboBoxDefaultRollOver;

	CBasicComboBox*			m_pComboBoxTexFilterOpen;
	CBasicComboBoxRollOver*	m_pComboBoxTexFilterRollOver;

	CBasicComboBox*			m_pComboBoxTexQualityOpen;
	CBasicComboBoxRollOver*	m_pComboBoxTexQualityRollOver;

	CBasicComboBox*			m_pComboBoxPhysXClothOpen;
	CBasicComboBoxRollOver*	m_pComboBoxPhysXClothRollOver;
	
private:
	UIGUID	m_RollOverID;
	BOOL	m_bFirstLBUP;

public:
	BOOL m_bBuff;
	//BOOL m_bShadowLand;
	BOOL m_bRealReflect;
	BOOL m_bRefract;
	BOOL m_bGlow;
	BOOL m_bPost;
	BOOL m_bFrameLimit;
	BOOL m_bTargetEffect;
	BOOL m_bMineEffect;
	BOOL m_bClickEffect;
	BOOL m_bUseNormalMap;

	DWORD m_dwSight;
	DWORD m_dwSkinDetail;
	DWORD m_dwShadowChar;
	DWORD m_dwVideoLevel;
	DWORD m_dwTexFilter;
	DWORD m_dwTexQuality;
	DWORD m_dwPhysXCloth;

	DWORD m_dwScrHeight;
	DWORD m_dwScrWidth;
	EMSCREEN_FORMAT m_emScrFormat;

protected:
	CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;
};