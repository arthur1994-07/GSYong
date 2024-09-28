#include "../pch.h"
#include <process.h>

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../BugTrap/BugTrap.h"

#include "../../EngineLib/Common/SubPath.h"
#include "../../EngineLib/DxTools/DxGrapUtils.h"
#include "../../EngineLib/DxTools/d3dfont.h"
#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/RANPARAM.h"

#include "../Init/NSInitClientLua.h"
#include "../Country/GLVictoriousCountryManClient.h"

#include "../Etc/BaseStringParser.h"
#include "../Stage/DxLobyStage.h"
#include "../GLGaeaClient.h"

#include "./LoadingDirectClient.h"
#include "./LoadingThread.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace LoadingThread
{
    enum
    {
        LODING_COUNT = 4,
        TIP_SIZE = 10,
    };
}

namespace LoadingThread
{
	LPDIRECT3DDEVICEQ g_pd3dDevice = NULL;
	HWND g_hWnd = NULL;
    CString g_strTextureName;
    SNATIVEID g_sMapID = NATIVEID_NULL();
    GLGaeaClient* g_pGaeaClient = NULL;
    bool g_bFirst = true;
	bool g_bOVER15 = false;
	BOOL g_bFullSizeLoadingImage = FALSE;

    HANDLE g_hThread = NULL;
    DWORD g_dwThreadID = 0;
    BOOL g_bWait = FALSE;
    BOOL g_bRender = FALSE;
    BOOL g_bRenderFailed = FALSE;

    LPDIRECT3DSTATEBLOCK9 g_pStateBlock = NULL;
    CD3DFontPar* g_pFont9 = NULL;
    CD3DFontPar* g_pFont10 = NULL;
    CD3DFontPar* g_pFont13 = NULL;
	CD3DFontPar* g_pFont18 = NULL;

    TextureResource g_sLoadingTex;
    TextureResource g_sGradeTex;
    TextureResource g_sGradeTex2;
    TextureResource g_sGradeTex3;
    TextureResource g_sHelpTex;
    TextureResource g_sLogoTex;
    TextureResource g_sMapTex;
    TextureResource g_sTopBackTex;
    TextureResource g_sUnderBackTex;
    TextureResource g_sLoadingBackTex;
	TextureResource g_sWinnerCountryFlagTex;

    std::string g_szTextureName;
    std::string g_szGradeName = "Loading_st.dds";
    std::string g_szGradeName2 = "Certification.dds";
    std::string g_szGradeName3 = "Agetext.dds";
    std::string g_szHelpName = "HintIcon.dds";
    std::string g_szLogoName = "mincomslogo.dds";
    std::string g_szMapName = "mapnameback.dds";
    std::string g_szTopBackName = "ld_top.dds";
    std::string g_szUnderBackName = "ld_under.dds";
    std::string g_szLoadingBackName = "ld_back.dds";
	std::string g_strWinnerCountryName = "";

    TEXTUREVERTEX g_vVertex[6];
    CString g_strTip[ TIP_SIZE ];
    CString g_strMessage;
    CString g_strMapName;
    INT g_nLoadingIndex = 0;
    INT g_nTipIndex = 0;
    INT g_nStrMapLength = 0;
	int g_nBattleIslandsFontSize = 18;

	const D3DXVECTOR2 vWinnerCountryFlag ( 10, 10 );
	const D3DXVECTOR2 vWinnerCountryFlagSize ( 128, 96 );

    const D3DXVECTOR2 vOver15 ( 100, 20 );		// Right 정렬 좌표
    const D3DXVECTOR2 vOver15Size ( 92, 109 );
    const D3DXVECTOR2 vOver15Tex ( 420, 0 );
    const D3DXVECTOR2 vOver15Full ( 512, 128 );

    const D3DXVECTOR2 vOverGrade ( 202, 20 );	// Right 정렬 좌표
    const D3DXVECTOR2 vOverGradeSize ( 92, 109 );
    const D3DXVECTOR2 vOverGradeTex ( 420, 147 );
    const D3DXVECTOR2 vOverGradeFull ( 512, 256 );

