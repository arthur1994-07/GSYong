#include "pch.h"

#include <boost/lexical_cast.hpp>

#include "./DxFontMan.h"

#include "NSLoadingLagTester.h"

namespace NSLoadingLagTester
{
	DWORD	g_DxEffSinglePropGMan_ProcessCompleteData(0); //1
	TSTRING	g_strDxEffSinglePropGMan_ProcessCompleteData;
	DWORD	g_DxAnimationListManager_ProcessCompleteData(0); //2
	TSTRING	g_strDxAnimationListManager_ProcessCompleteData;
	DWORD	g_DxSkeletonManager_ProcessCompleteData(0); //3
	TSTRING	g_strDxSkeletonManager_ProcessCompleteData;
	DWORD	g_DxSkinAnimationManager_ProcessCompleteData(0);//4
	TSTRING	g_strDxSkinAnimationManager_ProcessCompleteData;
	DWORD	g_DxSkinMeshManager_ProcessCompleteData(0);	//5
	TSTRING	g_strDxSkinMeshManager_ProcessCompleteData;
	DWORD	g_DxSimpleMeshMan_ProcessCompleteData(0);//6
	TSTRING	g_strDxSimpleMeshMan_ProcessCompleteData;
	DWORD	g_TextureManager_ProcessCompleteData(0);//7
	TSTRING	g_strTextureManager_ProcessCompleteData;
	DWORD	g_dwResourceUpdatorManager_Update(0);

	DWORD	g_DxSkinMeshManager_ProcessCompleteData1(0);
	DWORD	g_DxSkinMeshManager_ProcessCompleteData2(0);
	DWORD	g_DxSkinMeshManager_ProcessCompleteData3(0);
	DWORD	g_DxSkinMeshManager_ProcessCompleteData4(0);
	TSTRING	g_strSkinMeshManager_ProcessCompleteData4;

	void Display( CD3DFontPar* pD3dFont, float fNextLine )
	{
		D3DCOLOR fontColor = D3DCOLOR_ARGB(255,255,255,255);
		TCHAR szMsg[MAX_PATH] = TEXT("");

		TSTRING strFrameStats;
		strFrameStats += "_ egp:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxEffSinglePropGMan_ProcessCompleteData );
		strFrameStats += "_ ani:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxAnimationListManager_ProcessCompleteData );
		strFrameStats += "_ bon:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkeletonManager_ProcessCompleteData );
		strFrameStats += "_ cfg:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinAnimationManager_ProcessCompleteData );
		strFrameStats += "_ Skin:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData );
		strFrameStats += "_ sim:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSimpleMeshMan_ProcessCompleteData );
		strFrameStats += "_ Tex:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_TextureManager_ProcessCompleteData );
		strFrameStats += "_     Update:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_dwResourceUpdatorManager_Update );
		strFrameStats += "      file:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData1 );
		strFrameStats += "_ ani:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData2 );
		strFrameStats += "_ cal:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData3 );
		strFrameStats += "      Main:";
		strFrameStats += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData4 );

		TSTRING strFrameStats2;
		strFrameStats2 += "Update : ";

