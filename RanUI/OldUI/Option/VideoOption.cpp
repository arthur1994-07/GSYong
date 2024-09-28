#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../../EngineLib/DxTools/DxShadowMap.h"
#include "../../../EngineLib/DxEffect/Frame/DxEffectMan.h"

#include "../../../RanLogicClient/Tool/DxParamSet.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../InnerInterface.h"
#include "VideoOption.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const char CBasicVideoOption::cRESOLUTION_SEPERATOR = 'x';

CBasicVideoOption::CBasicVideoOption(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
    , m_pGaeaClient(pGaeaClient)
	, m_RollOverID ( NO_ID )
	, m_dwSight(0)
	, m_dwSkinDetail(0)
	, m_dwShadowChar(0)
	, m_dwScrHeight(0)
	, m_dwScrWidth(0)
	, m_emScrFormat(EMSCREEN_F16)
	, m_dwVideoLevel(0)
	, m_dwTexFilter(0)
	, m_dwTexQuality(2)
	, m_dwPhysXCloth(2)
{
}

CBasicVideoOption::~CBasicVideoOption ()
{
}

void CBasicVideoOption::CreateSubControl ()
{
	m_pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	CBasicTextBox* pTextBox = NULL;
	CBasicComboBox* pComboBox = NULL;
	CBasicComboBoxRollOver*	pComboBoxRollOver = NULL;

	{	//	�⺻ �ؽ�Ʈ		
		DWORD dwFontColor = NS_UITEXTCOLOR::DEFAULT;
		int nAlign = TEXT_ALIGN_LEFT;

		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_DEFAULT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 10 ) );

		//	�ػ�
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_RESOLUTION_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 0 ) );

		//	ĳ���� �׸���
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SHADOW_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 1 ) );

		//	ĳ���� ������
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_CHARDETAIL_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 2 ) );

		//	�þ�
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SIGHT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 3 ) );

		//	����
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_BUFF_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 4 ) );

		//	���� �׸���
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_SHADOWLAND_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 5 ) );

		//	�ǽð� �ݻ�
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_REFLECT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 6 ) );

		//	ȭ�� �ְ�
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_REFRACT_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 7 ) );

		//	�۷ο� ȿ��
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_GLOW_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 8 ) );

		//	�ǻ��� ȿ��
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_POST_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 9 ) );

		// ������ ���� ���
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_FRAME_LIMIT_STATIC",m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 11 ) );

		// Ÿ�� ����Ʈ
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_TARGET_EFFECT_STATIC",m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 12 ) );

		// ������ ���� ����Ʈ
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_MINE_EFFECT_STATIC",m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 13 ) );

		// ���� Ŭ�� ����Ʈ
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_CLICK_EFFECT_STATIC",m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 14 ) );

		// �븻��
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_NORMALMAP_STATIC",m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 15 ) );

		// Texture Filter
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_TEXFILTER_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 16 ) );

		// PhysX Cloth
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_PHYSXCLOTH_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 17 ) );

		// Texture Quality
		pTextBox = CreateStaticControl ( "HWOPTION_VIDEO_TEXQUALITY_STATIC", m_pFont9, dwFontColor, nAlign );
		pTextBox->SetOneLineText ( ID2GAMEWORD ( "HWOPTION_VIDEO_OPTION", 18 ) );
	}

	{	//	���

		//	�޺��ڽ�
		{
			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_DEFAULT_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_DEFAULT_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxDefaultOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxResolutionOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_SHADOW_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_SHADOW_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxShadowOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_DETAIL_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_DETAIL_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxCharDetailOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_SIGHT_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_SIGHT_COMBO_OPEN" );			
            RegisterControl ( pComboBox );
			m_pComboBoxSightOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_TEXFILTER_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_TEXFILTER_COMBO_OPEN" );			
			RegisterControl ( pComboBox );
			m_pComboBoxTexFilterOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_TEXQUALITY_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_TEXQUALITY_COMBO_OPEN" );			
			RegisterControl ( pComboBox );
			m_pComboBoxTexQualityOpen = pComboBox;

			pComboBox = new CBasicComboBox(m_pEngineDevice);
			pComboBox->CreateSub ( this, "BASIC_COMBOBOX", UI_FLAG_XSIZE, HWOPTION_VIDEO_PHYSXCLOTH_COMBO_OPEN );
			pComboBox->CreateBaseComboBox ( "HWOPTION_VIDEO_PHYSXCLOTH_COMBO_OPEN" );			
			RegisterControl ( pComboBox );
			m_pComboBoxPhysXClothOpen = pComboBox;
		}

		//	��ư
		{
			m_pBuffButton = CreateFlipButton ( "HWOPTION_VIDEO_BUFF_BUTTON", "HWOPTION_VIDEO_BUFF_BUTTON_F", HWOPTION_VIDEO_BUFF_BUTTON );
			m_pShadowlandButton = CreateFlipButton ( "HWOPTION_VIDEO_SHADOWLAND_BUTTON", "HWOPTION_VIDEO_SHADOWLAND_BUTTON_F", HWOPTION_VIDEO_SHADOWLAND_BUTTON );
			m_pReflectButton = CreateFlipButton ( "HWOPTION_VIDEO_REFLECT_BUTTON", "HWOPTION_VIDEO_REFLECT_BUTTON_F", HWOPTION_VIDEO_REFLECT_BUTTON );
			m_pRefractButton = CreateFlipButton ( "HWOPTION_VIDEO_REFRACT_BUTTON", "HWOPTION_VIDEO_REFRACT_BUTTON_F", HWOPTION_VIDEO_REFRACT_BUTTON );
			m_pGlowButton = CreateFlipButton ( "HWOPTION_VIDEO_GLOW_BUTTON", "HWOPTION_VIDEO_GLOW_BUTTON_F", HWOPTION_VIDEO_GLOW_BUTTON );
			m_pPostButton = CreateFlipButton ( "HWOPTION_VIDEO_POST_BUTTON", "HWOPTION_VIDEO_POST_BUTTON_F", HWOPTION_VIDEO_POSTPROCESSING_BUTTON );
			m_pFrameLimitButton = CreateFlipButton ( "HWOPTION_VIDEO_FRAME_LIMIT_BUTTON", "HWOPTION_VIDEO_FRAME_LIMIT_BUTTON_F", HWOPTION_VIDEO_FRAME_LIMIT_BUTTON );
			m_pTargetEffectButton = CreateFlipButton ( "HWOPTION_VIDEO_TARGET_EFFECT_BUTTON", "HWOPTION_VIDEO_TARGET_EFFECT_BUTTON_F", HWOPTION_VIDEO_TARGET_EFFECT_BUTTON );
			m_pMineEffectButton = CreateFlipButton ( "HWOPTION_VIDEO_MINE_EFFECT_BUTTON", "HWOPTION_VIDEO_MINE_EFFECT_BUTTON_F", HWOPTION_VIDEO_MINE_EFFECT_BUTTON );
			m_pClickEffectButton = CreateFlipButton ( "HWOPTION_VIDEO_CLICK_EFFECT_BUTTON", "HWOPTION_VIDEO_CLICK_EFFECT_BUTTON_F", HWOPTION_VIDEO_CLICK_EFFECT_BUTTON );
			m_pUseNormalMapButton = CreateFlipButton ( "HWOPTION_VIDEO_NORMALMAP_BUTTON", "HWOPTION_VIDEO_NORMALMAP_BUTTON_F", HWOPTION_VIDEO_NORMALMAP_BUTTON );
		}

		//	�� ���� ( ������ ���� �������� �ѷ������մϴ�. )
		{
			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER" );			
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxDefaultRollOver = pComboBoxRollOver;

            pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER" );			
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxResolutionRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxShadowRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxCharDetailRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxSightRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_TEXFILTER_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_TEXFILTER_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxTexFilterRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_TEXQUALITY_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_TEXQUALITY_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxTexQualityRollOver = pComboBoxRollOver;

			pComboBoxRollOver = new CBasicComboBoxRollOver(m_pEngineDevice);
			pComboBoxRollOver->CreateSub ( this, "BASIC_COMBOBOX_ROLLOVER", UI_FLAG_XSIZE | UI_FLAG_YSIZE, HWOPTION_VIDEO_PHYSXCLOTH_COMBO_ROLLOVER );
			pComboBoxRollOver->CreateBaseComboBoxRollOver ( "HWOPTION_VIDEO_PHYSXCLOTH_COMBO_ROLLOVER" );
			pComboBoxRollOver->SetVisibleSingle ( FALSE );
			RegisterControl ( pComboBoxRollOver );
			m_pComboBoxPhysXClothRollOver = pComboBoxRollOver;
		}
	}
}

