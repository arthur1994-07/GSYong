//----------------------------------------------------------------------------------------[DxResponseMan]
//
//		���� ��, ����Ŭ���̾�Ʈ���� �������� �����ؾ��ϴ� ��ġ���� ������.
//	
//		Render�� �������� �����Ƿ� �̺κп� ���ԵǴ� ����  �������� ���־��
//		�Ѵ�.
//		
//=========================================================
//
//		// Shadow Map Clear
//		//
//		DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice );
//
//=========================================================
//
//		// Light ����.
//		//
//		DxLightMan::GetInstance()->Render ( m_pd3dDevice );
//
//=========================================================
//
//		CDebugSet::Render();
//
//=========================================================
//
//		// DxEffGroupPlayer Render.
//		//
//		DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
//
//=========================================================
//
//----------------------------------------------------------------------------------------
#include "pch.h"
#include "shlobj.h"

#include "../SigmaCore/DebugSet.h"
#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Math/Random.h"
#include "../SigmaCore/String/charset.h"
#include "../SigmaCore/Loading/NSLoadingDirect.h"

#include "../VisualMaterialLib/Manager/DxVMManager.h"

#include "./DxResponseMan.h"
#include "./ResourceManager.h"

#include "./Common/CommonWeb.h"
#include "./Common/DxInputString.h"
#include "./Common/SubPath.h"

#include "./DxTools/D3DFont.h"
#include "./DxTools/EditMeshs.h"
#include "./DxTools/DxClubMan.h"
#include "./DxTools/DxCubeMap.h"
#include "./DxTools/DxCursor.h"
#include "./DxTools/DxDynamicVB.h"
#include "./DxTools/DxEnvironment.h"
#include "./DxTools/DxFontMan.h"
#include "./DxTools/DxFogMan.h"
#include "./DxTools/DxGlowMan.h"
#include "./DxTools/DxGrapUtils.h"
#include "./DxTools/DxInputDevice.h"
#include "./DxTools/Light/DxLightMan.h"
#include "./DxTools/DxPostProcess.h"
#include "./DxTools/DxRenderStates.h"
#include "./DxTools/DxShadowMap.h"
#include "./DxTools/DxSkyMan.h"
#include "./DxTools/DxSurfaceTex.h"
#include "./DxTools/DxViewPort.h"
#include "./DxTools/GammaControl.h"
#include "./DxTools/RenderParam.h"
#include "./DxTools/TextureManager.h"
#include "./DxTools/EncryptDataDef.h"
#include "./DxTools/NSCharAniSequence.h"
#include "./DxTools/Texture/NSTexCombine.h"
#include "./DxTools/CubeMap/NSCubeMapUtil.h"
#include "./DxTools/CubeMap/NSCubeMapCulling.h"
#include "./DxTools/Deffered/NSDeffered.h"
#include "./DxTools/Material/NSMaterialManager.h"
#include "./DxTools/Material/NSShadowRenderForMaterial.h"
#include "./DxTools/Occlusion/NSOcclusionCulling.h"
#include "./DxTools/Occlusion/NSOcclusionQueryFastRender.h"
#include "./DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "./DxTools/PostProcessing/NSLightShafts.h"
#include "./DxTools/PostProcessing/NSBloomHDR.h"
#include "./DxTools/PostProcessing/NSGlow.h"

#include "./DxEffect/NSEffectThread.h"
#include "./DxEffect/Frame/DxEffectMan.h"
#include "./DxEffect/Frame/DxEffectShadow.h"
#include "./DxEffect/DxLoadShader.h"
#include "./DxEffect/MapEdit/DxMapEditMan.h"
#include "./DxEffect/TexEff/DxTexEffMan.h"
#include "./DxEffect/TexEff/DxTextureEffMan.h"
#include "./DxEffect/Char/DxEffChar.h"
#include "./DxEffect/Char/DxEffCharData.h"
#include "./DxEffect/Char/DxEffCharHLSL.h"
#include "./DxEffect/Char/DxEffCharOverlay.h"
#include "./DxEffect/EffAni/DxEffAni.h"
#include "./DxEffect/EffKeep/DxEffKeep.h"
#include "./DxEffect/EffProj/DxEffProj.h"
#include "./DxEffect/Single/DxEffGroupPlayer.h"
#include "./DxEffect/Single/DxEffSingle.h"
#include "./DxEffect/Single/DxEffSingleMan.h"
#include "./DxEffect/Single/DxEffSinglePropGMan.h"
#include "./DxEffect/Single/DxEffGameHelp.h"
#include "./DxEffect/Single/DxEffBillboardChar.h"
#include "./DxEffect/EffectLua/NSEffectLua.h"
#include "./DxEffect/EffectLua/NSEffectLuaPlay.h"

#include "./DxLand/LightMap/NSLightMapFX.h"
#include "./DxLand/LightMap/NSLightMap.h"
#include "./DxLand/DxPiece/NSPieceInstancing.h"
#include "./DxLand/DxPiece/NSPieceQuickSort.h"
#include "./DxLand/DxPiece/DxPieceContainer.h"
#include "./DxLand/DxPiece/DxPieceEffMan.h"
#include "./DxLand/DxPiece/NsOCTree.h"
#include "./DxLand/NSLandThread.h"
#include "./DxLand/DxPrefabMan.h"
#include "./DxLand/DxLandMan.h"

#include "./Water/NSWaterMain.h"

#include "./DxSound/BgmSound.h"
#include "./DxSound/DxSoundLib.h"
#include "./DxSound/DxSoundMan.h"

#include "./G-Logic/GLogic.h"
#include "./G-Logic/GLPeriod.h"

#include "./GUInterface/Cursor.h"
#include "./GUInterface/UIMan.h"
#include "./GUInterface/UIRenderQueue.h"

#include "./DxMeshs/FrameMesh/DxReplaceContainer.h"
#include "./DxMeshs/FrameMesh/NSVERTEXNORTEX_FX.h"
#include "./DxMeshs/DxSimpleMeshMan.h"
#include "./DxMeshs/SkinMesh/NSChangeCPS.h"
#include "./DxMeshs/SkinMesh/NSBillboardLua.h"
#include "./DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "./DxMeshs/SkinMesh/DxSkeletonManager.h"
#include "./DxMeshs/SkinMesh/DxSkinAnimationManager.h"
#include "./DxMeshs/SkinMesh/DxAnimationListManager.h"
#include "./DxMeshs/SkinMesh/DxSkinChar.h"
#include "./DxMeshs/SkinMesh/DxAttBone.h"
#include "./DxMeshs/SkinMesh/DxAttBoneLink.h"
#include "./DxMeshs/SkinMesh/DxVehicle.h"
#include "./DxMeshs/SkinMesh/_new/DxSkinMesh9.h"
//#include "./DxMeshs/SkinMesh/_new/DxSkinMesh9_CPU.h"
#include "./DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"
#include "./DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"
#include "./DxMeshs/SkinMesh/_new/DxSkinMesh9_Material.h"
#include "./DxMeshs/SkinMesh/DxSkinMeshManager.h"
#include "./DxMeshs/SkinMesh/DxSkinObject.h"
#include "./DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "./DxMeshs/SkinMesh/_new/NsSMeshSceneGraph.h"

#include "./TextTexture/TextUtil.h"

#include "../MfcExLib/RanFilter.h"

#include "./Interface/ITextureManager.h"
#include "./Interface/IDxInputDevice.h"
#include "./Interface/IXFileRender.h"
#include "./Interface/ISerialFile.h"
#include "./Interface/IDebugSet.h"

