#include "StdAfx.h"
#include "GfxDisplay.h"

#include "../../enginelib/DxTools/DxViewPort.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../enginelib/Common/StlFunctions.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Stage/DxGlobalStage.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../../RanLogicClient/Widget/GLWidgetTexture.h"
#include "../../RanLogicClient/Widget/GLWidgetScript.h"
#include "../../RanLogicClient/Land/GLLandManClient.h"
#include "../../RanLogicClient/Tool/DxParamSet.h"
#include "../../RanLogicClient/GfxInterfaceBase.h"
#include "../../SigmaCore/Math/Math.h"

#include "../GfxInterface.h"

#define UpdatePositionGap 2.0f
#define PrivateMarkUIWidth 300 // ���Ŀ� UI Width ���� �޾ƿ���.

using namespace NSGFX_DISPLAY;

GfxDisplay::GfxDisplay()
:	m_bInit( false )
,	m_bOnceAddHpInit( false )
,	m_bForceUpdateNameDisp( false )
,	m_bForceUpdated( false )
,	m_bEnableItemNameClick( false )
{
}

GfxDisplay::GfxDisplay( const GfxDisplay& value )
{
}

GfxDisplay::~GfxDisplay()
{
	Reset();
}

GfxDisplay& GfxDisplay::GetInstance()
{
	static GfxDisplay Instance;
	return Instance;
}

void GfxDisplay::Init()
{
	Reset();

	m_bInit = true;
}

void GfxDisplay::Reset()
{
	std::for_each( m_mapName.begin(), m_mapName.end(), std_afunc::DeleteMapObject() );
	std::for_each( m_mapItemName.begin(), m_mapItemName.end(), std_afunc::DeleteMapObject() );
	std::for_each( m_mapHeadChat.begin(), m_mapHeadChat.end(), std_afunc::DeleteMapObject() );
	std::for_each( m_listDamage.begin(), m_listDamage.end(), std_afunc::DeleteObject() );
	std::for_each( m_mapPrivateMarket.begin(), m_mapPrivateMarket.end(), std_afunc::DeleteMapObject() );

	m_mapName.clear();
	m_mapItemName.clear();
	m_mapHeadChat.clear();
	m_listDamage.clear();
	m_mapPrivateMarket.clear();

	while( false == m_queDisableName.empty() )
	{
		SAFE_DELETE( m_queDisableName.front() );
		m_queDisableName.pop();
	}

	while( false == m_queDisableItemName.empty() )
	{
		SAFE_DELETE( m_queDisableItemName.front() );
		m_queDisableItemName.pop();
	}

	while( false == m_queDisableChat.empty() )
	{
		SAFE_DELETE( m_queDisableChat.front() );
		m_queDisableChat.pop();
	}

	while( false == m_queDisableDamage.empty() )
	{
		SAFE_DELETE( m_queDisableDamage.front() );
		m_queDisableDamage.pop();
	}

	while( false == m_queDisablePM.empty() )
	{
		SAFE_DELETE( m_queDisablePM.front() );
		m_queDisablePM.pop();
	}

	m_MyHp.valDisplay.SetUndefined();

	for( int i = 0; i < EMHP_SIZE; ++i )
	{
		m_CursorTargetHp[ i ].valDisplay.SetUndefined();
		m_SelectTargetHp[ i ].valDisplay.SetUndefined();
	}

	m_funcAddDisplay.SetUndefined();
	m_funcChangeLayer.SetUndefined();
	m_funcSetDamage.SetUndefined();
	m_funcSortItemLayer.SetUndefined();
	m_valUpdateClubIconRT.SetUndefined();

	m_bInit = false;
	m_bOnceAddHpInit = false;

	m_sHp = GLDWDATA();
}

void GfxDisplay::OnResetDevice()
{
	MAP_NAME_ITER iter = m_mapName.begin();
	for( ; iter != m_mapName.end(); ++iter )
	{
		SNameDisplay* pNameDisplay = iter->second;
		pNameDisplay->RemoveState(EMNDS_UPDATECLUBICON);
	}

	SetEnableItemClick( false );
}

void GfxDisplay::OnFrameMove( float fElspTime )
{
	if( false == m_bInit )
		return;

	if( false == m_bOnceAddHpInit )
	{
		m_bOnceAddHpInit = AddHp();
	}
	else
	{
		// �� ĳ���� HP ������Ʈ
	//	UpdateMyHP();

		// ���콺 �ö� Ÿ�� HP ������Ʈ
	//	UpdateCursorTargetHP();

		// ������ Ÿ�� HP ���÷��� ������Ʈ
	//	UpdateSelectTargetHP();
	}

	// ��� ���ϴ� �̸� ���÷��� ����
	//RemoveNotUsedName();

	// ����� ������Ʈ
	UpdateHeadChat( fElspTime );

	// ������ ���÷��� ������Ʈ
	//UpdateDamage( fElspTime );

	UpdatePrivateMarket();

	// ������ �̸� Ŭ�� ������̸� �̸� �Ȱ�ġ�� �������ش�.
	if( true == m_bEnableItemNameClick && true == m_funcSortItemLayer.IsClosure() )
		m_funcSortItemLayer.InvokeSelf();

	if( true == m_bForceUpdated )
	{
		m_bForceUpdateNameDisp = false;
		m_bForceUpdated = false;
	}
}

void GfxDisplay::Clear()
{
	GfxInterfaceBase::GetInstance()->ClearNameList();

	while( false == m_mapName.empty() )
	{
		MAP_NAME_ITER iter = m_mapName.begin();

		SNameDisplay* pDisplay = iter->second;

		SetVisibleDispInfo( pDisplay->valDisplay, false );
		pDisplay->sDispName = CROWREN();

		iter = m_mapName.erase( iter );
		m_queDisableName.push( pDisplay );
	}

	while( false == m_mapItemName.empty() )
	{
		MAP_NAME_ITER iter = m_mapItemName.begin();

		SNameDisplay* pDisplay = iter->second;
		SetVisibleDispInfo( pDisplay->valDisplay, false );

		iter = m_mapItemName.erase( iter );
		m_queDisableItemName.push( pDisplay );
	}

	while( false == m_mapHeadChat.empty() )
	{
		MAP_HEADCHAT_ITER iter = m_mapHeadChat.begin();

		SHeadChatDispaly* pDisplay = iter->second;
		SetVisibleDispInfo( pDisplay->valDisplay, false );

		iter = m_mapHeadChat.erase( iter );
		m_queDisableChat.push( pDisplay );
	}

	while( false == m_listDamage.empty() )
	{
		LIST_DMG_ITER iter = m_listDamage.begin();
		
		SDamageDisplay* pDisplay = (*iter);
		SetVisibleDispInfo( pDisplay->valDisplay, false );

		iter = m_listDamage.erase( iter );
		m_queDisableDamage.push( pDisplay );
	}

	while( false == m_mapPrivateMarket.empty() )
	{
		MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.begin();

		SPrivateMarket* pDisplay = iter->second;
		SetVisibleDispInfo( pDisplay->valDisplay, false );

		iter = m_mapPrivateMarket.erase( iter );
		m_queDisablePM.push( pDisplay );
	}
}

bool GfxDisplay::CheckNameOption( const CROWREN& sDispName, const char* szOwnerName )
{
	switch( sDispName.m_emCROW )
	{
	case CROW_PC :
		{ 
			if( sDispName.m_dwTYPE & TYPE_TARGET )
			{
				if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::CharEnemy )
					return true;
			}
			else
			{
				if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::CharAlly )
					return true;
			}
		}
		break;

	case CROW_PET :
		{
			GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

			// �� ���̴�.
			if( 0 == strcmp( pClient->GetCharacter()->GetName(), szOwnerName ) )
			{
				if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::CharAlly )
					return true;
			}
			else
			{
				// �����ڰ� ���� �������� üũ
				std::tr1::shared_ptr< GLCharClient > spChar = pClient->GetLandManClient()->FindChar( szOwnerName );
				if( NULL == spChar )
					return false;

				CROWREN tempCrowRen;
				tempCrowRen.SetData( pClient, spChar.get() );

				if( tempCrowRen.m_dwTYPE & TYPE_TARGET )
				{
					if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::CharEnemy )
						return true;
				}
				else
				{
					if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::CharAlly )
						return true;
				}
			}
		}
		break;

	case CROW_MOB :
		{
			if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::Mob )
				return true;
		}
		break;

	case CROW_NPC :
		{
			if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::NPC )
				return true;
		}
		break;

	case CROW_SUMMON :
		{
			if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::Summon )
				return true;
		}
		break;

	case CROW_ITEM :
	case CROW_MONEY :
		{
			// �̸� Ŭ�� ���������� ������ �����ش�.
			if( true == m_bEnableItemNameClick )
				return true;

			if( RANPARAM::dwNameDisplay & RANPARAM::ENameDisplay::Item )
				return true;
		}
		break;
	}

	return false;
}

