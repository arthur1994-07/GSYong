#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../../../RanLogic/Item/GLItemMan.h"
#include "../../../RanLogic/GLogicDataMan.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"

#include "../Interface/SkillFunc.h"
#include "../Item/ItemImage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const DWORD	RECHARGECOLOR = D3DCOLOR_ARGB(150,255,0,0);

CItemImage::CItemImage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pItemImage ( NULL )
    , m_pItemImageOverlaped( NULL )
	, m_pOverImage(NULL)
	, m_pd3dDevice ( NULL )
	, m_sICONINDEX ( NATIVEID_NULL () )
	, m_TotElapsedTime(0.0f)
	, m_bItemUsed(false)
	, m_bItemEnd(false)
	, m_bTwinkle(true)
	, m_pItemProcess( NULL )
	, m_sNativeID(false)
    , m_sNativeID_Overlaped(false)
	, m_pNumberText( NULL )
{
}

CItemImage::~CItemImage ()
{
}

HRESULT CItemImage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

//	m_pItemImage->InitDeviceObjects(pd3dDevice);

	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

void	CItemImage::CreateSubControl (const std::string& overImageKey ,DWORD color, const char* szControlKeyword)
{
	m_pItemImage = new CUIControl(m_pEngineDevice);
	m_pItemImage->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, ITEM_IMAGE_SLOT );	
	m_pItemImage->SetUseRender ( TRUE );
	m_pItemImage->SetVisibleSingle ( FALSE );
	RegisterControl ( m_pItemImage );

    m_pItemImageOverlaped = new CUIControl(m_pEngineDevice);
    m_pItemImageOverlaped->CreateSub ( this, szControlKeyword, UI_FLAG_DEFAULT, ITEM_IMAGE_OVER );	
    m_pItemImageOverlaped->SetUseRender ( TRUE );
    m_pItemImageOverlaped->SetVisibleSingle ( FALSE );
    RegisterControl ( m_pItemImageOverlaped );

	if (!overImageKey.empty())
	{
		m_pOverImage = new CUIControl(m_pEngineDevice); 
		m_pOverImage->CreateSub( this, overImageKey.c_str() );	
		m_pOverImage->SetVisibleSingle(FALSE);
		m_pOverImage->SetDiffuse( color );
		RegisterControl(m_pOverImage);
	}


// 	m_pOverImage = new CUIControl(m_pEngineDevice);
// 	m_pOverImage->CreateSub(this,"CHARACTER_WINDOW_VEHICLE_MY_VEHICLE_LIST_SLOT_OVER");
// 	m_pOverImage->SetVisibleSingle(TRUE);
// 	RegisterControl( m_pOverImage );


	CBasicProgressBar* pItemProcess = new CBasicProgressBar(m_pEngineDevice);
	pItemProcess->CreateSub ( this, "ITEM_IMAGE_PROGRESS" );
	pItemProcess->CreateOverImage ( "ITEM_IMAGE_PROGRESS_OVER" );
	pItemProcess->SetType ( CBasicProgressBar::VERTICAL );
	pItemProcess->SetOverImageUseRender ( TRUE );
	pItemProcess->SetOverImageDiffuse ( 0 );
	pItemProcess->SetPercent( 0 );
	pItemProcess->SetControlNameEx ( "프로그래스바" );	
	pItemProcess->SetVisibleSingle( FALSE );
	RegisterControl ( pItemProcess );
	m_pItemProcess = pItemProcess;

	m_AnmateBeginPosition = GetGlobalPos();
	m_AnimateOriginalPosition = GetGlobalPos();

	CD3DFontPar* pFont8 = m_pEngineDevice->LoadFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW | D3DFONT_ASCII );

	CBasicTextBox* pTextBox = new CBasicTextBox(m_pEngineDevice);
	pTextBox->CreateSub ( this, "ITEM_NUMBER0" );
	pTextBox->SetFont ( pFont8 );
	pTextBox->SetTextAlign ( TEXT_ALIGN_RIGHT );
	RegisterControl ( pTextBox );

	m_pNumberText = pTextBox;

	if( m_pNumberText ) m_pNumberText->SetVisibleSingle(FALSE);
	
}