#include "./PhysX/NSPhysX.h"

#include "./Freetype/FreetypeLib.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL g_bWIREFRAME_VIEW = FALSE;
TSTRING g_strGameForceExit = _T("");

static IDxInputDevice s_cInstance_InputDevice;
static ITextureManager s_cInstance_TextureManager;
static IDebugSet s_cInstance_DebugSet;

DxResponseMan::DxResponseMan(EMSERVICE_PROVIDER emServiceProvider, SUBPATH* pPath)
: EngineDeviceMan(emServiceProvider)
, m_pPath(pPath)
, m_pWndApp(NULL)
, m_pd3dDevice(NULL)
//, m_pD3D(NULL)
, m_bGameMode( false )
{
	memset(m_szFullPath, 0, sizeof(char) * (MAX_PATH));
	m_pInputString = new DXInputString(emServiceProvider);
}

DxResponseMan::~DxResponseMan(void)
{
	SAFE_DELETE(m_pInputString);
	m_pd3dDevice = NULL;
}

HRESULT DxResponseMan::OneTimeSceneInit(
										const char* szAppPath,
										CWnd* pWndApp,
										const CString& strFontSys,
										bool bGameMode,
										language::LANGFLAG dwLang,
										const CString& strFontName)
{
	GASSERT(pWndApp);
	GASSERT(szAppPath);

	m_pWndApp = pWndApp;
	StringCchCopy( m_szFullPath, MAX_PATH, szAppPath );

	m_bGameMode = bGameMode;

	TCHAR szFullPath[MAX_PATH] = {0};

	// ������ ���� �ʱ�ȭ.
	sc::Random::getInstance();

	// �������� ���� ����.
	m_pPath->AppRootSet(std::string(szAppPath));
	//StringCchCopy( SUBPATH::APP_ROOT, MAX_PATH, szAppPath );

	// ����� ���� ������ �����Ǿ�� �ϴ� ���� ����.
	TCHAR szPROFILE[MAX_PATH] = {0};

	SHGetSpecialFolderPath( NULL, szPROFILE, CSIDL_PERSONAL, FALSE );

	StringCchCopy(szFullPath, MAX_PATH, szPROFILE);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->SaveRoot().c_str()); //SUBPATH::SAVE_ROOT);
	CreateDirectory(szFullPath, NULL);

	StringCchCopy  (szFullPath, MAX_PATH, szPROFILE);
	StringCchCat   (szFullPath, MAX_PATH, m_pPath->DebugInfoRoot().c_str()); //SUBPATH::DEBUGINFO_ROOT);
	CreateDirectory(szFullPath, NULL);

	StringCchCopy  (szFullPath, MAX_PATH, szPROFILE);
	StringCchCat   (szFullPath, MAX_PATH, m_pPath->PlayInfoRoot().c_str()); //SUBPATH::PLAYINFO_ROOT);
	CreateDirectory(szFullPath, NULL);

	StringCchCopy  (szFullPath, MAX_PATH, szPROFILE);
	StringCchCat   (szFullPath, MAX_PATH, m_pPath->Capture().c_str()); //SUBPATH::CAPTURE);
	CreateDirectory(szFullPath, NULL);

	StringCchCopy( szFullPath, MAX_PATH, szPROFILE );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->UiInfoRoot().c_str()); //SUBPATH::UIINFO_ROOT);
	CreateDirectory ( szFullPath, NULL );

	// ��ũ�� ĸ�� ���丮(���׸�)
	//
	StringCchCopy  (szFullPath, MAX_PATH, szPROFILE);
	StringCchCat   (szFullPath, MAX_PATH, m_pPath->Capture().c_str()); //SUBPATH::CAPTURE);
	DxGrapUtils::SetPath ( szFullPath );

	// �⺻ ���� ���� ����
	StringCchCopy  (szFullPath, MAX_PATH, szAppPath);
	StringCchCat   (szFullPath, MAX_PATH, m_pPath->Cache().c_str()); //SUBPATH::CACHE );
	CreateDirectory(szFullPath, NULL);

	// ����׻��� �ʱ�ȭ
#ifdef _RELEASED
	CDebugSet::OneTimeSceneInit( m_pPath, true );
	sc::DebugSet::GetInstance()->OneTimeSceneInit( szAppPath, true );
#else
	CDebugSet::OneTimeSceneInit( m_pPath, false );
	sc::DebugSet::GetInstance()->OneTimeSceneInit( szAppPath, false );
#endif

	// Ű���� ���콺
	DxInputDevice::GetInstance().OneTimeSceneInit(m_pWndApp->m_hWnd);

	// cursor �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileEdit().c_str()); //SUBPATH::OBJ_FILE_EDIT );
	CCursor::GetInstance().OneTimeSceneInit(szFullPath, m_pWndApp->m_hWnd);

	// ��Ʈ �޴��� �ʱ�ȭ.
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->Cache().c_str()); //SUBPATH::CACHE );
	DxFontMan::GetInstance().SetPath ( szFullPath );
	DxFontMan::GetInstance().SetFontSys ( strFontSys, dwLang );

#ifdef DEF_FREETYPE
	FreetypeLib::InitFreetype(dwLang);
