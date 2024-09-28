#pragma once

class CD3DFontPar;

namespace NSLoadingLagTester
{
	extern DWORD	g_DxEffSinglePropGMan_ProcessCompleteData; //1
	extern TSTRING	g_strDxEffSinglePropGMan_ProcessCompleteData;
	extern DWORD	g_DxAnimationListManager_ProcessCompleteData; //2
	extern TSTRING	g_strDxAnimationListManager_ProcessCompleteData;
	extern DWORD	g_DxSkeletonManager_ProcessCompleteData; //3
	extern TSTRING	g_strDxSkeletonManager_ProcessCompleteData;
	extern DWORD	g_DxSkinAnimationManager_ProcessCompleteData;//4
	extern TSTRING	g_strDxSkinAnimationManager_ProcessCompleteData;
	extern DWORD	g_DxSkinMeshManager_ProcessCompleteData;	//5
	extern TSTRING	g_strDxSkinMeshManager_ProcessCompleteData;
	extern DWORD	g_DxSimpleMeshMan_ProcessCompleteData;//6
	extern TSTRING	g_strDxSimpleMeshMan_ProcessCompleteData;
	extern DWORD	g_TextureManager_ProcessCompleteData;//7
	extern TSTRING	g_strTextureManager_ProcessCompleteData;
	extern DWORD	g_dwResourceUpdatorManager_Update;

	extern DWORD	g_DxSkinMeshManager_ProcessCompleteData1;
	extern DWORD	g_DxSkinMeshManager_ProcessCompleteData2;
	extern DWORD	g_DxSkinMeshManager_ProcessCompleteData3;
	extern DWORD	g_DxSkinMeshManager_ProcessCompleteData4;
	extern TSTRING	g_strSkinMeshManager_ProcessCompleteData4;

	void Display( CD3DFontPar* pD3dFont, float fNextLine );
}