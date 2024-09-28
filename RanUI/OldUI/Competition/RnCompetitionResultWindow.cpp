#include "StdAfx.h"
#include "./RnCompetitionResultWindow.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"
#include "../../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../../EngineLib/GUInterface/UIWindowTitle.h"
#include "../../../EngineLib/GUInterface/UIWindowBody.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"

#include "../../../EngineLib/DxTools/d3dfont.h"
#include "../../../EngineLib/DxTools/DxClubMan.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../Interface/SkillFunc.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

#include <algorithm>

#define RESULT_INFO_DEFAULT_RESERVE_SIZE 12

void RnCompetitionResultRewardTooltip::CreateSubControl( const char* szTextureInfoControl )
{
    CBasicLineBoxSmart::CreateSubControl(szTextureInfoControl);

    CUIControl* pDummyBody = new CUIControl(m_pEngineDevice);
    pDummyBody->CreateSub(this, GetControlNameEx().c_str());
    pDummyBody->SetVisibleSingle(FALSE);
    RegisterControl(pDummyBody);
    pDummyTooltip = pDummyBody;

    CD3DFontPar* pFont9 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    CBasicTextBox* pDummyTextTitleBox = new CBasicTextBox(m_pEngineDevice);
    pDummyTextTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_TEXT");
    pDummyTextTitleBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyTextTitleBox);
    pDummyTextTitle = pDummyTextTitleBox;

    CBasicTextBox* pDummyTextBox = new CBasicTextBox(m_pEngineDevice);
    pDummyTextBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TEXT");
    pDummyTextBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyTextBox);
    pDummyText = pDummyTextBox;

    CBasicTextBox* pDummyItemTitleBox = new CBasicTextBox(m_pEngineDevice);
    pDummyItemTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_ITEM");
    pDummyItemTitleBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyItemTitleBox);
    pDummyItemTitle = pDummyItemTitleBox;

    CBasicTextBox* pDummyItemBox = new CBasicTextBox(m_pEngineDevice);
    pDummyItemBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_ITEM");
    pDummyItemBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyItemBox);
    pDummyItem = pDummyItemBox;

    CBasicTextBox* pDummyBuffTitleBox = new CBasicTextBox(m_pEngineDevice);
    pDummyBuffTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_BUFF");
    pDummyBuffTitleBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyBuffTitleBox);
    pDummyBuffTitle = pDummyBuffTitleBox;

    CBasicTextBox* pDummyBuffBox = new CBasicTextBox(m_pEngineDevice);
    pDummyBuffBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_BUFF");
    pDummyBuffBox->SetVisibleSingle(FALSE);
    RegisterControl(pDummyBuffBox);
    pDummyBuff = pDummyBuffBox;

    // 기여도;
    {
        CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
        pTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_TEXT");

        pTitleBox->SetFont ( pFont9 );
        pTitleBox->SetText(ID2GAMEWORD("RN_COMPETITION_REWARD", 0));
        RegisterControl(pTitleBox);
        pContributionTitleTextBox = pTitleBox;

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TEXT");

        pTextBox->SetFont ( pFont9 );
        RegisterControl(pTextBox);
        pContributionPointTextBox = pTextBox;
    }

    // 경험치;
    {
        CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
        pTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_TEXT");
        
//         UIRECT titlePos =  pTitleBox->GetLocalPos();
//         titlePos.top += (float)titlePos.sizeY;
//         pTitleBox->SetLocalPos(titlePos);

        pTitleBox->SetFont ( pFont9 );
        pTitleBox->SetText(ID2GAMEWORD("RN_COMPETITION_REWARD", 1));
        RegisterControl(pTitleBox);
        pExpTitleTextBox = pTitleBox;

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TEXT");

//         UIRECT textPos = pTextBox->GetLocalPos();
//         textPos.top += (float)textPos.sizeY;
//         pTextBox->SetLocalPos(textPos);

        pTextBox->SetFont ( pFont9 );
        RegisterControl(pTextBox);
        pExpTextBox = pTextBox;
    }

    // 게임머니;
    {
        CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
        pTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_TEXT");

//         UIRECT titlePos =  pTitleBox->GetLocalPos();
//         titlePos.top += (float)(titlePos.sizeY * 2);
//         pTitleBox->SetLocalPos(titlePos);

        pTitleBox->SetFont ( pFont9 );
        pTitleBox->SetText(ID2GAMEWORD("RN_COMPETITION_REWARD", 2));
        RegisterControl(pTitleBox);
        pMoneyTitleTextBox = pTitleBox;

        CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
        pTextBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TEXT");

//         UIRECT textPos = pTextBox->GetLocalPos();
//         textPos.top += (float)(textPos.sizeY * 2);
//         pTextBox->SetLocalPos(textPos);

        pTextBox->SetFont ( pFont9 );
        RegisterControl(pTextBox);
        pMoneyTextBox = pTextBox;
    }

    // 아이템;
    {
        CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
        pTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_ITEM");

//         UIRECT titlePos =  pTitleBox->GetLocalPos();
//         titlePos.top += (float)(titlePos.sizeY * 3);
//         pTitleBox->SetLocalPos(titlePos);

        pTitleBox->SetFont ( pFont9 );
        pTitleBox->SetText(ID2GAMEWORD("RN_COMPETITION_REWARD", 3));
        RegisterControl(pTitleBox);
        pItemTitleTextBox = pTitleBox;

        for ( int i = 0; i < EM_MAX_REWARD; i++ )
        {
            pItemImage[i] = new CUIControl( m_pEngineDevice );
            pItemImage[i]->CreateSub( this, "RN_COMPETITION_RESULT_INFO_REWARD_ITEM", UI_FLAG_DEFAULT );
//            UIRECT rect = pItemImage[i]->GetLocalPos();
            //const int nRow = i / 5;
            //const int nCol = i % 5;
//            rect.top += (float)(titlePos.sizeY * 3);
//            rect.left += (rect.sizeX + 2.0f) * (float)i;
            //rect.left += (rect.sizeX + 2.0f) * (float)nCol;            
            //rect.top += (rect.sizeY + 2.0f) * (float)nRow;
//            pItemImage[i]->SetLocalPos(rect);
            pItemImage[i]->SetUseRender(TRUE);
            RegisterControl( pItemImage[i] );
            pItemImage[i]->SetVisibleSingle(FALSE);

			pItemCount[i] = new CBasicTextBox( m_pEngineDevice );
			pItemCount[i]->CreateSub( this, "RN_COMPETITION_RESULT_INFO_REWARD_ITEM", UI_FLAG_DEFAULT );
			pItemCount[i]->SetFont( DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, _DEFAULT_FONT_SHADOW_EX_FLAG ) );
			pItemCount[i]->SetTextAlign( TEXT_ALIGN_BOTTOM | TEXT_ALIGN_RIGHT );
			//pItemCount[i]->SetTextColor( 0, NS_UITEXTCOLOR::RED );
			RegisterControl( pItemCount[i] );
			pItemCount[i]->SetVisibleSingle( FALSE );
        }
    }

    // 버프;
    {
        CBasicTextBox* pTitleBox = new CBasicTextBox(m_pEngineDevice);
        pTitleBox->CreateSub(this, "RN_COMPETITION_RESULT_INFO_REWARD_TITLE_BUFF");

//         UIRECT titlePos =  pTitleBox->GetLocalPos();
//         titlePos.top += (float)(titlePos.sizeY * 5);
//         pTitleBox->SetLocalPos(titlePos);

        pTitleBox->SetFont ( pFont9 );
        pTitleBox->SetText(ID2GAMEWORD("RN_COMPETITION_REWARD", 4));
        RegisterControl(pTitleBox);
        pBuffTitleTextBox = pTitleBox;

        for ( int i = 0; i < EM_MAX_REWARD; i++ )
        {
            pBuffImage[i] = new CUIControl( m_pEngineDevice );
            pBuffImage[i]->CreateSub( this, "RN_COMPETITION_RESULT_INFO_REWARD_BUFF", UI_FLAG_DEFAULT );
//            UIRECT rect = pBuffImage[i]->GetLocalPos();
            //const int nRow = i / 5;
            //const int nCol = i % 5;
//          rect.top += (float)(titlePos.sizeY * 5);
//          rect.left += (rect.sizeX + 2.0f) * (float)i;
            //rect.left += (rect.sizeX + 2.0f) * (float)nCol;
            //rect.top += (rect.sizeY + 2.0f) * (float)nRow;
//            pBuffImage[i]->SetLocalPos(rect);
            pBuffImage[i]->SetUseRender(TRUE);
            RegisterControl( pBuffImage[i] );
            pBuffImage[i]->SetVisibleSingle(FALSE);
        }
    }
}