#endif

	CHARSET::SetCodePage ( CD3DFontPar::nCodePage[dwLang] );
	CRanFilter::GetInstance().SetCodePage( CD3DFontPar::nCodePage[dwLang] );

	//StringCchCopy( _DEFAULT_FONT, MAX_PATH, strFontName.GetString() );
	_DEFAULT_FONT = strFontName.GetString();
	switch ( dwLang )
	{
	case language::DEFAULT:
		_DEFAULT_FONT_FLAG = NULL;
		break;

	case language::TRADITIONAL_CHINESE:
		_DEFAULT_FONT_FLAG = D3DFONT_TCBIGFIVE;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_TCBIGFIVE;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_TCBIGFIVE;
		break;

	case language::JAPANESE:
		_DEFAULT_FONT_FLAG = D3DFONT_SHIFTJIS;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_SHIFTJIS;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_SHIFTJIS;
		break;

	case language::KOREAN:
		_DEFAULT_FONT_FLAG = NULL;
		break;

	case language::SIMPLIFIED_CHINESE:
		_DEFAULT_FONT_FLAG = D3DFONT_SCGB;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_SCGB;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_SCGB;
		break;

	case language::THAILAND:
		_DEFAULT_FONT_FLAG = D3DFONT_THAI;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_THAI;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_THAI;
		break;

	case language::VIETNAM:
		_DEFAULT_FONT_FLAG = D3DFONT_VIETNAM;
		_DEFAULT_FONT_SHADOW_FLAG = D3DFONT_SHADOW | D3DFONT_VIETNAM;
		_DEFAULT_FONT_SHADOW_EX_FLAG = D3DFONT_SHADOW_EX | D3DFONT_VIETNAM;
		break;
	};

	// �ؽ��� �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->TextureFileRoot().c_str());
	TextureManager::GetInstance().SetPath( szFullPath );

	// Char Billboard (Texture)
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileBillboard().c_str() );
	NSBillboard::SetTexturePath( szFullPath );

	// Char Billboard (Object)
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->ObjFileBillboard().c_str() );
	NSBillboardLua::SetPath( szFullPath );

	// NSChangeCPS
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->ObjFileSkinObject().c_str() );
	NSChangeCPS::SetPath( szFullPath );

	// ShaderMan �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ShaderFileRoot().c_str()); //SUBPATH::SHADER_FILE_ROOT );
	DXShaderMan::GetInstance().SetPath ( szFullPath );
	NSLightShafts::SetPath( szFullPath );
	NSGlow::SetPath( szFullPath );

	// ReplaceContainer �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE );
	DxReplaceContainer::GetInstance().OneTimeSceneInit(szFullPath);

	// DxSkeletonManager �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileSkeleton().c_str()); //SUBPATH::OBJ_FILE_SKELETON );
	DxSkeletonManager::GetInstance().SetPath( szFullPath );

	// Animation �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileAnimation().c_str()); //SUBPATH::OBJ_FILE_ANIMATION );
	DxSkinAnimationManager::GetInstance().SetPath( szFullPath );
	DxAnimationListManager::GetInstance().SetPath( szFullPath );

	// SkinObject �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileSkinObject().c_str()); //SUBPATH::OBJ_FILE_SKINOBJECT );
	DxSkinObject::SetPath ( szFullPath );
	DxSkinCharDataContainer::GetInstance().SetPath ( szFullPath );
	DxSkinPieceContainer::GetInstance().SetPath ( szFullPath );
	DxAttBoneDataContainer::GetInstance().SetPath ( szFullPath );
	DxVehicleDataContainer::GetInstance().SetPath( szFullPath );
	DxAttBoneLinkContainer::GetInstance().SetPath( szFullPath );

	DxAttBoneLink::SetPath ( szFullPath );

	// Note : SimpleMesh �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileObject().c_str()); //SUBPATH::OBJ_FILE_OBJECT );

	DxSimpleMeshMan::GetInstance().SetPath ( szFullPath );

	// Note : DxPrefabMan �⺻ ���� ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFilePrefab().c_str()); //SUBPATH::OBJ_FILE_OBJECT );

	DxPrefabMan::SetPath( szFullPath );

	// SkinMesh
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->ObjFileSkin().c_str()); //SUBPATH::OBJ_FILE_SKIN );
	DxSkinMeshManager::GetInstance().SetPath( szFullPath );

	// ShaderMan �⺻ ���� ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->EffFileSingle().c_str()); //SUBPATH::EFF_FILE_SINGLE );
	DxEffSinglePropGMan::GetInstance().OneTimeSceneInit ( szFullPath );

	// DxLandMan
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->MapFile().c_str()); //SUBPATH::MAP_FILE );
	DxLandMan::SetPath ( szFullPath );

	// ��Ʈ�� ��ǲ �ν��Ͻ�.
	m_pInputString->Create(pWndApp, CRect(0,0,0,0));

	// GUI Directory
	//
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->GuiFileRoot().c_str()); // SUBPATH::GUI_FILE_ROOT );
	CUIMan::SetPath( szFullPath );

	// ������ ���� ��� ã��.
	//
	TCHAR szPersonal[MAX_PATH] = {0};
	SHGetSpecialFolderPath( NULL, szPersonal, CSIDL_PERSONAL, FALSE);
	StringCchCat( szPersonal, MAX_PATH, _T("\\") );

	// Ŭ�� ��ũ ���丮(������)
	//
	DxClubMan::GetInstance().SetRegistPath ( szPersonal );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->GLogicPath().c_str()); //SUBPATH::GLOGIC_FILE );
	GLOGIC::SetPath ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->GLogicServerPath().c_str()); // SUBPATH::GLOGIC_SERVER_FILE );
	GLOGIC::SetServerPath ( szFullPath );

	// ���� ���� �Ŵ��� �ʱ�ȭ
	//	
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->SoundFileRoot().c_str()); //SUBPATH::SOUND_FILE_ROOT );
	DxSoundMan::GetInstance().OneTimeSceneInit ( pWndApp->m_hWnd, szFullPath );

	// BGM ���� �ʱ�ȭ
	// ���� ī�尡 ���ٸ� DxSoundMan::GetInstance().GetSoundManager() �� FALSE �� ���´�.
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->BgmFileRoot().c_str()); //SUBPATH::BGM_FILE_ROOT );
	DxBgmSound::GetInstance().SetPath ( szFullPath );
	DxBgmSound::GetInstance().Run ( pWndApp->m_hWnd, DxSoundMan::GetInstance().GetSoundManager() ? TRUE : FALSE );

	// Char Skin Effect Directory
	StringCchCopy( szFullPath, MAX_PATH,  szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->EffFileChar().c_str()); //SUBPATH::EFF_FILE_CHAR );
	DxEffcharDataMan::GetInstance().SetPath ( szFullPath );

	// ���� �׸��� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileShadow().c_str()); //SUBPATH::TEXTURE_FILE_SHADOW );
	DxEffectShadow::SetPath ( szFullPath );

	// Ŭ�� �����͵��� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileClub().c_str()); //SUBPATH::TEXTURE_FILE_CLUB );
	DxClubMan::SetPath ( szFullPath );

	// Note : Ÿ�� �����Ͱ� ����� ��ġ ����
	StringCchCopy(szFullPath, MAX_PATH, szAppPath);
	StringCchCat (szFullPath, MAX_PATH, m_pPath->TileFileData().c_str()); //SUBPATH::TILE_FILE_DATA );
	DxMapEditMan::SetPathData(szFullPath);

	// Note : Ÿ�� �ؽ��İ� ����� ��ġ ����
	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->TileFileTexture().c_str()); //SUBPATH::TILE_FILE_TEXTURE );
	DxMapEditMan::SetPathTex ( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->ObjFilePiece().c_str()); //SUBPATH::OBJ_FILE_PIECE );
	DxPieceContainer::SetPath( szFullPath );

	StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	StringCchCat( szFullPath, MAX_PATH, m_pPath->ObjFilePieceEdit().c_str()); //SUBPATH::OBJ_FILE_PIECE );
	DxPieceContainer::GetInstance().SetPieceEditPath( szFullPath );
	DxPieceContainer::GetInstancePSF().SetPieceEditPath( szFullPath );

	// Note : DxVisualMaterial    
	//::StringCchCopy(SUBPATH::FULL_PATH_VISUAL_MATERIAL, MAX_PATH, szAppPath);
	std::string FullPathVisualMaterial(szAppPath);
	//::StringCchCat (SUBPATH::FULL_PATH_VISUAL_MATERIAL, MAX_PATH, m_pPath->ObjFileVisualMaterial().c_str()); //SUBPATH::OBJ_FILE_VISUAL_MATERIAL );
	FullPathVisualMaterial.append(m_pPath->ObjFileVisualMaterial());
	m_pPath->FullPathVisualMaterialSet(FullPathVisualMaterial);
	NS_VM_MANAGER::g_strFullPath = m_pPath->FullPathVisualMaterial(); //SUBPATH::FULL_PATH_VISUAL_MATERIAL;

	// Note : LightMap
	::StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	::StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileShadow().c_str()); //SUBPATH::TEXTURE_FILE_SHADOW );
	NSLightMap::g_strPath = szFullPath;

	// Note : Material Manager
	::StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	::StringCchCat( szFullPath, MAX_PATH, m_pPath->MaterialsFileRoot().c_str());
	NSMaterialManager::SetPath( szFullPath );
	NSMaterialSkinManager::SetPath( szFullPath );

	// Note : NSSequenceTGA && NSSaveTexRT
	::StringCchCopy( szFullPath, MAX_PATH, szAppPath );
	::StringCchCat( szFullPath, MAX_PATH, m_pPath->TextureFileShadow().c_str() );
	NSCharAniSequence::g_strPath = szFullPath;

	// Interface Instance
	ic::DxInputDeviceInterface::SetInstance( &s_cInstance_InputDevice );
	ic::TextureManagerInterface::SetInstance( &s_cInstance_TextureManager );
	ic::DebugSetInterface::SetInstance( &s_cInstance_DebugSet );
	ic::IDxSound::SetInstance( DxSoundLib::GetInstance() );
	ic::g_funcXFileRender = &IXFileRender_Func;
	ic::g_funcSerialFile = &ISerialFile_Func;

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->OneTimeSceneInit();

	// Note : 
	NSOCTREE::OneTimeSceneInit();

	// Note : CMemPool ����.
	DxEffSingleMan::GetInstance().OneTimeSceneInit();

	// Note : Thread
	NSLandThread::StartThread();
	NSEffectThread::StartThread();
	NSSkinAniThread::StartThread();

	// ���� ��Ʈ�� ���.
	GammaControl::GetInstance().BackUp( m_pWndApp->m_hWnd );

	//	�׷��� ���� ��ȣȭ Init()
	ENCRYPT_DATA::InitKey();

	return S_OK;
}

