#include "StdAfx.h"
#include "VideoOption.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicComboBox.h"
#include "../../../EngineLib/GUInterface/BasicComboBoxRollOver.h"
#include "../../../RanLogicClient/Tool/DxParamSet.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CBasicVideoOption::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case HWOPTION_VIDEO_BUFF_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bBuff = !DXPARAMSET::GetInstance().m_bBuff;
				//LoadBuffButton();
				m_bBuff = !m_bBuff;
				m_pBuffButton->SetFlip( m_bBuff );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	//case HWOPTION_VIDEO_SHADOWLAND_BUTTON:
	//	{
	//		if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
	//		{
	//			//DXPARAMSET::GetInstance().m_bShadowLand = !DXPARAMSET::GetInstance().m_bShadowLand;
	//			//LoadShadowlandButton ();
	//			m_bShadowLand = !m_bShadowLand;
	//			m_pShadowlandButton->SetFlip( m_bShadowLand );
	//			SetTextDefaultOptionCombo( 0 );
	//		}
	//	}
	//	break;

	case HWOPTION_VIDEO_REFLECT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bRealReflect = !DXPARAMSET::GetInstance().m_bRealReflect;
				//LoadReflectButton ();
				m_bRealReflect = !m_bRealReflect;
				m_pReflectButton->SetFlip( m_bRealReflect );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_REFRACT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bRefract = !DXPARAMSET::GetInstance().m_bRefract;
				//LoadRefractButton ();
				m_bRefract = !m_bRefract;
				m_pRefractButton->SetFlip( m_bRefract );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_GLOW_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bGlow = !DXPARAMSET::GetInstance().m_bGlow;
				//LoadGlowButton ();
				m_bGlow = !m_bGlow;
				m_pGlowButton->SetFlip( m_bGlow );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_POSTPROCESSING_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bPost = !DXPARAMSET::GetInstance().m_bPost;
				//LoadPostButton();
				m_bPost = !m_bPost;
				m_pPostButton->SetFlip( m_bPost );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_FRAME_LIMIT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				//DXPARAMSET::GetInstance().m_bPost = !DXPARAMSET::GetInstance().m_bPost;
				//LoadPostButton();
				m_bFrameLimit = !m_bFrameLimit;
				m_pFrameLimitButton->SetFlip( m_bFrameLimit );

				m_pInterface->DoModal ( ID2GAMEINTEXT("OPTION_RESTARTAPPLY") );
			}
		}
		break;

	case HWOPTION_VIDEO_TARGET_EFFECT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bTargetEffect = !m_bTargetEffect;
				m_pTargetEffectButton->SetFlip( m_bTargetEffect );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_MINE_EFFECT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bMineEffect = !m_bMineEffect;
				m_pMineEffectButton->SetFlip( m_bMineEffect );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_CLICK_EFFECT_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				m_bClickEffect = !m_bClickEffect;
				m_pClickEffectButton->SetFlip( m_bClickEffect );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_NORMALMAP_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( !DXPARAMSET::GetInstance().m_bPossibleNormalMap ) 
				{
					m_pInterface->DoModal ( ID2GAMEINTEXT("OPTION_NOTPOSSIBLE") );
					break;
				}
				m_bUseNormalMap = !m_bUseNormalMap;
				m_pUseNormalMapButton->SetFlip( m_bUseNormalMap );
				SetTextDefaultOptionCombo( 0 );

				m_pInterface->DoModal ( ID2GAMEINTEXT("OPTION_RESTARTAPPLY") );

			}
		}
		break;

	case HWOPTION_VIDEO_DEFAULT_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(TRUE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;
	case HWOPTION_VIDEO_DEFAULT_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				int nIndex = m_pComboBoxDefaultRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				
				SetVideoLevel(nIndex);
				m_dwVideoLevel = (DWORD)nIndex;
				SetTextDefaultOptionCombo( m_dwVideoLevel );
			}
		}
		break;

	case HWOPTION_VIDEO_RESOLUTION_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( TRUE );
					m_pComboBoxResolutionRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );	
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_RESOLUTION_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );
				break;
			}

			// 스크린 설정 적용.
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg )  )
			{
				// 창모드가 아닐경우만 동작되도록 한다.
				if ( RANPARAM::bScrWindowed )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT( "NEED_FULLSCREEN_WINDOW_MESSAGE" ), UI::MODAL_ERROR, UI::OK, NO_ID );
					//"This option does not support window mode. Please change to fullscreen mode."
					break;
				}

				int nIndex = m_pComboBoxResolutionRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				CString strTemp = m_pComboBoxResolutionRollOver->GetSelectText ( nIndex );					

				CString strNumber;
				int nStrLength = 0;
				int	nNumberStrLength = 0;


				strNumber = strTemp.Left ( strTemp.Find ( cRESOLUTION_SEPERATOR ) );					
				//DXPARAMSET::GetInstance().m_dwScrWidth = (DWORD) atoi ( strNumber.GetString () );
				m_dwScrWidth = (DWORD) atoi ( strNumber.GetString () );

				nNumberStrLength = strNumber.GetLength () + 1;	// 'x'문자 포함
				nStrLength = strTemp.GetLength ();
				strTemp = strTemp.Right ( nStrLength - nNumberStrLength );

				strNumber = strTemp.Left ( strTemp.Find ( cRESOLUTION_SEPERATOR ) );			
				//DXPARAMSET::GetInstance().m_dwScrHeight = (DWORD) atoi ( strNumber.GetString () );
				m_dwScrHeight = (DWORD) atoi ( strNumber.GetString () );

				nNumberStrLength = strNumber.GetLength () + 1;	// 'x'문자 포함
				nStrLength = strTemp.GetLength ();
				strTemp = strTemp.Right ( nStrLength - nNumberStrLength );

				strNumber = strTemp.Left ( 2 );	// 2자리로 예상 다르면 문제 생김
				DWORD dwBIT = (DWORD) atoi ( strNumber.GetString () );
				EMSCREEN_FORMAT emScrFormat;
				if ( dwBIT == 32 )	emScrFormat = EMSCREEN_F32;
				else				emScrFormat = EMSCREEN_F16;
				//DXPARAMSET::GetInstance().m_emScrFormat = emScrFormat;
				m_emScrFormat = emScrFormat;

				//LoadResolution ();
				SetTextResolutionCombo( m_dwScrWidth, m_dwScrHeight, m_emScrFormat );

				if( m_dwScrWidth < 1024 || m_dwScrHeight < 768 )
				{
					m_pInterface->DoModal( ID2GAMEINTEXT( "DISCOURAGES_MESSAGE" ), UI::MODAL_ERROR, UI::OK, NO_ID );
				}

			}
		}
		break;

	case HWOPTION_VIDEO_SHADOW_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( TRUE );
					m_pComboBoxShadowRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_SHADOW_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{				
				int nIndex = m_pComboBoxShadowRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				//DXPARAMSET::GetInstance().m_dwShadowChar = (DWORD)nIndex;
				//LoadShadow ();
				m_dwShadowChar = (DWORD)nIndex;
				SetTextShadowCharCombo( m_dwShadowChar );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_DETAIL_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( TRUE );
					m_pComboBoxCharDetailRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_CHARDETAIL_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxCharDetailRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				//DXPARAMSET::GetInstance().m_dwSkinDetail = (DWORD)nIndex;
				//LoadSkinDetail ();
				m_dwSkinDetail = (DWORD)nIndex;
				SetTextSkinDetailCombo( m_dwSkinDetail );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_SIGHT_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );					
					m_pComboBoxSightRollOver->SetVisibleSingle ( TRUE );
					m_pComboBoxSightRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_SIGHT_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxSightRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				//DXPARAMSET::GetInstance().m_dwFogRange = (DWORD)nIndex;
				//LoadSight ();
				m_dwSight = (DWORD)nIndex;
				SetTextSightCombo( m_dwSight );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_TEXFILTER_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );					
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxSightRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( TRUE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_TEXFILTER_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_TEXFILTER_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxTexFilterRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				m_dwTexFilter = (DWORD)nIndex;
				SetTextTexFilterCombo( m_dwTexFilter );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;

	case HWOPTION_VIDEO_TEXQUALITY_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );					
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxSightRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( TRUE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( FALSE );

					m_RollOverID = HWOPTION_VIDEO_TEXQUALITY_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_TEXQUALITY_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxTexQualityRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) 
					return;

				m_dwTexQuality = (DWORD)nIndex;
				SetTextTexQualityCombo( m_dwTexQuality );
				SetTextDefaultOptionCombo( 0 );

				m_pInterface->DoModal ( ID2GAMEINTEXT("OPTION_CHANGEMAP_APPLY") );
			}
		}
		break;

	case HWOPTION_VIDEO_PHYSXCLOTH_COMBO_OPEN:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( dwMsg & UIMSG_LB_DOWN )
				{
					m_pComboBoxDefaultRollOver->SetVisibleSingle(FALSE);
					m_pComboBoxResolutionRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxShadowRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxCharDetailRollOver->SetVisibleSingle ( FALSE );					
					m_pComboBoxSightRollOver->SetVisibleSingle ( FALSE );
					m_pComboBoxSightRollOver->SetScrollPercent ( 0.0f );
					m_pComboBoxTexFilterRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxTexQualityRollOver->SetVisibleSingle( FALSE );
					m_pComboBoxPhysXClothRollOver->SetVisibleSingle( TRUE );

					m_RollOverID = HWOPTION_VIDEO_PHYSXCLOTH_COMBO_ROLLOVER;
					m_bFirstLBUP = TRUE;
				}
			}
		}
		break;

	case HWOPTION_VIDEO_PHYSXCLOTH_COMBO_ROLLOVER:
		{
			if ( dwMsg & UI_MSG_COMBOBOX_ROLLOVER_SCROLL )
			{				
				DWORD dwMsg = GetMessageEx ();
				dwMsg &= ~UI_MSG_COMBOBOX_ROLLOVER_SCROLL;
				ResetMessageEx ();
				AddMessageEx ( dwMsg );

				break;
			}

			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{		
				int nIndex = m_pComboBoxPhysXClothRollOver->GetSelectIndex ();
				if ( nIndex < 0 ) return ;
				m_dwPhysXCloth = (DWORD)nIndex;
				SetTextPhysXClothCombo( m_dwPhysXCloth );
				SetTextDefaultOptionCombo( 0 );
			}
		}
		break;
	}
}