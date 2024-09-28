#include "StdAfx.h"
#include "GfxLoadingScene.h"

#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/Util/EBTime.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogicClient/Etc/BaseStringParser.h"

GfxLoadingScene::GfxLoadingScene()
: m_sMapID( NATIVEID_NULL() )
, m_bShowGrade( false )
{
}

GfxLoadingScene::~GfxLoadingScene()
{
}

HRESULT GfxLoadingScene::OnInit( LPDIRECT3DDEVICE9 pDevice )
{
	m_strFilePath = NSGFXFILE::g_strGfxPath;
	m_strFilePath.append( NSGFXFILE::g_strLoadingFileName.c_str() );
	m_strFilePath = sc::string::ansi_to_utf8( m_strFilePath );

	return LoadMovie( pDevice );
}

void GfxLoadingScene::SetData( const SNATIVEID& sMapID, const std::string& strLoadingImgFile, bool bShowGrade )
{
	m_sMapID = sMapID;
	m_strLoadingImgFile = strLoadingImgFile;
	m_bShowGrade = bShowGrade;
}

void GfxLoadingScene::OnBegin( ELoadingSceneType eType )
{
	GfxRenderThread* pRenderThread = GetCore()->GetRenderThread();
	if( NULL == pRenderThread )
		return;

	if( true == pRenderThread->m_bIsRunAD )
		return;

	SetDisplay( eType );

	pRenderThread->m_bIsRunAD = true;
	pRenderThread->m_fTimeAD = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	pRenderThread->AdvanceDrawFrame( m_pMovie, m_hMovie );

	// 간혹 로딩화면 출력이 안되는 경우가 있다.
	// 렌더링 한번 할 때까지 기다리자.
	pRenderThread->WaitAdvanceDrawFrame();
}

void GfxLoadingScene::OnEnd()
{
	GfxRenderThread* pRenderThread = GetCore()->GetRenderThread();
	if( NULL == pRenderThread )
		return;

	pRenderThread->m_bIsRunAD = false;
}

void GfxLoadingScene::SetDisplay( ELoadingSceneType eType )
{
	GFx::Value args[ 2 ], result;

	// 프레임 구간 이동 ---------------------------------------
	switch( eType )
	{
	case ELoadingSceneType::Intro :
		{
			args[ 0 ].SetString( "Intro" );
		}
		break;

	case ELoadingSceneType::MoveMap :
		{
			args[ 0 ].SetString( "MoveMap" );
		}
		break;

	case ELoadingSceneType::LogOut :
		{
			args[ 0 ].SetString( "LogOut" );
		}
		break;
	}
	m_pMovie->Invoke( "root.gotoAndStop", &result, &args[ 0 ], 1 );

	// 텍스트 설정 ----------------------------------------------
	if( ELoadingSceneType::MoveMap == eType )
	{
		std::wstring str;
		CString strTemp;

		// 맵 이름 -----------------------------
		if( true == m_sMapID.IsValidNativeID() )
			strTemp.Format( _T("< %s >"), GfxInterface::GetInstance()->GetGaeaClient()->GetMapName( m_sMapID ) );
		str = GfxInterface::GetInstance()->EncodeText( strTemp.GetBuffer() );

		args[ 0 ].SetInt( (int) ELoadingTextID::MapName );
		args[ 1 ].SetStringW( str.c_str() );
		m_pMovie->Invoke( "root.CF_SetText", &result, args, 2 );

		// 팁 ----------------------------------
		if( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CGameTextMan::GetInstance().LoadText( 
				RANPARAM::strTipText.GetString(), CGameTextMan::EM_TIP_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
			strTemp = CGameTextMan::GetInstance().GetTipRandomText();

			std::string strOrigin = strTemp.GetString(), strConvert;
			STRUTIL::StringReplace( strOrigin, "\\r\\n", "\r\n", true, &strConvert );
			strTemp = strConvert.c_str();
		}
		else
		{
			const SUBPATH* pPath = GfxInterface::GetInstance()->GetGaeaClient()->GetSubPath();
			std::string strAppPath = pPath->AppRoot();

			CString strFullPath = strAppPath.c_str();
			strFullPath += pPath->GLogicPath().c_str();
			strFullPath += ID2GAMEWORD( "TIP_FILE" );

			std::vector< CString > vecString;
			CBaseStringParser::Parse( strFullPath, vecString );

			INT nSelect = CBaseStringParser::RandomSelect( vecString );
			if( nSelect >= 0 )
			{
			//	strTemp = vecString[ nSelect ];
				std::string strConv;
				STRUTIL::StringReplace(vecString[ nSelect ].GetString(), "\r\n", "\n", true, &strConv);
				strTemp = strConv.c_str();
			}
		}
		str = GfxInterface::GetInstance()->EncodeText( strTemp.GetBuffer() );

		args[ 0 ].SetInt( ELoadingTextID::Tip );
		args[ 1 ].SetStringW( str.c_str() );
		m_pMovie->Invoke( "root.CF_SetText", &result, args, 2 );
	}
	else
	{
		args[ 1 ].SetString( "" );

		args[ 0 ].SetInt( ELoadingTextID::MapName );
		m_pMovie->Invoke( "root.CF_SetText", &result, args, 2 );

		args[ 0 ].SetInt( ELoadingTextID::Tip );
		m_pMovie->Invoke( "root.CF_SetText", &result, args, 2 );
	}

	// 로딩 이미지 설정 ----------------------------------------
	std::string strPathFile = NSGFXFILE::g_strTexturePath;
	if( true == m_strLoadingImgFile.empty() )
	{
		int nIndex = (rand() % 2) + 1;

		CString str;
		str.Format( "loading_%03d.dds", nIndex );

		m_strLoadingImgFile = str.GetBuffer();
	}
	strPathFile.append( m_strLoadingImgFile );
	strPathFile = sc::string::ansi_to_utf8( strPathFile );

	args[ 0 ].SetString( strPathFile.c_str() );
	m_pMovie->Invoke( "root.CF_LoadImage", &result, &args[ 0 ], 1 );
}