CBasicButton* CBasicVideoOption::CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID )
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, szButton, UI_FLAG_DEFAULT, ControlID );
	pButton->CreateFlip ( szButtonFlip, CBasicButton::RADIO_FLIP );
	pButton->SetControlNameEx ( szButton );
	RegisterControl ( pButton );
	return pButton;
}

CBasicTextBox* CBasicVideoOption::CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign )
{
	CBasicTextBox* pStaticText = new CBasicTextBox(m_pEngineDevice);
	pStaticText->CreateSub ( this, szControlKeyword );
	pStaticText->SetFont ( pFont );
	pStaticText->SetTextAlign ( nAlign );	
	RegisterControl ( pStaticText );
	return pStaticText;
}

void CBasicVideoOption::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	if ( m_RollOverID == NO_ID )
	{
		CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
	}
	else
	{
		ResetMessageEx ();

		CUIControl::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		CUIControl* pControl = m_ControlContainer.FindControl ( m_RollOverID );
		if ( !pControl )
		{
			GASSERT ( 0 && "�ɰ��� ����, ��尡 ���Դϴ�." );
			return ;
		}

		pControl->Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

		DWORD dwControlMsg = pControl->GetMessageEx ();
		NS_UIDEBUGSET::BlockBegin ();
		if ( dwControlMsg ) TranslateUIMessage ( m_RollOverID, dwControlMsg );
		NS_UIDEBUGSET::BlockEnd ();

		//	��ũ�ѹٿ� ����� �޽����� �ƴϰ�		
		if ( !(dwControlMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL) )
		{
			DWORD dwMsg = GetMessageEx ();
			if ( dwMsg & UIMSG_LB_UP )
			{
				if ( !m_bFirstLBUP )
				{
					m_RollOverID = NO_ID;
					pControl->SetVisibleSingle ( FALSE );					
				}
				m_bFirstLBUP = FALSE;
			}
		}
	}

	//CDebugSet::ToView ( 1, 18, "%d", m_RollOverID );
}