void RnCompetitionResultRewardTooltip::SetTitleInfo(const int emTitleType)
{
    if ( emTitleType == emCurrentTitleType )
        return;

    emCurrentTitleType = emTitleType;

    if ( (emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD) == false )
        return;

    pContributionTitleTextBox->SetVisibleSingle(FALSE);
    pExpTitleTextBox->SetVisibleSingle(FALSE);
    pMoneyTitleTextBox->SetVisibleSingle(FALSE);
    pItemTitleTextBox->SetVisibleSingle(FALSE);
    pBuffTitleTextBox->SetVisibleSingle(FALSE);

    pContributionPointTextBox->SetVisibleSingle(FALSE);
    pExpTextBox->SetVisibleSingle(FALSE);
    pMoneyTextBox->SetVisibleSingle(FALSE);
    for ( unsigned int i(EM_MAX_REWARD); i--; )
    {
        pItemImage[i]->SetVisibleSingle(FALSE);
		pItemCount[i]->SetVisibleSingle(FALSE);
        pBuffImage[i]->SetVisibleSingle(FALSE);
    }

    float totalTopPos(0.0f);

    if ( emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD_CONTRIBUTION )
    {
        UIRECT _rect = pDummyTextTitle->GetLocalPos();
        _rect.top += totalTopPos;
        pContributionTitleTextBox->SetLocalPos(_rect);
        pContributionTitleTextBox->SetVisibleSingle(TRUE);

        UIRECT _rect2 = pDummyText->GetLocalPos();
        _rect2.top += totalTopPos;
        pContributionPointTextBox->SetLocalPos(_rect2);
        pContributionPointTextBox->SetVisibleSingle(TRUE);

        totalTopPos += _rect.sizeY;
    }
    if ( emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD_EXP )
    {
        UIRECT _rect = pDummyTextTitle->GetLocalPos();
        _rect.top += totalTopPos;
        pExpTitleTextBox->SetLocalPos(_rect);
        pExpTitleTextBox->SetVisibleSingle(TRUE);

        UIRECT _rect2 = pDummyText->GetLocalPos();
        _rect2.top += totalTopPos;
        pExpTextBox->SetLocalPos(_rect2);
        pExpTextBox->SetVisibleSingle(TRUE);

        totalTopPos += _rect.sizeY;
    }
    if ( emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD_MONEY )
    {
        UIRECT _rect = pDummyTextTitle->GetLocalPos();
        _rect.top += totalTopPos;
        pMoneyTitleTextBox->SetLocalPos(_rect);
        pMoneyTitleTextBox->SetVisibleSingle(TRUE);

        UIRECT _rect2 = pDummyText->GetLocalPos();
        _rect2.top += totalTopPos;
        pMoneyTextBox->SetLocalPos(_rect2);
        pMoneyTextBox->SetVisibleSingle(TRUE);

        totalTopPos += _rect.sizeY;
    }
    if ( emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD_ITEM )
    {
        UIRECT _rect = pDummyItemTitle->GetLocalPos();
        _rect.top += totalTopPos;
        pItemTitleTextBox->SetLocalPos(_rect);
        pItemTitleTextBox->SetVisibleSingle(TRUE);

        for ( int i = 0; i < EM_MAX_REWARD; i++ )
        {
            UIRECT _rect2 = pDummyItem->GetLocalPos();
            _rect2.top += totalTopPos;

            const int nRow = i / 4;
            const int nCol = i % 4;
            _rect2.left += (_rect2.sizeX + 2.0f) * (float)nCol;
            _rect2.top += (_rect2.sizeY + 2.0f) * (float)nRow;

            pItemImage[i]->SetLocalPos(_rect2);
            pItemImage[i]->SetVisibleSingle(TRUE);

			pItemCount[i]->SetLocalPos(_rect2);
			pItemCount[i]->SetVisibleSingle(TRUE);
        }

        totalTopPos += _rect.sizeY;
    }
    if ( emTitleType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD_BUFF )
    {
        UIRECT _rect = pDummyBuffTitle->GetLocalPos();
        _rect.top += totalTopPos;
        pBuffTitleTextBox->SetLocalPos(_rect);
        pBuffTitleTextBox->SetVisibleSingle(TRUE);

        for ( int i = 0; i < EM_MAX_REWARD; i++ )
        {
            UIRECT _rect2 = pDummyBuff->GetLocalPos();
            _rect2.top += totalTopPos;

            const int nRow = i / 4;
            const int nCol = i % 4;
            _rect2.left += (_rect2.sizeX + 2.0f) * (float)nCol;
            _rect2.top += (_rect2.sizeY + 2.0f) * (float)nRow;

            pBuffImage[i]->SetLocalPos(_rect2);
            pBuffImage[i]->SetVisibleSingle(TRUE);
        }

        totalTopPos += _rect.sizeY;
    }

    {
        UIRECT _rect = GetGlobalPos();
        UIRECT _rect2 = pDummyTooltip->GetLocalPos();
        _rect.sizeY = _rect2.sizeY + totalTopPos;
        SetGlobalPos(_rect);
        InitDeviceObjects(m_pEngineDevice->GetDevice());
        RestoreDeviceObjects(m_pEngineDevice->GetDevice());
    }
}

void RnCompetitionResultRewardTooltip::SetInfo(const RnCompetitionResultContentRewardObj* _pRewardObj)
{
    if ( _pRewardObj == NULL )
        return;

    pContributionPointTextBox->SetText(_pRewardObj->strContributionPoint.c_str());
    pExpTextBox->SetText(_pRewardObj->strExpReward.c_str());
    pMoneyTextBox->SetText(_pRewardObj->strMoneyReward.c_str());

    for ( int i = 0; i < EM_MAX_REWARD; i++ )
    {
        pItemImage[i]->SetVisibleSingle(FALSE);
		pItemCount[i]->SetVisibleSingle(FALSE);
    }

    for ( unsigned i = 0; i < _pRewardObj->vecItemReward.size(); i++ )
    {
        if ( i >= EM_MAX_REWARD )
            break;

        pItemImage[i]->Reset();

        const SITEM* pItemData = GLogicData::GetInstance().GetItem( _pRewardObj->vecItemReward[i].itemID );
        if ( !pItemData )
            return;

        pItemImage[i]->SetVisibleSingle(TRUE);
        pItemImage[i]->SetTexturePos ( NS_SKILL::GetIconTexurePos ( pItemData->sBasicOp.sICONID ) );
        pItemImage[i]->SetTextureName ( pItemData->GetInventoryFile() );
        pItemImage[i]->InitDeviceObjects ( m_pEngineDevice->GetDevice() );

		pItemCount[i]->SetText(
			sc::string::format( "%1%",
			_pRewardObj->vecItemReward[i].wAmount ).c_str() );
		pItemCount[i]->SetTextColor( 0, NS_UITEXTCOLOR::GREEN );
		pItemCount[i]->SetVisibleSingle(TRUE);
    }

    for ( int i = 0; i < EM_MAX_REWARD; i++ )
    {
        pBuffImage[i]->SetVisibleSingle(FALSE);
    }

    for ( unsigned i = 0; i < _pRewardObj->vecBuffReward.size(); i++ )
    {
        if ( i >= EM_MAX_REWARD )
            break;

        pBuffImage[i]->Reset();

        PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( _pRewardObj->vecBuffReward[i] );
        if ( !pSkill )
            return;

        SNATIVEID sICONINDEX	 = pSkill->m_sEXT_DATA.sICONINDEX;
        CString	  strTextureName = pSkill->m_sEXT_DATA.strICONFILE.c_str();	
        pBuffImage[i]->SetVisibleSingle(TRUE);
        pBuffImage[i]->SetTexturePos ( NS_SKILL::GetIconTexurePos ( sICONINDEX ) );
        pBuffImage[i]->SetTextureName ( strTextureName );
        pBuffImage[i]->InitDeviceObjects ( m_pEngineDevice->GetDevice() );         
    }
}