void CItemImage::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ITEM_IMAGE_SLOT:
	case ITEM_IMAGE_OVER:
		{
			if ( CHECK_MOUSE_IN(dwMsg) && CHECK_RB_UP_LIKE(dwMsg) )
			{
				if ( DxInputDevice::GetInstance().GetKeyState ( DIK_LALT ) & DXKEY_PRESSED )
				{
					const SITEM* pItemData = GLogicData::GetInstance().GetItem ( m_sNativeID );
					if ( pItemData == NULL )
						break;

					SSETITEM* pSetItem = const_cast<SSETITEM*>(GLItemSet::GetInstance().GetInfoSetItem(pItemData->BasicId()));
					if ( pSetItem )
					{
//						m_pGaeaClient->GetInterface()->SetitemGroupComposition(pSetItem);
//						m_pGaeaClient->GetInterface()->OpenSetitemPreviewWindow();
					}
				}
			}
		}
		break;
	}
}

void CItemImage::SetNumber ( int nNumber, int nMaxNumber )
{
	CString strNumber;

	if( nNumber > 1 )
	{		
		strNumber.Format ( "%d", nNumber );
		DWORD dwColor = NS_UITEXTCOLOR::GREENYELLOW;
		//if ( nNumber == nMaxNumber ) dwColor = NS_UITEXTCOLOR::ORANGE;
		m_pNumberText->SetOneLineText ( strNumber, dwColor );
		m_pNumberText->SetVisibleSingle ( TRUE );

		return;
	}	

	ResetNumber();
}

void CItemImage::ResetNumber()
{
	m_pNumberText->ClearText ();
	m_pNumberText->SetVisibleSingle ( FALSE );
}


void CItemImage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( !m_pItemImage->GetTexture() )
		m_pItemImage->InitDeviceObjects ( m_pd3dDevice );

    if( !m_pItemImageOverlaped->GetTexture() )
        m_pItemImageOverlaped->InitDeviceObjects ( m_pd3dDevice );


	if( m_bItemUsed && m_bItemEnd )
	{
		m_TotElapsedTime += fElapsedTime;

		if( m_TotElapsedTime < 0.3f )		m_pItemProcess->SetPercent(0);
		else if( m_TotElapsedTime < 0.6f )	m_pItemProcess->SetPercent(1);
		else if( m_TotElapsedTime < 0.9f )	m_pItemProcess->SetPercent(0);
		else if( m_TotElapsedTime < 1.2f )	m_pItemProcess->SetPercent(1);
		else
		{
			m_pItemProcess->SetPercent(0);
			m_TotElapsedTime = 0.0f;
			m_bItemEnd = false;
			m_bItemUsed = false;
		}
	}

	//	쿨타임 표시
    if ( !m_pItemProcess )
	{
		GASSERT ( 0 && "프로그래스바가 만들어지지 않았습니다." );
		return ;
	}

	const SITEM* pItem = GLogicData::GetInstance().GetItem( m_sNativeID );
	if (!pItem)
        return;

	if (!m_pGaeaClient->GetCharacter()->IsCoolTime(m_sNativeID))
	{
		m_bItemEnd = true;
		return;
	}	

	const ITEM_COOLTIME& pItemCoolTime = m_pGaeaClient->GetCharacter()->GetCoolTime( m_sNativeID );
	if (pItemCoolTime.IsZeroTime())
	{
		m_bItemEnd = true;
		return;
	}

	m_pItemProcess->SetVisibleSingle( TRUE );
	m_pItemProcess->SetOverImageDiffuse ( RECHARGECOLOR );

	m_bItemEnd = false;

	__time64_t tCurTime = m_pGaeaClient->GetCurrentTime().GetTime();
	__time64_t tUseTime = pItemCoolTime.tUseTime;
	__time64_t tCoolTime = pItemCoolTime.tCoolTime;

	float fCoolTime = static_cast<float>(tCoolTime - tUseTime);
	float fCurTime = fCoolTime - ( tCurTime - tUseTime );
	
	if ( fCoolTime == 0.0f )
	{
		fCoolTime = 1.0f;
	}

	const float fPercent = fCurTime / fCoolTime;
	m_pItemProcess->SetPercent ( fPercent );
	m_bItemUsed = true;
}