void CBasicVideoOption::LoadResolution ()
{
	m_dwScrWidth = DXPARAMSET::GetInstance().m_dwScrWidth;
	m_dwScrHeight = DXPARAMSET::GetInstance().m_dwScrHeight;
	m_emScrFormat = DXPARAMSET::GetInstance().m_emScrFormat;
	SetTextResolutionCombo( m_dwScrWidth, m_dwScrHeight, /*DXPARAMSET::GetInstance().m_uScrRefreshHz,*/ m_emScrFormat);
}

void CBasicVideoOption::LoadShadow ()
{
	m_dwShadowChar = DXPARAMSET::GetInstance().m_dwShadowChar;
	SetTextShadowCharCombo( m_dwShadowChar );
}

void CBasicVideoOption::LoadSkinDetail ()
{
	m_dwSkinDetail = DXPARAMSET::GetInstance().m_dwSkinDetail;
	SetTextSkinDetailCombo( m_dwSkinDetail );
}

void CBasicVideoOption::LoadSight ()
{
	m_dwSight = DXPARAMSET::GetInstance().m_dwFogRange;
	SetTextSightCombo( m_dwSight );
}

void CBasicVideoOption::LoadTexFilter ()
{
	m_dwTexFilter = DXPARAMSET::GetInstance().m_dwTexFilter;
	SetTextTexFilterCombo( m_dwTexFilter );
}

void CBasicVideoOption::LoadTexQuality ()
{
	m_dwTexQuality = DXPARAMSET::GetInstance().m_dwTexQuality;
	SetTextTexQualityCombo( m_dwTexQuality );
}

void CBasicVideoOption::LoadPhysXCloth ()
{
	m_dwPhysXCloth = DXPARAMSET::GetInstance().m_dwPhysXCloth;
	SetTextPhysXClothCombo( m_dwPhysXCloth );
}

void CBasicVideoOption::LoadDefaultOption()
{
	m_dwVideoLevel = DXPARAMSET::GetInstance().m_dwVideoLevel;
	SetTextDefaultOptionCombo( m_dwVideoLevel );
}