void RnCompetitionResultInfoSlot::SetInfo( const RnCompetitionResultContent& _sResultContent )
{
    static const std::string strClass[GLCI_NUM_ETC] = 
    {
        "CREATE_CHAR_CLASS_BRAWLER_MALE",
        "CREATE_CHAR_CLASS_SWORDMAN_MALE",
        "CREATE_CHAR_CLASS_ARCHER_FEMALE",
        "CREATE_CHAR_CLASS_SHAMAN_FEMALE",
        "CREATE_CHAR_CLASS_EXTREME_MALE",
        "CREATE_CHAR_CLASS_EXTREME_FEMALE",
        "CREATE_CHAR_CLASS_BRAWLER_FEMALE",
        "CREATE_CHAR_CLASS_SWORDMAN_FEMALE",
        "CREATE_CHAR_CLASS_ARCHER_MALE",
        "CREATE_CHAR_CLASS_SHAMAN_MALE",
        "CREATE_CHAR_CLASS_SCIENTIST_MALE",
        "CREATE_CHAR_CLASS_SCIENTIST_FEMALE",
        "CREATE_CHAR_CLASS_ASSASSIN_MALE",
        "CREATE_CHAR_CLASS_ASSASSIN_FEMALE",
		"CREATE_CHAR_CLASS_TRICKER_MALE",
		"CREATE_CHAR_CLASS_TRICKER_FEMALE",
        "CREATE_CHAR_CLASS_ETC_MALE",
        "CREATE_CHAR_CLASS_ETC_FEMALE",
    };

    static const std::string strWinLoseXmlKeyword[2] = 
    {
        "COMPETITION_RESULT_ICON_WIN",
        "COMPETITION_RESULT_ICON_LOSE",
    };

    pContentData = &_sResultContent;

    const unsigned int _contentUISize = vecContentUI.size();
    const unsigned int _contentSize = _sResultContent.vecResultObj.size();

    if ( _contentUISize != _contentSize )
        return;

    for ( unsigned int i = 0; i < _contentUISize; i++ )
    {
        RnCompetitionResultObjUI& _resultOjbUI = vecContentUI[i];
        switch ( _resultOjbUI.emItemType )
        {
        case RnCompetitionResultTitleObj::EM_CONTENT_INT:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                CBasicTextBox* _pTextBox = (CBasicTextBox*)_resultOjbUI.pUI;
                if ( _pTextBox == NULL )
                    return;
                _pTextBox->SetText(sc::string::format("%1%", _resultContent.ContentItemInt).c_str(), (bMyInfoSlot ? NS_UITEXTCOLOR::GREEN : NS_UITEXTCOLOR::WHITE));
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_FLOAT:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                CBasicTextBox* _pTextBox = (CBasicTextBox*)_resultOjbUI.pUI;
                if ( _pTextBox == NULL )
                    return;
                _pTextBox->SetText(sc::string::format("%1%", _resultContent.ContentItemFloat).c_str(), (bMyInfoSlot ? NS_UITEXTCOLOR::GREEN : NS_UITEXTCOLOR::WHITE));
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_STRING:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                CBasicTextBox* _pTextBox = (CBasicTextBox*)_resultOjbUI.pUI;
                if ( _pTextBox == NULL )
                    return;
                _pTextBox->SetText(_resultContent.ContentItemString.c_str(), (bMyInfoSlot ? NS_UITEXTCOLOR::GREEN : NS_UITEXTCOLOR::WHITE));
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_ICON_SCHOOL:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                CSwapImage* _pImage = (CSwapImage*)_resultOjbUI.pUI;
                if ( _pImage == NULL )
                    return;
                _pImage->SetImage(sc::string::format("RN_COMPETITION_RESULT_IMAGE_SCHOOL%1%", _resultContent.ContentItemInt).c_str());
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLASS:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                if ( _resultContent.ContentItemInt >= GLCI_NUM_ETC )
                    return;
                CSwapImage* _pImage = (CSwapImage*)_resultOjbUI.pUI;
                if ( _pImage == NULL )
                    return;
                if ( (_resultContent.ContentItemInt < 0) || (_resultContent.ContentItemInt >= GLCI_NUM_ETC) )
                    return;
                _pImage->SetImage(strClass[_resultContent.ContentItemInt].c_str());
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
				_resultOjbUI.iParam = _resultContent.ContentItemInt;
            }
            break;
        case RnCompetitionResultTitleObj::EM_CONTENT_ICON_WINLOSE:
            {
                const RnCompetitionResultContentObj& _resultContent =  _sResultContent.vecResultObj[i];
                _resultOjbUI.iParam = _resultContent.ContentItemInt;
                CSwapImage* _pImage = (CSwapImage*)_resultOjbUI.pUI;
                if ( _pImage == NULL )
                    return;
                if ( (_resultContent.ContentItemInt < 0) || (_resultContent.ContentItemInt >= 2) )
                    return;
                _pImage->SetImage(strWinLoseXmlKeyword[_resultContent.ContentItemInt].c_str());
            }
            break;
		case RnCompetitionResultTitleObj::EM_CONTENT_ICON_COUNTRYMARK:
			{
				const RnCompetitionResultContentObj& resultContent =
					_sResultContent.vecResultObj[i];

				CSwapImage* pImage = static_cast< CSwapImage* >( _resultOjbUI.pUI );
				if ( NULL == pImage )
					return;

				Country::GLCountryManClient* pCountryMan =
					Country::GLCountryManClient::Instance();
				if ( NULL == pCountryMan )
					return;

				Country::GLCountry* pCountry =
					pCountryMan->GetCountryUsingID( resultContent.ContentItemInt );
				if ( NULL == pCountry )
					return;

				pImage->SetImage( pCountry->GetCountryMiniImageID().c_str() );
			}
			break;
        }
    }
}

void RnCompetitionResultInfoSlot::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void RnCompetitionResultInfoSlot::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    if ( pContentData )
    {
        if ( (RNCOMPETITION_RESULT_REWARD_BUTTON <= ControlID) && ( ControlID < (RNCOMPETITION_RESULT_REWARD_BUTTON + (int)pContentData->vecRewardObj.size()) ) )
        {
            if ( CHECK_MOUSE_IN(dwMsg) )
            {
                const int nRewardNum = ControlID - RNCOMPETITION_RESULT_REWARD_BUTTON;
                //pGaeaClient->GetInterface()->ShowRnCompetitionRewardTooltop(vecContentRewardUI[nRewardNum].emItemType, &(pContentData->vecRewardObj[nRewardNum]));
            }
        }
    }
    CUIGroup::TranslateUIMessage(ControlID, dwMsg);
}