const bool	CItemImage::SetItem ( const SITEMCUSTOM& itemCustom )
{
    // 박스 포장품이라면 포장지이미지를 포장된아이템 이미지위에 덧씌우는 방식으로 표현;
    if ( itemCustom.IsBoxWrapped() )
    {
        const SITEM* pItemData = GLogicData::GetInstance().GetItem ( itemCustom.GetBoxWrappedItemID() );
        if ( pItemData == NULL )
            return false;

        const SITEM* pBoxWrapperItemData = GLogicData::GetInstance().GetItem ( itemCustom.GetRealNativeID() );
        if ( pBoxWrapperItemData == NULL )
            return false;

        return SetItem(pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID,
                pBoxWrapperItemData->sBasicOp.sICONID, pBoxWrapperItemData->GetInventoryFile(), pBoxWrapperItemData->sBasicOp.sNativeID);
    }
    return SetItem(itemCustom.GetNativeID());
}

const bool CItemImage::SetItem( SNATIVEID sNativeID )
{
	if( NATIVEID_NULL() == sNativeID )
	{
		ResetItem();
		return false;
	}

	const SITEM* pItemData = GLogicData::GetInstance().GetItem ( sNativeID );
    if ( pItemData == NULL )
        return false;

    return SetItem( pItemData->sBasicOp.sICONID, pItemData->GetInventoryFile(), pItemData->sBasicOp.sNativeID );
}

const bool CItemImage::SetItem ( SNATIVEID sICONINDEX, const char* szTexture, SNATIVEID sNativeID)
{	
	if ( m_pItemImage == NULL )
		return false;

	//	NOTE
	//		이전 프레임과 아이콘 ID가 같고,
	//		텍스쳐가 동일하다면 같은 이미지 이므로,
	//		더 이상 작업을 수행할 필요가 없다.

	if( m_sNativeID == sNativeID && sNativeID != NATIVEID_NULL() )
		return false;

	// 여기서 리턴하면 sNativeID가 적용안된다
// 	if ( (sICONINDEX == m_sICONINDEX) && (szTexture == m_pItemImage->GetTextureName ()) )
// 		return ;

	ResetItem ();
	SetItemProcess( sNativeID );

    m_pItemImage->SetTexturePos ( NS_SKILL::GetIconTexurePos ( sICONINDEX ) );
    m_pItemImage->SetTextureName ( szTexture );	

    //		if ( m_pd3dDevice )
    {	
        //			m_pItemImage->InitDeviceObjects ( m_pd3dDevice );
        m_pItemImage->SetVisibleSingle ( TRUE );

        m_sICONINDEX = sICONINDEX;
        m_strTextureName = szTexture;
    }

    if ( m_pd3dDevice )
        m_pItemImage->InitDeviceObjects ( m_pd3dDevice );

	return true;
}