HRESULT DxResponseMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bCAPTUREMOUSE )
{
	m_pd3dDevice = pd3dDevice;

	RENDERPARAM::OnCreateDevice( pd3dDevice );
	DxSkeletonManager::GetInstance().SetDevice( pd3dDevice );    
	DxSkinMeshManager::GetInstance().SetDevice( pd3dDevice );
	DxAnimationListManager::GetInstance().SetDevice(pd3dDevice);
	DxSkinAnimationManager::GetInstance().SetDevice(pd3dDevice);

	LPDIRECT3DQ			pD3D(NULL);
	m_pd3dDevice->GetDirect3D ( &pD3D );
	m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );

	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	pBackBuffer->Release();

	DxViewPort::GetInstance().InitDeviceObjects ( m_pd3dDevice, m_pWndApp->m_hWnd );

	TextureManager::GetInstance().SetDevice( pd3dDevice );

	// TAG_StartResourceThreadManager_1
	ResourceThreadManager::GetInstance().Start();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	if( m_bGameMode )
	{
		// ����� ���� PC������ �ؽ��� ũ�Ⱑ ū���� ������
		// ������ �Ұ����� ������ �������� ��������
		// �������� ���δ� �ϴ� PS �������� ����
		// ���߿� �ʿ��ϸ� ������ ���� �� �ָ��
		//
		if( m_d3dCaps.PixelShaderVersion < D3DPS_VERSION( 1, 4 ) )
			TextureManager::GetInstance().SetForceLessTexture( true );
	}

	// 
	DxSurfaceTex::GetInstance().InitDeviceObjects ( pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// �⺻ �����̽� 

	// Note : ���� Param
	//			DxSurfaceTex::GetInstance().InitDeviceObjects �Ŀ� �۾��ȴ�.
	RENDERPARAM::LOAD( pD3D, m_pd3dDevice, m_szFullPath );

	// ShaderMan �⺻ ����.
	//
	DXShaderMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// Ű���� ���콺.
	//
	DxInputDevice::GetInstance().InitDeviceObjects ( m_pd3dDevice, FALSE, bCAPTUREMOUSE );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	//// ��Ʈ �޴��� �ʱ�ȭ.
	////
	//DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	//CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	//CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

	////	Note	:	����׼� ��� ���� �� �ʱ�ȭ
	//CDebugSet::InitDeviceObjects ( pD3dFont9 );

	EDITMESHS::InitDeviceObjects( m_pd3dDevice );

	// �ؽ�Ʈ �ؽ���
	SIZE sizeTexrure = { m_d3dCaps.MaxTextureWidth, m_d3dCaps.MaxTextureHeight };
	CTextUtil::Get()->InitDeviceObjects( m_pd3dDevice, &sizeTexrure );

	// UI ����ť
	CUIRenderQueue::Get()->InitDeviceObjects( m_pd3dDevice );

	COMMON_WEB::InitDeviceObjects( m_pd3dDevice );

	DxCursor::GetInstance().OnCreateDevice( m_pd3dDevice, m_pWndApp->m_hWnd );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// ġȯ ���� �����̳� �ʱ�ȭ.
	NSCHARSG::OnCreateDevice( m_pd3dDevice );
	DxSkinMesh9::StaticCreate ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticCreate ( m_pd3dDevice );
	DxSkinMesh9_NORMAL::StaticCreate ( m_pd3dDevice );
	DxSkinMesh9_Material::StaticCreate ( m_pd3dDevice );
	DxReplaceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSkinPieceContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxAttBoneDataContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxAttBoneLinkContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxVehicleDataContainer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	//DxSkinMeshManager::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxSimpleMeshMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// ȿ�� ��ü.
	DxEffectMan::GetInstance().InitDeviceObjects ( pD3D, m_pd3dDevice, m_d3dsdBackBuffer );
	DxEffSinglePropGMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffGroupPlayer::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffGameHelp::GetInstance().InitDeviceObjects( m_pd3dDevice );
	DxEffBillboardChar::GetInstance().InitDeviceObjects( m_pd3dDevice );
	DxEffCharMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffAniMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffcharDataMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	DxEffKeepMan::GetInstance().InitDeviceObjects( m_pd3dDevice );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxEffProjMan::GetInstance().OnCreateDevice( m_pd3dDevice );	

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxWeatherMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );							// ���� �޴��� �ʱ�ȭ.
	DxShadowMap::GetInstance().InitDeviceObjects ( pD3D, m_pd3dDevice, m_d3dsdBackBuffer );	// ������ �� �ʱ�ȭ
	DxEnvironment::GetInstance().InitDeviceObjects ( pD3D, m_pd3dDevice, m_d3dsdBackBuffer );
	NSLightShafts::OnCreateDevice( pd3dDevice );												// NSLightShafts
	NSGlow::OnCreateDevice( pd3dDevice );                                                       // NSGlow
	NSWaterMain::InitDeviceObjects( m_pd3dDevice );												// NSWaterMain
	NSWaterMainPSF::InitDeviceObjects( m_pd3dDevice );											// NSWaterMain

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// �ݻ� �� ����
	DxCubeMap::GetInstance().InitDeviceObjects ( pD3D, m_pd3dDevice, m_d3dsdBackBuffer );		// ť��� �޴��� �ʱ�ȭ.
	DxSkyMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );									// �ϴ� �޴��� �ʱ�ȭ.
	DxClubMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );								// Ŭ�� �޴��� �ʱ�ȭ

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	NSLightMapFX::LoadFX( m_pd3dDevice );                                                       // LightMapFX �ʱ�ȭ
	NSVERTEXNORTEX_FX::LoadFX( m_pd3dDevice );                                                  // NSVERTEXNORTEX_FX �ʱ�ȭ
	NSOcclusionQueryFastRender::LoadFX( m_pd3dDevice );
	NSShadowRenderForMaterial::LoadFX( m_pd3dDevice );

	NSBillboardLua::LuaLoad();

	DxEffCharOverlay::GetInstance().InitDeviceObjects( m_pd3dDevice );							// �������� �ʱ�ȭ.

	NSPhysX::InitNx_AvoidUnknownError( m_pd3dDevice, m_pPath->SaveRoot().c_str() );
	NSMaterialManager::Init( m_pd3dDevice );
	NSMaterialSkinManager::Init( m_pd3dDevice );

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	// ������ D3D�� ī��Ʈ�� ���ҽ�Ŵ.
	//
	SAFE_RELEASE( pD3D );

	return S_OK;
}