void GfxDisplay::AddName( const CROWREN& sDispName, const char* szOwnerName )
{
	if( false == m_bInit /*|| sDispName.m_szNAME[ 0 ] == _T( '\0' )*/ )
		return;

	if( false == m_funcAddDisplay.IsClosure() ||
		false == m_funcChangeLayer.IsClosure() )
		return;

	if( CROW_ITEM == sDispName.m_emCROW || CROW_MONEY == sDispName.m_emCROW )
	{
		AddItemName( sDispName );
		return;
	}

	// ������ Ŭ�� Ȱ�� ���¸� ������� �ʴ´�.
	if( true == m_bEnableItemNameClick )
		return;

	bool bVisible = CheckNameOption( sDispName, szOwnerName );


	if( true == bVisible )
	{
		if( m_mapHeadChat.find( sDispName.m_szNAME ) != m_mapHeadChat.end() )
			return ;

		// ���� ���� ���÷��� �߰�, �̸��� ��¾��Ѵ�.
		if( sDispName.m_bPMARKET )
		{
			AddPrivateMarket( sDispName );
			return ;						// ���� ������ �������� �̸��� �߰��ϴ� �κ��� �ǳʶ���
		}
		else
			bVisible = true;
	}

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// �̸�/HP ������ ���߱� �ɼ� üũ
	if( CROW_MOB == sDispName.m_emCROW || CROW_NPC == sDispName.m_emCROW )
	{
		ClientActor* pActor = pClient->GetCopyActor( sDispName.m_emCROW, sDispName.m_dwID );

		const GLCrowClient* const pCrow = dynamic_cast< GLCrowClient* >( pActor );
		if( NULL != pCrow && true == pCrow->IsHideNaming() )
			bVisible = false;
	}

	GLCharacter* pChar = pClient->GetCharacter();

	// �������� ��?
	if( pChar->GetGaeaID() != pChar->GetSelectID().GaeaId &&
		sDispName.m_dwID == pChar->GetSelectID().GaeaId )
	{
		return;
	}

	// ���콺 �ö� ��?
	if( pChar->GetGaeaID() != pChar->GetCursorID().GaeaId &&
		sDispName.m_dwID == pChar->GetCursorID().GaeaId )
	{
		// ���� PC�� ���
		if( CROW_PC == sDispName.m_emCROW &&
			true == pChar->IsReActionableOver( pChar->GetCursorID(), EMIMPACT_SIDE_ENEMY, false ) )
			return;

		if( CROW_MOB == sDispName.m_emCROW )
			return;
	}

	// ĳ���Ϳ� �Ÿ� üũ
	if( true == bVisible )
	{
		D3DXVECTOR3 vLength = sDispName.m_vPOS - pChar->GetPositionActor();

		float fLength = D3DXVec3Length( &vLength );
		if( fLength / MAX_VIEWRANGE > NSWIDGET_SCRIPT::g_fDef_Display_ShowNameDistanceRatio )
			bVisible = false;
	}

	// ȭ�� ��ġ ���
	// ȭ�� ������ ����� �̸��� visible ���ش�.
	D3DXVECTOR3 vScreen( 0.0f, 0.0f, 0.0f );
	if( true == bVisible )
		bVisible = IsVisible( sDispName.m_vPOS, vScreen );

	PAIR_NAME key = std::make_pair( sDispName.m_emCROW, sDispName.m_dwID );
	MAP_NAME_ITER iter = m_mapName.find( key );

	if( iter != m_mapName.end() )
	{
		SNameDisplay* pDisplay = iter->second;

		// visible ���� ���� RemoveNotUsedName()���� ����� display ��ü�� ť�� �־� ��Ȱ���Ѵ�. 
		if( false == bVisible )
		{
			pDisplay->RemoveState(EMNDS_USED);
			pDisplay->RemoveState(EMNDS_UPDATECLUBICON);
			return;
		}

		pDisplay->AddState(EMNDS_USED);

		bool bUpdateHP = false;
		bool bDiffernt = pDisplay->sDispName.DIFFERENT( sDispName );
		// ������ �ٲ������ ���÷��� ��ü ������Ʈ
		if( false == pDisplay->IsState(EMNDS_UPDATECLUBICON) ||
			true == bDiffernt ||
			true == m_bForceUpdateNameDisp )
		{
			if( true == bDiffernt )
				pDisplay->RemoveState(EMNDS_UPDATECLUBICON);

			pDisplay->sDispName = sDispName;

			UpdateName( pDisplay, szOwnerName, vScreen );

			m_bForceUpdated = true;
			bUpdateHP = true;
		}
		// �Ȱ����� ��ġ�� ������Ʈ
		else
		{
			if( pDisplay->sDispName.m_fPERHP != sDispName.m_fPERHP )
			{
				pDisplay->sDispName.m_fPERHP = sDispName.m_fPERHP;
				bUpdateHP = true;
			}

			pDisplay->sDispName.m_dwCOUNT = sDispName.m_dwCOUNT;

			GFx::Value::DisplayInfo info;
			pDisplay->valDisplay.GetDisplayInfo( &info );

			if( UpdatePositionGap <= abs( pDisplay->vPos.x - vScreen.x ) ||
				UpdatePositionGap <= abs( pDisplay->vPos.y - vScreen.y ) )
			{
				// ��ġ ����
				pDisplay->vPos.x = floorf( vScreen.x );
				pDisplay->vPos.y = floorf( vScreen.y );
				info.SetPosition( pDisplay->vPos.x - pDisplay->fOffsetX, pDisplay->vPos.y );
			}

			// visible ����
			info.SetVisible( true );
			pDisplay->valDisplay.SetDisplayInfo( info );
		}

		if( true == bUpdateHP )
			UpdateNameHP( pDisplay );
	}
	else
	{
		if( true == RANPARAM::bDisplayLimit && NSWIDGET_SCRIPT::g_nDisplayLimitCount <= m_mapName.size() )
			return;

		if( false == bVisible )
			return;

		// ĵ���� ���̾�
		int nLayer = (int) Layer_Middle;
		if( CROW_ITEM == sDispName.m_emCROW || CROW_MONEY == sDispName.m_emCROW )
			nLayer = (int) Layer_Bottom;
		else if( sDispName.m_dwID == pChar->GetGaeaID() )
			nLayer = (int) Layer_Top;

		// ��Ȱ�� �Ұ� ������ ť���� ��
		SNameDisplay* pDisplay = NULL;
		if( false == m_queDisableName.empty() )
		{
			pDisplay = m_queDisableName.front();
			m_queDisableName.pop();

			// ���̾� ����
			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( nLayer );
			args[ 1 ] = pDisplay->valDisplay;
			m_funcChangeLayer.InvokeSelf( &result, args, 2 );
		}
		else
		{
			pDisplay = new SNameDisplay();

			// ĵ������ �� ����Ŭ�� ����
			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( nLayer );
			args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_NameMC.c_str() );

			if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
			{
				SAFE_DELETE( pDisplay );
				return;
			}
			else
			{
				pDisplay->valDisplay = result;
			}
		}

		// ���÷��� ���� ����
		pDisplay->sDispName = sDispName;
		pDisplay->AddState(EMNDS_USED);
		pDisplay->RemoveState(EMNDS_UPDATECLUBICON);

		UpdateName( pDisplay, szOwnerName, vScreen );
		UpdateNameHP( pDisplay );

		m_mapName.insert( std::make_pair( key, pDisplay ) );
	}
}

void GfxDisplay::AddItemName( const CROWREN& sDispName )
{
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();

	bool bVisible = CheckNameOption( sDispName );

	// �Ÿ� üũ
	if( true == bVisible )
	{
		D3DXVECTOR3 vLength = sDispName.m_vPOS - pChar->GetPositionActor();
		float fLength = D3DXVec3Length( &vLength );
		if( fLength / MAX_VIEWRANGE > NSWIDGET_SCRIPT::g_fDef_Display_ShowNameDistanceRatio )
			bVisible = false;
	}

	// ȭ��ȿ� �ִ��� üũ
	D3DXVECTOR3 vScreen( 0.0f, 0.0f, 0.0f );
	if( true == bVisible )
		bVisible = IsVisible( sDispName.m_vPOS, vScreen );

	SNameDisplay* pDisplay = NULL;

	PAIR_NAME key = std::make_pair( sDispName.m_emCROW, sDispName.m_dwID );
	MAP_NAME_ITER iter = m_mapItemName.find( key );
	if( iter != m_mapItemName.end() )
	{
		pDisplay = iter->second;

		// ���� ���� - RemoveNotUsedName
		if( false == bVisible )
		{
			pDisplay->RemoveState(EMNDS_USED);
			return;
		}

		pDisplay->AddState(EMNDS_USED);
	
		if( UpdatePositionGap <= abs( pDisplay->vPos.x - vScreen.x ) ||
			UpdatePositionGap <= abs( pDisplay->vPos.y - vScreen.y ) )
		{
			pDisplay->vPos = D3DXVECTOR2( floorf( vScreen.x ), floorf( vScreen.y ) );

			if( false == m_bEnableItemNameClick )
			{
				GFx::Value::DisplayInfo info;
				pDisplay->valDisplay.GetDisplayInfo( &info );

				info.SetX( pDisplay->vPos.x - pDisplay->fOffsetX );
				info.SetY( pDisplay->vPos.y );
				pDisplay->valDisplay.SetDisplayInfo( info );
			}
			else
			{
				pDisplay->valDisplay.SetMember( "origX", pDisplay->vPos.x - pDisplay->fOffsetX );
				pDisplay->valDisplay.SetMember( "origY", pDisplay->vPos.y );
			}
		}
	}
	else
	{
		if( false == bVisible )
			return;

		if( false == m_queDisableItemName.empty() )
		{
			pDisplay = m_queDisableItemName.front();
			m_queDisableItemName.pop();
		}
		else
		{
			pDisplay = new SNameDisplay();

			// ĵ������ �� ����Ŭ�� ����
			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( (int) Layer_Item );
			args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_ItemNameMC.c_str() );

			if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
			{
				SAFE_DELETE( pDisplay );
				return;
			}
			else
			{
				pDisplay->valDisplay = result;
			}
		}

		m_mapItemName.insert( std::make_pair( key, pDisplay ) );

		std::string strName = NSWIDGET_SCRIPT::ToHtmlColorText( 
			sc::string::ansi_to_utf8( std::string( sDispName.m_szNAME ) ), sDispName.m_dwCOLOR );

		GFx::Value args[ 3 ];
		args[ 0 ].SetString( strName.c_str() );
		args[ 1 ].SetUInt( (UInt32) sDispName.m_dwID );
		args[ 2 ].SetInt( (int) sDispName.m_emCROW );

		pDisplay->valDisplay.SetMember( "text", args[ 0 ] );
		pDisplay->valDisplay.SetMember( "id", args[ 1 ] );
		pDisplay->valDisplay.SetMember( "crow", args[ 2 ] );
		pDisplay->vPos = D3DXVECTOR2( floorf( vScreen.x ), floorf( vScreen.y ) );
		pDisplay->sDispName = sDispName;
		pDisplay->AddState(EMNDS_USED);

		GFx::Value valBG;
		if( true == pDisplay->valDisplay.GetMember( "bg", &valBG ) )
		{
			GFx::Value valBgWidth;
			valBG.GetMember( "width", &valBgWidth );

			pDisplay->fOffsetX = (float) valBgWidth.GetNumber() / 2.0f;
		}

		GFx::Value::DisplayInfo info;
		pDisplay->valDisplay.GetDisplayInfo( &info );
		info.SetX( pDisplay->vPos.x - pDisplay->fOffsetX );
		info.SetY( pDisplay->vPos.y );
		info.SetVisible( true );
		pDisplay->valDisplay.SetDisplayInfo( info );

		// ������ �̸� Ŭ�� ��뿩�� ����
		GFx::Value arg;
		arg.SetBoolean( m_bEnableItemNameClick );
		pDisplay->valDisplay.SetMember( "enabledClick", arg );
	}
}