void CBasicVideoOption::LoadCurrentOption()
{
	LoadComboData();		// �⺻ �޺� ��Ʈ�� �����
	LoadDefaultOption();
	LoadResolution();		// ���� �ػ� ������
							// ���� �ֻ��� ������
	LoadShadow();			// �׸��� ����
    LoadSkinDetail();		// ��Ų ������
	LoadSight();			// �þ� ����
	LoadTexFilter();		// �ؽ��� ���͸�
	LoadTexQuality();		// �ؽ��� Quality
	LoadPhysXCloth();		// PhysX Level
	LoadBuffButton();		// ����
	//LoadShadowlandButton();	// ���� �׸���
	LoadReflectButton();	// �ǽð� �ݻ�
	LoadRefractButton();	// �ְ�
	LoadGlowButton();		// �۷ο�
	LoadPostButton();		// �ǻ���
	LoadFrameLimitButton(); // ����������
	LoadTargetEffectButton ();// Ÿ�� ����Ʈ ǥ��
	LoadClickEffectButton (); // Ŭ���� ����Ʈ ǥ��
	LoadMineEffectButton ();  // ������ ǥ��
	LoadUseNormalMapButton(); // �븻�� ���
}

void CBasicVideoOption::LoadBuffButton()
{
	m_bBuff = DXPARAMSET::GetInstance().m_bBuff;
	m_pBuffButton->SetFlip ( m_bBuff );
}

//void	CBasicVideoOption::LoadShadowlandButton ()
//{
//	m_bShadowLand = DXPARAMSET::GetInstance().m_bShadowLand;
//	m_pShadowlandButton->SetFlip ( m_bShadowLand );
//}

void	CBasicVideoOption::LoadReflectButton ()
{
	m_bRealReflect = DXPARAMSET::GetInstance().m_bRealReflect;
	m_pReflectButton->SetFlip ( m_bRealReflect );
}

void	CBasicVideoOption::LoadRefractButton ()
{
	m_bRefract = DXPARAMSET::GetInstance().m_bRefract;
	m_pRefractButton->SetFlip ( m_bRefract );
}

void	CBasicVideoOption::LoadGlowButton ()
{
	m_bGlow = DXPARAMSET::GetInstance().m_bGlow;
	m_pGlowButton->SetFlip ( m_bGlow );
}

void	CBasicVideoOption::LoadPostButton()
{
	m_bPost = DXPARAMSET::GetInstance().m_bPost;
	m_pPostButton->SetFlip( m_bPost );
}

void	CBasicVideoOption::LoadFrameLimitButton()
{
	m_bFrameLimit = DXPARAMSET::GetInstance().m_bFrameLimit;
	m_pFrameLimitButton->SetFlip( m_bFrameLimit );
}

void	CBasicVideoOption::LoadTargetEffectButton()
{
	m_bTargetEffect= DXPARAMSET::GetInstance().m_bTargetEffect;
	m_pTargetEffectButton->SetFlip( m_bTargetEffect );
}

void	CBasicVideoOption::LoadClickEffectButton()
{
	m_bClickEffect= DXPARAMSET::GetInstance().m_bClickEffect;
	m_pClickEffectButton->SetFlip( m_bClickEffect );
}

void	CBasicVideoOption::LoadMineEffectButton()
{
	m_bMineEffect= DXPARAMSET::GetInstance().m_bMineEffect;
	m_pMineEffectButton->SetFlip( m_bMineEffect );
}

void	CBasicVideoOption::LoadUseNormalMapButton ()
{
	if( DXPARAMSET::GetInstance().m_bPossibleNormalMap )
	{
		m_bUseNormalMap= DXPARAMSET::GetInstance().m_bUseNormalMap;
	}else{
		m_bUseNormalMap = FALSE;
	}
	m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );
}