const bool CItemImage::SetItem ( SNATIVEID sICONINDEX, const char* szTexture, SNATIVEID sNativeID, SNATIVEID sICONINDEX_Overlaped, const char* szTexture_Overlaped, SNATIVEID sNativeID_Overlaped )
{
    if ( m_pItemImage == NULL )
        return false;

    if ( m_pItemImageOverlaped == NULL )
        return false;

    //	NOTE
    //		이전 프레임과 아이콘 ID가 같고,
    //		텍스쳐가 동일하다면 같은 이미지 이므로,
    //		더 이상 작업을 수행할 필요가 없다.

    if( m_sNativeID == sNativeID && sNativeID != NATIVEID_NULL() )
        return false;

	// 여기서 리턴하면 포장된 아이템끼리 교환시 이미지가 변경되지 않는다;
	//if( m_sNativeID_Overlaped == sNativeID_Overlaped && sNativeID_Overlaped != NATIVEID_NULL() )
	//	return false;

    // 여기서 리턴하면 sNativeID가 적용안된다
    // 	if ( (sICONINDEX == m_sICONINDEX) && (szTexture == m_pItemImage->GetTextureName ()) )
    // 		return ;

    ResetItem ();
    SetItemProcess( sNativeID );
    
    // 내용물 이미지
    {
        m_pItemImage->SetTexturePos ( NS_SKILL::GetIconTexurePos ( sICONINDEX ) );
        m_pItemImage->SetTextureName ( szTexture );	

        //		if ( m_pd3dDevice )
        {	
            //			m_pItemImage->InitDeviceObjects ( m_pd3dDevice );
            m_pItemImage->SetVisibleSingle ( TRUE );

            m_sICONINDEX = sICONINDEX;
            m_strTextureName = szTexture;
        }

        if ( m_pd3dDevice )
            m_pItemImage->InitDeviceObjects ( m_pd3dDevice );
    }

    // 포장지 이미지 덧씌움;
    {
        m_sNativeID_Overlaped = sNativeID_Overlaped;

        m_pItemImageOverlaped->SetTexturePos ( NS_SKILL::GetIconTexurePos ( sICONINDEX_Overlaped ) );
        m_pItemImageOverlaped->SetTextureName ( szTexture_Overlaped );

        //		if ( m_pd3dDevice )
        {	
            //			m_pItemImage->InitDeviceObjects ( m_pd3dDevice );
            m_pItemImageOverlaped->SetVisibleSingle ( TRUE );
        }

        if ( m_pd3dDevice )
            m_pItemImageOverlaped->InitDeviceObjects ( m_pd3dDevice );
    }

    return true;
}

void CItemImage::SetAnimatePosition(const float fGapX, const float fGapY, const float fGapWidth, const float fGapHeight)
{
	m_AnimateOriginalPosition = GetGlobalPos();
	m_AnmateBeginPosition.left = m_AnimateOriginalPosition.left + fGapX;	
	m_AnmateBeginPosition.top = m_AnimateOriginalPosition.top + fGapY;	
	m_AnmateBeginPosition.sizeX = m_AnimateOriginalPosition.sizeX * fGapWidth;	
	m_AnmateBeginPosition.sizeY = m_AnimateOriginalPosition.sizeY * fGapHeight;
	m_AnmateBeginPosition.right = m_AnmateBeginPosition.left + m_AnmateBeginPosition.sizeX;
	m_AnmateBeginPosition.bottom = m_AnmateBeginPosition.top + m_AnmateBeginPosition.sizeY;	
}

void CItemImage::SetGlobalPos(const UIRECT& Pos)
{	
	const UIRECT& _interval = GetGlobalPos();
	m_AnimateOriginalPosition.left += (Pos.left - _interval.left);
	m_AnimateOriginalPosition.top += (Pos.top - _interval.top);
	m_AnmateBeginPosition.left += (Pos.left - _interval.left);
	m_AnmateBeginPosition.left += (Pos.top - _interval.top);
	CUIGroup::SetGlobalPos(Pos);
}
void CItemImage::SetGlobalPos(const D3DXVECTOR2& vPos)
{
	const UIRECT& _interval = GetGlobalPos();
	m_AnimateOriginalPosition.left += (vPos.x - _interval.left);
	m_AnimateOriginalPosition.top += (vPos.y - _interval.top);
	m_AnmateBeginPosition.left += (vPos.x - _interval.left);
	m_AnmateBeginPosition.left += (vPos.y - _interval.top);
	CUIGroup::SetGlobalPos(vPos);
}