void GfxDisplay::AddHeadChat( const std::string& strName, DWORD dwNameColor, 
							  const std::string& strChat, DWORD dwColor, const D3DXVECTOR2& vPos )
{

	if( false == m_bInit )
		return;

	if( false == m_funcAddDisplay.IsClosure() )
		return;

	SHeadChatDispaly* pHeadChat = NULL;

	MAP_HEADCHAT_ITER iter = m_mapHeadChat.find( strName );
	if( iter != m_mapHeadChat.end() )
	{
		pHeadChat = iter->second;
	}
	else
	{
		// ��Ȱ�� �Ұ� ������ ť���� ��
		if( false == m_queDisableChat.empty() )
		{
			pHeadChat = m_queDisableChat.front();
			m_queDisableChat.pop();

			// ���̾� �ʱ�ȭ
			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt(  (int) Layer_Top );
			args[ 1 ] = pHeadChat->valDisplay;
			m_funcChangeLayer.InvokeSelf( &result, args, 2 );
		}
		else
		{
			pHeadChat = new SHeadChatDispaly();

			// ĵ������ �� ����Ŭ�� ����
			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( (int) Layer_Top );
			args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_HeadChatMC.c_str() );

			if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
			{
				SAFE_DELETE( pHeadChat );
				return;
			}
			else
			{
				pHeadChat->valDisplay = result;
			}
		}

		m_mapHeadChat.insert( std::make_pair( strName, pHeadChat ) );
	}

	// ������ �ð� ����
	pHeadChat->fLifeTime = NSWIDGET_SCRIPT::g_fDef_Display_HeadChatLifeTime;
	// ��ġ�� ����
	pHeadChat->vPos = vPos;

	// �ؽ�Ʈ ����
	GFx::Value valText;
	if( true == pHeadChat->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_HeadChat_tf.c_str(), &valText ) )
	{
		std::string strText = "";

		// �̸�
		// ä�� ����
		if( EMGFX_FONT_KOREAN == GfxInterface::GetInstance()->GetFontLanguage() )
		{
			strText.append( NSWIDGET_SCRIPT::ToHtmlColorText( 
				sc::string::ansi_to_utf8( std::string( strName ) ), dwNameColor ) );
			strText.append( "\n" );
			strText.append( NSWIDGET_SCRIPT::ToPrepareHTMLColorText( 
				sc::string::ansi_to_utf8( std::string( strChat ) ).c_str(), dwColor ) );
		}
		else
		{
			strText.append( NSWIDGET_SCRIPT::ToHtmlColorText( 
				sc::string::ansi_to_utf8( std::string( strName ) ), dwNameColor ) );
			strText.append( "\n" );
			strText.append( NSWIDGET_SCRIPT::ToPrepareHTMLColorText( strChat.c_str(), dwColor ) );
		}

		valText.SetTextHTML( strText.c_str() );

		// �ؽ�Ʈ �ʵ� ������ ����
		GFx::Value valTextHeight;
		valText.GetMember( "textHeight", &valTextHeight );

		float fTextHeight = ceilf( (float) valTextHeight.GetNumber() ) + 4.0f;
		pHeadChat->fOffsetY = fTextHeight + 5.0f;

		valTextHeight.SetNumber( fTextHeight );
		valText.SetMember( "height", valTextHeight );

		GFx::Value valBG;
		if( true == pHeadChat->valDisplay.GetMember(
			NSWIDGET_SCRIPT::g_strDef_Display_HeadChat_bg.c_str(), &valBG ) )
		{
			GFx::Value valBGHeight;
			valBGHeight.SetNumber( fTextHeight + 4.0f );

			valBG.SetMember( "height", valBGHeight );
		}
	}

	// ��ġ, visible ����
	GFx::Value::DisplayInfo info;
	pHeadChat->valDisplay.GetDisplayInfo( &info );

	info.SetPosition( vPos.x, vPos.y - pHeadChat->fOffsetY );
	info.SetVisible( true );
	pHeadChat->valDisplay.SetDisplayInfo( info );
}

void GfxDisplay::AddPrivateMarket( const CROWREN& sDispName )
{
	if( false == m_bInit )
		return;

	if( false == m_funcAddDisplay.IsClosure() )
		return;

	// ������ Ŭ�� Ȱ�� ���¸� ������� �ʴ´�.
	if( true == m_bEnableItemNameClick )
		return;

	D3DXVECTOR3 vScreen;

	bool bVisible = true;

	// ���ڿ� ����
	int nCharNameLen = 0;
	int nMaketNameLen = 0;

	// �� ���λ����� ��� ȭ�� ������� üũ��������.
	GLCharacter* pChar = GfxInterface::GetInstance()->GetGaeaClient()->GetCharacter();
	if( sDispName.m_dwID != pChar->GetGaeaID() )
		bVisible = IsVisible( sDispName.m_vPOS, vScreen );


	SPrivateMarket* pPrivateMarket = NULL;
	MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.find( sDispName.m_dwID );
	if( iter != m_mapPrivateMarket.end() )
	{
		if( bVisible )
		{
			pPrivateMarket = iter->second;
			SetVisibleDispInfo( pPrivateMarket->valDisplay, true );
		}
		else
		{
			DelPrivateMarket( sDispName );
			return;
		}

	}
	else
	{
		// ��Ȱ�� �Ұ� ������ ť���� ��
		if( m_queDisablePM.empty() == false )
		{
			pPrivateMarket = m_queDisablePM.front();
			m_queDisablePM.pop();
		}
		else
		{
			pPrivateMarket = new SPrivateMarket();

			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( (int) Layer_Top );
			args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket.c_str() );

			if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
			{
				SAFE_DELETE( pPrivateMarket );
				return;
			}
			else
			{
				pPrivateMarket->valDisplay = result;
			}
		}

		pPrivateMarket->strCharName = sDispName.m_szNAME;
		pPrivateMarket->dwId = sDispName.m_dwID;
		pPrivateMarket->fOffsetY = 0.0f;

		//D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &sDispName.m_vPOS, NULL );
		pPrivateMarket->vPos = D3DXVECTOR2( 0.0f, 0.0f );

		// ĳ���� dwId ����
		GFx::Value valID, valResult;
		valID.SetUInt( pPrivateMarket->dwId );
		pPrivateMarket->valDisplay.Invoke( "SetID", &valResult, &valID, 1 );

		m_mapPrivateMarket.insert( std::make_pair( sDispName.m_dwID, pPrivateMarket ) );
	}

	// �ؽ�Ʈ ����
	GFx::Value valButton;
	GFx::Value valTextHeightTotal;
	if( true == pPrivateMarket->valDisplay.GetMember( 
		NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_Button.c_str(), &valButton ) )
	{
		GFx::Value valText;
		pPrivateMarket->fOffsetY = 0.0f;

		if( true == valButton.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_TextPM.c_str(), &valText ) )
		{
			std::string strTextMarketPM = "";
			std::string strTextMarketName = "";
			//strText = sc::string::ansi_to_utf8( std::string( ID2GAMEWORD("PRIVATE_MARKET") ) );
			//valText.SetText( strText.c_str() );
			//valText.SetColorTransform("HTML_TEXT_COLOR_YELLOW");


			if(	pChar->IsMarketClick(sDispName.m_dwID) == false )
			{
				strTextMarketPM = ( NSWIDGET_SCRIPT::ToHtmlColorText( 
					(  std::string( ID2GAMEWORD("PRIVATE_MARKET") ) ), 
					NS_UITEXTCOLOR::GREEN ) );
			}
			else
			{
				strTextMarketPM =  ( NSWIDGET_SCRIPT::ToHtmlColorText( 
					(  std::string( ID2GAMEWORD("PRIVATE_MARKET") ) ), 
					NS_UITEXTCOLOR::AQUAMARINE ) );
			}

			strTextMarketName = ( std::string( sDispName.m_szNAME ) );

			if(	pChar->IsMarketClick(sDispName.m_dwID) == false )
			{
				strTextMarketName =( NSWIDGET_SCRIPT::ToHtmlColorText( 
					(  std::string( sDispName.m_szNAME ) ), 
					NS_UITEXTCOLOR::ORANGE ) );
			}
			else
			{
				strTextMarketName = ( NSWIDGET_SCRIPT::ToHtmlColorText( 
					(  std::string( sDispName.m_szNAME ) ), 
					NS_UITEXTCOLOR::AQUA ) );
			}

			nCharNameLen = strlen( sDispName.m_szNAME ) + strlen(  ID2GAMEWORD("PRIVATE_MARKET") ) + 2;

			std::string strText = stringformat( "%1% %2%" , strTextMarketPM , strTextMarketName  );
			valText.SetTextHTML( GfxInterfaceBase::GetInstance()->EncodeText( strText ).c_str() );

			GFx::Value valTextHeight;
			valText.GetMember( "textHeight", &valTextHeight ); 
						
			float fTextHeight = ceilf( (float) valTextHeight.GetNumber() ) + 4.0f;
			pPrivateMarket->fOffsetY += fTextHeight + 5.0f;
		}

		//if( true == valButton.GetMember( 
		//	NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_TextName.c_str(), &valText ) )
		//{		
		//	
		//	std::string strText = "";
		//	strText = sc::string::ansi_to_utf8( std::string( sDispName.m_szNAME ) );
		//
		//	if(	pChar->IsMarketClick(sDispName.m_dwID) == false )
		//	{
		//		strText =( NSWIDGET_SCRIPT::ToHtmlColorText( 
		//			sc::string::ansi_to_utf8(  std::string( sDispName.m_szNAME ) ), 
		//			NS_UITEXTCOLOR::ORANGE ) );
		//	}
		//	else
		//	{
		//		strText = ( NSWIDGET_SCRIPT::ToHtmlColorText( 
		//			sc::string::ansi_to_utf8(  std::string( sDispName.m_szNAME ) ), 
		//			NS_UITEXTCOLOR::AQUA ) );
		//	}


		//	valText.SetTextHTML(strText.c_str());

		//	GFx::Value valTextHeight, valTextWidth;
		//	valText.GetMember( "textHeight", &valTextHeight );

		//	float fTextHeight = ceilf( (float) valTextHeight.GetNumber() ) + 4.0f;
		//	pPrivateMarket->fOffsetY += fTextHeight + 5.0f;
		//}

		if( true == valButton.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_TextPMName.c_str(), &valText ) )
		{
			if ( GfxInterfaceBase::GetInstance()->GetFontLanguage() == EMGFX_FONT_KOREAN )
			{
				std::string strText = sc::string::ansi_to_utf8( std::string( sDispName.m_szPMARKET ) );
				valText.SetText( strText.c_str() );
				nMaketNameLen = strlen( sDispName.m_szPMARKET );
			}
			else
			{
				nMaketNameLen = strlen(sDispName.m_szPMARKET);
				std::wstring strText = sc::string::utf8ToUnicode( std::string( sDispName.m_szPMARKET )  );
				valText.SetText( strText.c_str() );
			}

			GFx::Value valTextHeight;
			valText.GetMember( "textHeight", &valTextHeight );

			float fTextHeight = ceilf( (float) valTextHeight.GetNumber() ) + 4.0f;
			pPrivateMarket->fOffsetY += fTextHeight + 5.0f;
		}


		// ���� �̸� ���� ũ�� ����
		int nLongStrLen = max( nCharNameLen , nMaketNameLen);

		GFx::Value valWidth, valResult;

		float fPercent = (float)nLongStrLen / (float)35;

		valWidth.SetUInt( ( PrivateMarkUIWidth * fPercent ) );
		pPrivateMarket->valDisplay.Invoke( "SetBGWidth", &valResult, &valWidth, 1 );
		pPrivateMarket->fOffsetX = ( PrivateMarkUIWidth - ( PrivateMarkUIWidth * fPercent )) * 0.5f ;
	}

}

void GfxDisplay::DelPrivateMarket( const CROWREN& sDispName )
{
	if( false == m_bInit )
		return;

	DelPrivateMarket( sDispName.m_dwID );
}

void GfxDisplay::DelPrivateMarket( DWORD dwID )
{
	MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.find( dwID );
	if( iter == m_mapPrivateMarket.end() )
		return;

	SPrivateMarket* pPrivateMarket = iter->second;
	SetVisibleDispInfo( pPrivateMarket->valDisplay, false );

	m_queDisablePM.push( pPrivateMarket );
	m_mapPrivateMarket.erase( dwID );
}

void GfxDisplay::ToggleOffPrivateMarket( DWORD dwID )
{
	if( false == m_bInit )
		return;

	MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.find( dwID );
	if( iter == m_mapPrivateMarket.end() )
		return;

	SPrivateMarket* pPrivateMarket = iter->second;

	GFx::Value valBtn;
	if( true == pPrivateMarket->valDisplay.GetMember( 
		NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_Button.c_str(), &valBtn ) )
	{
		valBtn.SetMember( "selected", false );
	}
}