HRESULT DxResponseMan::SetRenderState ()
{
	HRESULT hr = S_OK;

	// Setup a material
	//
	D3DMATERIALQ mtrl;
	D3DUtil_InitMaterial ( mtrl, 1.0f, 1.0f, 1.0f, 1.0f );
	m_pd3dDevice->SetMaterial( &mtrl );



	RENDERPARAM::SetTextureFiltering( RENDERPARAM::g_emTextureFiltering );


	// Set up the textures

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_COLORARG2, D3DTA_CURRENT );

	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 3, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 4, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 5, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 5, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 6, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 6, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 7, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
	m_pd3dDevice->SetSamplerState( 7, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );


	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 4, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 5, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 6, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 7, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
	m_pd3dDevice->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 2 );
	m_pd3dDevice->SetTextureStageState( 3, D3DTSS_TEXCOORDINDEX, 3 );
	m_pd3dDevice->SetTextureStageState( 4, D3DTSS_TEXCOORDINDEX, 4 );
	m_pd3dDevice->SetTextureStageState( 5, D3DTSS_TEXCOORDINDEX, 5 );
	m_pd3dDevice->SetTextureStageState( 6, D3DTSS_TEXCOORDINDEX, 6 );
	m_pd3dDevice->SetTextureStageState( 7, D3DTSS_TEXCOORDINDEX, 7 );

	// Set miscellaneous render states
	float fBias = -0.0000f;
	m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE,		FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,			TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,				D3DCMP_LESSEQUAL );
	m_pd3dDevice->SetRenderState( D3DRS_DEPTHBIAS,				*((DWORD*)&fBias) );
	m_pd3dDevice->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS,	*((DWORD*)&fBias) );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,			TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,		TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x40 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATEREQUAL );

	float fFogStart=1720.0f, fFogEnd=1790.0f, fFongDensity=0.0f;
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0x3F,0xB0,0xFF);
	m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,		colorClear);
	m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,		*((DWORD *)(&fFogStart)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,		*((DWORD *)(&fFogEnd)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGDENSITY,	*((DWORD *)(&fFongDensity)) );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGTABLEMODE,	D3DFOG_NONE );

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGRANGE )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_RANGEFOGENABLE,	TRUE );
	}


	// Set the world matrix
	//
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity( &matIdentity );
	m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	return S_OK;
}

HRESULT DxResponseMan::RestoreDeviceObjects ()
{
	HRESULT hr = S_OK;
	if ( !m_pd3dDevice )	return S_FALSE;

	NSEffectLua::LuaLoad();
	NSChangeCPS::LuaLoad();

	DxRenderStates::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	// ���� ���� �Ӽ� ����.
	//	
	SetRenderState ();

	// Set up lighting states
	//
	DxLightMan::GetInstance()->InitDeviceObjects ( m_pd3dDevice );

	// ��Ʈ �޴��� �ʱ�ȭ.
	//
	DxFontMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->RestoreDeviceObjects( m_pd3dDevice );

	// UI ����ť
	CUIRenderQueue::Get()->RestoreDeviceObjects( m_pd3dDevice );

	DxCursor::GetInstance().OnResetDevice( m_pd3dDevice );

	// ĳ���ͺ��ٴ� ���ʿ� �־�� �Ѵ�.
	NSTexCombine::OnResetDevice( m_pd3dDevice );
	SMATERIAL_PIECE::OnResetDevice_STATIC ( m_pd3dDevice );
	NSMaterialManager::OnResetDevice( m_pd3dDevice );
	NSMaterialSkinManager::OnResetDevice( m_pd3dDevice );

	DxPostProcess::GetInstance().RestoreDeviceObjects(m_pd3dDevice);		// ��ó��
	DxShadowMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// ������ ��
	DxEnvironment::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// �ݻ� �� ����
	DxSurfaceTex::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );		// �⺻ �����̽� 
	DxGlowMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// �۷ο� �� 
	DxWeatherMan::GetInstance()->RestoreDeviceObjects ( m_pd3dDevice );		// ���� �޴���
	DxCubeMap::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// ť��� �޴���
	NSCubeMapUtil::NSCubeMapBlend::OnResetDevice( m_pd3dDevice );
	DxSkyMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// �ϴ� �޴��� �ʱ�ȭ.
	DxFogMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );			// ���� �ʱ�ȭ
	DxDynamicVB::RestoreDeviceObjects( m_pd3dDevice );		// DynamicVB �ʱ�ȭ
	DxPieceEffMan::GetInstance().OnResetDevice( m_pd3dDevice );				// DxPieceEffMan - OnResetDevice
	OPTMManager::GetInstance().OnResetDevice( m_pd3dDevice );				// OPTMManager - OnResetDevice
	NSLightShafts::OnResetDevice( m_pd3dDevice );							// NSLightShafts
	NSGlow::OnResetDevice( m_pd3dDevice );                                  // NSGlow
	NSWaterMain::RestoreDeviceObjects();									// NSWaterMain
	NSWaterMainPSF::RestoreDeviceObjects();									// NSWaterMain
	NSBloomHDR::OnResetDevice( m_pd3dDevice );

	// ġȯ ���� �����̳� �ʱ�ȭ.
	DxSkinMesh9::StaticResetDevice ( m_pd3dDevice );
	DxSkinMesh9_HLSL::StaticResetDevice( m_pd3dDevice );
	DxSkinMesh9_NORMAL::StaticResetDevice( m_pd3dDevice );
	DxSkinMesh9_Material::StaticResetDevice( m_pd3dDevice );
	DxReplaceContainer::GetInstance().RestoreDeviceObjects ();
	DxSkinPieceContainer::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	//DxSkinMeshManager::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	//DxSimpleMeshMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );

	// ȿ�� ��ü.
	DxEffectMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxEffSingleMan::GetInstance().RestoreDeviceObjects ( m_pd3dDevice );
	DxEffSinglePropGMan::GetInstance().RestoreDeviceObjects ();
	DxEffGroupPlayer::GetInstance().RestoreDeviceObjects ();
	DxEffGameHelp::GetInstance().RestoreDeviceObjects();
	DxEffBillboardChar::GetInstance().RestoreDeviceObjects();
	DxEffCharMan::GetInstance().RestoreDeviceObjects ();
	DxEffAniMan::GetInstance().RestoreDeviceObjects ();
	DxEffKeepMan::GetInstance().RestoreDeviceObjects();
	DxTexEffMan::GetInstance().OnCreateDevice( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnResetDevice( m_pd3dDevice );
	NSEffectPlay::OnResetDevice( m_pd3dDevice );

	// namespace
	NS_VM_MANAGER::OnResetDevice( m_pd3dDevice );
	NSLightMapFX::OnResetDevice( m_pd3dDevice );
	NSVERTEXNORTEX_FX::OnResetDevice( m_pd3dDevice );
	NSOcclusionQueryFastRender::OnResetDevice( m_pd3dDevice );
	NSShadowRenderForMaterial::OnResetDevice( m_pd3dDevice );
	NSDeffered::OnResetDevice( m_pd3dDevice );
	//NSPieceInstancing::OnResetDevice( m_pd3dDevice );

	// �ӽ� CubeMap
	NSCubeMapUtil::SetTempCubeMapName( _T("incloudscub1emap.dds") );

	return S_OK;
}

DWORD SKINDRAWCOUNT = 0;
DWORD UPDATEBONECOUNT = 0;
DWORD POLYGONCOUNT = 0;

HRESULT DxResponseMan::FrameMove ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame )
{	
	FrameMoveFirst( fTime, fElapsedTime );
	FrameMoveSecond( fTime, fElapsedTime, bDefWin, bGame );

	return S_OK;
}