    const D3DXVECTOR2 vAgeText ( 0, 0 );			// Right 정렬 좌표
    const D3DXVECTOR2 vAgeTextSize ( 726, 380 );
    const D3DXVECTOR2 vAgeTextTex ( 2, 2 );
    const D3DXVECTOR2 vAgeTextFull ( 1024, 512 );

    const D3DXVECTOR2 vHelpTex ( 0, 0 );
    const D3DXVECTOR2 vHelpSize ( 97, 60 );
    const D3DXVECTOR2 vHelpTexSize ( 128, 64 );

    const D3DXVECTOR2 vLodingTex ( 0, 0 );
    const D3DXVECTOR2 vLodingSize ( 104, 104 );
    const D3DXVECTOR2 vLodingTexSize ( 512, 128 );
    const D3DXVECTOR2 vLodingTexGap ( 105, 0 );

    const D3DXVECTOR2 vLogoTex ( 0, 0 );
    const D3DXVECTOR2 vLogoSize ( 155, 38 );
    const D3DXVECTOR2 vLogoTexSize ( 256, 64 );

    const D3DXVECTOR2 vMapTex ( 0, 0 );
    const D3DXVECTOR2 vMapSize ( 340, 56 );
    const D3DXVECTOR2 vMapTexSize ( 512, 64 );

    const D3DXVECTOR2 vImageTex ( 0, 0 );
    const D3DXVECTOR2 vImageSize ( 1024, 512 );
    const D3DXVECTOR2 vImageTexSize ( 1024, 512 );

    const D3DXVECTOR2 vTopTex ( 0, 0 );
    const D3DXVECTOR2 vTopSize ( 1024, 147 );
    const D3DXVECTOR2 vTopTexSize ( 1024, 256 );

    const D3DXVECTOR2 vUnderTex ( 0, 0 );
    const D3DXVECTOR2 vUnderSize ( 1024, 147 );
    const D3DXVECTOR2 vUnderTexSize ( 1024, 256 );

    const D3DXVECTOR2 vLoadingBackTex ( 0, 0 );
    const D3DXVECTOR2 vLoadingBackSize ( 126, 126 );
    const D3DXVECTOR2 vLoadingBackTexSize ( 128, 128 );
}

namespace LoadingThread
{
    TextureResource LoadTexture( const TSTRING& pathFileName )
    {
        return TextureManager::GetInstance().LoadTexture(
            pathFileName,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            1,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_ETC,
			false,
			FALSE );
    }

    void SetVertexPos( float LEFT, float TOP, float SIZEX, float SIZEY )
    {
	    float RIGHT = LEFT + SIZEX;
	    float BOTTOM = TOP + SIZEY;

	    g_vVertex[0].SetPos( LEFT, TOP );
	    g_vVertex[1].SetPos( RIGHT, TOP);
	    g_vVertex[2].SetPos( LEFT, BOTTOM );

	    g_vVertex[3].SetPos( LEFT, BOTTOM );
	    g_vVertex[4].SetPos( RIGHT, TOP);
	    g_vVertex[5].SetPos( RIGHT, BOTTOM );
    }

    void SetTexturePos( float LEFT, float TOP, float SIZEX, float SIZEY, float TEX_SIZEX, float TEX_SIZEY )
    {
	    float RIGHT = LEFT + SIZEX;
	    float BOTTOM = TOP + SIZEY;

	    g_vVertex[0].SetTexPos( LEFT / TEX_SIZEX, TOP / TEX_SIZEY );
	    g_vVertex[1].SetTexPos( RIGHT / TEX_SIZEX, TOP / TEX_SIZEY);
	    g_vVertex[2].SetTexPos( LEFT / TEX_SIZEX, BOTTOM / TEX_SIZEY );

	    g_vVertex[3].SetTexPos( LEFT / TEX_SIZEX, BOTTOM / TEX_SIZEY );
	    g_vVertex[4].SetTexPos( RIGHT / TEX_SIZEX, TOP / TEX_SIZEY );
	    g_vVertex[5].SetTexPos( RIGHT / TEX_SIZEX, BOTTOM / TEX_SIZEY );
    }