void RnCompetitionResultInfoSlot::CreateSubControl( const RnCompetitionResultTitle& _sResultTab, const unsigned int _nSlotNumber)
{
    vecContentUI.clear();
    vecContentUI.reserve(RESULT_INFO_DEFAULT_RESERVE_SIZE);
    vecContentRewardUI.clear();

    unsigned int _tabTotalBoxLeft(0);
    const unsigned int _nTabSize = _sResultTab.vecTitleObj.size();
    for ( unsigned int i = 0; i < _nTabSize ; i++ )
    {
        const RnCompetitionResultTitleObj& _titleObj = _sResultTab.vecTitleObj[i];
        if ( _titleObj.emItemType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD )
        {
            CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
            pButton->CreateSub ( this, "RN_COMPETITION_RESULT_INFO_REWARD_BUTTON", UI_FLAG_DEFAULT, RNCOMPETITION_RESULT_REWARD_BUTTON + vecContentRewardUI.size() );
            pButton->CreateFlip ( "RN_COMPETITION_RESULT_INFO_REWARD_BUTTON_OVER", CBasicButton::MOUSEIN_FLIP );
            {
                UIRECT _boxRect = pButton->GetLocalPos();
                _boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
                _boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
                pButton->SetLocalPos(_boxRect);

                _tabTotalBoxLeft += _titleObj.nBoxSize;
            }
            RegisterControl ( pButton );

            RnCompetitionResultObjUI _objUI;
            _objUI.emItemType = _titleObj.emItemType;
            _objUI.pUI = pButton;
            vecContentRewardUI.push_back(_objUI);
        }
        else
        {
            switch ( _titleObj.emItemType )
            {
            case RnCompetitionResultTitleObj::EM_CONTENT_INT:
            case RnCompetitionResultTitleObj::EM_CONTENT_FLOAT:
            case RnCompetitionResultTitleObj::EM_CONTENT_STRING:
                {
                    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
                    const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

                    CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
                    pTextBox->CreateSub(this,"RN_COMPETITION_RESULT_INFO_TEXT_BOX");

                    {
                        UIRECT _boxRect = pTextBox->GetLocalPos();
                        _boxRect.top += (float)(_nSlotNumber * _boxRect.sizeY);
                        _boxRect.left += (float)_tabTotalBoxLeft;
                        _boxRect.sizeX = (float)_titleObj.nBoxSize;
                        pTextBox->SetLocalPos(_boxRect);

                        _tabTotalBoxLeft += _titleObj.nBoxSize;
                    }

                    pTextBox->SetFont ( pFont );
                    pTextBox->SetTextAlign ( nAlignCenter );	
                    RegisterControl(pTextBox);

                    RnCompetitionResultObjUI _objUI;
                    _objUI.emItemType = _titleObj.emItemType;
                    _objUI.pUI = pTextBox;
                    vecContentUI.push_back(_objUI);
                }
                break;
            case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLASS:
                {
                    CSwapImage* pImage = new CSwapImage(m_pEngineDevice);
                    pImage->CreateSub(this,"RN_COMPETITION_RESULT_INFO_CLASS_BOX");

                    {
                        UIRECT _boxRect = pImage->GetLocalPos();
                        _boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
                        _boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
                        pImage->SetLocalPos(_boxRect);

                        _tabTotalBoxLeft += _titleObj.nBoxSize;
                    }

                    RegisterControl(pImage);

                    RnCompetitionResultObjUI _objUI;
                    _objUI.emItemType = _titleObj.emItemType;
                    _objUI.pUI = pImage;
                    vecContentUI.push_back(_objUI);
                }
                break;
            case RnCompetitionResultTitleObj::EM_CONTENT_ICON_SCHOOL:
                {
                    CSwapImage* pImage = new CSwapImage(m_pEngineDevice);
                    pImage->CreateSub(this,"RN_COMPETITION_RESULT_INFO_SCHOOL_BOX");

                    {
                        UIRECT _boxRect = pImage->GetLocalPos();
                        _boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
                        _boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
                        pImage->SetLocalPos(_boxRect);

                        _tabTotalBoxLeft += _titleObj.nBoxSize;
                    }

                    RegisterControl(pImage);

                    RnCompetitionResultObjUI _objUI;
                    _objUI.emItemType = _titleObj.emItemType;
                    _objUI.pUI = pImage;
                    vecContentUI.push_back(_objUI);
                }
                break;
            case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK:
                {
                    CSwapImage* pImage = new CSwapImage(m_pEngineDevice);
                    pImage->CreateSub(this,"RN_COMPETITION_RESULT_INFO_CLUBMARK_BOX");

                    {
                        UIRECT _boxRect = pImage->GetLocalPos();
                        _boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
                        _boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
                        pImage->SetLocalPos(_boxRect);

                        _tabTotalBoxLeft += _titleObj.nBoxSize;
                    }

                    RegisterControl(pImage);

					// ClubMark의 경우 직접 렌더링을 해줘야 함;
					pImage->SetUseRender(TRUE);

                    RnCompetitionResultObjUI _objUI;
                    _objUI.emItemType = _titleObj.emItemType;
                    _objUI.pUI = pImage;
                    vecContentUI.push_back(_objUI);
                }
                break;
            case RnCompetitionResultTitleObj::EM_CONTENT_ICON_WINLOSE:
                {
                    CSwapImage* pImage = new CSwapImage(m_pEngineDevice);
                    pImage->CreateSub(this,"RN_COMPETITION_RESULT_INFO_WINLOSE_BOX");

                    {
                        UIRECT _boxRect = pImage->GetLocalPos();
                        _boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
                        _boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
                        pImage->SetLocalPos(_boxRect);

                        _tabTotalBoxLeft += _titleObj.nBoxSize;
                    }

                    RegisterControl(pImage);

                    RnCompetitionResultObjUI _objUI;
                    _objUI.emItemType = _titleObj.emItemType;
                    _objUI.pUI = pImage;
                    vecContentUI.push_back(_objUI);
                }
                break;

			case RnCompetitionResultTitleObj::EM_CONTENT_ICON_COUNTRYMARK:
				{
					CSwapImage* pImage = new CSwapImage( m_pEngineDevice );
					pImage->CreateSub(
						this, "RN_COMPETITION_RESULT_INFO_CLUBMARK_BOX" );

					{
						UIRECT _boxRect = pImage->GetLocalPos();
						_boxRect.top = (float)((_nSlotNumber * _boxRect.top) + (((_boxRect.top - _boxRect.sizeY) /2 ) + _boxRect.top));
						_boxRect.left = (float)(_tabTotalBoxLeft + ((_titleObj.nBoxSize - _boxRect.sizeX) /2 ));
						pImage->SetLocalPos(_boxRect);

						_tabTotalBoxLeft += _titleObj.nBoxSize;
					}

					RegisterControl( pImage );

					RnCompetitionResultObjUI _objUI;
					_objUI.emItemType = _titleObj.emItemType;
					_objUI.pUI = pImage;
					vecContentUI.push_back(_objUI);
				}
				break;
            }   
        }
    }
}

HRESULT RnCompetitionResultInfoSlot::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	// 주의! 클럽마크 랜더링 방식이 괴상함 [7/29/2014 hsshin];
	// 렌더링 직전에 클럽마크 UI를 끄고 랜더링 한 후 클럽마크 UI켜서 수동으로 Render를 호출;
	// ( 클럽마크 UI는 Texture설정을 렌더링 시점에서 매번 해주는 것으로 보아 수동으로 랜더링하기 위한게 이유로 보임 );
	const unsigned int _numContentUI = vecContentUI.size();
	for ( unsigned int i = 0; i < _numContentUI ; i++ )
	{
		if ( vecContentUI[i].emItemType == RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK )
			vecContentUI[i].pUI->SetVisibleSingle(FALSE);
	}

	hr = CUIGroup::Render ( pd3dDevice );
	if ( FAILED ( hr ) )
		return hr;

	const int nServer = pGaeaClient->GetCharacter()->m_dwServerID;

	for ( unsigned int i = 0; i < _numContentUI ; i++ )
	{
		RnCompetitionResultObjUI& _resultObj = vecContentUI[i];
		if ( _resultObj.emItemType == RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK )
		{
			_resultObj.pUI->SetVisibleSingle(TRUE);

			const clubmark::DXDATA& sMarkData = DxClubMan::GetInstance().GetClubData ( pd3dDevice, nServer, vecContentUI[i].iParam );

			_resultObj.pUI->SetTexturePos ( 0, sMarkData.vTex_1_LU );
			_resultObj.pUI->SetTexturePos ( 1, sMarkData.vTex_2_RU );
			_resultObj.pUI->SetTexturePos ( 3, sMarkData.vTex_3_LD );
			_resultObj.pUI->SetTexturePos ( 2, sMarkData.vTex_4_RD );

			_resultObj.pUI->SetTexture ( sMarkData.pddsTexture );
			hr = _resultObj.pUI->Render ( pd3dDevice );
			if ( FAILED ( hr ) )
				return hr;
		}
	}

	return S_OK;
}

void RnCompetitionResultTabButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void RnCompetitionResultTabButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case RNCOMPETITION_RESULT_TAB_BUTTON:
        {
            if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
            {
                AddMessageEx(RNCOMPETITION_RESULT_TAB_BUTTON_MESSAGE);
            }
        }
        break;
    }
    CUIGroup::TranslateUIMessage(ControlID, dwMsg);
}

void RnCompetitionResultTabButton::CreateSubControl( const unsigned int _nTabNum, std::string& strTabName )
{
    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // 탭 활성화;
    {
        pActiveBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pActiveBox->CreateSub( this, "RN_COMPETITION_RESULT_TAB_BUTTON" );
        pActiveBox->CreateSubControl( "RN_COMPETITION_RESULT_TAB_BUTTON_ACTIVE" );
        UIRECT _boxPos = pActiveBox->GetLocalPos();
        _boxPos.left += (float)((_boxPos.sizeX + _boxPos.left) * _nTabNum);
        pActiveBox->SetLocalPos(_boxPos);
        RegisterControl( pActiveBox );
    }

    // 탭 비활성화;
    {
        pInactiveBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pInactiveBox->CreateSub( this, "RN_COMPETITION_RESULT_TAB_BUTTON" );
        pInactiveBox->CreateSubControl( "RN_COMPETITION_RESULT_TAB_BUTTON_INACTIVE" );
        UIRECT _boxPos = pInactiveBox->GetLocalPos();
        _boxPos.left += (float)((_boxPos.sizeX + _boxPos.left) * _nTabNum);
        pInactiveBox->SetLocalPos(_boxPos);
        RegisterControl( pInactiveBox );
    }

    // 탭 제목 텍스트 박스;
    {
        pTabNameTextBox = new CBasicTextBox(m_pEngineDevice);
        pTabNameTextBox->CreateSub(this, "RN_COMPETITION_RESULT_TAB_BUTTON", UI_FLAG_DEFAULT, RNCOMPETITION_RESULT_TAB_BUTTON);
        pTabNameTextBox->SetFont( pFont );
        pTabNameTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
        pTabNameTextBox->SetText(strTabName.c_str());
        UIRECT _boxPos = pTabNameTextBox->GetLocalPos();
        _boxPos.left += (float)((_boxPos.sizeX + _boxPos.left) * _nTabNum);
        pTabNameTextBox->SetLocalPos(_boxPos);
        RegisterControl(pTabNameTextBox);
        pTabNameTextBox->SetVisibleSingle(TRUE);
    }
    SetEnable(false);
}

void RnCompetitionResultTabButton::SetEnable( const bool bEnable )
{
    pActiveBox->SetVisibleSingle( bEnable );
    pInactiveBox->SetVisibleSingle( !bEnable );
    pTabNameTextBox->SetText(pTabNameTextBox->GetText(0), bEnable ? NS_UITEXTCOLOR::BLACK : NS_UITEXTCOLOR::WHITESMOKE );
}