HRESULT DxResponseMan::FrameMoveFirst ( float fTime, float fElapsedTime )
{
	// Effect ������Ʈ.
	//
	PROFILE_BEGIN("DxEffGroupPlayer::FrameMove");
	DxEffGroupPlayer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxEffGroupPlayer::FrameMove");

	return S_OK;
}

HRESULT DxResponseMan::FrameMoveSecond ( float fTime, float fElapsedTime, BOOL bDefWin, BOOL bGame )
{
	//	Note : �ܵ� ����Ʈ ( single effect ) ���� �ø�.
	++DxEffSingleGroup::g_nEFF_COUNT;
	if( DxEffSingleGroup::g_nEFF_COUNT > 10000 )
	{
		DxEffSingleGroup::g_nEFF_COUNT = 0;
	}

	// Note : Default 
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE );

	// Ű���� ���콺.
	//
	PROFILE_BEGIN("DxInputDevice,DXInputString::FrameMove");
	DxInputDevice::GetInstance().FrameMove ( fTime, fElapsedTime, bDefWin, bGame );
	PROFILE_END("DxInputDevice,DXInputString::FrameMove");

	//CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );
	CCursor::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note 
	DxShadowMap::GetInstance().FrameMove ( fTime, fElapsedTime );

	// Note : post effect
	//if ( DxPostEffect::GetInstance().IsRadialBlurON() )
	//DxPostEffect::GetInstance().FrameMoveRadialBlur( fTime, fElapsedTime );

	// Note : DxPostProcess
	DxPostProcess::GetInstance().FrameMove();

	// Note : Character Shader Setting
	NSCHARHLSL::FrameMove();

	// Note : Character Scene Graph
	NSCHARSG::Reset();

	// TAG_UpdateResourceManager_1
	ResourceUpdatorManager::GetInstance().Update();

	//// Note : Ŀ�� ���ư�.
	//DxCursor::GetInstance().FrameMove( fElapsedTime );

	// ġȯ ������ Ani-Time ����.
	//
	PROFILE_BEGIN("DxReplaceContainer::FrameMove");
	DxReplaceContainer::SetTime ( fTime, fElapsedTime );
	DxReplaceContainer::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxReplaceContainer::FrameMove");

	// ����׼�
	//
	PROFILE_BEGIN("CDebugSet::FrameMove");
	CDebugSet::FrameMove ( fTime, fElapsedTime );
	PROFILE_END("CDebugSet::FrameMove");

	// �� ������Ʈ.
	//
	PROFILE_BEGIN("DxLightMan::FrameMove");
	DxLightMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxLightMan::FrameMove");

	// ���� �޴��� 
	//
	PROFILE_BEGIN("DxWeatherMan::FrameMove");
	DxWeatherMan::GetInstance()->FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxWeatherMan::FrameMove");

	// ���� �÷��� ���� ����Ʈ ������Ʈ
	//
	PROFILE_BEGIN("DxEffGameHelp::FrameMove");
	DxEffGameHelp::GetInstance().FrameMove ( fTime, fElapsedTime );
	PROFILE_END("DxEffGameHelp::FrameMove");

	// ���� ĳ���� �ܼ�ȭ 
	//
	PROFILE_BEGIN("DxEffBillboardChar::FrameMove");
	DxEffBillboardChar::GetInstance().FrameMove( fTime, fElapsedTime );
	PROFILE_END("DxEffBillboardChar::FrameMove");

	// ī�޶� ���⿡ ���� �Ҹ���ȭ �ݿ�	
	//
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();
	D3DXVECTOR3	&vLookDir = DxViewPort::GetInstance().GetLookDir();

	PROFILE_BEGIN("DxSoundMan::FrameMove");
	DxSoundLib::GetInstance()->Update ();
	DxSoundMan::GetInstance().FrameMove( vLookDir, vLookatPt );	
	PROFILE_END("DxSoundMan::FrameMove");

	PROFILE_BEGIN("DxEffectMan::FrameMove");
	DxEffectMan::GetInstance().FrameMove ();
	PROFILE_END("DxEffectMan::FrameMove");

	GLPeriod::GetInstance().FrameMove ( fTime, fElapsedTime, DxLightMan::GetInstance()->IsNightAndDay() );

	DxSurfaceTex::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxEnvironment::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxSkyMan::GetInstance().FrameMove ( fTime, fElapsedTime );
	DxGlowMan::GetInstance().FrameMove( fTime, fElapsedTime );

	DxSkinPieceContainer::GetInstance().NSThreadCORE_UpdateData( m_pd3dDevice, fElapsedTime );

	OPTMManager::GetInstance().FrameMove();

	NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );

	NSEffectPlay::FrameMove( m_pd3dDevice, fElapsedTime );

	// ��ũ�� ĸ��.
	//
	DWORD dwSCROLL = DxInputDevice::GetInstance().GetKeyState(DIK_SCROLL);
	DWORD dwPRINTSCR = DxInputDevice::GetInstance().GetKeyState(DIK_SYSRQ);

	if ( dwSCROLL&DXKEY_UP || dwPRINTSCR&DXKEY_UP )
	{
		DxGrapUtils::CaptureScreen ( m_pd3dDevice );
	}

	//CDebugSet::ToView( 10, _T("SKINDRAWCOUNT : %d"), SKINDRAWCOUNT );
	//CDebugSet::ToView( 11, _T("UPDATEBONECOUNT : %d"), UPDATEBONECOUNT );
	//CDebugSet::ToView( 12, _T("VERTEXCOUNT : %d"), POLYGONCOUNT );
	//CDebugSet::ToView( 13, _T("g_dwHIGHDRAW_NUM : %d"), DxSkinChar::g_dwHIGHDRAW_NUM );

	DxSkinChar::g_dwHIGHDRAW_NUM = 0;
	SKINDRAWCOUNT = 0;
	UPDATEBONECOUNT = 0;
	POLYGONCOUNT = 0;

	return S_OK;
}