void GfxDisplay::ToggleOffOtherPrivateMarket( DWORD dwID )
{
	if( false == m_bInit )
		return;

	MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.begin();
	for( ; iter != m_mapPrivateMarket.end(); ++iter )
	{
		if( iter->first == dwID )
			continue;

		SPrivateMarket* pPrivateMarket = iter->second;

		GFx::Value valBtn;
		if( true == pPrivateMarket->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_PrivateMarket_Button.c_str(), &valBtn ) )
		{
			valBtn.SetMember( "selected", false );
		}
	}
}

void GfxDisplay::ChangeClubIcon( DWORD dwClubID )
{
	MAP_NAME_ITER iter = m_mapName.begin();
	for( ; iter != m_mapName.end(); ++iter )
	{
		NSGFX_DISPLAY::SNameDisplay* pDisplay = iter->second;
		
		if( false == pDisplay->sDispName.IsCLUB() )
			continue;

		if( pDisplay->sDispName.m_dwCLUB == dwClubID )
		{
			// Ŭ�� ������ �����ؽ�ó ����
			GFx::Value valClubRT;
			if( true == pDisplay->valDisplay.GetMember( 
				NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIcon.c_str(), &valClubRT ) )
			{
				GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

				GFx::Value args[ 6 ], result;
				args[ 0 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIconMC.c_str() );
				args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIconLink.c_str() );
				args[ 2 ].SetInt( pClient->GetCharacter()->m_dwServerID );
				args[ 3 ].SetInt( pDisplay->sDispName.m_dwCLUB );
				args[ 4 ].SetInt( (int) NSGFX_DISPLAY::CLUB_MARK_SIZE_WIDTH );
				args[ 5 ].SetInt( (int) NSGFX_DISPLAY::CLUB_MARK_SIZE_HEIGHT );
				valClubRT.Invoke( "CreateClubIconTexture", &result, args, 6 );
			}
		}
	}
}

bool GfxDisplay::AddHp()
{
	if( false == m_bInit )
		return false;

	if( false == m_funcAddDisplay.IsClosure() )
		return false;

	GFx::Value args[ 2 ], result;

	//// �� ĳ���� HP �߰�
	//args[ 0 ].SetInt( (int) Layer_Top );
	//args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_HpGreenSimpleMC.c_str() );
	//if( true == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
	//{
	//	m_MyHp.valDisplay = result;
	//	SetVisibleDispInfo( m_MyHp.valDisplay, false );
	//}

	//// ������ Ÿ�� HP �߰�
	//for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
	//{
	//	std::string strMC;
	//	switch( i )
	//	{
	//	case EMHP_RED :
	//		strMC = NSWIDGET_SCRIPT::g_strDef_Display_HpRedMC;
	//		break;

	//	case EMHP_GREEN :
	//		strMC = NSWIDGET_SCRIPT::g_strDef_Display_HpGreenMC;
	//		break;

	//	case EMHP_BLUE :
	//		strMC = NSWIDGET_SCRIPT::g_strDef_Display_HpBlueMC;
	//		break;
	//	}

	//	SHpDisplay& HpDisplay_sel = m_SelectTargetHp[ i ];

	//	args[ 0 ].SetInt( (int) Layer_Top );
	//	args[ 1 ].SetString( strMC.c_str() );

	//	// ĵ������ �� ����Ŭ�� ����
	//	if( true == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
	//	{
	//		HpDisplay_sel.valDisplay = result;
	//		SetVisibleDispInfo( HpDisplay_sel.valDisplay, false );
	//	}

	//	SHpDisplay& HpDisplay_cur = m_CursorTargetHp[i];
	//	args[ 0 ].SetInt( (int) Layer_Top );
	//	args[ 1 ].SetString( strMC.c_str() );

	//	// ĵ������ �� ����Ŭ�� ����
	//	if( true == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
	//	{
	//		HpDisplay_cur.valDisplay = result;
	//		SetVisibleDispInfo( HpDisplay_cur.valDisplay, false );
	//	}
	//}

	return true;
}

void GfxDisplay::AddDamage( const D3DXVECTOR3& vPos, int nDamage, DWORD dwDamageFlag, bool bAttack )
{
	if( false == m_bInit )
		return;

	if( false == m_funcAddDisplay.IsClosure() ||
		false == m_funcSetDamage.IsClosure() )
		return;

	// ������ Ŭ�� Ȱ�� ���¸� ������� �ʴ´�.
	if( true == m_bEnableItemNameClick )
		return;

	// ī�޶� ������ �����ȿ� ������ ��ġ�� �ִ��� �˻�.
	const CLIPVOLUME& CV = DxViewPort::GetInstance().GetClipVolume();
	if( FALSE == COLLISION::IsCollisionVolume( CV, vPos, vPos ) )
		return;

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	GLLandManClient* pLandClient = pClient->GetActiveMap();
	if( NULL == pLandClient )
		return;

	DxLandMan* pLandMan = pLandClient->GetLandMan();
	if( NULL == pLandMan )
		return;

	//ī�޶��� �þ߷� ���� �ø�
	D3DXVECTOR3 vPoint1 = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 vPoint2 = vPos;
	D3DXVECTOR3 vCollision;
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame;
	pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );
	if( TRUE == bCollision )
		return;

	//������Ʈ �ø� �ɼ��� �����ִ� ���� ���
	if( true == pLandClient->IsCullByObjectMap() )
	{
		//ĳ������ �þ߷� ���� �ø�
		bCollision = FALSE;
		vPoint1 = pClient->GetCharacter()->GetPosition();
		pLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );
		if( TRUE == bCollision )
			return;
	}

	// ȭ�� ��ġ ���
	D3DXVECTOR3 vScreen;
	if( false == IsVisible( vPos, vScreen ) )
		return;

	// ��� ���ϴ� ���÷��� ������ �������� �ƴϸ� ���� �����
	SDamageDisplay* pDisp = NULL;
	if( true == RANPARAM::bDisplayLimit )
	{
		if( 0 < m_listDamage.size() &&
			NSWIDGET_SCRIPT::g_nDisplayLimitCount <= m_listDamage.size() )
		{
			pDisp = m_listDamage.front();
			m_listDamage.pop_front();
		}
		else
		{
			if( true == m_queDisableDamage.empty() )
			{
				pDisp = new SDamageDisplay();

				GFx::Value args[ 2 ], result;
				args[ 0 ].SetInt( (int) Layer_Top );
				args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_DmgMc.c_str() );

				if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
				{
					SAFE_DELETE( pDisp );
					return;
				}
				else
				{
					pDisp->valDisplay = result;
				}
			}
			else
			{
				pDisp = m_queDisableDamage.front();
				m_queDisableDamage.pop();
			}
		}
	}
	else
	{
		if( true == m_queDisableDamage.empty() )
		{
			pDisp = new SDamageDisplay();

			GFx::Value args[ 2 ], result;
			args[ 0 ].SetInt( (int) Layer_Top );
			args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_DmgMc.c_str() );

			if( false == m_funcAddDisplay.InvokeSelf( &result, args, 2 ) )
			{
				SAFE_DELETE( pDisp );
				return;
			}
			else
			{
				pDisp->valDisplay = result;
			}
		}
		else
		{
			pDisp = m_queDisableDamage.front();
			m_queDisableDamage.pop();
		}
	}

	if( NULL == pDisp )
		return;

	// ����Ʈ �߰�
	m_listDamage.push_back( pDisp );

	// ���� �ð� �ʱ�ȭ
	pDisp->fLifeTime = NSWIDGET_SCRIPT::g_fDef_Display_DmgLifeTime;

	// ��ġ/visible ����
	GFx::Value::DisplayInfo info;
	pDisp->valDisplay.GetDisplayInfo( &info );

	float fRandOffsetX = ( sc::Random::RANDOM_NUM() + 1.0f ) * NSWIDGET_SCRIPT::g_fDef_Display_DmgRandOffsetRange;
	float fRandOffsetY = sc::Random::RANDOM_NUM() * NSWIDGET_SCRIPT::g_fDef_Display_DmgRandOffsetRange;

	// �Ӹ� ��ġ���� ������
	fRandOffsetX += NSWIDGET_SCRIPT::g_fDef_Display_DmgHeadOffsetX;
	fRandOffsetY += NSWIDGET_SCRIPT::g_fDef_Display_DmgHeadOffsetY;

	info.SetX( vScreen.x + fRandOffsetX );
	info.SetY( vScreen.y + fRandOffsetY );
	pDisp->valDisplay.SetDisplayInfo( info );

	// ������ ��� �Լ� ȣ��
	GFx::Value args[ 5 ];
	args[ 0 ] = pDisp->valDisplay;
	args[ 1 ].SetInt( nDamage );
	args[ 2 ].SetBoolean( dwDamageFlag & DAMAGE_TYPE_CRITICAL );
	args[ 3 ].SetBoolean( dwDamageFlag & DAMAGE_TYPE_CRUSHING_BLOW );
	args[ 4 ].SetBoolean( bAttack );

	GFx::Value result;
	m_funcSetDamage.InvokeSelf( &result, args, 5 );
}

void GfxDisplay::SetVisibleDispInfo( GFx::Value& val, bool bVisible )
{
	GFx::Value::DisplayInfo info;
	val.GetDisplayInfo( &info );

	info.SetVisible( bVisible );
	val.SetDisplayInfo( info );
}

void GfxDisplay::AllNameVisibleOff()
{
	MAP_NAME_ITER iter = m_mapName.begin();
	for( ; iter != m_mapName.end(); ++iter )
	{
		SNameDisplay* pDisplay = iter->second;
		SetVisibleDispInfo( pDisplay->valDisplay, false );
	}
}

void GfxDisplay::RemoveNotUsedName()
{
	MAP_NAME_ITER iter = m_mapName.begin();
	while( iter != m_mapName.end() )
	{
		SNameDisplay* pDisplay = iter->second;
		if( pDisplay->IsState(EMNDS_USED) )
		{
			pDisplay->RemoveState(EMNDS_USED);
			++iter;
		}
		else
		{
			SetVisibleDispInfo( pDisplay->valDisplay, false );

			m_queDisableName.push( pDisplay );
			iter = m_mapName.erase( iter );
		}
	}

	iter = m_mapItemName.begin();
	while( iter != m_mapItemName.end() )
	{
		SNameDisplay* pDisplay = iter->second;
		if( pDisplay->IsState(EMNDS_USED) )
		{
			pDisplay->RemoveState(EMNDS_USED);
			++iter;
		}
		else
		{
			SetVisibleDispInfo( pDisplay->valDisplay, false );

			m_queDisableItemName.push( pDisplay );
			iter = m_mapItemName.erase( iter );
		}
	}
}