void RnCompetitionResultTabWindow::CreateSubControl( const RnCompetitionResultTab& _sResultTab )
{
    pTabData = &_sResultTab;

    CD3DFontPar* pFont = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );

    // 해당 탭의 뒷 배경;
    {
        CBasicLineBoxSmart* pLineBox = new CBasicLineBoxSmart(m_pEngineDevice);
        pLineBox->CreateSub( this, "RN_COMPETITION_RESULT_INFO_LINEBOX", UI_FLAG_DEFAULT );
        pLineBox->CreateSubControl( "RN_COMPETITION_RESULT_INFO_LINEBOX_TEXTINFO" );
        RegisterControl( pLineBox );
        pLineWindowBox = pLineBox;
    }

    // 탭 스크롤;
    {
        //! 배경;
        CUIControl* pStaticBackgroundHead = new CUIControl(m_pEngineDevice);
        pStaticBackgroundHead->CreateSub ( this, "RN_COMPETITION_RESULT_SCROLLBAR_BACKGROUND", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        pStaticBackgroundHead->SetDiffuse( NS_UITEXTCOLOR::BLACK );
        pStaticBackgroundHead->SetUseRender( TRUE );
        RegisterControl ( pStaticBackgroundHead );

        pScrollBarBackground = pStaticBackgroundHead;

        //! 스크롤;
        CBasicScrollBarEx* _pScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
        _pScrollBar->CreateSub ( this, "BASIC_SCROLLBAR", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
        _pScrollBar->CreateBaseScrollBar ( "RN_COMPETITION_RESULT_SCROLLBAR" );
        _pScrollBar->GetThumbFrame()->SetState ( 150, 10 );

        //  해당 컨트롤 위에서만 마우스 휠이 동작한다.;
        _pScrollBar->GetThumbFrame()->SetScrollParent( this ); 
        RegisterControl ( _pScrollBar );  

        pScrollBar = _pScrollBar;
    }

    unsigned int _titleBoxLeft(0);
    // 결과 제목 슬롯 UI;
    {
        vecTitleNameTextBox.reserve(RESULT_INFO_DEFAULT_RESERVE_SIZE);
        nRewardTitleNum = -1;
        const unsigned int numTitle(_sResultTab.sTabInfo.sTitle.vecTitleObj.size());
        for ( unsigned int i = 0 ; i < numTitle; i++ )
        {
            const RnCompetitionResultTitleObj& _titleObj = _sResultTab.sTabInfo.sTitle.vecTitleObj[i];

            if ( _titleObj.emItemType & RnCompetitionResultTitleObj::EM_CONTENT_REWARD )
            {
                nRewardTitleNum = i;
            }

            CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
            pTextBox->CreateSub(this,"RN_COMPETITION_RESULT_INFO_SLOT");

            UIRECT _textRect = pTextBox->GetLocalPos();
            _textRect.left += (float)_titleBoxLeft;
            _textRect.sizeX = (float)_titleObj.nBoxSize;
            pTextBox->SetLocalPos(_textRect);

            _titleBoxLeft += _titleObj.nBoxSize;

            pTextBox->SetFont( pFont );
            pTextBox->SetTextAlign( TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y );
            pTextBox->SetText(_titleObj.strTitleName.c_str());

            RegisterControl(pTextBox);
            vecTitleNameTextBox.push_back(pTextBox);

            if ( i != (numTitle -1) )
            {
                CUIControl* pSlotVerticalLine = new CUIControl(m_pEngineDevice);
                pSlotVerticalLine->CreateSub ( this, "RN_COMPETITION_RESULT_SLOT_LINE", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
                UIRECT _headPos = pSlotVerticalLine->GetLocalPos();
                _headPos.left = _textRect.left + _textRect.sizeX;
                pSlotVerticalLine->SetLocalPos(_headPos);
                pSlotVerticalLine->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
                pSlotVerticalLine->SetUseRender( TRUE );
                RegisterControl ( pSlotVerticalLine );
            }

            CUIControl* pTitleHorizonLine = new CUIControl(m_pEngineDevice);
            pTitleHorizonLine->CreateSub ( this, "RN_COMPETITION_RESULT_TITLE_LINE", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
            UIRECT _headPos = pTitleHorizonLine->GetLocalPos();
            _headPos.sizeX = (float)_titleBoxLeft;
            pTitleHorizonLine->SetLocalPos(_headPos);
            pTitleHorizonLine->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
            pTitleHorizonLine->SetUseRender( TRUE );
            RegisterControl ( pTitleHorizonLine );
        }
    }

    // 결과 내용 슬롯 UI;
    {
        myInfoSlot = new RnCompetitionResultInfoSlot(pGaeaClient, m_pEngineDevice, true);
        myInfoSlot->CreateSub(this, "RN_COMPETITION_RESULT_MY_INFO_SLOT");
        myInfoSlot->CreateSubControl(_sResultTab.sTabInfo.sTitle, 0);
        RegisterControl(myInfoSlot);

        vecInfoSlot.reserve(RESULT_INFO_MAX_SLOT);
        for ( unsigned int i = 0; i < RESULT_INFO_MAX_SLOT; i++ )
        {
            RnCompetitionResultInfoSlot* pInfoSlot = new RnCompetitionResultInfoSlot(pGaeaClient, m_pEngineDevice);
            pInfoSlot->CreateSub(this, "RN_COMPETITION_RESULT_INFO_SLOT");
            pInfoSlot->CreateSubControl(_sResultTab.sTabInfo.sTitle, i);
            RegisterControl(pInfoSlot);
            vecInfoSlot.push_back(pInfoSlot);
        }

//         CUIControl* pStaticBackgroundHead = new CUIControl(m_pEngineDevice);
//         pStaticBackgroundHead->CreateSub ( this, "RN_COMPETITION_RESULT_MY_INFO_LINE", UI_FLAG_RIGHT | UI_FLAG_YSIZE );
//         pStaticBackgroundHead->SetDiffuse( NS_UITEXTCOLOR::DARKGRAY );
//         pStaticBackgroundHead->SetUseRender( TRUE );
//         RegisterControl ( pStaticBackgroundHead );
    }

    {
        // 제목 크기에 따라 윈도우테두리 크기를 조정;
        UIRECT _childRect = pLineWindowBox->GetLocalPos();
        _childRect.sizeX = (float)_titleBoxLeft;
        pLineWindowBox->SetLocalPos(_childRect);

        // 윈도우테두리 크기에 따라 스크롤바 위치 조정;
        UIRECT _childRect2 = pScrollBar->GetLocalPos();
        _childRect2.left += (float)_titleBoxLeft;
        pScrollBar->SetLocalPos(_childRect2);

        UIRECT _childRect3 = pScrollBarBackground->GetLocalPos();
        _childRect3.left += (float)_titleBoxLeft;
        pScrollBarBackground->SetLocalPos(_childRect3);

        // 테두리와 스크롤바크기에 따라 윈도우 총 크기 조정;
        UIRECT _rect = GetLocalPos();
        _rect.sizeX = _childRect2.left + _childRect2.sizeX;
        SetLocalPos(_rect);
    }
}

void RnCompetitionResultTabWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

    {
        CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame();
        const int nTotal       = pTabData->vecContent.size();
        const int nVisibleLine = RESULT_INFO_MAX_SLOT;
        const int nMovableLine = nTotal - nVisibleLine;
        unsigned int currentScroll(0);
        if ( nVisibleLine < nTotal )
        {
            float fPercent = pThumbFrame->GetPercent();
            currentScroll = (unsigned int)floor(fPercent * nMovableLine);
        }

        if ( currentScroll != currentScrollPos )
        {
            currentScrollPos = currentScroll;
            UpdateInfoSlot(currentScrollPos);
        }
    }
}

const bool RnCompetitionResultTabWindow::CompareInfo(const unsigned int _Info1, const unsigned int _Info2)
{
    const unsigned int nTitleNum = pTabData->sTabInfo.wSortTitleNum;

    bool bResult(false);

    switch ( pTabData->sTabInfo.sTitle.vecTitleObj[nTitleNum].emItemType )
    {
    case RnCompetitionResultTitleObj::EM_CONTENT_FLOAT:
        {
            const float _fInfo1Data(pTabData->vecContent[_Info1].vecResultObj[nTitleNum].ContentItemFloat);
            const float _fInfo2Data(pTabData->vecContent[_Info2].vecResultObj[nTitleNum].ContentItemFloat);
            switch ( pTabData->sTabInfo.emSortType )
            {
            case RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC:
                bResult  = _fInfo1Data < _fInfo2Data;
                break;
            case RnCompetitionResultTabInfo::EM_RESULT_SOR_DESC:
                bResult  = _fInfo1Data > _fInfo2Data;
                break;
            }
        }
        break;
    case RnCompetitionResultTitleObj::EM_CONTENT_INT:
    case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLASS:
    case RnCompetitionResultTitleObj::EM_CONTENT_ICON_SCHOOL:
    //case RnCompetitionResultTitleObj::EM_CONTENT_ICON_CLUBMARK:
    case RnCompetitionResultTitleObj::EM_CONTENT_ICON_WINLOSE:
        {
            const int _fInfo1Data(pTabData->vecContent[_Info1].vecResultObj[nTitleNum].ContentItemInt);
            const int _fInfo2Data(pTabData->vecContent[_Info2].vecResultObj[nTitleNum].ContentItemInt);
            switch ( pTabData->sTabInfo.emSortType )
            {
            case RnCompetitionResultTabInfo::EM_RESULT_SOR_ASC:
                bResult  = _fInfo1Data < _fInfo2Data;
                break;
            case RnCompetitionResultTabInfo::EM_RESULT_SOR_DESC:
                bResult  = _fInfo1Data > _fInfo2Data;
                break;
            }
        }
        break;
    }

    return bResult;
}

void RnCompetitionResultTabWindow::SortInfo()
{
    if ( pTabData->sTabInfo.emSortType == RnCompetitionResultTabInfo::EM_RESULT_SOR_NON )
        return;

    const unsigned int _contentSize = pTabData->vecContent.size();
    sortList.clear();
    sortList.reserve(_contentSize);
    for ( unsigned int i =0; i < _contentSize; i++ )
    {
        sortList.push_back(i);
    }

    const unsigned int _stepSize = _contentSize - 1;
    for ( unsigned int step = 0; step < _stepSize; step++)
    {
        unsigned int minKey(step);
        for ( unsigned int selectStep = step + 1; selectStep < _contentSize; selectStep++ )
        {
            if ( !CompareInfo(sortList[minKey], sortList[selectStep]))
            {
                minKey = selectStep;
            }
        }
        unsigned int tempSwap = sortList[minKey];
        sortList[minKey] = sortList[step];
        sortList[step] = tempSwap;
    }
}

void RnCompetitionResultTabWindow::UpdateInfo()
{
    if ( pTabData == NULL )
        return;

    const unsigned int _contentSize = pTabData->vecContent.size();
    const unsigned int _nSlotSize = vecInfoSlot.size();

    if ( _contentSize == 0 )
        return;

    SortInfo();

    CBasicScrollThumbFrame* const pThumbFrame = pScrollBar->GetThumbFrame();
    const int nTotal       = _contentSize;
    const int nVisibleLine = RESULT_INFO_MAX_SLOT;
    if ( nVisibleLine < nTotal )
    {
        pScrollBar->GetThumbFrame()->SetState( nTotal, nVisibleLine );
    }
    else
    {
        pScrollBar->GetThumbFrame()->SetState( 1, 1 );
    }
    pScrollBar->GetThumbFrame()->SetPercent(0.0f);
    currentScrollPos = 0;

    UpdateInfoSlot(0);

    // 내 정보 업데이트;
    if ( pTabData->sTabInfo.emMyResultType != RnCompetitionResultTabInfo::EM_MY_RESULT_NONE )
    {
        myInfoSlot->SetVisibleSingle(FALSE);

        DWORD dwMyKeyID(0);
        switch( pTabData->sTabInfo.emMyResultType )
        {
        case RnCompetitionResultTabInfo::EM_MY_RESULT_CHAR_DBNUM:
            dwMyKeyID = pGaeaClient->GetCharacter()->CharDbNum();
            break;
        case RnCompetitionResultTabInfo::EM_MY_RESULT_CHAR_GAEAID:
            dwMyKeyID = pGaeaClient->GetCharacter()->GetGaeaID();
            break;
        case RnCompetitionResultTabInfo::EM_MY_RESULT_CLUB:
            dwMyKeyID = pGaeaClient->GetCharacter()->GetClubID();
            break;
        case RnCompetitionResultTabInfo::EM_MY_RESULT_PARTY:
            dwMyKeyID = pGaeaClient->GetCharacter()->GetPartyID().GetID();
            break;
		case RnCompetitionResultTabInfo::EM_MY_RESULT_COUNTRY:
			dwMyKeyID = pGaeaClient->GetCharacter()->GetCountryInfo().dwID;
			break;
        }

        for ( unsigned int i = 0; i < _contentSize; i++ )
        {   
            if ( pTabData->vecContent[i].dwKeyID == dwMyKeyID )
            {
                myInfoSlot->SetInfo(pTabData->vecContent[i]);
                myInfoSlot->SetVisibleSingle(TRUE);
                break;
            }
        }
    }
}

void RnCompetitionResultTabWindow::UpdateInfoSlot(const unsigned int _nCurrentScoroll)
{
    const unsigned int _contentSize = pTabData->vecContent.size();
    const unsigned int _nSlotSize = vecInfoSlot.size();

    // 인포 업데이트;
    // 정렬을 사용할 경우와 아닐경우 참조할 정보가 다름;
    if ( pTabData->sTabInfo.emSortType == RnCompetitionResultTabInfo::EM_RESULT_SOR_NON )
    {
        for ( unsigned int i = 0; i < _nSlotSize; i++ )
        {
            RnCompetitionResultInfoSlot* _infoSlot = vecInfoSlot[i];
            const unsigned int _currentSlot(_nCurrentScoroll + i);
            if ( _currentSlot < _contentSize)
            {
                _infoSlot->SetInfo( pTabData->vecContent[_currentSlot] );
                _infoSlot->SetVisibleSingle(TRUE);
            }
            else
            {
                _infoSlot->SetVisibleSingle(FALSE);
            }
        }
    }
    else
    {
        for ( unsigned int i = 0; i < _nSlotSize; i++ )
        {
            RnCompetitionResultInfoSlot* _infoSlot = vecInfoSlot[i];
            const unsigned int _currentSlot(_nCurrentScoroll + i);
            if ( _currentSlot < _contentSize)
            {
                // 정렬 리스트에서 정보를 참조;
                const unsigned int _contentNum = sortList[_currentSlot];
                _infoSlot->SetInfo( pTabData->vecContent[_contentNum] );
                _infoSlot->SetVisibleSingle(TRUE);
            }
            else
            {
                _infoSlot->SetVisibleSingle(FALSE);
            }
        }
    }
}

void RnCompetitionResultWindow::CreateSubControl()
{
    pDummyBody = new CUIControl(m_pEngineDevice);
    pDummyBody->CreateSub(this, "RN_COMPETITION_RESULT_WINDOW");
    pDummyBody->SetVisibleSingle(FALSE);
    RegisterControl(pDummyBody);

    //! 남은시간
    {
        CD3DFontPar* pFont_Shadow = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_SHADOW_FLAG );
        const int nAlignCenter = TEXT_ALIGN_CENTER_X | TEXT_ALIGN_CENTER_Y;	

        pText_Time = new CBasicTextBox(m_pEngineDevice);
        pText_Time->CreateSub ( this, "RN_COMPETITION_RESULT_TEXT_TIME", UI_FLAG_CENTER_X | UI_FLAG_XSIZE );
        pText_Time->SetFont ( pFont_Shadow );
        pText_Time->SetTextAlign ( nAlignCenter );	
        RegisterControl ( pText_Time );
    }

    //! 나가기 버튼
    {
        pCloseButton = new CBasicTextButton(m_pEngineDevice);
        pCloseButton->CreateSub(this, "BASIC_TEXT_BUTTON19", UI_FLAG_RIGHT , RNCOMPETITION_RESULT_CLOSE_BUTTON);
        pCloseButton->CreateBaseButton( "RN_COMPETITION_RESULT_CLOSE_BUTTON", CBasicTextButton::SIZE19, 
            CBasicButton::CLICK_FLIP, ID2GAMEWORD("CTF_RESULT_BUTTON", 4) );
        RegisterControl( pCloseButton );
    }

    // 나의 결과 정보
    {
        UIRECT _rect = GetGlobalPos();

        CUIControl* pStaticBackgroundHead = new CUIControl(m_pEngineDevice);
        pStaticBackgroundHead->CreateSub ( this, "RN_COMPETITION_MY_RESULT_LINE_HEAD", UI_FLAG_XSIZE );
        pStaticBackgroundHead->SetDiffuse( NS_UITEXTCOLOR::BLACK );
        pStaticBackgroundHead->SetUseRender( TRUE );
        RegisterControl ( pStaticBackgroundHead );

        pBackgroundHead = pStaticBackgroundHead;

        CUIControl* pStaticBackgroundTail = new CUIControl(m_pEngineDevice);
        pStaticBackgroundTail->CreateSub ( this, "RN_COMPETITION_MY_RESULT_LINE_TAIL", UI_FLAG_XSIZE );
        pStaticBackgroundTail->SetDiffuse( NS_UITEXTCOLOR::BLACK );
        pStaticBackgroundTail->SetUseRender( TRUE );
        RegisterControl ( pStaticBackgroundTail );

        pBackgroundTail = pStaticBackgroundTail;
    }
}

void RnCompetitionResultWindow::CreateResultUI()
{
    // 새로운 창이 전장 결과이 떴다는 뜻으로 기존에 있던 UI는 삭제한다;
    for ( unsigned int i(vecTabButton.size()); i--; )
    {
        this->DeleteControl(vecTabButton[i]->GetWndID(), 0);
    }
    for ( unsigned int i(vecResultTab.size()); i--; )
    {
        this->DeleteControl(vecResultTab[i]->GetWndID(), 0);
    }
    
    vecTabButton.clear();
    vecResultTab.clear();    

    const unsigned int nTabSize = sResultInfo.vecTab.size();
    for ( unsigned int i = 0; i < nTabSize; i++ )
    {
        RnCompetitionResultTabButton* _pButton = new RnCompetitionResultTabButton(m_pEngineDevice);
        _pButton->CreateSub(this, "RN_COMPETITION_RESULT_TAB_BOX", UI_FLAG_DEFAULT, RNCOMPETITION_RESULT_TAB_BUTTON_START + i);
        _pButton->CreateSubControl(i, sResultInfo.vecTab[i].sTabInfo.strTabName);
        RegisterControl(_pButton);
        vecTabButton.push_back(_pButton);

        RnCompetitionResultTabWindow* _pTab = new RnCompetitionResultTabWindow(pGaeaClient, m_pEngineDevice);
        _pTab->CreateSub(this, "RN_COMPETITION_RESULT_TAB_WINDOW", UI_FLAG_DEFAULT);
        _pTab->CreateSubControl(sResultInfo.vecTab[i]);
        _pTab->UpdateInfo();
        RegisterControl(_pTab);
        vecResultTab.push_back(_pTab);
        _pTab->SetVisibleSingle(FALSE);
    }
    nCurrentTabNum = -1;
    SelectResultTab(0);

    // 전장 보상 툴팁;
    {
        if ( pRewardTooltip )
            this->DeleteControl(pRewardTooltip->GetWndID(), 0);

        RnCompetitionResultRewardTooltip* _pRewardTooltip = new RnCompetitionResultRewardTooltip(m_pEngineDevice);
        _pRewardTooltip->CreateSub(this, "RN_COMPETITION_RESULT_REWARD_TOOLTIP", UI_FLAG_DEFAULT);
        _pRewardTooltip->CreateSubControl("RN_COMPETITION_RESULT_REWARD_TOOLTIP_BACK_LINE");
        RegisterControl(_pRewardTooltip);
        pRewardTooltip = _pRewardTooltip;
        _pRewardTooltip->SetVisibleSingle(FALSE);
    }

    LPDIRECT3DDEVICEQ _device = m_pEngineDevice->GetDevice();
    InitDeviceObjects(_device);
    RestoreDeviceObjects(_device);
}

const bool RnCompetitionResultWindow::msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage)
{
    GLMSG::NET_INSTANCE_CONTENTS_MSGPACK_MESSAGE HeaderMsg;
    if (CastNetMsg(_pMessage, HeaderMsg) == false)
    {
        return false;
    }
    switch ( HeaderMsg.emType )
    {
    case EMMSG_COMPETITION_RESULT_TAB_FC:
        {
            GLMSG::NET_INSTANCE_COMPETITION_RESULT_TAB_FC NetMsg;
            if (CastNetMsg(_pMessage, NetMsg) == false)
            {
                return false;
            }

            if ( IsVisible() )
                SetVisibleSingle(FALSE);

            RnCompetitionResultTab _sTab;
            _sTab.sTabInfo = NetMsg.sTabInfo;
            sResultInfo.vecTab.push_back(_sTab);
        }
        break;
    case EMMSG_COMPETITION_RESULT_INFO_FC:
        {
            GLMSG::NET_INSTANCE_COMPETITION_RESULT_INFO_FC NetMsg;

            if (CastNetMsg(_pMessage, NetMsg) == false)
            {
                return false;
            }

            const unsigned int _tabNum = NetMsg.nTabNumber;
            if ( _tabNum >= sResultInfo.vecTab.size() )
                return false;

            sResultInfo.vecTab[_tabNum].vecContent.push_back(NetMsg.sContents);
        }
        break;
    case EMMSG_COMPETITION_RESULT_END_INFO_FC:
        {
            GLMSG::NET_INSTANCE_COMPETITION_RESULT_END_INFO_FC NetMsg;
            if (CastNetMsg(_pMessage, NetMsg) == false)
            {
                return false;
            }

            sResultInfo.fOutTime = NetMsg.fOutTime;
            nCloseButtonParam = NetMsg.nCloseButtonParam;

            m_pInterface->UiShowGroupFocus( RNCOMPETITION_RESULT_WINDOW );
        }
        break;

    default:
        return false;
    }
    return true;
}

void RnCompetitionResultWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    if (pRewardTooltip)
    {
        pRewardTooltip->SetVisibleSingle(FALSE);
        UIRECT tooltipPos = pRewardTooltip->GetGlobalPos();
        tooltipPos.left = (float)x;
        tooltipPos.top = (float)y;
        pRewardTooltip->SetGlobalPos(tooltipPos);
    }

    sResultInfo.fOutTime -= fElapsedTime;
    if ( sResultInfo.fOutTime > 0.0f )
    {
        pText_Time->ClearText();
        pText_Time->AddText(ID2GAMEWORD("CTF_RESULT_TEXT", 0), NS_UITEXTCOLOR::WHITE );
        pText_Time->AddString(0, " ", NS_UITEXTCOLOR::WHITE );
        pText_Time->AddString(0, sc::string::format(ID2GAMEWORD("CTF_RESULT_TEXT", 1), ((int)sResultInfo.fOutTime / 60), ((int)sResultInfo.fOutTime % 60) ).c_str(), NS_UITEXTCOLOR::ORANGE );
        pText_Time->AddString(0, ">", NS_UITEXTCOLOR::WHITE);
    }

    CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

void RnCompetitionResultWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    CUIWindowEx::TranslateUIMessage(ControlID, dwMsg);
    if ( (RNCOMPETITION_RESULT_TAB_BUTTON_START <= ControlID) && ( ControlID < (RNCOMPETITION_RESULT_TAB_BUTTON_START + (int)vecResultTab.size()) ) )
    {
        // 탭 셀렉트 메시지;
        if( dwMsg & RNCOMPETITION_RESULT_TAB_BUTTON_MESSAGE )
        {
            SelectResultTab(ControlID - RNCOMPETITION_RESULT_TAB_BUTTON_START);
        }
    }
    else
    {
        switch ( ControlID )
        {
        case RNCOMPETITION_RESULT_CLOSE_BUTTON:
            {
                if ( CHECK_MOUSEIN_LBUPLIKE(dwMsg) )
                {
                    // 닫기 버튼에 커스텀 메시지를 지정해놓은 경우 필드로 메시지를 보낸다;
                    if ( nCloseButtonParam != 0 )
                    {
                        GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE netMsg(nCloseButtonParam);
                        netMsg.emDestType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;
                        netMsg.emSrcType = InstanceSystem::EM_MESSAGE_USER;
                        netMsg.dwSrcID = pGaeaClient->GetCharacter()->CharDbNum();
                        pGaeaClient->NETSENDTOFIELD(&netMsg);
                    }
                    SetVisibleSingle(FALSE);
                }
            }
            break;
        }
    }
}