void	CBasicVideoOption::LoadComboData ()
{	
	CString strTemp;

	{	// �⺻����
		m_pComboBoxDefaultRollOver->ClearText ();
		for ( int i = 0; i < DEFAULT_OPTION_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_DEFAULT_OPTION", i );
			m_pComboBoxDefaultRollOver->AddText ( strTemp );
		}
	}

	{	//	�׸���
		m_pComboBoxShadowRollOver->ClearText ();
		for ( int i = 0; i < LAND_QT_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SHADOW", i );
			m_pComboBoxShadowRollOver->AddText ( strTemp );
		}
	}

	{	//	��Ų ������
		m_pComboBoxCharDetailRollOver->ClearText ();
		for ( int i = 0; i < SKD_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", i );
			m_pComboBoxCharDetailRollOver->AddText ( strTemp );
		}
	}

	{	//	�þ�
		m_pComboBoxSightRollOver->ClearText ();
		for ( int i = 0; i < EMFR_NSIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SIGHT", i );
			m_pComboBoxSightRollOver->AddText ( strTemp );
		}
	}

	{	//	Texture Filter
		m_pComboBoxTexFilterRollOver->ClearText ();
		for ( int i = 0; i < TEXTURE_FILTER_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_TEXFILTER", i );
			m_pComboBoxTexFilterRollOver->AddText ( strTemp );
		}
	}

	{	//	Texture Quality
		m_pComboBoxTexQualityRollOver->ClearText ();
		for ( int i = 0; i < TEXTURE_QUALITY_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", i );
			m_pComboBoxTexQualityRollOver->AddText ( strTemp );
		}
	}

	{	//	PhysX Level
		m_pComboBoxPhysXClothRollOver->ClearText ();
		for ( int i = 0; i < PHYSX_CLOTH_LEVEL_SIZE; i++ )
		{
			strTemp = ID2GAMEWORD ( "HWOPTION_VIDEO_PHYSXCLOTH", i );
			m_pComboBoxPhysXClothRollOver->AddText ( strTemp );
		}
	}

    DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	CD3DApplication* const pD3DApp = pGlobalStage->GetD3DApp();
	if ( !pD3DApp )
	{
		GASSERT ( pD3DApp && "����̽��� �������� �ʾҽ��ϴ�." );
		return ;
	}
	D3DDeviceInfo* const pDeviceInfo = pD3DApp->GetCurDeviceInfo();

	{	//	�ػ�
		m_pComboBoxResolutionRollOver->ClearText ();
		for ( DWORD i = 0; i < pDeviceInfo->dwNumModes; i++ )
		{	
			DWORD dwWidth  = pDeviceInfo->modes[i].Width;
			DWORD dwHeight = pDeviceInfo->modes[i].Height;
			DWORD dwFormat = 0;

			switch ( pDeviceInfo->modes[i].Format )
			{
			case D3DFMT_R8G8B8:
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
				dwFormat = 32;
				break;
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
				dwFormat = 16;
				break;
			}

			if ( dwWidth < RANPARAM::X_RESOLUTION || dwHeight < RANPARAM::Y_RESOLUTION )
				strTemp.Format ( "%d%c%d%c%d (%s)", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, dwFormat, ID2GAMEWORD("DISCOURAGES") );
			else	
				strTemp.Format ( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, dwFormat );


			m_pComboBoxResolutionRollOver->AddText ( strTemp );
		}
	}

	//int index = 0;
	//DEVMODE dm;
	//ZeroMemory(&dm, sizeof(dm));
	//dm.dmSize = sizeof(dm);

	//m_pComboBoxResolutionRollOver->ClearText ();

	//while( EnumDisplaySettings(NULL, index++, &dm))
	//{
	//	DWORD dwWidth  = dm.dmPelsWidth;
	//	DWORD dwHeight = dm.dmPelsHeight;
	//	DWORD dwFormat = dm.dmBitsPerPel;

	//	if( dwWidth >= 800 && dwHeight >= 600 && dwFormat >= 16 )
	//	{
	//		strTemp.Format ( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, dwFormat );			
	//		m_pComboBoxResolutionRollOver->AddText ( strTemp );
	//	}

	//	ZeroMemory(&dm, sizeof(dm));
	//	dm.dmSize = sizeof(dm);
	//}

	//{	//	���ļ�
		//UINT* pHertz = NULL;
		//pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.SetHead();
		//while ( !pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.IsEnd() )
		//{
		//	pHertz = pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.GetCurrent();
		//	Temp.Format("%d Hz", *pHertz);
		//	((CAdvListBox*)pCont)->AddString(Temp);

		//	pDeviceInfo->modes[m_dwCurModeIndex].RefreshRate.GoNext();
		//}
	//}
}

void CBasicVideoOption::SetVisibleSingle( BOOL bVisible )
{
	CUIGroup::SetVisibleSingle ( bVisible );

	if( bVisible )
	{
		LoadCurrentOption();
	}
}