		DWORD dwDelay(0);
		TSTRING strDely;
		if ( dwDelay < g_DxEffSinglePropGMan_ProcessCompleteData )
		{
			dwDelay = g_DxEffSinglePropGMan_ProcessCompleteData;
			strDely = g_strDxEffSinglePropGMan_ProcessCompleteData;
		}
		if ( dwDelay < g_DxAnimationListManager_ProcessCompleteData )
		{
			dwDelay = g_DxAnimationListManager_ProcessCompleteData;
			strDely = g_strDxAnimationListManager_ProcessCompleteData;
		}
		if ( dwDelay < g_DxSkeletonManager_ProcessCompleteData )
		{
			dwDelay = g_DxSkeletonManager_ProcessCompleteData;
			strDely = g_strDxSkeletonManager_ProcessCompleteData;
		}
		if ( dwDelay < g_DxSkinAnimationManager_ProcessCompleteData )
		{
			dwDelay = g_DxSkinAnimationManager_ProcessCompleteData;
			strDely = g_strDxSkinAnimationManager_ProcessCompleteData;
		}
		if ( dwDelay < g_DxSkinMeshManager_ProcessCompleteData )
		{
			dwDelay = g_DxSkinMeshManager_ProcessCompleteData;
			strDely = g_strDxSkinMeshManager_ProcessCompleteData;
		}
		if ( dwDelay < g_DxSimpleMeshMan_ProcessCompleteData )
		{
			dwDelay = g_DxSimpleMeshMan_ProcessCompleteData;
			strDely = g_strDxSimpleMeshMan_ProcessCompleteData;
		}
		if ( dwDelay < g_TextureManager_ProcessCompleteData )
		{
			// 큰 값으로 여기에 들어온다는 건 텍스쳐 이미지가 어떤지 한번 확인해 봐야한다.
			dwDelay = g_TextureManager_ProcessCompleteData;
			strDely = g_strTextureManager_ProcessCompleteData;
		}

		if ( dwDelay > 1 )
		{
			strFrameStats2 += strDely;
			strFrameStats2 += "___";
			strFrameStats2 += boost::lexical_cast<TSTRING>( dwDelay );
		}

		strFrameStats2 += ",   DirectLoading: ";
		if ( g_DxSkinMeshManager_ProcessCompleteData4 > 1 )
		{
			strFrameStats2 += g_strSkinMeshManager_ProcessCompleteData4;
			strFrameStats2 += "___";
			strFrameStats2 += boost::lexical_cast<TSTRING>( g_DxSkinMeshManager_ProcessCompleteData4 );
		}

		StringCchCopy( szMsg, MAX_PATH, strFrameStats.c_str() );
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );
		fNextLine += 20.0f;

		StringCchCopy( szMsg, MAX_PATH, strFrameStats2.c_str() );
		pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );
		fNextLine += 20.0f;

		static DWORD g_dwCount(0);
		++g_dwCount;
		if ( g_dwCount > 20 )
		{
			g_dwCount = 0;

			if ( g_DxEffSinglePropGMan_ProcessCompleteData > 0 )
				--g_DxEffSinglePropGMan_ProcessCompleteData;

			if ( g_DxAnimationListManager_ProcessCompleteData > 0 )
				--g_DxAnimationListManager_ProcessCompleteData;

			if ( g_DxSkeletonManager_ProcessCompleteData > 0 )
				--g_DxSkeletonManager_ProcessCompleteData;

			if ( g_DxSkinAnimationManager_ProcessCompleteData > 0 )
				--g_DxSkinAnimationManager_ProcessCompleteData;

			if ( g_DxSkinMeshManager_ProcessCompleteData > 0 )
				--g_DxSkinMeshManager_ProcessCompleteData;

			if ( g_DxSkinMeshManager_ProcessCompleteData1 > 0 )
				--g_DxSkinMeshManager_ProcessCompleteData1;

			if ( g_DxSkinMeshManager_ProcessCompleteData2 > 0 )
				--g_DxSkinMeshManager_ProcessCompleteData2;

			if ( g_DxSkinMeshManager_ProcessCompleteData3 > 0 )
				--g_DxSkinMeshManager_ProcessCompleteData3;

			if ( g_DxSkinMeshManager_ProcessCompleteData4 > 0 )
				--g_DxSkinMeshManager_ProcessCompleteData4;

			if ( g_DxSimpleMeshMan_ProcessCompleteData > 0 )
				--g_DxSimpleMeshMan_ProcessCompleteData;

			if ( g_TextureManager_ProcessCompleteData > 0 )
				--g_TextureManager_ProcessCompleteData;

			if ( g_dwResourceUpdatorManager_Update > 0 )
				--g_dwResourceUpdatorManager_Update;
		}
	}
}