    void RenderVertex( TextureResource& sTexture )
    {
        if( !sTexture.IsValid() )
            return;

        g_pd3dDevice->SetFVF( TEXTUREVERTEXFVF );
	    g_pd3dDevice->SetTexture( 0, sTexture.GetTexture() );
	    g_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, g_vVertex, sizeof(TEXTUREVERTEX) );
    }

    bool LoadResource( BOOL bShowWinnerCountryFlag )
    {
        g_pd3dDevice = LoadingDirectClient::s_pd3dDevice;
		g_hWnd = LoadingDirectClient::s_hWnd;
        g_pGaeaClient = LoadingDirectClient::s_pGaeaClient;

        const SUBPATH* pPath = g_pGaeaClient->GetSubPath();
        std::string strAppPath = pPath->AppRoot();

        SAFE_RELEASE( g_pStateBlock );
	    g_pd3dDevice->CreateStateBlock( D3DSBT_ALL, &g_pStateBlock );

	    g_pFont9 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	    if( g_pFont9 )
		    g_pFont9->UsageCS( TRUE );

	    g_pFont13 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 13, _DEFAULT_FONT_FLAG );
	    if( g_pFont13 )
		    g_pFont13->UsageCS( TRUE );

	    g_pFont10 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, 10, _DEFAULT_FONT_FLAG );
	    if( g_pFont10 )
		    g_pFont10->UsageCS( TRUE );

		g_pFont18 = DxFontMan::GetInstance().LoadDxFont( _DEFAULT_FONT, g_nBattleIslandsFontSize, D3DFONT_BOLD | D3DFONT_SHADOW );
		if( g_pFont18 )
			g_pFont18->UsageCS( TRUE );

        if( g_strTextureName.IsEmpty() || g_strTextureName == _T("null") )
	    {
		   // srand( (UINT) time(NULL) );
		    int nIndex = (rand() % 2) + 1;
		    g_strTextureName.Format( "loading_%03d.dds", nIndex );
	    }

     //   TCHAR szTemp[256] = {0};
	    //StringCchPrintf(
     //       szTemp,
     //       256,
     //       g_strTextureName.GetString(),
     //       strAppPath.c_str(),
     //       pPath->TextureFileRoot().c_str() );
     //   g_szTextureName = szTemp;

		g_szTextureName = g_strTextureName.GetString();

        g_sLoadingTex = LoadTexture( g_szTextureName );
        g_sGradeTex = LoadTexture( g_szGradeName );
        g_sHelpTex = LoadTexture( g_szHelpName );
        g_sMapTex = LoadTexture( g_szMapName );
        g_sTopBackTex = LoadTexture( g_szTopBackName );
        g_sUnderBackTex = LoadTexture( g_szUnderBackName );
        g_sLoadingBackTex = LoadTexture( g_szLoadingBackName );

		// 승리 국가 국기 이름을 얻는다;
		if ( bShowWinnerCountryFlag || NSInitClientLua::IsBattleIslandsWarLoadingImageOn( g_sMapID.dwID ) )
		{
			if ( GLUseFeatures::GetInstance().IsUsingSelectCountry() )
			{
				Country::GLVictoriousCountryManClient* pVCountryMan =
					Country::GLVictoriousCountryManClient::Instance();
				if ( pVCountryMan )
				{
					const TSTRING strFlagName =
						pVCountryMan->GetVCountryFileNameFirst(
						Country::CAPTURE_THE_ISLANDS );

					if ( !strFlagName.empty() )
					{
						g_sWinnerCountryFlagTex = LoadTexture( strFlagName.c_str() );
					}

					std::string strBattleIslandsWinnerName = pVCountryMan->GetVCountryNameFirst( Country::CAPTURE_THE_ISLANDS );
					if ( !strBattleIslandsWinnerName.empty() )
					{
						g_strWinnerCountryName = sc::string::format( ID2GAMEINTEXT( "BATTLE_ISLANDS_WINNER_INFO" ), strBattleIslandsWinnerName );
					}
				}
			}
		}

        if( g_bFirst )
            g_sLogoTex = LoadTexture( g_szLogoName );

	    if( g_bOVER15 )
        {
            g_sGradeTex2 = LoadTexture( g_szGradeName2 );
            g_sGradeTex3 = LoadTexture( g_szGradeName3 );
        }

        if( !g_bFirst )
        {
			/*
	        bool bTutoMap = false;
	        for ( int i = 0; i < GLCONST_CHAR::wTUTORIA_NUM; i++ )
	        {
		        if ( g_sMapID.wMainID == GLCONST_CHAR::nidTUTORIALMAP[ i ].wMainID )
		        {
			        bTutoMap = true;
			        break;
		        }
	        }

	        if( bTutoMap )
	        {
		        CString strName;
		        strName.Format( "TUTORIAL_HINT_0" );
		        g_strMessage = ID2GAMEINTEXT( strName );

                if( !g_strMessage.IsEmpty() )
                {
		            while( g_nTipIndex < TIP_SIZE - 1 )
		            {
			            if( !AfxExtractSubString( g_strTip[g_nTipIndex], g_strMessage, g_nTipIndex, _T('\n') ) )
				            break;
                        ++g_nTipIndex;
		            }
                }
	        }
	        else
			*/
	        {
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
//					CGameTextMan::GetInstance().LoadText( RANPARAM::strTipText.GetString(), CGameTextMan::EM_TIP_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT );
					g_strMessage = CGameTextMan::GetInstance().GetTipRandomText();
					std::string strOrigin(g_strMessage.GetString()), strConvert;
					STRUTIL::StringReplace(strOrigin, "\\r\\n", "\r\n", true, &strConvert);
					g_strMessage = strConvert.c_str();
				}
				else
				{
					CString strFullPath = strAppPath.c_str();
					strFullPath += pPath->GLogicPath().c_str();
					strFullPath += ID2GAMEWORD("TIP_FILE");

					std::vector< CString > vecString;
					CBaseStringParser::Parse( strFullPath, vecString );
					INT nSelect = CBaseStringParser::RandomSelect( vecString );
					if( nSelect >= 0 )
						g_strMessage = vecString[ nSelect ];
				}                

                if( !g_strMessage.IsEmpty() )
                {
		            while( g_nTipIndex < TIP_SIZE - 1 )
		            {
			            if( !AfxExtractSubString( g_strTip[g_nTipIndex], g_strMessage, g_nTipIndex, _T('\n') ) )
				            break;
                        ++g_nTipIndex;
		            }
                }
	        }

            if( g_sMapID != NATIVEID_NULL() && g_pGaeaClient->GetMapName( g_sMapID ) != NULL )
	        {
		        g_strMapName.Format( _T("< %s >"), g_pGaeaClient->GetMapName( g_sMapID ) );
		        g_nStrMapLength = g_strMapName.GetLength();
	        }
	        else
	        {
		        g_nStrMapLength = 0;
	        }
        }

        return true;
    }

    void ReleaseResource()
    {
        g_sLoadingTex.Clear();
        g_sGradeTex.Clear();
        g_sGradeTex2.Clear();
        g_sHelpTex.Clear();
        g_sLogoTex.Clear();
        g_sMapTex.Clear();
        g_sTopBackTex.Clear();
        g_sUnderBackTex.Clear();
        g_sLoadingBackTex.Clear();
		g_sWinnerCountryFlagTex.Clear();

		if( g_pFont18 )
			g_pFont18->UsageCS( FALSE );
        if( g_pFont13 )
		    g_pFont13->UsageCS( FALSE );
        if( g_pFont10 )
		    g_pFont10->UsageCS( FALSE );
        if( g_pFont9 )
		    g_pFont9->UsageCS( FALSE );

        SAFE_RELEASE( g_pStateBlock );

		g_strWinnerCountryName = "";
        g_strTextureName.Empty();
		g_sMapID = NATIVEID_NULL();

        g_strMessage.Empty();
        g_strMapName.Empty();
        g_nLoadingIndex = 0;
        g_nTipIndex = 0;
        g_nStrMapLength = 0;
    }

    void FrameMove()
    {
        DWORD dwTickCur = timeGetTime();
		static DWORD dwTickLast = dwTickCur;
	    if( dwTickCur - dwTickLast > 500 )
	    {
		    dwTickLast = dwTickCur;
		    if( ++g_nLoadingIndex >= LODING_COUNT )
			    g_nLoadingIndex = 0;
	    }
    }

    void Render()
    {
        LPDIRECT3DDEVICEQ pd3dDevice = g_pd3dDevice;

	    if( FAILED( pd3dDevice->TestCooperativeLevel() ) )
	    {
		    g_bRenderFailed = TRUE;
			return;
	    }

	    pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0L );

	    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	    {
            RECT rect;
	        GetClientRect( g_hWnd, &rect );

	        const float fWidth = float( rect.right - rect.left );
	        const float fHeight = float( rect.bottom - rect.top );

	        const float	fRealImageX = 1024.0f;
	        const float	fRealImageY = 768.0f;
	        const float	fImageSize = 1023.0f;

	        const float fWidthRatio = fWidth / fRealImageX;
	        const float fHeightRatio= fHeight / fRealImageY;

	        const float LeftPos  = 0.0f;
	        const float TopPos   = 0.0f;
	        const float RightPos = fWidth;
	        const float BottomPos= fHeight;

	        float ROOT_LEFT		= LeftPos;
	        float ROOT_TOP		= TopPos;
	        float ROOT_SIZEX	= fWidth;
	        float ROOT_SIZEY	= fHeight;

            D3DXVECTOR2 vImageAlign;
	        vImageAlign.x = 0.5f;
	        vImageAlign.y = ( vTopSize.y * 0.95f );

	        D3DXVECTOR2 vTopAlign;
	        vTopAlign.x = 0.5f;
	        vTopAlign.y = 0.5f;

	        D3DXVECTOR2 vUnderAlign;
	        vUnderAlign.x = 0.5f;
	        vUnderAlign.y = fHeight - vUnderSize.y + 0.5f;

	        D3DXVECTOR2 vLoadingBackAlign;
	        vLoadingBackAlign.x = (fWidth - vLoadingBackSize.x) * 0.95f + 0.5f;
	        vLoadingBackAlign.y = vUnderAlign.y + 10.0f;

	        D3DXVECTOR2 vLoadingAlign;
	        vLoadingAlign.x = vLoadingBackAlign.x + ( ( vLoadingBackSize.x - vLodingSize.x ) / 2.0f );
	        vLoadingAlign.y = vLoadingBackAlign.y + ( ( vLoadingBackSize.y - vLodingSize.y ) / 2.0f );

	        D3DXVECTOR2 vLogoAlign;
	        vLogoAlign.x = ( vLoadingBackAlign.x - (vLogoSize.x + 30.0f ) );
	        vLogoAlign.y = ( fHeight * 0.9f );

	        D3DXVECTOR2 vHelpAlign;
	        vHelpAlign.x = ( fWidth * 0.10f ) + 0.5f;
	        vHelpAlign.y = vUnderAlign.y + ( ( vUnderSize.y - vHelpSize.y ) / 2.0f );

            D3DXVECTOR2 vMapBackAlign;
	        vMapBackAlign.x = ( ( fWidth - vMapSize.x ) / 2.0f ) + 20.0f + 0.5f ;
	        vMapBackAlign.y = vTopAlign.y + ( ( vTopSize.y - vMapSize.y ) * 0.7f );

			D3DXVECTOR2 vWinnerCountryAlign;
			vWinnerCountryAlign.x = (fWidth - ( g_strWinnerCountryName.size() * g_nBattleIslandsFontSize * 0.55f ) ) / 2.0f;
			vWinnerCountryAlign.y = vImageAlign.y * 1.25f;

            D3DXVECTOR2 vOver15Align;
	        vOver15Align.x = (fWidth - vOver15.x) * 0.95f + 0.5f;
	        vOver15Align.y = vOver15.y;

	        D3DXVECTOR2 vOverGradeAlign;
	        vOverGradeAlign.x = vOver15Align.x - vOverGradeSize.x;
	        vOverGradeAlign.y = vOverGrade.y;

	        D3DXVECTOR2 vAgeTextAlign;
	        vAgeTextAlign.x = (fWidth -  ( vAgeTextSize.x * fWidthRatio  )) / 2.0f + 0.5f;
	        vAgeTextAlign.y = (fHeight - ( vAgeTextSize.y * fHeightRatio )) / 2.0f + 0.5f;

            g_pStateBlock->Capture();

			pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR );
		    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR );
		    pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR );

			// 로딩 이미지.
			if ( g_bFullSizeLoadingImage )
			{
				SetVertexPos ( ROOT_LEFT, ROOT_TOP, ROOT_SIZEX, ROOT_SIZEY );
			}
			else
			{
				SetVertexPos ( ROOT_LEFT + vImageAlign.x, ROOT_TOP + vImageAlign.y, ROOT_SIZEX, vUnderAlign.y - vTopSize.y + 20.0f );
			}
			SetTexturePos( vImageTex.x, vImageTex.y, vImageSize.x, vImageSize.y, vImageTexSize.x, vImageTexSize.y );
			RenderVertex ( g_sLoadingTex );

		    pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
		    pd3dDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		    pd3dDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		    pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
		    pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
		    pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );

            pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

			if ( !g_bFullSizeLoadingImage )
			{
				SetVertexPos ( ROOT_LEFT + vTopAlign.x, ROOT_TOP + vTopAlign.y, ROOT_SIZEX, vTopSize.y );
				SetTexturePos( vTopTex.x, vTopTex.y, vTopSize.x, vTopSize.y, vTopTexSize.x, vTopTexSize.y );
				RenderVertex ( g_sTopBackTex );

				SetVertexPos ( ROOT_LEFT + vUnderAlign.x, ROOT_TOP + vUnderAlign.y, ROOT_SIZEX, vUnderSize.y );
				SetTexturePos( vUnderTex.x, vUnderTex.y, vUnderSize.x, vUnderSize.y, vUnderTexSize.x, vUnderTexSize.y );
				RenderVertex ( g_sUnderBackTex );
			}

		    if( !g_strMessage.IsEmpty() )
            {
                SetVertexPos ( ROOT_LEFT + vHelpAlign.x, ROOT_TOP + vHelpAlign.y, vHelpSize.x, vHelpSize.y );
	            SetTexturePos( vHelpTex.x, vHelpTex.y, vHelpSize.x, vHelpSize.y, vHelpTexSize.x, vHelpTexSize.y );
			    RenderVertex ( g_sHelpTex );
            }

		    if( !g_strMapName.IsEmpty() )
            {
                SetVertexPos ( ROOT_LEFT + vMapBackAlign.x, ROOT_TOP + vMapBackAlign.y, vMapSize.x, vMapSize.y );
	            SetTexturePos( vMapTex.x, vMapTex.y, vMapSize.x, vMapSize.y, vMapTexSize.x, vMapTexSize.y );
			    RenderVertex ( g_sMapTex );
            }

            SetVertexPos ( ROOT_LEFT + vLoadingBackAlign.x, ROOT_TOP + vLoadingBackAlign.y, vLoadingBackSize.x, vLoadingBackSize.y );
	        SetTexturePos( vLoadingBackTex.x, vLoadingBackTex.y, vLoadingBackSize.x, vLoadingBackSize.y, vLoadingBackTexSize.x, vLoadingBackTexSize.y );
		    RenderVertex ( g_sLoadingBackTex );

            SetVertexPos ( ROOT_LEFT + vLoadingAlign.x, ROOT_TOP + vLoadingAlign.y, vLodingSize.x, vLodingSize.y );
		    SetTexturePos( vLodingTex.x + ( g_nLoadingIndex * vLodingTexGap.x ), vLodingTex.y , vLodingSize.x, vLodingSize.y, vLodingTexSize.x, vLodingTexSize.y );
            RenderVertex ( g_sGradeTex );

		    if( g_pFont9 && g_bFirst )
            {
                D3DXVECTOR2 vCopyrightAlign;
	            vCopyrightAlign.x = 15.0f;
	            vCopyrightAlign.y = ( fHeight * 0.93f );

				static std::string strCompany = ID2GAMEWORD ( "COPYRIGHT_COMPANY_LOAD" );
				static std::string strVersion = sc::getFileVersion();
				
				std::string strCopyright = strCompany;
				strCopyright += " ";
				strCopyright += strVersion;

			    g_pFont9->DrawText(
                    vCopyrightAlign.x,
                    vCopyrightAlign.y,
                    NS_UITEXTCOLOR::WHITE,
                    strCopyright.c_str() );
            }

		    if( g_pFont10 )
		    {
                D3DXVECTOR2 vTipAlign;
	            vTipAlign.x = ( ( fWidth * 0.10f ) + vHelpSize.x ) + 0.5f;//( fWidth - vHelpSize.x ) / 2.0f + 65.0f;
	            vTipAlign.y = vUnderAlign.y + ( ( vUnderSize.y - vHelpSize.y ) / 2.0f );

			    for( INT i=0; i<g_nTipIndex; ++i )
			    {
				    g_pFont10->DrawText(
                        vTipAlign.x,
                        vTipAlign.y + ( i * 16 ),
                        NS_UITEXTCOLOR::WHITE,
                        g_strTip[ i ] );
			    }
		    }

		    if( g_pFont13 )
            {
                D3DXVECTOR2 vMapNameAlign;
	            vMapNameAlign.x = ( fWidth - ( g_nStrMapLength * 9 ) ) / 2.0f;
	            vMapNameAlign.y = vMapBackAlign.y + ( vMapSize.y * 0.4f );

			    g_pFont13->DrawText(
                    vMapNameAlign.x,
                    vMapNameAlign.y,
                    NS_UITEXTCOLOR::WHITE,
                    g_strMapName );
            }

		    if( g_bOVER15 )
            {
                SetVertexPos ( ROOT_LEFT + vOver15Align.x, ROOT_TOP + vOver15Align.y, vOver15Size.x, vOver15Size.y );
	            SetTexturePos( vOver15Tex.x, vOver15Tex.y, vOver15Size.x, vOver15Size.y, vOver15Full.x, vOver15Full.y );
                RenderVertex ( g_sGradeTex );

                SetVertexPos ( ROOT_LEFT + vLogoAlign.x, ROOT_TOP + vLogoAlign.y, vLogoSize.x, vLogoSize.y );
	            SetTexturePos( vLogoTex.x, vLogoTex.y, vLogoSize.x, vLogoSize.y, vLogoTexSize.x, vLogoTexSize.y );
                RenderVertex ( g_sLogoTex );

                SetVertexPos ( ROOT_LEFT + vOverGradeAlign.x, ROOT_TOP + vOverGradeAlign.y, vOverGradeSize.x, vOverGradeSize.y );
	            SetTexturePos( vOverGradeTex.x, vOverGradeTex.y, vOverGradeSize.x, vOverGradeSize.y, vOverGradeFull.x, vOverGradeFull.y );
                RenderVertex ( g_sGradeTex2 );

				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_LINEAR );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_LINEAR );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_LINEAR );

                D3DXVECTOR2 vAgeTextSize2;
	            vAgeTextSize2.x = vAgeTextSize.x * fWidthRatio + 0.5f;
	            vAgeTextSize2.y = vAgeTextSize.y * fHeightRatio + 0.5f;
	            SetVertexPos ( ROOT_LEFT + vAgeTextAlign.x, ROOT_TOP + vAgeTextAlign.y, vAgeTextSize2.x , vAgeTextSize2.y );
	            SetTexturePos( vAgeTextTex.x, vAgeTextTex.y, vAgeTextSize.x, vAgeTextSize.y, vAgeTextFull.x, vAgeTextFull.y );
                RenderVertex ( g_sGradeTex3 );

				pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER,	D3DTEXF_POINT );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER,	D3DTEXF_POINT );
				pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER,	D3DTEXF_NONE );
            }

			// 난사군도에서 승리한 국가가 있을 경우 그 나라 국기를 렌더링 해준다.
			if ( g_sWinnerCountryFlagTex.GetTexture() && !g_strWinnerCountryName.empty() )
			{
				float fWinnerCountryFlagAlign = (vImageAlign.y - vWinnerCountryFlagSize.y) * 0.5f;

				SetVertexPos ( ROOT_LEFT + fWinnerCountryFlagAlign, ROOT_TOP + fWinnerCountryFlagAlign, vWinnerCountryFlagSize.x, vWinnerCountryFlagSize.y );
				SetTexturePos( 0, 0, 1, 1, 1, 1 );
				RenderVertex ( g_sWinnerCountryFlagTex );

				// 텍스트도 출력해 준다.
				if( g_pFont18 )
				{
					g_pFont18->DrawText(
						vWinnerCountryAlign.x,
						vWinnerCountryAlign.y,
						NS_UITEXTCOLOR::WHITE,
						g_strWinnerCountryName.c_str() );
				}
			}

			g_pStateBlock->Apply();

		    pd3dDevice->EndScene();
		    pd3dDevice->Present( NULL, NULL, NULL, NULL );
	    }
        else
		{
			pd3dDevice->EndScene();
		}
    }
}