void RnCompetitionResultWindow::SetVisibleSingle( BOOL bVisible )
{
    CUIWindowEx::SetVisibleSingle(bVisible);
    if ( bVisible == TRUE )
    {
        CreateResultUI();
    }
    else
    {
        sResultInfo.vecTab.clear();
    }
}

void RnCompetitionResultWindow::SelectResultTab( const unsigned int _nTabNum )
{
    if ( _nTabNum == nCurrentTabNum )
        return;

    const unsigned int nTabSize = vecResultTab.size();

    if ( _nTabNum >= nTabSize )
        return;

    vecResultTab[_nTabNum]->SetVisibleSingle(TRUE);
    vecTabButton[_nTabNum]->SetEnable(true);

    {
        UIRECT _childRect = vecResultTab[_nTabNum]->GetLocalPos();
        
        //UIRECT _HeadLineRect = pBackgroundHead->GetLocalPos();
        //_HeadLineRect.sizeX = _childRect.left + _childRect.sizeX;
        //pBackgroundHead->SetLocalPos(_HeadLineRect);

        //UIRECT _TailLineRect = pBackgroundTail->GetLocalPos();
        //_TailLineRect.sizeX = _childRect.left + _childRect.sizeX;
        //pBackgroundTail->SetLocalPos(_TailLineRect);

        UIRECT _dummyRect = pDummyBody->GetLocalPos();
        //_rect.sizeX += _childRect.sizeX;
        //SetLocalPos(_rect);
        
        UIRECT rcOldpos = this->GetGlobalPos();
        WORD wAlignFlag = GetAlignFlag ();
        SetAlignFlag ( UI_FLAG_XSIZE | UI_FLAG_YSIZE );
        {
            UIRECT rcNewPos(rcOldpos.left
                , rcOldpos.top
                , _childRect.left + _childRect.sizeX + _dummyRect.sizeX
                , rcOldpos.sizeY);
            AlignSubControl( rcOldpos, rcNewPos );
            SetGlobalPos( rcNewPos );
        }
        SetAlignFlag( wAlignFlag );

        InitDeviceObjects(m_pEngineDevice->GetDevice());
        RestoreDeviceObjects(m_pEngineDevice->GetDevice());
    }

    if ( nCurrentTabNum < nTabSize)
    {
        vecResultTab[nCurrentTabNum]->SetVisibleSingle(FALSE);
        vecTabButton[nCurrentTabNum]->SetEnable(false);
    }

    nCurrentTabNum = _nTabNum;
}

void RnCompetitionResultWindow::ShowRnCompetitionRewardTooltop(const int emTitleType, const RnCompetitionResultContentRewardObj* _pRewardOjb)
{
    pRewardTooltip->SetTitleInfo(emTitleType);
    pRewardTooltip->SetInfo(_pRewardOjb);
    pRewardTooltip->SetVisibleSingle(TRUE);
}

//----------------------------------------------------------------------------------------------------//

MyRnCompetitionResultWindow::MyRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: RnCompetitionResultWindow( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyRnCompetitionResultWindow::CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice )
{
    RnCompetitionResultWindow::Create( RNCOMPETITION_RESULT_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    RnCompetitionResultWindow::CreateBaseWindowBodyLightGray( "RN_COMPETITION_RESULT_WINDOW" );
    RnCompetitionResultWindow::CreateSubControl();
    RnCompetitionResultWindow::SetAlignFlag( UI_FLAG_CENTER_X );
    RnCompetitionResultWindow::m_pInterface->UiRegisterControl( this, true );
}

const bool MyRnCompetitionResultWindow::msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage)
{
    return RnCompetitionResultWindow::msgProc_RnCompetitionResult(_pMessage);
}

void MyRnCompetitionResultWindow::ShowRnCompetitionRewardTooltop(const int emTitleType, const RnCompetitionResultContentRewardObj* pReward)
{
    RnCompetitionResultWindow::ShowRnCompetitionRewardTooltop(emTitleType, pReward);
}