void CItemImage::doAnimate(const float fT)
{
	const float fReT = fT > 1.0f ? 1.0f : fT;
	const float fUT = (1.0f - fReT);
	const UIRECT newPosition = m_AnimateOriginalPosition * fReT + m_AnmateBeginPosition * fUT;
	CUIGroup::SetGlobalPos(newPosition);

	UIRECT imagePosition = m_pItemImage->GetGlobalPos();
	imagePosition.sizeX = newPosition.sizeX;
	imagePosition.sizeY = newPosition.sizeY;
	imagePosition.right = imagePosition.left + imagePosition.sizeX;
	imagePosition.bottom = imagePosition.top + imagePosition.sizeY;
	m_pItemImage->SetGlobalPos(imagePosition);
    m_pItemImageOverlaped->SetGlobalPos(imagePosition);

	if ( m_pOverImage != NULL )
		m_pOverImage->SetGlobalPos(imagePosition);
}

SNATIVEID	 CItemImage::GetItemImageSlotPos ()
{
	return m_sICONINDEX;
}

void CItemImage::ResetItem ()
{
	if ( m_pItemImage )
	{
		if ( m_sICONINDEX != NATIVEID_NULL () )
		{
			m_pItemImage->DeleteDeviceObjects ();
			m_pItemImage->SetTextureName ( NULL );
			m_pItemImage->SetVisibleSingle ( FALSE );

			m_sICONINDEX = NATIVEID_NULL ();

			ReSetItemProcess();

			m_strTextureName.Empty ();
		}		
	}
    if ( m_pItemImageOverlaped )
    {
        m_pItemImageOverlaped->DeleteDeviceObjects ();
        m_pItemImageOverlaped->SetTextureName ( NULL );
        m_pItemImageOverlaped->SetVisibleSingle(FALSE);
    }

	m_sNativeID = NATIVEID_NULL();
    m_sNativeID_Overlaped = NATIVEID_NULL();

	ResetNumber();
}

void CItemImage::SetItemProcess( SNATIVEID sNativeID )
{
	m_bItemUsed = false;	
	m_sNativeID = sNativeID;

	if ( sNativeID == NATIVEID_NULL() )
	{
		m_pItemProcess->SetVisibleSingle( FALSE );
	}
	else
	{
		m_pItemProcess->SetVisibleSingle( TRUE );
	}

	return;
}

void CItemImage::ReSetItemProcess()
{
	m_bItemUsed = false;
	m_sNativeID = NATIVEID_NULL();
	m_pItemProcess->SetVisibleSingle( FALSE );
	m_pItemProcess->SetOverImageDiffuse ( 0 );
	m_pItemProcess->SetPercent( 0.0f );
	SetVisibleOverImage(false);
	return;
}

void CItemImage::SetDiffuse(D3DCOLOR _diffuse)
{
	CUIGroup::SetDiffuse( _diffuse );
	m_pItemProcess->SetOverImageDiffuse ( RECHARGECOLOR );
}

void CItemImage::SetVisibleOverImage( bool isVisible )
{
	if(m_pOverImage)
		m_pOverImage->SetVisibleSingle(isVisible);
}

bool CItemImage::IsVisibleOverImage()
{
	return m_pOverImage->IsVisible() ? true : false;
}

SNATIVEID CItemImage::GetItemID()
{
	return m_sNativeID;
}

const CString& CItemImage::GetItemTextureName() const
{
	return m_strTextureName;
}

// HRESULT CItemImage::Render( LPDIRECT3DDEVICEQ pd3dDevice )
// {
// 	if( m_pItemImage->GetTexture() == NULL )
// 		return S_OK;
// 
// 	return CUIGroup::Render(m_pd3dDevice);
// }