namespace LoadingThread
{
  //  unsigned int WINAPI	LoadingThread( LPVOID pData )
  //  {
  //      BT_SetTerminate();

		//if( !LoadResource() )
		//{
		//	g_bWait = FALSE;
		//	g_bRender = FALSE;
		//	return 0;
		//}

		//g_bWait = FALSE;

		//while( g_bRender )
		//{
		//	SleepEx( 33, FALSE );

		//	if( g_bRenderFailed )
		//	{
		//		g_bRender = FALSE;
		//		break;
		//	}

		//	FrameMove();
		//	Render();
		//}

		//ReleaseResource();

		//g_bWait = FALSE;

		//return 0;
  //  }

    void Wait()
	{
		while( g_bWait )
			Sleep( 33 );
	}

    void SetData(
	    const CString& strTextureName,
	    SNATIVEID sMapID,
	    bool bFirst,
	    bool bOVER15,
		BOOL bFullSizeLoadingImage )
    {
		g_strTextureName = strTextureName;
		g_sMapID = sMapID;
        g_bFirst = bFirst;
		g_bOVER15 = bOVER15;
		g_bFullSizeLoadingImage = bFullSizeLoadingImage;
    }

 //   void Start()
	//{
 //       g_bWait = TRUE;
	//	g_bRender = TRUE;
 //       g_bRenderFailed = FALSE;