void CBasicVideoOption::SetTextSightCombo( DWORD dwIndex )
{
	if (dwIndex < EMFR_NSIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SIGHT", dwIndex ) );
		m_pComboBoxSightOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "�þ� ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextSkinDetailCombo( DWORD dwIndex )
{
	if (dwIndex < SKD_NSIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", dwIndex ) );
		m_pComboBoxCharDetailOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "��Ų ������ ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextShadowCharCombo( DWORD dwIndex )
{
	if ( 0 <= dwIndex && dwIndex < LAND_QT_NSIZE )
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SHADOW", dwIndex ) );
		m_pComboBoxShadowOpen->SetText ( strTemp );
	}
	else
	{
		GASSERT ( 0 && "�׸��� ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextResolutionCombo( DWORD dwWidth, DWORD dwHeight, EMSCREEN_FORMAT emFormat)
{
	int nBIT(0);

	if ( emFormat == EMSCREEN_F16 ) nBIT = 16;
	else							nBIT = 32;	

	CString strTemp;

	if ( dwWidth < RANPARAM::X_RESOLUTION || dwHeight < RANPARAM::Y_RESOLUTION )
		strTemp.Format ( "%d%c%d%c%d (%s)", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, nBIT, ID2GAMEWORD("DISCOURAGES") );
	else	
		strTemp.Format( "%d%c%d%c%d", dwWidth, cRESOLUTION_SEPERATOR, dwHeight, cRESOLUTION_SEPERATOR, nBIT );
	
	m_pComboBoxResolutionOpen->SetText ( strTemp );
}

void CBasicVideoOption::SetTextDefaultOptionCombo( DWORD dwIndex )
{
	if (dwIndex < DEFAULT_OPTION_SIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_DEFAULT_OPTION", dwIndex ) );
		m_pComboBoxDefaultOpen->SetText ( strTemp );
		m_dwVideoLevel = dwIndex;
	}
	else
	{
		GASSERT ( 0 && "�׸��� ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextTexFilterCombo( DWORD dwIndex )
{
	if (dwIndex < TEXTURE_FILTER_SIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_TEXFILTER", dwIndex ) );
		m_pComboBoxTexFilterOpen->SetText ( strTemp );
		m_dwTexFilter = dwIndex;
	}
	else
	{
		GASSERT ( 0 && "�ؽ��� Filter ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextTexQualityCombo( DWORD dwIndex )
{
	if (dwIndex < TEXTURE_QUALITY_SIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_SKINDETAIL", dwIndex ) );
		m_pComboBoxTexQualityOpen->SetText ( strTemp );
		m_dwTexQuality = dwIndex;
	}
	else
	{
		GASSERT ( 0 && "�ؽ��� Quality ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetTextPhysXClothCombo( DWORD dwIndex )
{
	if (dwIndex < PHYSX_CLOTH_LEVEL_SIZE)
	{
		CString strTemp( ID2GAMEWORD ( "HWOPTION_VIDEO_PHYSXCLOTH", dwIndex ) );
		m_pComboBoxPhysXClothOpen->SetText ( strTemp );
		m_dwPhysXCloth = dwIndex;
	}
	else
	{
		GASSERT ( 0 && "PhysX Level ������ �Ѿ�ϴ�." );
	}
}

void CBasicVideoOption::SetVideoLevel(int nIndex)
{
	switch(nIndex)
	{
	case 1:
		SetLowLevel();
		break;
	case 2:
		SetMediumLevel();
		break;
	case 3:
		SetHighLevel();
		break;
	case 4:
		SetVeryHighLevel();
		break;
	default:
		break;
	}
}

void CBasicVideoOption::SetVeryHighLevel()
{
	m_bBuff = true;
	//m_bShadowLand = true;
	m_bRealReflect = true;
	m_bRefract = true;
	m_bGlow = true;
	m_bPost = true;
	m_bTargetEffect = true;
	m_bMineEffect = true;
	m_bClickEffect = true;
	m_bUseNormalMap = true;

	m_dwShadowChar = 4;
	m_dwSight = 3;
	m_dwSkinDetail = 2;
	m_dwTexFilter = 4;	// Anisotropic_16X
	m_dwTexQuality = 2;	// High
	m_dwPhysXCloth = 2;

	m_pBuffButton->SetFlip( m_bBuff );
	//m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );	
	m_pTargetEffectButton->SetFlip( m_bTargetEffect );
	m_pMineEffectButton->SetFlip( m_bMineEffect );
	m_pClickEffectButton->SetFlip( m_bClickEffect );
	m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
	SetTextTexFilterCombo( m_dwTexFilter );
	SetTextTexQualityCombo( m_dwTexQuality );
	SetTextPhysXClothCombo( m_dwPhysXCloth );
}

void CBasicVideoOption::SetHighLevel()
{
	m_bBuff = true;
	//m_bShadowLand = true;
	m_bRealReflect = false;
	m_bRefract = true;
	m_bGlow = true;
	m_bPost = true;
	m_bTargetEffect = true;
	m_bMineEffect = true;
	m_bClickEffect = true;
	m_bUseNormalMap = true;

	m_dwShadowChar = 2;
	m_dwSight = 3;
	m_dwSkinDetail = 1;
	m_dwTexFilter = 2;	// Anisotropic_4X
	m_dwTexQuality = 2;	// High
	m_dwPhysXCloth = 2;

	m_pBuffButton->SetFlip( m_bBuff );
	//m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );	
	m_pTargetEffectButton->SetFlip( m_bTargetEffect );
	m_pMineEffectButton->SetFlip( m_bMineEffect );
	m_pClickEffectButton->SetFlip( m_bClickEffect );
	m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
	SetTextTexFilterCombo( m_dwTexFilter );
	SetTextTexQualityCombo( m_dwTexQuality );
	SetTextPhysXClothCombo( m_dwPhysXCloth );
}

void CBasicVideoOption::SetMediumLevel()
{
	m_bBuff = true;
	//m_bShadowLand = true;
	m_bRealReflect = false;
	m_bRefract = false;
	m_bGlow = false;
	m_bPost = false;
	m_bTargetEffect = true;
	m_bMineEffect = false;
	m_bClickEffect = true;
	m_bUseNormalMap = false;

	m_dwShadowChar = 1;
	m_dwSight = 2;
	m_dwSkinDetail = 1;
	m_dwTexFilter = 0;	// Trilinear
	m_dwTexQuality = 1;	// High
	m_dwPhysXCloth = 1;

	m_pBuffButton->SetFlip( m_bBuff );
	//m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );
	m_pTargetEffectButton->SetFlip( m_bTargetEffect );
	m_pMineEffectButton->SetFlip( m_bMineEffect );
	m_pClickEffectButton->SetFlip( m_bClickEffect );
	m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
	SetTextTexFilterCombo( m_dwTexFilter );
	SetTextTexQualityCombo( m_dwTexQuality );
	SetTextPhysXClothCombo( m_dwPhysXCloth );
}

void CBasicVideoOption::SetLowLevel()
{
	m_bBuff = false;
	//m_bShadowLand = false;
	m_bRealReflect = false;
	m_bRefract = false;
	m_bGlow = false;
	m_bPost = false;
	m_bTargetEffect = false;
	m_bMineEffect = false;
	m_bClickEffect = false;
	m_bUseNormalMap = false;

	m_dwShadowChar = 0;
	m_dwSight = 1;
	m_dwSkinDetail = 0;
	m_dwTexFilter = 0;	// Trilinear
	m_dwTexQuality = 1;	// Mid
	m_dwPhysXCloth = 1;

	m_pBuffButton->SetFlip( m_bBuff );
	//m_pShadowlandButton->SetFlip( m_bShadowLand );
	m_pReflectButton->SetFlip( m_bRealReflect );
	m_pRefractButton->SetFlip( m_bRefract );
	m_pGlowButton->SetFlip( m_bGlow );
	m_pPostButton->SetFlip( m_bPost );
	m_pTargetEffectButton->SetFlip( m_bTargetEffect );
	m_pMineEffectButton->SetFlip( m_bMineEffect );
	m_pClickEffectButton->SetFlip( m_bClickEffect );
	m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );

	SetTextShadowCharCombo( m_dwShadowChar );
	SetTextSightCombo( m_dwSight );
	SetTextSkinDetailCombo( m_dwSkinDetail );
	SetTextTexFilterCombo( m_dwTexFilter );
	SetTextTexQualityCombo( m_dwTexQuality );
	SetTextPhysXClothCombo( m_dwPhysXCloth );
}