bool GfxDisplay::IsVisible( const D3DXVECTOR3& vPos, D3DXVECTOR3& vScreen )
{
	vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

	const D3DPRESENT_PARAMETERS& pp = GfxInterface::GetInstance()->GetPresentParams();
	WORD wResX = pp.BackBufferWidth - 30;
	WORD wResY = pp.BackBufferHeight - 30;

	if( vScreen.x < 40.0f || wResX < vScreen.x ||
		vScreen.y < 40.0f || wResY < vScreen.y )
		return false;

	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLCharacter* pChar = pClient->GetCharacter();

	D3DXVECTOR3 vLookAtPt = pChar->GetPosition() - DxViewPort::GetInstance().GetFromPt();
	vLookAtPt.y = 0.0f;

	D3DXVECTOR3 vTargetPos = vPos - DxViewPort::GetInstance().GetFromPt();
	vTargetPos.y = 0.0f;

	float fLookAtLength = D3DXVec3Length( &vLookAtPt );
	float fPosLength = D3DXVec3Length( &vTargetPos );

	float fCosTheta = D3DXVec3Dot( &vLookAtPt, &vTargetPos ) / ( fLookAtLength * fPosLength );
	if( fCosTheta < 0.0f )
		return false;

	return true;
}

bool GfxDisplay::IsAllowAttack( ClientActor* pActor, const STARGETID& sTargetID )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
	GLCharacter* pCharacter = pClient->GetCharacter();

	GLLandManClient* pLand = pClient->GetActiveMap();
	if( NULL == pLand )
		return false;

	bool bOldVersion = false;

	Faction::ManagerClient* const pFactionMan = pLand->GetFactionManager();
	if( NULL != pFactionMan )
	{
		const Faction::EMRELATION_FLAG_TYPE emHpVisible = pFactionMan->getHPVisible();
		if( Faction::EMRELATION_FLAG_ERROR == emHpVisible )
		{
			bOldVersion = true;
		}
		else
		{
			const Faction::EMRELATION_TYPE emRelation = pFactionMan->getRelationClient( pClient->GetCharacter(), pActor );
			switch( emRelation )
			{
			case Faction::EMRELATION_ENEMY :
				return true;

			case Faction::EMRELATION_NEUTRAL_ENEMY :
			case Faction::EMRELATION_NEUTRAL_ALLY :
			case Faction::EMRELATION_ALLY :
				return false;
			}
		}
	}
	else
	{
		bOldVersion = true;
	}

	if( true == bOldVersion )
	{
		bool bPKServer		= pClient->IsPKServer();
		bool bBRIGHTEVENT   = pClient->IsBRIGHTEVENT();
		bool bSCHOOL_FREEPK = pClient->IsSchoolFreePk();
		bool bCTFPlayer	    = pClient->IsCTFPlayer();
		bool bPKMap         = pLand->IsPKMap();
		bool bGuidBattleMap = ( pLand->m_bClubBattle || pLand->m_bClubDeathMatch );

		if( FALSE == pCharacter->ISCONFRONT_TAR( sTargetID ) &&
			FALSE == pCharacter->IS_PLAYHOSTILE( pActor->GetCharID() ) )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void GfxDisplay::UpdateName( SNameDisplay* pDisplay, const char* szOwnerName, const D3DXVECTOR3& vScreen )
{
	// ���� �ؽ�Ʈ
	std::wstring strClubText = L"";
	std::wstring strNameText = L"";

	// ��, ��ȯ�� ������ �̸� �÷�
	if( NULL != szOwnerName )
	{
		if( pDisplay->sDispName.m_szNAME[ 0 ] == _T( '\0' ) )
		{
			strNameText.append( GfxInterface::GetInstance()->EncodeText( szOwnerName ) );
		}
		else
		{
			if( CROW_PET == pDisplay->sDispName.m_emCROW )
			{
				strClubText.append( NSWIDGET_SCRIPT::ToHtmlColorText( 
					GfxInterface::GetInstance()->EncodeText( szOwnerName ), 
					NS_UITEXTCOLOR::SILVER ) );
			}
			else if( CROW_SUMMON == pDisplay->sDispName.m_emCROW && FALSE == pDisplay->sDispName.m_bSIMPLE )
			{
				strClubText.append( NSWIDGET_SCRIPT::ToHtmlColorText( 
					GfxInterface::GetInstance()->EncodeText( szOwnerName ), 
					pDisplay->sDispName.m_dwCOLOR ) );
			}
		}
	}

	// Ŭ�� �̸� �÷�
	if( true == pDisplay->sDispName.IsCLUB() )
	{
		if( pDisplay->sDispName.m_szNICK[ 0 ] != _T( '\0' ) )
		{
			/*DWORD dwColor = UINT_MAX;
			if( pDisplay->sDispName.m_dwTYPE & TYPE_TARGET )
				dwColor = pDisplay->sDispName.m_dwCOLOR;
			if( pDisplay->sDispName.m_dwTYPE & TYPE_CLUBBATTLE )
				dwColor = pDisplay->sDispName.m_dwCOLOR;

			strClubText.append( NSWIDGET_SCRIPT::ToHtmlColorText(
				sc::string::ansi_to_utf8( std::string( pDisplay->sDispName.m_szNICK ) ), dwColor ) );*/

			strClubText.append( NSWIDGET_SCRIPT::ToHtmlColorText(
				GfxInterface::GetInstance()->EncodeText( pDisplay->sDispName.m_szNICK ), 0xFFFFFFFF ) );
		}
	}
	else
	{
		pDisplay->AddState(EMNDS_UPDATECLUBICON);
	}

	// Ÿ��Ʋ �̸� �÷�
	if( pDisplay->sDispName.m_szTitle[ 0 ] != _T( '\0' ) )
	{
		std::string strTitle = "<";
		strTitle.append( pDisplay->sDispName.m_szTitle );
		strTitle.append( ">" );

		strNameText.append( 
			NSWIDGET_SCRIPT::ToPrepareHTMLColorText( 
				GfxInterface::GetInstance()->EncodeText( strTitle ).c_str(),
				NS_UITEXTCONTROL::GetActivityColor( pDisplay->sDispName.m_nTitleClass ) ) );
	}

	// Crow �̸� �÷�
	strNameText.append( NSWIDGET_SCRIPT::ToHtmlColorText( 
		GfxInterface::GetInstance()->EncodeText( pDisplay->sDispName.m_szNAME ), 
		pDisplay->sDispName.m_dwCOLOR ) );

	// �߾����� ��ġ�� ���߱� ���� ����
	float fClubWidth = 0.0f;
	float fNameWidth = 0.0f;

	// Ŭ�� �̸� ����
	GFx::Value valClubText;
	if( true == pDisplay->valDisplay.GetMember( 
		NSWIDGET_SCRIPT::g_strDef_Display_NameMC_tfClub.c_str(), &valClubText ) )
	{
		valClubText.SetTextHTML( strClubText.c_str() );

		GFx::Value valTextWidth;
		valClubText.GetMember( "textWidth", &valTextWidth );
		fClubWidth = ( (float) valTextWidth.GetNumber() * 0.5f );

		GFx::Value valClubBG;
		if( true == pDisplay->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcClubBG.c_str(), &valClubBG ) )
		{
			GFx::Value val;
			val.SetNumber( valTextWidth.GetNumber() + 4.0f + NSWIDGET_SCRIPT::g_nDef_Display_NameMC_bgOffsetX * 2.0f );
			valClubBG.SetMember( "width", val );

			SetVisibleDispInfo( valClubBG, !strClubText.empty() );
		}

		// Ŭ�� ������ �����ؽ�ó ����
		GFx::Value valClubRT;
		if( true == pDisplay->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIcon.c_str(), &valClubRT ) )
		{
			GFx::Value valIconWidth;
			valClubRT.GetMember( "width", &valIconWidth );
			fClubWidth += (float) valIconWidth.GetNumber();

			if( true == pDisplay->sDispName.IsCLUB() )
			{
				if( false == GfxInterface::GetInstance()->GetUpdatedClubRT() )
				{
					if( false == pDisplay->IsState(EMNDS_UPDATECLUBICON) )
					{
						GfxInterface::GetInstance()->SetUpdatedClubRT( true );

						GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
						GFx::Value args[ 7 ], result;
						args[ 0 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIconMC.c_str() );
						args[ 1 ].SetString( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_rtClubIconLink.c_str() );
						args[ 2 ].SetInt( pClient->GetCharacter()->m_dwServerID );
						args[ 3 ].SetInt( pDisplay->sDispName.m_dwCLUB );
						args[ 4 ].SetInt( (int) NSGFX_DISPLAY::CLUB_MARK_SIZE_WIDTH );
						args[ 5 ].SetInt( (int) NSGFX_DISPLAY::CLUB_MARK_SIZE_HEIGHT );
						args[ 6 ].SetInt( pDisplay->sDispName.m_dwCLUB_MARK_VER );
						valClubRT.Invoke( "CreateClubIconTexture", &result, args, 7 );

						SetVisibleDispInfo( valClubRT, true );

						m_valUpdateClubIconRT = valClubRT;
						pDisplay->AddState(EMNDS_UPDATECLUBICON);
					}
				}
				else
				{
					SetVisibleDispInfo( valClubRT, false );
				}
			}
			// Ŭ���� ������ ������ visible off
			else
			{
				if( false == GfxInterface::GetInstance()->GetUpdatedClubRT() )
					pDisplay->AddState(EMNDS_UPDATECLUBICON);

				SetVisibleDispInfo( valClubRT, false );
			}
		}
	}

	// �̸� ����
	GFx::Value valNameText;
	if( true == pDisplay->valDisplay.GetMember( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_tfName.c_str(), &valNameText ) )
	{
		valNameText.SetTextHTML( strNameText.c_str() );

		GFx::Value valTextX;
		valNameText.GetMember( "x", &valTextX );

		GFx::Value valTextWidth;
		valNameText.GetMember( "textWidth", &valTextWidth );
		fNameWidth = ( (float) valTextWidth.GetNumber() / 2.0f );
		
		GFx::Value valBgX;
		GFx::Value valNameBG;

		float fBgWidth = 0.0f;

		// ��� �ڽ� ������ ����
		if( true == pDisplay->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcNameBG.c_str(), &valNameBG ) )
		{
			valNameBG.GetMember( "x", &valBgX );

			fBgWidth = (float) valTextWidth.GetNumber() + 4.0f + NSWIDGET_SCRIPT::g_nDef_Display_NameMC_bgOffsetX * 2.0f;

			GFx::Value val;
			val.SetNumber( fBgWidth );
			valNameBG.SetMember( "width", val );
		}

		// HP �� ��ġ ����
		GFx::Value valHP;
		if( true == pDisplay->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_NameMC_barHP.c_str(), &valHP ) )
		{
			GFx::Value valHpWidth;
			valHP.GetMember( "width", &valHpWidth );

			float fGap = (float) valHpWidth.GetNumber() - (float) valTextWidth.GetNumber();
			float fNewX = (float) valTextX.GetNumber() - fGap / 2.0f;

			valHP.SetMember( "x", floorf( fNewX ) );
		}

		// ���籺�� ������ �ƴϸ� �п� �������� �����ش�.
		if( false == GfxInterface::GetInstance()->GetDisplayCountryMark() )
		{
			// �п� ������ ����
			GFx::Value valSchoolIcon;
			if( true == pDisplay->valDisplay.GetMember( 
				NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcSchool.c_str(), &valSchoolIcon ) )
			{
				SetVisibleDispInfo( valSchoolIcon, true );

				GFx::Value valIconWidth;
				valSchoolIcon.GetMember( "width", &valIconWidth );
				fNameWidth += (float) valIconWidth.GetNumber();

				// �ش� �п� ���������� �̵�
				if( NODATA != pDisplay->sDispName.m_wSCHOOL )
				{
					switch( pDisplay->sDispName.m_wSCHOOL )
					{
					case SCHOOL_SM :
						valSchoolIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_SMSchool.c_str() );
						break;

					case SCHOOL_HA :
						valSchoolIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_HASchool.c_str() );
						break;

					case SCHOOL_BH :
						valSchoolIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_BHSchool.c_str() );
						break;
					}
				}
				// ������ ����.
				else
				{
					valSchoolIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_NoSchool.c_str() );
				}
			}

			// ���� �̹��� visible ����
			GFx::Value valCountryImg;
			if( true == pDisplay->valDisplay.GetMember( 
				NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcCountry.c_str(), &valCountryImg ) )
			{
				SetVisibleDispInfo( valCountryImg, false );
			}
		}
		else
		{
			// ���� �̹��� ����
			GFx::Value valCountryImg;
			if( true == pDisplay->valDisplay.GetMember( 
				NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcCountry.c_str(), &valCountryImg ) )
			{				
				//  [3/21/2016 gbgim];
				// ������ ������Ʈ���� ���������� ����� �� �����Ƿ� ������Ʈ Ÿ������ �˻������ʴ´�;
				// ���� ������ ���þȵǾ��ִٸ� �Ʒ� ������ -1���̴�;
				// �Ʒ� ���� ��ũ��Ʈ�� ���� �ٲ� �� �����Ƿ� �׶����� ����������Ѵ�;
				if (pDisplay->sDispName.m_dwCountryID == 0)
				{
					SetVisibleDispInfo( valCountryImg, false );
				}
				else
				{
					SetVisibleDispInfo( valCountryImg, true );

					GFx::Value valIconWidth;
					valCountryImg.GetMember( "width", &valIconWidth );
					fNameWidth += (float) valIconWidth.GetNumber();

					const char* szFrameID = GLWidgetScript::GetInstance().LuaCallFunc< const char* >(
						NSWIDGET_SCRIPT::g_strFunc_Display_GetCountryFrameID,
						"-dw", pDisplay->sDispName.m_dwCountryID );
					if( NULL != szFrameID )
					{
						valCountryImg.GotoAndPlay( szFrameID );
					}
				}
			}

			// �п� ������ visible ����
			GFx::Value valSchoolIcon;
			if( true == pDisplay->valDisplay.GetMember( 
				NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcSchool.c_str(), &valSchoolIcon ) )
			{
				SetVisibleDispInfo( valSchoolIcon, false );
			}
		}

		// ��Ƽ ������ ����
		GFx::Value valPartyIcon;
		if( true == pDisplay->valDisplay.GetMember( 
			NSWIDGET_SCRIPT::g_strDef_Display_NameMC_mcParty.c_str(), &valPartyIcon ) )
		{
			// ��Ƽ �������� ��� �ٷ� ���� �ٵ��� ��ġ�� ����ش�.
			GFx::Value valNewX;
			valNewX.SetNumber( valTextX.GetNumber() + valTextWidth.GetNumber() + 4.0 );
			valPartyIcon.SetMember( "x", valNewX );

			// �ش� ��Ƽ ������ ���������� �̵�
			if( NODATA != pDisplay->sDispName.m_emPARTY )
			{
				switch( pDisplay->sDispName.m_emPARTY )
				{
				case PARTY_MASTER :
					valPartyIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_PartyMaster.c_str() );
					break;

				case PARTY_MASTER_TARGET :
					valPartyIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_PartyTarget.c_str() );
					break;

				case PARTY_MASTER_GENERAL :
					valPartyIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_PartyGeneral.c_str() );
					break;

				default :
					valPartyIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_NoParty.c_str() );
					break;
				}
			}
			// ������ ����.
			else
			{
				valPartyIcon.GotoAndPlay( NSWIDGET_SCRIPT::g_strDef_Display_NameMC_NoParty.c_str() );
			}
		}
	}

	// ��� ���� ������ ��� 
	float fOffsetX = 0.0f;
	if( fClubWidth < fNameWidth )
		fOffsetX = ceilf( fNameWidth );
	else
		fOffsetX = ceilf( fClubWidth );

	if( pDisplay->fOffsetX != fOffsetX ||
		UpdatePositionGap <= abs( pDisplay->vPos.x - vScreen.x ) ||
		UpdatePositionGap <= abs( pDisplay->vPos.y - vScreen.y ) )
	{
		pDisplay->fOffsetX = fOffsetX;

		GFx::Value::DisplayInfo info;
		pDisplay->valDisplay.GetDisplayInfo( &info );

		pDisplay->vPos.x = floorf( vScreen.x );
		pDisplay->vPos.y = floorf( vScreen.y );

		// ��ġ, visible ����
		info.SetPosition( pDisplay->vPos.x - pDisplay->fOffsetX, pDisplay->vPos.y );
		info.SetVisible( true );
		pDisplay->valDisplay.SetDisplayInfo( info );
	}
}