HRESULT DxResponseMan::InvalidateDeviceObjects ()
{
	if ( !m_pd3dDevice )	return S_FALSE;

	// ��Ʈ �޴��� 
	//
	DxFontMan::GetInstance().InvalidateDeviceObjects ();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->InvalidateDeviceObjects();

	// UI ����ť
	CUIRenderQueue::Get()->InvalidateDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnLostDevice();

	DxPostProcess::GetInstance().InvalidateDeviceObjects();					// ��ó��
	DxWeatherMan::GetInstance()->InvalidateDeviceObjects ( m_pd3dDevice );	// ���� �޴���
	DxShadowMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// ������ ��
	DxEnvironment::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// �ݻ� �� ����
	DxSurfaceTex::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );	// �⺻ �����̽� 
	DxGlowMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// �۷ο� �� 
	DxCubeMap::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// ť���
	NSCubeMapUtil::NSCubeMapBlend::OnLostDevice();
	DxSkyMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// �ϴ� �Ŵ���
	DxFogMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );		// ����
	DxDynamicVB::InvalidateDeviceObjects();									// DynamicVB : OnLostDevice
	DxPieceEffMan::GetInstance().OnLostDevice( m_pd3dDevice );				// DxPieceEffMan : OnLostDevice
	OPTMManager::GetInstance().OnLostDevice();								// OPTMManager - OnLostDevice
	NSLightShafts::OnLostDevice();											// NSLightShafts
	NSGlow::OnLostDevice();													// NSGlow
	NSWaterMain::InvalidateDeviceObjects();									// NSWaterMain
	NSWaterMainPSF::InvalidateDeviceObjects();								// NSWaterMain

	// ġȯ ���� �����̳� �ʱ�ȭ.
	//
	DxSkinMesh9::StaticLost();
	DxSkinMesh9_HLSL::StaticLost();
	DxSkinMesh9_NORMAL::StaticLost();
	DxSkinMesh9_Material::StaticLost();
	DxReplaceContainer::GetInstance().InvalidateDeviceObjects ();
	DxSkinPieceContainer::GetInstance().InvalidateDeviceObjects ();
	//DxSkinMeshManager::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	// ȿ�� ��ü.
	DxEffectMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSingleMan::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );
	DxEffSinglePropGMan::GetInstance().InvalidateDeviceObjects ();
	DxEffGroupPlayer::GetInstance().InvalidateDeviceObjects ();
	DxEffGameHelp::GetInstance().InvalidateDeviceObjects();
	DxEffBillboardChar::GetInstance().InvalidateDeviceObjects();
	DxEffCharMan::GetInstance().InvalidateDeviceObjects ();
	DxEffAniMan::GetInstance().InvalidateDeviceObjects ();
	DxEffKeepMan::GetInstance().InvalidateDeviceObjects();
	DxTexEffMan::GetInstance().OnReleaseDevice( m_pd3dDevice );
	DxEffProjMan::GetInstance().OnLostDevice( m_pd3dDevice );
	NSEffectPlay::OnLostDevice();

	// SimpleMesh�� ����� ��ü���� Invalidate �� ȣ���Ѵ�.
	//	DxSimpleMeshMan::GetInstance().InvalidateDeviceObjects ();

	DxRenderStates::GetInstance().InvalidateDeviceObjects ( m_pd3dDevice );

	// namespace
	NS_VM_MANAGER::OnLostDevice();
	NSLightMapFX::OnLostDevice();
	NSVERTEXNORTEX_FX::OnLostDevice();
	NSOcclusionQueryFastRender::OnLostDevice();
	NSShadowRenderForMaterial::OnLostDevice();
	NSMaterialManager::OnLostDevice();
	NSMaterialSkinManager::OnLostDevice();
	NSDeffered::OnLostDevice();
	//NSPieceInstancing::OnLostDevice();
	NSTexCombine::OnLostDevice();
	SMATERIAL_PIECE::OnLostDevice_STATIC();
	NSBloomHDR::OnLostDevice();

	return S_OK;
}

HRESULT DxResponseMan::DeleteDeviceObjects ()
{
	// Note : Thread
	NSSkinAniThread::EndThread();
	NSEffectThread::EndThread();
	NSLandThread::EndThread();

	if ( !m_pd3dDevice )
		return S_FALSE;

	DxBgmSound::GetInstance().ForceStop ();

	ResourceThreadManager::GetInstance().Stop();

	// Ű���� ���콺.
	//
	DxInputDevice::GetInstance().DeleteDeviceObjects ();

	// ��Ʈ �޴��� �ʱ�ȭ.
	//
	DxFontMan::GetInstance().DeleteDeviceObjects ();

	// �ؽ�Ʈ �ؽ���
	CTextUtil::Get()->DeleteDeviceObjects();

	// UI ����ť
	CUIRenderQueue::Get()->DeleteDeviceObjects();

	COMMON_WEB::DeleteDeviceObjects();

	// Cursor
	DxCursor::GetInstance().OnDestroyDevice();

	EDITMESHS::DeleteDeviceObjects();

	// ġȯ ���� �����̳� �ʱ�ȭ.
	NSCHARSG::OnDestroyDevice();
	DxSkinMesh9::StaticDestroy();
	DxSkinMesh9_HLSL::StaticDestroy();
	DxSkinMesh9_NORMAL::StaticDestroy();
	DxSkinMesh9_Material::StaticDestroy();
	DxReplaceContainer::GetInstance().DeleteDeviceObjects ();
	DxSkinPieceContainer::GetInstance().DeleteDeviceObjects ();
	//DxSkinMeshManager::GetInstance().DeleteDeviceObjects ();

	// ȿ�� ��ü.
	//
	DxEffectMan::GetInstance().DeleteDeviceObjects ();
	DxEffSinglePropGMan::GetInstance().DeleteDeviceObjects ();
	DxEffGroupPlayer::GetInstance().DeleteDeviceObjects ();
	DxEffGameHelp::GetInstance().DeleteDeviceObjects();
	DxEffBillboardChar::GetInstance().DeleteDeviceObjects();
	DxEffCharMan::GetInstance().DeleteDeviceObjects ();
	DxEffAniMan::GetInstance().DeleteDeviceObjects ();
	DxEffKeepMan::GetInstance().DeleteDeviceObjects();
	DxEffProjMan::GetInstance().OnDestroyDevice();
	NSEffectPlay::OnDestroyDevice();

	DxSurfaceTex::GetInstance().DeleteDeviceObjects ();		// �⺻ �����̽� 
	DxWeatherMan::GetInstance()->DeleteDeviceObjects ();	// �ݻ� �� ����
	DxShadowMap::GetInstance().DeleteDeviceObjects ();		// ������ ��
	DxEnvironment::GetInstance().DeleteDeviceObjects ();	// �ݻ� �� ����
	DxCubeMap::GetInstance().DeleteDeviceObjects ();		// ť���
	DxSkyMan::GetInstance().DeleteDeviceObjects ();			// �ϴ� �Ŵ���	
	DxClubMan::GetInstance().DeleteDeviceObjects ();		// Ŭ�� �޴���
	NSLightShafts::OnDestroyDevice();						// NSLightShafts
	NSGlow::OnDestroyDevice();								// NSGlow
	NSWaterMain::DeleteDeviceObjects( m_pd3dDevice );		// NSWaterMain
	NSWaterMainPSF::DeleteDeviceObjects( m_pd3dDevice );	// NSWaterMain

	// SimpleMesh�� ����� ��ü���� Delete �� ȣ���Ѵ�.
	//	DxSimpleMeshMan::GetInstance().DeleteDeviceObjects ();

	NSPhysX::ReleaseNx();

	// �ؽ��� ���� ��ܿ� ���� �ؾ���.
	NS_VM_MANAGER::ClearUpVMFX( TRUE );

	// �ؽ��� �޴��� ���� ( ���� �ϴܿ� �����Ͽ��� ��. )
	ResourceUpdatorManager::GetInstance().Clear();

	return S_OK;
}