 //       g_hThread = (HANDLE)::_beginthreadex(
 //           NULL,
 //           0,
 //           LoadingThread,
 //           NULL,
 //           0,
 //           (unsigned int*)&g_dwThreadID );

 //       if( !g_hThread )
	//	{
	//		g_bWait = FALSE;
 //           g_bRender = FALSE;
	//		return;
	//	}

 //       if( g_bRender )
 //       {
 //           Wait();
 //           g_bWait = TRUE;
 //       }
	//}

	//void End()
	//{
 //       if( g_bRender )
 //       {
	//	    g_bRender = FALSE;
 //           Wait();
 //       }

 //       if( g_hThread )
 //       {
	//	    CloseHandle( g_hThread );
	//	    g_hThread = NULL;
 //       }
	//}

    void Start_DirectRender( BOOL bShowWinnerCountryFlag )
	{
        g_bWait = TRUE;
		g_bRender = TRUE;
		g_bRenderFailed = FALSE;

		if( !LoadResource( bShowWinnerCountryFlag ) )
		{
			g_bWait = FALSE;
			g_bRender = FALSE;
			return;
		}

		g_bWait = FALSE;
    }

    void End_DirectRender()
	{
		ReleaseResource();

		g_bWait = FALSE;
		g_bRender = FALSE;
	}
}