void GfxDisplay::UpdateNameHP( NSGFX_DISPLAY::SNameDisplay* pDisplay )
{
	if( NULL == pDisplay )
		return;

	// HP ��
	GFx::Value valHP;
	if( true == pDisplay->valDisplay.GetMember( 
		NSWIDGET_SCRIPT::g_strDef_Display_NameMC_barHP.c_str(), &valHP ) )
	{
		if( false == RANPARAM::bAUTO_CROW_HP )
		{
			SetVisibleDispInfo( valHP, false );
			return;
		}

		if( TRUE == pDisplay->sDispName.m_bHP )
		{
			GFx::Value val;
			val.SetNumber( pDisplay->sDispName.m_fPERHP * 100.0f );
			valHP.SetMember( "value", val );

			// ������ �Ʊ����� �Ǻ��� HP�� ����;
			{
				bool bAlly(false);
				if ( pDisplay->sDispName.m_emCROW == CROW_PC ||
					pDisplay->sDispName.m_emCROW == CROW_SUMMON )
				{
					GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
					DWORD dwCharID(pDisplay->sDispName.m_dwID);
					if ( pDisplay->sDispName.m_emCROW == CROW_SUMMON )
					{
						std::tr1::shared_ptr< GLSummonClient > spSummon = pClient->GetLandManClient()->GetSummon(dwCharID);
						if ( spSummon )
							dwCharID = spSummon->m_dwOwnerGaeaID;
					}

					if ( pClient->GetCharacter()->GetGaeaID() == dwCharID )
					{
						bAlly = true;
					}
					else
					{
						std::tr1::shared_ptr<GLCharClient> spChar = pClient->GetLandManClient()->GetChar(dwCharID);
						STARGETID sTarID(spChar->GetCrow(), spChar->GetGaeaID());
						if ( pClient->GetCharacter()->IsReActionable(sTarID, EMIMPACT_SIDE_ENEMY) == FALSE )
							bAlly = true;
					}
				}

				// ������ �ٲ��ٸ�;
				if ( (pDisplay->IsState(EMNDS_HPBAR_ALLY) == false && bAlly == true)
					||(pDisplay->IsState(EMNDS_HPBAR_ALLY) == true && bAlly == false) )
				{
					GFx::Value valHPBar_Bar, valVisible;
					valHP.GetMember("enemyBar", &valHPBar_Bar);
					SetVisibleDispInfo( valHPBar_Bar, !bAlly );
					bAlly ? pDisplay->AddState(EMNDS_HPBAR_ALLY):pDisplay->RemoveState(EMNDS_HPBAR_ALLY);
				}
			}

			/*
			�̰� ��޻��� �ڵ��ε� ���� Ŀ���� �ñⰡ �ƴ϶� �ּ�ó����
			if ( pDisplay->sDispName.m_emCROW == CROW_MOB )
			{
				GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
				ClientActor* pActor = pClient->GetCopyActor( CROW_MOB, pDisplay->sDispName.m_dwID );
				if ( pActor == NULL )
					return;

				const GLCrowClient* const pCrow = dynamic_cast<GLCrowClient*>(pActor);
				if ( pDisplay->IsState(EMNDS_EMERGENCYBLINK) == false 
					&& pCrow->IsActState(EM_ACT_EMERGENCY) == true )
				{
					GFx::Value val;
					valHP.GetMovie()->CreateObject(&val);
					val.SetMember("use", true);
					val.SetMember("inst", "fillTray");
					val.SetMember("FPS", (UInt32)GLCONST_CHAR::nEmergency_BlinkCount);
					valHP.SetMember("blink", val);
					pDisplay->AddState(EMNDS_EMERGENCYBLINK);
				}
				else if ( pDisplay->IsState(EMNDS_EMERGENCYBLINK) == true
					&& pCrow->IsActState(EM_ACT_EMERGENCY) == false )
				{
					GFx::Value val;
					valHP.GetMovie()->CreateObject(&val);
					val.SetMember("use", false);
					valHP.SetMember("blink", val);
					pDisplay->RemoveState(EMNDS_EMERGENCYBLINK);
				}
			}
		*/
		}

		SetVisibleDispInfo( valHP, TRUE == pDisplay->sDispName.m_bHP ? true : false );
	}
}

void GfxDisplay::UpdateHeadChat( float fElspTime )
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	// ���ê �ð� üũ
	MAP_HEADCHAT_ITER iter = m_mapHeadChat.begin();
	while( iter != m_mapHeadChat.end() )
	{
		SHeadChatDispaly* pHeadChat = iter->second;

		pHeadChat->fLifeTime -= fElspTime;
		// �ð� �� �������� ����
		if( 0.0f >= pHeadChat->fLifeTime )
		{
			SetVisibleDispInfo( pHeadChat->valDisplay, false );

			m_queDisableChat.push( pHeadChat );
			iter = m_mapHeadChat.erase( iter );
		}
		// �ð� ���� ���� ��ġ ����
		else
		{
			D3DXVECTOR3* pvHeadPos = pClient->FindCharHeadPos( iter->first );
			if( NULL != pvHeadPos )
			{
				D3DXVECTOR3 vPos = *pvHeadPos;
				D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

				// ĳ���Ͱ� ������ ���� �� ������ ������ �ִ�.
				// �� ��ȭ�� Ŭ ���� �̵� ��Ų��.
				if( UpdatePositionGap <= abs( pHeadChat->vPos.x - vScreen.x ) ||
					UpdatePositionGap <= abs( pHeadChat->vPos.y - vScreen.y ) )
				{
					pHeadChat->vPos = D3DXVECTOR2( floorf( vScreen.x ), floorf( vScreen.y ) );

					GFx::Value::DisplayInfo info;
					pHeadChat->valDisplay.GetDisplayInfo( &info );

					info.SetPosition( pHeadChat->vPos.x, pHeadChat->vPos.y - pHeadChat->fOffsetY );
					pHeadChat->valDisplay.SetDisplayInfo( info );
				}
			}

			++iter;
		}
	}
}