HRESULT DxResponseMan::FinalCleanup ()
{
	// Note : Thread
	NSSkinAniThread::DataCleanUp();
	NSEffectThread::DataCleanUp();
	NSLandThread::DataCleanUp();

	// Note : 
	DxSkinMesh9::DestroyBoneMatrices();

	// ��Ʈ �޴��� �ʱ�ȭ.
	DxFontMan::GetInstance().DeleteDxFontAll ();

#ifdef DEF_FREETYPE
	FreetypeLib::DestroyFreetype();
#endif

	DxLightMan::GetInstance()->CleanUp ();
	DxLightMan::ReleaseInstance();

	// ���ϸ��̼� bin ���� �б�� ������ ����.
	//	WARNING : Bone ���Ϻ��� ���� ���� �����־�� �Ѵ�.
	DxSkinAnimationManager::GetInstance().Clear();

	// ġȯ ���� �����̳� �ʱ�ȭ.
	//
	DxSkinCharDataContainer::GetInstance().CleanUp ();
	DxVehicleDataContainer::GetInstance().CleanUp();
	DxAttBoneLinkContainer::GetInstance().CleanUp();
	DxAttBoneDataContainer::GetInstance().CleanUp();
	DxReplaceContainer::GetInstance().FinalCleanup ();
	DxSkinPieceContainer::GetInstance().FinalCleanup ();
	DxPieceContainer::GetInstancePSF().CleanUp();
	DxPieceContainer::GetInstance().CleanUp();
	NSPieceQuickSort::CleanUp();
	NSLightMapFX::CleanUp();
	NSLightMap::CleanUp();
	NSVERTEXNORTEX_FX::CleanUp();
	NSOcclusionQueryFastRender::CleanUp();
	NSShadowRenderForMaterial::CleanUp();

	NSWaterMain::FinalCleanUp();							// NSWaterMain
	NSWaterMainPSF::FinalCleanUp();							// NSWaterMain

	// �ؽ�Ʈ �ؽ���
	CTextUtil::CleanUp();

	// Note : CUIRenderQueue �� �ϸ� leak
	CUIRenderQueue::CleanUp();

	// �������� ����ϴ� �� ��Ʈ�� ����
	COMMON_WEB::Destroy();

	// Note : CMemPool ����.
	DxEffSingleMan::GetInstance().FinalCleanup();

	// ���� ��ü ������
	//
	DxSoundLib::GetInstance()->ReleaseInstance ();
	DxSoundMan::GetInstance().FinalCleanup ();	

	// Material Manager
	NSMaterialManager::Delete();
	NSMaterialSkinManager::Delete();

	// Note : 
	NSOCTREE::CleanUp();

	NSCubeMapCulling::CleanUp();

	NSOcclusionCulling::CleanUp();

	// ���� ��Ʈ�� ���.
	if( m_pWndApp != NULL )
		GammaControl::GetInstance().RollBack( m_pWndApp->m_hWnd );

	ResourceUpdatorManager::GetInstance().Clear();

	m_pd3dDevice = NULL;

	// ����� ���� �ʱ�ȭ
	//
	CDebugSet::FinalCleanup();
	sc::DebugSet::GetInstance()->FinalCleanup();

	// delete �� engine ���� �ϸ� �ȵȴ�.
	//sc::DebugSet::ReleaseInstance();

	//// 
	//sc::LogMan::releaseInstance();
	//sc::finalize();

	// �⺻������ �̰� �ѱ����� üũ�ϰ� �ֵ��� ����. ReleaseD_VLD �� Debug ������ ���۵�. ������ Release ������ ���۵Ǿ������� ���� �ɸ��°� �ֳ���.
	//���� �ջ� ���� ������ �ϰ� �ش� ��ġ�� �̵�
	if(_heapchk() != _HEAPOK) 
		DebugBreak();

	return S_OK;
}

void DxResponseMan::DoInterimClean_Prev()
{
	// ���̵��� DoInterimClean_Prev �Լ��� ȣ�� �ǹǷ� ���⼭ TextureQuality �� Apply �ϴ� ������ �Ѵ�.
	RENDERPARAM::ApplyTextureQuality();


	// Note : Thread
	NSSkinAniThread::DataCleanUp();
	NSEffectThread::DataCleanUp();
	NSLandThread::DataCleanUp();
}

HRESULT DxResponseMan::DoInterimClean ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//TextureManager::GetInstance().DeleteCacheDataAll();	// ������ DxSkinAniMan ���� ���� �ؾ� �Ѵ�.

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxSkinCharDataContainer::GetInstance().CleanUp ();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxAttBoneLinkContainer::GetInstance().CleanUp();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxAttBoneDataContainer::GetInstance().CleanUp();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxVehicleDataContainer::GetInstance().CleanUp();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxReplaceContainer::GetInstance().CleanUp ();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxPieceContainer::GetInstance().CleanUp ();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxSkinPieceContainer::GetInstance().CleanUp ();

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	DxEffGroupPlayer::GetInstance().RemoveAllEff ();
	DxEffGameHelp::GetInstance().RemoveAllEff();

	DxEffSinglePropGMan::GetInstance().DeleteCacheDataAll();
	DxSkeletonManager::GetInstance().DeleteCacheDataAll();
	DxAnimationListManager::GetInstance().DeleteCacheDataAll();

	//DxSimpleMeshMan::GetInstance().DeleteCacheDataAll();		// DxSimpleMeshMan �� ���̵��Ͽ��ٰ� ������������.
	// ������� �� ���� MESH_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup ), PARTICLESYS_PROPERTY::NEWOBJ ( boost::shared_ptr<DxEffSingleGroup>& spThisGroup ) ����
	// �� �ε� �ؾ� �ϴ� �۾��� �ʿ��ϴ�.
	// �׸��� Mesh �ؽ��Ĵ� Cache �ϴ� �κ��� �����Ѵ�.

	DxSkinAnimationManager::GetInstance().DeleteCacheDataAll();		// Preload �� �����ʹ� Effect�� ���� ����


	// Texture�� ���������� Effect ���ο��� ���� ����.
	//DxSkinAnimationManager::GetInstance().ReloadBackUpList();

	DxSkinMeshManager::GetInstance().DeleteCacheDataAll();

	// [shhan][2015.05.13] �ּ�ó���� �ϸ�, ���̵� �� Ư�� SKINMESH_PROPERTY ��ų ���� Crash �� �߻���.
	// �̰��� ���� ������ SKINMESH_PROPERTY::InitDeviceObjects ���� 
	// DxSkinCharData* pSkinCharData = DxSkinCharDataContainer::GetInstance().LoadData( m_szMeshFile, pd3dDevice ); �� ���� �ʰ� �ȴ�.
	// m_pSkinCharData �� ����� DxSkinCharDataContainer::GetInstance().CleanUp (); ���� ������ �����̱� ������
	// �� �ε��� �ʿ��ؼ� InitDeviceObjects �۾��� �ʿ��ϰ� ��.
	DxEffSinglePropGMan::GetInstance().InitDeviceObjects( pd3dDevice );

	DxSurfaceTex::GetInstance().MakeSurfaceBlackColor( pd3dDevice );

	// ������ �´ٸ� �̰� ���� �ؿ� �־�� �Ѵ�.
	TextureManager::GetInstance().DeleteCacheDataAll();

	NSBillboard::CleanUp();

	// [shhan][2015.02.13] �����ð� �����ϰ� Stage �� ������ ���
	//						�޸� ����ȭ �������� ���� ���� ���������� �߻��Ͽ� �޸� ���������� ������.
	sc::heapCompact();


	// [shhan][2015.06.15] ������ �޸𸮸� �����Ѵ�.
	//						���� �ٸ��� �ε��ϱ����� ���� �޸𸮸� ������ ���´�.
	HANDLE hProcess	= GetCurrentProcess();
	if (hProcess)
	{
		EmptyWorkingSet( hProcess );
	}


	return S_OK;
}

LRESULT DxResponseMan::MsgProc ( MSG* pMsg )
{
	return 0L;
}

void DxResponseMan::InitTest(HWND hWnd,LPDIRECT3DDEVICEQ pd3dDevice)
{
	LPDIRECT3DSURFACEQ pBackBuffer;
	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	pBackBuffer->Release();

	m_pInputString->Create(CWnd::FromHandle( hWnd ), CRect(0,0,0,0));
}

CD3DFontPar* DxResponseMan::LoadFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags)
{
	return DxFontMan::GetInstance().LoadDxFont(FontName, dwHeight, dwFlags);
}

CD3DFontPar* DxResponseMan::FindFont(const std::string& FontName, DWORD dwHeight, DWORD dwFlags)
{
	return DxFontMan::GetInstance().FindFont(FontName, dwHeight, dwFlags);
}