void GfxDisplay::UpdatePrivateMarket()
{
	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();

	if( DxGlobalStage::EM_STAGE_GAME != pClient->GetGlobalStage()->GetStageType() )
	{
		MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.begin();
		while( iter != m_mapPrivateMarket.end() )
		{
			SPrivateMarket* pPrivateMarket = iter->second;

			SetVisibleDispInfo( pPrivateMarket->valDisplay, false );
			m_queDisablePM.push( pPrivateMarket );

			iter = m_mapPrivateMarket.erase( iter );
		}
	}
	else
	{
		MAP_PRIVATEMARKET_ITER iter = m_mapPrivateMarket.begin();
		while( iter != m_mapPrivateMarket.end() )
		{
			SPrivateMarket* pPrivateMarket = iter->second;

			const D3DXVECTOR3* pvHeadPos = pClient->FindCharHeadPos( pPrivateMarket->strCharName );
			if( NULL != pvHeadPos )
			{
				D3DXVECTOR3 vPos = *pvHeadPos;
				D3DXVECTOR3 vScreen = DxViewPort::GetInstance().ComputeVec3Project( &vPos, NULL );

				if( UpdatePositionGap <= abs( pPrivateMarket->vPos.x - vScreen.x ) ||
					UpdatePositionGap <= abs( pPrivateMarket->vPos.y - vScreen.y ) )
				{
					pPrivateMarket->vPos = D3DXVECTOR2( floorf( vScreen.x ), floorf( vScreen.y ) );

					GFx::Value::DisplayInfo info;
					pPrivateMarket->valDisplay.GetDisplayInfo( &info );

					info.SetPosition( pPrivateMarket->vPos.x + pPrivateMarket->fOffsetX , pPrivateMarket->vPos.y - pPrivateMarket->fOffsetY );
					pPrivateMarket->valDisplay.SetDisplayInfo( info );
				}

				++iter;
			}
			else
			{
				SetVisibleDispInfo( pPrivateMarket->valDisplay, false );
				m_queDisablePM.push( pPrivateMarket );

				iter = m_mapPrivateMarket.erase( iter );
			}
		}
	}
}


//void GfxDisplay::UpdateMyHP()
//{
//	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
//
//	if( DxGlobalStage::EM_STAGE_GAME != pClient->GetGlobalStage()->GetStageType() )
//	{
//		SetVisibleDispInfo( m_MyHp.valDisplay, false );
//		return;
//	}
//
//	GLCharacter* pChar = pClient->GetCharacter();
//	if( NULL == pChar )
//		return;
//
//	if( true == DXPARAMSET::GetInstance().m_bSHOW_SIMPLEHP )
//	{
//		D3DXVECTOR3 vScreen;
//		IsVisible( pChar->GetPosition(), vScreen );
//
//		// ��ġ ����, visible on
//		GFx::Value::DisplayInfo info;
//		m_MyHp.valDisplay.GetDisplayInfo( &info );
//
//		info.SetPosition( vScreen.x, vScreen.y );
//		info.SetVisible( true );
//		m_MyHp.valDisplay.SetDisplayInfo( info );
//
//		if( m_sHp != pChar->GetHp() )
//		{
//			// HP ����
//			GFx::Value valBar;
//			if( true == m_MyHp.valDisplay.GetMember( 
//				NSWIDGET_SCRIPT::g_strDef_Display_Hp_bar.c_str(), &valBar ) )
//			{
//				double dHpRatio = (double) pChar->GetHp().dwNow / (double) pChar->GetHp().dwMax * 100.0;
//
//				GFx::Value val;
//				val.SetNumber( dHpRatio );
//				valBar.SetMember( "value", val );
//			}
//
//// 			m_sHp = pChar->GetHp();
//// 			if ( m_MyHp.IsState(EMNDS_EMERGENCYBLINK) == false 
//// 				&& pChar->IsActState(EM_ACT_EMERGENCY) == true )
//// 			{
//// 				GFx::Value val;
//// 				valBar.GetMovie()->CreateObject(&val);
//// 				val.SetMember("use", true);
//// 				val.SetMember("inst", "fillTray");
//// 				val.SetMember("FPS", (UInt32)GLCONST_CHAR::nEmergency_BlinkCount);
//// 				valBar.SetMember("blink", val);
//// 				m_MyHp.AddState(EMNDS_EMERGENCYBLINK);
//// 			}
//// 			else if ( m_MyHp.IsState(EMNDS_EMERGENCYBLINK) == true
//// 				&& pChar->IsActState(EM_ACT_EMERGENCY) == false )
//// 			{
//// 				GFx::Value val;
//// 				valBar.GetMovie()->CreateObject(&val);
//// 				val.SetMember("use", false);
//// 				valBar.SetMember("blink", val);
//// 				m_MyHp.RemoveState(EMNDS_EMERGENCYBLINK);
//// 			}
//		}
//	}
//	else
//	{
//		SetVisibleDispInfo( m_MyHp.valDisplay, false );
//	}
//}

//void GfxDisplay::UpdateCursorTargetHP()
//{
//	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
//
//	if( false == GfxInterface::GetInstance()->GetDisplayOn() ||
//		DxGlobalStage::EM_STAGE_GAME != pClient->GetGlobalStage()->GetStageType() )
//	{
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_CursorTargetHp[i].valDisplay, false );
//
//		return;
//	}
//
//	GLCharacter* pCharacter = pClient->GetCharacter();
//	if( NULL == pCharacter )
//		return;
//
//	const STARGETID& sTargetID = pCharacter->GetCursorID();
//
//	if( true == sTargetID.ISNULL() )
//	{
//		if ( m_emDPCursorTarget == EMDPTARGET_CTF_AUTH )
//			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplayClose);
//
//		m_emDPCursorTarget = EMDPTARGET_NULL;
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_CursorTargetHp[i].valDisplay, false );
//
//		return;
//	}
//
//	ClientActor* pActor = pClient->GetCopyActor( sTargetID );
//
//	if( NULL != pActor )
//	{
//		// �̸�/HP ������ ���߱�?
//		if( CROW_MOB == sTargetID.emCrow || CROW_NPC == sTargetID.emCrow )
//		{
//			const GLCrowClient* pCrow = dynamic_cast< GLCrowClient* >( pActor );
//			if( NULL != pCrow && true == pCrow->IsHideNaming() )
//			{
//				for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//					SetVisibleDispInfo( m_CursorTargetHp[i].valDisplay, false );
//
//				return;
//			}
//		}
//		
//		// Ÿ���� ���� ��� ��ũ��Ʈ���� HP ������ �ʿ��ϴ� �����ش�.
//		if( CROW_MOB == sTargetID.emCrow )
//		{
//			GLWidgetScript::GetInstance().LuaCallFunc< void >(
//				NSWIDGET_SCRIPT::g_strFunc_CursorTargetInfo_SetHP,
//				"-dw, -dw",	
//				pActor->GetHp().dwNow,
//				pActor->GetHp().dwMax );
//		}
//	}
//
//	// ���콺 �ö� �� ������ visible ����
//	// ������ ���̶� ������ visible ����
//	if( NULL == pActor || sTargetID.GaeaId == pCharacter->GetSelectID().GaeaId )
//	{
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_CursorTargetHp[i].valDisplay, false );
//
//		return;
//	}
//
//	bool bVisible = false;
//
//	NSGFX_DISPLAY::EMTARGET_TYPE emDPCursor( EMDPTARGET_NORMAL );
//	BOOL bFeignDead = FALSE;
//	BOOL bHpZero    = FALSE;
//
//	EMSELECT_HPTYPE emType = (EMSELECT_HPTYPE)pClient->GetCrowIdentifyColorType(sTargetID);
//	switch( pActor->GetCrow() )
//	{
//	case CROW_PC :		// PC�� ���� ������ �� �����ֱ�
//		{
//			//bVisible = IsAllowAttack( pActor, sTargetID );
//
//			bVisible = pCharacter->IsReActionableOver( sTargetID, EMIMPACT_SIDE_ENEMY, false );
//
//			if( pActor->GetGaeaID() != pCharacter->GetGaeaID() )
//			{
//				GLCharClient* pCharTemp = dynamic_cast<GLCharClient*>(pActor);
//
//				bFeignDead = pCharTemp->IsFeignDie() ? FALSE : TRUE;
//				if( bFeignDead )
//					bHpZero = TRUE;
//			}
//		}		
//		break;
//
//	case CROW_MOB :
//		{
//			bVisible = true;
//
//			const GLCrowClient* const pCrow = dynamic_cast<GLCrowClient*>(pActor);
//			if ( pCrow != NULL && pCrow->m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR )
//			{
//				const GLDWDATA sHP = pCrow->GetHp();
//
//				const float fPercentSM = float(pCrow->m_arrHP4School[SCHOOL_SM]) / float(sHP.dwMax)
//					, fPercentHA = float(pCrow->m_arrHP4School[SCHOOL_HA]) / float(sHP.dwMax)
//					, fPercentBH = float(pCrow->m_arrHP4School[SCHOOL_BH]) / float(sHP.dwMax);
//
//				D3DXVECTOR3 vScreen;
//				IsVisible( pActor->GetPosBodyHeight(), vScreen );
//
//				GLWidgetScript::GetInstance().LuaCallFunc< void >(
//					NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplaySetPos, 
//					"-f, -f, -f, -f, -f",
//					fPercentSM, fPercentHA, fPercentBH, vScreen.x, vScreen.y);
//
//				emDPCursor = EMDPTARGET_CTF_AUTH;
//			}
//		}
//		break;
//	}
//
//	if ( emDPCursor != EMDPTARGET_CTF_AUTH && m_emDPCursorTarget == EMDPTARGET_CTF_AUTH )
//		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplayClose);
//
//	m_emDPCursorTarget = emDPCursor;
//	if( false == bVisible && !bHpZero )
//	{
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_CursorTargetHp[i].valDisplay, false );
//
//		return;
//	}
//
//	GFx::Value::DisplayInfo info;
//	const INT nType((INT)emType);
//	m_CursorTargetHp[nType].valDisplay.GetDisplayInfo( &info );
//
//	// �̸� ����
//	GFx::Value valName;
//	if( bVisible && true == m_CursorTargetHp[nType].valDisplay.GetMember( 
//		NSWIDGET_SCRIPT::g_strDef_Display_Hp_tf.c_str(), &valName ) )
//	{
//		valName.SetText( sc::string::ansi_to_utf8( std::string( pActor->GetName() ) ).c_str() );
//	}
//
//	// HP ���� ����
//	GFx::Value valBar;
//	if( true == m_CursorTargetHp[nType].valDisplay.GetMember( 
//		NSWIDGET_SCRIPT::g_strDef_Display_Hp_bar.c_str(), &valBar ) )
//	{
//		float fHpRatio = (float) pActor->GetHp().dwNow / (float) pActor->GetHp().dwMax * 100.0f;
//
//		if( bHpZero && bVisible )
//			fHpRatio = 0.0f;
//
//		GFx::Value val;
//		val.SetNumber( fHpRatio );
//		valBar.SetMember( "value", val );
//
//		if ( pActor->GetCrow() == CROW_MOB )
//		{
//// 			const GLCrowClient* const pCrow = dynamic_cast<GLCrowClient*>(pActor);
//// 			if ( m_CursorTargetHp[nType].IsState(EMNDS_EMERGENCYBLINK) == false 
//// 				&& pCrow->IsActState(EM_ACT_EMERGENCY) == true )
//// 			{
//// 				GFx::Value val;
//// 				valBar.GetMovie()->CreateObject(&val);
//// 				val.SetMember("use", true);
//// 				val.SetMember("inst", "fillTray");
//// 				val.SetMember("FPS", (UInt32)GLCONST_CHAR::nEmergency_BlinkCount);
//// 				valBar.SetMember("blink", val);
//// 				m_CursorTargetHp[nType].AddState(EMNDS_EMERGENCYBLINK);
//// 			}
//// 			else if ( m_CursorTargetHp[nType].IsState(EMNDS_EMERGENCYBLINK) == true
//// 				&& pCrow->IsActState(EM_ACT_EMERGENCY) == false )
//// 			{
//// 				GFx::Value val;
//// 				valBar.GetMovie()->CreateObject(&val);
//// 				val.SetMember("use", false);
//// 				valBar.SetMember("blink", val);
//// 				m_CursorTargetHp[nType].RemoveState(EMNDS_EMERGENCYBLINK);
//// 			}
//		}
//	}
//
//	D3DXVECTOR3 vScreen;
//	IsVisible( pActor->GetPosBodyHeight(), vScreen );
//
//	// ��ġ �� visible ����
//	info.SetPosition( vScreen.x, vScreen.y );
//	info.SetVisible( true );
//	m_CursorTargetHp[nType].valDisplay.SetDisplayInfo( info );
//}

//void GfxDisplay::UpdateSelectTargetHP()
//{
//	GLGaeaClient* pClient = GfxInterface::GetInstance()->GetGaeaClient();
//	GLLandManClient* pLandClient = pClient->GetActiveMap();
//
//	if( true == m_bEnableItemNameClick ||
//		false == GfxInterface::GetInstance()->GetDisplayOn() ||
//		DxGlobalStage::EM_STAGE_GAME != pClient->GetGlobalStage()->GetStageType() )
//	{
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_SelectTargetHp[ i ].valDisplay, false );
//
//		return;
//	}
//
//	GLCharacter* pCharacter = pClient->GetCharacter();
//	if( NULL == pCharacter )
//		return;
//
//	const STARGETID& sTargetID = pCharacter->GetSelectID();
//	if( true == sTargetID.ISNULL() )
//	{
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_SelectTargetHp[ i ].valDisplay, false );
//
//		if ( m_emDPSelectTarget == EMDPTARGET_CTF_AUTH )
//			GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplayClose);
//
//		m_emDPSelectTarget = EMDPTARGET_NULL;
//		return;
//	}
//
//	ClientActor* pActor = pClient->GetCopyActor( sTargetID );
//	if( NULL == pActor || 
//		pActor->GetGaeaID() == pCharacter->GetGaeaID() || 
//		true == pActor->IsHideNaming() )
//	{
//		// ������ �� ������ visible �� ����
//		for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//			SetVisibleDispInfo( m_SelectTargetHp[ i ].valDisplay, false );
//
//		return;
//	}
//
//	if ( pActor->GetCrow() != CROW_MOB )
//	{
////		GLWidgetScript::GetInstance().LuaCallFunc< void >(
////			NSWIDGET_SCRIPT::g_strFunc_CTFieldSetAuthCtrlHPDisplay, "-n, -n", -1, -1);
//	}
//	NSGFX_DISPLAY::EMTARGET_TYPE emDPSelect(EMDPTARGET_NORMAL);
//
//	BOOL bFeignDead = FALSE;
//	BOOL bHpZero    = FALSE;
//
//	// Crow Ÿ�Ժ��� HP ǥ�� ���� �ٸ���.
//	EMSELECT_HPTYPE emType = (EMSELECT_HPTYPE)pClient->GetCrowIdentifyColorType(sTargetID);
//	switch( pActor->GetCrow() )
//	{
//	case CROW_PC :
//		{			
//			emType = EMHP_GREEN;
//
//			// ���� ������ PC�� ������, �ƴϸ� �ʷϻ�
//			if( !pLandClient->IsPeaceZone() &&
//				TRUE == pCharacter->IsReActionableOver( sTargetID, EMIMPACT_SIDE_ENEMY, false ) )
//			{
//				emType = EMHP_RED;				
//			}
//
//			if( pActor->GetGaeaID() != pCharacter->GetGaeaID() )
//			{
//				GLCharClient* pCharTemp = dynamic_cast<GLCharClient*>(pActor);
//
//				bFeignDead = pCharTemp->IsFeignDie() ? FALSE : TRUE;
//
//				if( bFeignDead ) bHpZero = TRUE;
//			}
//		}
//		break;
//
//	case CROW_MOB :
//		{
//			const GLCrowClient* const pCrow = dynamic_cast<GLCrowClient*>(pActor);
//			if ( pCrow == NULL)
//				break;
//
//			// CTF ������;
//			if ( pCrow->m_pCrowData->m_emNPCType == NPC_TYPE_AUTHENTICATOR )
//			{
//				const GLDWDATA sHP = pCrow->GetHp();
//
//				float fPercentSM = float(pCrow->m_arrHP4School[SCHOOL_SM]) / float(sHP.dwMax);
//				float fPercentHA = float(pCrow->m_arrHP4School[SCHOOL_HA]) / float(sHP.dwMax);
//				float fPercentBH = float(pCrow->m_arrHP4School[SCHOOL_BH]) / float(sHP.dwMax);
//
//				D3DXVECTOR3 vScreen;
//				IsVisible( pActor->GetPosBodyHeight(), vScreen );
//
//				GLWidgetScript::GetInstance().LuaCallFunc< void >(
//					NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplaySetPos, "-f, -f, -f, -f, -f", fPercentSM, fPercentHA, fPercentBH, vScreen.x, vScreen.y);
//				emDPSelect = EMDPTARGET_CTF_AUTH;
//			}
//		}
//		break;
//	}
//
//	if ( emDPSelect != EMDPTARGET_CTF_AUTH && m_emDPSelectTarget == EMDPTARGET_CTF_AUTH )
//		GLWidgetScript::GetInstance().LuaCallFunc< void >(NSWIDGET_SCRIPT::g_strFunc_CTFieldHpDisplayClose);
//
//	m_emDPSelectTarget = emDPSelect;
//
//	for( int i = EMHP_RED; i < EMHP_SIZE; ++i )
//	{
//		GFx::Value::DisplayInfo info;
//		m_SelectTargetHp[ i ].valDisplay.GetDisplayInfo( &info );
//
//		if( i == emType )
//		{			
//			// �̸� ����
//			GFx::Value valName;
//			if( true == m_SelectTargetHp[ i ].valDisplay.GetMember( 
//				NSWIDGET_SCRIPT::g_strDef_Display_Hp_tf.c_str(), &valName ) )
//			{
//				valName.SetText( sc::string::ansi_to_utf8( std::string( pActor->GetName() ) ).c_str() );
//			}
//
//			// HP ���� ����
//			GFx::Value valBar;
//			if( true == m_SelectTargetHp[ i ].valDisplay.GetMember( 
//				NSWIDGET_SCRIPT::g_strDef_Display_Hp_bar.c_str(), &valBar ) )
//			{
//				double dHpRatio = (double) pActor->GetHp().dwNow / (double) pActor->GetHp().dwMax * 100.0;
//
//				if ( bHpZero && emType == EMHP_RED )
//				{
//					dHpRatio = 0.0;
//				}
//				else if ( emType == EMHP_GREEN )
//				{
//					if ( pLandClient->IsPeaceZone() )
//						dHpRatio = 100.0;
//				}
//
//				GFx::Value val;
//				val.SetNumber( dHpRatio );
//				valBar.SetMember( "value", val );
//
//				if ( pActor->GetCrow() == CROW_MOB )
//				{
//// 					const GLCrowClient* const pCrow = dynamic_cast<GLCrowClient*>(pActor);
//// 					if ( m_SelectTargetHp[ i ].IsState(EMNDS_EMERGENCYBLINK) == false 
//// 						&& pCrow->IsActState(EM_ACT_EMERGENCY) == true )
//// 					{
//// 						GFx::Value val;
//// 						valBar.GetMovie()->CreateObject(&val);
//// 						val.SetMember("use", true);
//// 						val.SetMember("inst", "fillTray");
//// 						val.SetMember("FPS", (UInt32)GLCONST_CHAR::nEmergency_BlinkCount);
//// 						valBar.SetMember("blink", val);
//// 						m_SelectTargetHp[ i ].AddState(EMNDS_EMERGENCYBLINK);
//// 					}
//// 					else if ( m_SelectTargetHp[ i ].IsState(EMNDS_EMERGENCYBLINK) == true
//// 						&& pCrow->IsActState(EM_ACT_EMERGENCY) == false )
//// 					{
//// 						GFx::Value val;
//// 						valBar.GetMovie()->CreateObject(&val);
//// 						val.SetMember("use", false);
//// 						valBar.SetMember("blink", val);
//// 						m_SelectTargetHp[ i ].RemoveState(EMNDS_EMERGENCYBLINK);
//// 					}
//				}
//			}
//
//			D3DXVECTOR3 vScreen;
//			bool bVisible = IsVisible( pActor->GetPosBodyHeight(), vScreen );
//
//			// ��ġ �� visible ����
//			info.SetPosition( vScreen.x, vScreen.y );
//			info.SetVisible( bVisible );
//		}
//		else
//		{
//			info.SetVisible( false );
//		}
//
//		m_SelectTargetHp[ i ].valDisplay.SetDisplayInfo( info );
//	}
//}

void GfxDisplay::UpdateDamage( float fElspTime )
{
	// �ð� �� �ȳ��� ����Ʈ���� �����ϰ� ť�� �ֱ�
	LIST_DMG_ITER iter = m_listDamage.begin();
	for( ; iter != m_listDamage.end(); )
	{
		SDamageDisplay* pDisp = (*iter);

		pDisp->fLifeTime -= fElspTime;
		if( 0.0f >= pDisp->fLifeTime )
		{
			SetVisibleDispInfo( pDisp->valDisplay, false );

			iter = m_listDamage.erase( iter );
			m_queDisableDamage.push( pDisp );
		}
		else
		{
			++iter;
		}
	}
}

bool GfxDisplay::GetItemNameDisplayName( DWORD dwID, EMCROW emCrow, CROWREN& dispName )
{
	PAIR_NAME key = std::make_pair( emCrow, dwID );
	MAP_NAME_ITER iter = m_mapItemName.find( key );
	if( iter == m_mapItemName.end() )
		return false;

	dispName = iter->second->sDispName;

	return true;
}

void GfxDisplay::SetEnableItemClick( bool bEnable )
{
	m_bEnableItemNameClick = bEnable;

	GFx::Value arg;
	arg.SetBoolean( m_bEnableItemNameClick );

	MAP_NAME_ITER iter = m_mapItemName.begin();
	for( ; iter != m_mapItemName.end(); ++iter )
	{
		SNameDisplay* pDisplay = iter->second;
		pDisplay->valDisplay.SetMember( "enabledClick", arg );
	}

	// ���� ��ġ�� �ǵ����� ���� ����� ��ġ �ʱ�ȭ ������
	if( false == m_bEnableItemNameClick )
	{
		MAP_NAME_ITER iter = m_mapItemName.begin();
		for( ; iter != m_mapItemName.end(); ++iter )
		{
			SNameDisplay* pDisplay = iter->second;
			pDisplay->vPos = D3DXVECTOR2( 0.0f, 0.0f );
		}
	}
}

bool GfxDisplay::CheckHeadChatDisplay( const char* pName )
{
	if( m_mapHeadChat.find( pName ) != m_mapHeadChat.end() )
		return true;

	return false;
}