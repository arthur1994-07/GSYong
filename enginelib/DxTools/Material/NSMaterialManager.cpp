#include "pch.h"

#include <fstream>
#include <boost/tr1/memory.hpp>

#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../../../SigmaCore/String/StringFormat.h"
#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/String/StringUtil.h"
#include "../../../SigmaCore/Memory/SmartPtr.h"
#include "../../../SigmaCore/File/BaseStream.h"
#include "../../../SigmaCore/File/FileFind.h"
#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxEffect/DxLoadShader.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../G-Logic/GLogic.h"

#include "../Occlusion/NSOcclusionQueryFastRender.h"
#include "../Deffered/NSDeffered.h"
#include "../CubeMap/NSCubeMapUtil.h"
#include "../Light/NSShadowLight.h"
#include "../Light/DxShadowLight.h"
#include "../Light/DxLightMan.h"
#include "../DxBackUpRendTarget.h"
#include "../TextureManager.h"
#include "../DxSurfaceTex.h"
#include "../RENDERPARAM.h"
#include "../DxVertexFVF.h"
#include "../DxShadowMap.h"
#include "../DxViewPort.h"
#include "../DxFogMan.h"

#include "./NSVertexDeclaration.h"
#include "./MaterialFieldDef.h"

#include "NSMaterialManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL	g_bWORLD_TOOL;
extern BOOL g_bWIREFRAME_VIEW;

namespace NSMaterialManager
{
	TSTRING				g_strPath;
	sc::CFileFindTree	g_fileTree;		// mat 정보를 알아놓기 위함.

	// 특정 폴더에 있는 파일들만 검사를 하기위함.
	typedef std::tr1::unordered_map<TSTRING,std::tr1::shared_ptr<sc::CFileFindTree>>	MAP_FILE_FIND;
	typedef MAP_FILE_FIND::iterator														MAP_FILE_FIND_ITER;

	MAP_FILE_FIND	g_mapFileFind;


	typedef std::tr1::unordered_map<TSTRING,std::tr1::shared_ptr<Material>>	MAP_MATERIAL;
	typedef MAP_MATERIAL::iterator											MAP_MATERIAL_ITER;
	typedef MAP_MATERIAL::const_iterator									MAP_MATERIAL_CITER;
	typedef MAP_MATERIAL::value_type										MAP_MATERIAL_VALUE;

	// User 가 만든 FX 를 관리하는 변수
	std::map<TSTRING,ID3DXEffect*>	g_mapCustomFX;

	// System 에서 만든 FX 를 관리하는 변수
	std::vector<ID3DXEffect*>		g_vecBaseFX;

	enum
	{
		FX_RENDER_TYPE_NUM = 2,			// 0-Default, 1-CubeMap
	};

	LPDIRECT3DTEXTUREQ				g_pWhiteTex(NULL);
	LPDIRECT3DTEXTUREQ				g_pBlackTex(NULL);
	LPDIRECT3DTEXTUREQ				g_pMaskTex(NULL);

	// FX - Tool
	std::tr1::shared_ptr<Material>	g_spDefaultMaterialTool_Deffered;

	// FX - Game
	std::tr1::shared_ptr<Material>	g_spDefaultMaterial_Deffered;

	// FX - Tool
	std::tr1::shared_ptr<Material>	g_spLightMapMaterialTool;
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterialTool;
	//std::tr1::shared_ptr<Material>	g_spVoxelColorVCMaterialTool;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterialTool;
	std::tr1::shared_ptr<Material>	g_spDecalMaterialTool;

	// FX - Tool. NormalMap
	std::tr1::shared_ptr<Material>	g_spLightMapMaterialTool_NM;
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterialTool_NM;
	//std::tr1::shared_ptr<Material>	g_spVoxelColorVCMaterialTool_NM;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterialTool_NM;
	std::tr1::shared_ptr<Material>	g_spDecalMaterialTool_NM;

	// Option - 하, 중
	// FX - [Albedo] ShaderModel.2.0
	std::tr1::shared_ptr<Material>	g_spDefaultMaterial[FX_RENDER_TYPE_NUM];
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterial[FX_RENDER_TYPE_NUM];
	std::tr1::shared_ptr<Material>	g_spVoxelColorVCMaterial[FX_RENDER_TYPE_NUM];
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterial[FX_RENDER_TYPE_NUM];
	std::tr1::shared_ptr<Material>	g_spAmbientCubeVCMaterial[FX_RENDER_TYPE_NUM];
	std::tr1::shared_ptr<Material>	g_spDecalMaterial[FX_RENDER_TYPE_NUM];			// 0-Default, 1-CubeMap

	// Option - 상, 최상
	// FX - [Albedo] ShaderModel.3.0
	std::tr1::shared_ptr<Material>	g_spDefaultMaterial_PS30;
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterial_PS30;
	std::tr1::shared_ptr<Material>	g_spVoxelColorVCMaterial_PS30;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterial_PS30;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeVCMaterial_PS30;
	std::tr1::shared_ptr<Material>	g_spDecalMaterial_PS30;

	// Option - 상, 최상
	// FX - [Albedo] ShaderModel.3.0.NormalMap
	std::tr1::shared_ptr<Material>	g_spDefaultMaterialNM;
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterialNM;
	std::tr1::shared_ptr<Material>	g_spVoxelColorVCMaterialNM;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterialNM;
	std::tr1::shared_ptr<Material>	g_spAmbientCubeVCMaterialNM;
	std::tr1::shared_ptr<Material>	g_spDecalMaterialNM;

	// Option - 상
	// FX - [Shading] ShaderModel.3.0
	std::tr1::shared_ptr<Material>	g_spLightMapMaterial_PS30_Shading;				// Default, Blend(Custom)
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterial_PS30_Shading;			// VoxelColor
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterial_PS30_Shading;			// AmbientCube

	// Option - 최상
	// FX - [Shading] ShaderModel.3.0.CascadeShadow
	std::tr1::shared_ptr<Material>	g_spLightMapMaterial_PS30_CSM_Shading;			// Default, Blend(Custom)
	std::tr1::shared_ptr<Material>	g_spVoxelColorMaterial_PS30_CSM_Shading;		// VoxelColor
	std::tr1::shared_ptr<Material>	g_spAmbientCubeMaterial_PS30_CSM_Shading;		// AmbientCube

	MAP_MATERIAL					g_mapMaterial;

	void CreateMaterials( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_fileTree.CreateTree(g_strPath);

		VEC_TSTRING vecListString;
		g_fileTree.FindListInExt( vecListString, _T("mat") );

		for ( DWORD i=0; i<vecListString.size(); ++i )
		{
			std::tr1::shared_ptr<Material> spMaterial(std::tr1::shared_ptr<Material>(new Material));

			std::string strFullName = g_fileTree.FindPathName( vecListString[i].c_str() );
			spMaterial->LuaLoad( pd3dDevice, strFullName, vecListString[i], g_fileTree );

			//fun( strFullName.c_str() );

			CString strName;
			STRUTIL::ChangeExt( vecListString[i].c_str(), strName, _T("") );
			g_mapMaterial.insert( MAP_MATERIAL_VALUE( strName.GetString(), spMaterial) );
		}

		std::string strFX;

		//-----------------------------------------------------------------------------------
		// FX - Tool
		SetDefaultMaterialTool_Deffered( strFX );
		g_spDefaultMaterialTool_Deffered = std::tr1::shared_ptr<Material>(new Material);
		g_spDefaultMaterialTool_Deffered->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf, FX_TYPE_DEFAULT );

		//-----------------------------------------------------------------------------------
		// FX - Game
		SetDefaultMaterial_Deffered( strFX );
		g_spDefaultMaterial_Deffered = std::tr1::shared_ptr<Material>(new Material);
		g_spDefaultMaterial_Deffered->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_DEFAULT );

		//-----------------------------------------------------------------------------------
		// FX - Tool
		SetLightMapMaterialTool( strFX );
		g_spLightMapMaterialTool = std::tr1::shared_ptr<Material>(new Material);
		g_spLightMapMaterialTool->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_DEFAULT );

		SetVoxelColorMaterialTool( strFX );
		g_spVoxelColorMaterialTool = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterialTool->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_DEFAULT );

		//SetVoxelColorVCMaterialTool( strFX );
		//g_spVoxelColorVCMaterialTool = std::tr1::shared_ptr<Material>(new Material);
		//g_spVoxelColorVCMaterialTool->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_DEFAULT );

		SetAmbientCubeMaterialTool( strFX );
		g_spAmbientCubeMaterialTool = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterialTool->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_DEFAULT );

		SetDecalMaterialTool( strFX );
		g_spDecalMaterialTool = std::tr1::shared_ptr<Material>(new Material);
		g_spDecalMaterialTool->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_DEFAULT );

		//-----------------------------------------------------------------------------------
		// FX - Tool. NormalMap
		SetLightMapMaterialTool_NM( strFX );
		g_spLightMapMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
		g_spLightMapMaterialTool_NM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_DEFAULT );

		SetVoxelColorMaterialTool_NM( strFX );
		g_spVoxelColorMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterialTool_NM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_DEFAULT );

		//SetVoxelColorVCMaterialTool_NM( strFX );
		//g_spVoxelColorVCMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
		//g_spVoxelColorVCMaterialTool_NM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_DEFAULT );

		SetAmbientCubeMaterialTool_NM( strFX );
		g_spAmbientCubeMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterialTool_NM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_DEFAULT );

		SetDecalMaterialTool_NM( strFX );
		g_spDecalMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
		g_spDecalMaterialTool_NM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_DEFAULT );

		//-----------------------------------------------------------------------------------
		// Base
		SetDefaultFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spDefaultMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_LM );
		}

		SetVoxelColorFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spVoxelColorMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spVoxelColorMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_VC );
		}

		SetVoxelColorVertexColorFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spVoxelColorVCMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spVoxelColorVCMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_VC );
		}

		SetAmbientCubeFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spAmbientCubeMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spAmbientCubeMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_AC );
		}

		SetAmbientCubeVertexColorFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spAmbientCubeVCMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spAmbientCubeVCMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_AC );
		}

		SetDecalFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDecalMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
			g_spDecalMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf, FX_TYPE_2_0_DECAL );
		}

		//-----------------------------------------------------------------------------------
		// 3.0		
		SetDefault_PS30_FX( strFX );
		g_spDefaultMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spDefaultMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_LM );

		SetVoxelColor_PS30_FX( strFX );
		g_spVoxelColorMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_VC );

		SetVoxelColorVertexColor_PS30_FX( strFX );
		g_spVoxelColorVCMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorVCMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_VC );

		SetAmbientCube_PS30_FX( strFX );
		g_spAmbientCubeMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_AC );

		SetAmbientCubeVertexColor_PS30_FX( strFX );
		g_spAmbientCubeVCMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeVCMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_AC );

		SetDecal_PS30_FX( strFX );
		g_spDecalMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
		g_spDecalMaterial_PS30->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf0, FX_TYPE_3_0_DECAL );

		//-----------------------------------------------------------------------------------
		// // FX - [Albedo] ShaderModel.3.0.NormalMap
		SetDefaultNM_FX( strFX );
		g_spDefaultMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spDefaultMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_LM );

		SetVoxelColorNM_FX( strFX );
		g_spVoxelColorMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_VC );

		SetVoxelColorVertexColorNM_FX( strFX );
		g_spVoxelColorVCMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorVCMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_VC );

		SetAmbientCubeNM_FX( strFX );
		g_spAmbientCubeMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_AC );

		SetAmbientCubeVertexColorNM_FX( strFX );
		g_spAmbientCubeVCMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeVCMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_AC );

		SetDecalNM_FX( strFX );
		g_spDecalMaterialNM = std::tr1::shared_ptr<Material>(new Material);
		g_spDecalMaterialNM->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00, FX_TYPE_3_0_DECAL );

		//-----------------------------------------------------------------------------------
		// FX - [Shading] ShaderModel.3.0
		SetLightMapMaterial_PS30_Shading( strFX );
		g_spLightMapMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spLightMapMaterial_PS30_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		SetVoxelColorMaterial_PS30_Shading( strFX );
		g_spVoxelColorMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterial_PS30_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		SetAmbientCubeFX_PS30_Shading( strFX );
		g_spAmbientCubeMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterial_PS30_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		//-----------------------------------------------------------------------------------
		// FX - [Shading] ShaderModel.3.0 CascasdeShadow
		SetLightMapMaterial_POS_NOR_TEX2_PS30_CSM( strFX );
		g_spLightMapMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spLightMapMaterial_PS30_CSM_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		SetVoxelColorMaterial_PS30_CSM( strFX );
		g_spVoxelColorMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spVoxelColorMaterial_PS30_CSM_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		SetAmbientCubeMaterial_PS30_CSM( strFX );
		g_spAmbientCubeMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
		g_spAmbientCubeMaterial_PS30_CSM_Shading->RefreshFX_Default( pd3dDevice, strFX, 0, 0, FX_TYPE_LBUFFER );

		//-----------------------------------------------------------------------------------
		// Custom
		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->RefreshFX( pd3dDevice, 0xf );
		}
	}

	void SetPath( const TSTRING& path )
	{
		g_strPath = path;
	}

	void Init( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		CreateMaterials( pd3dDevice );
	}

	//void OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	std::string strFX;

	//	////-----------------------------------------------------------------------------------
	//	//// FX - Tool
	//	//SetDefaultMaterialTool_Deffered( strFX );
	//	//g_spDefaultMaterialTool_Deffered = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDefaultMaterialTool_Deffered->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf );

	//	////-----------------------------------------------------------------------------------
	//	//// FX - Tool
	//	//SetDefaultMaterial_Deffered( strFX );
	//	//g_spDefaultMaterial_Deffered = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDefaultMaterial_Deffered->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	////-----------------------------------------------------------------------------------
	//	//// FX - Tool
	//	//SetLightMapMaterialTool( strFX );
	//	//g_spLightMapMaterialTool = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spLightMapMaterialTool->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetVoxelColorMaterialTool( strFX );
	//	//g_spVoxelColorMaterialTool = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterialTool->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	////SetVoxelColorVCMaterialTool( strFX );
	//	////g_spVoxelColorVCMaterialTool = std::tr1::shared_ptr<Material>(new Material);
	//	////g_spVoxelColorVCMaterialTool->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetAmbientCubeMaterialTool( strFX );
	//	//g_spAmbientCubeMaterialTool = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterialTool->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetDecalMaterialTool( strFX );
	//	//g_spDecalMaterialTool = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDecalMaterialTool->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	////-----------------------------------------------------------------------------------
	//	//// FX - Tool. NormalMap
	//	//SetLightMapMaterialTool_NM( strFX );
	//	//g_spLightMapMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spLightMapMaterialTool_NM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetVoxelColorMaterialTool_NM( strFX );
	//	//g_spVoxelColorMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterialTool_NM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	////SetVoxelColorVCMaterialTool_NM( strFX );
	//	////g_spVoxelColorVCMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
	//	////g_spVoxelColorVCMaterialTool_NM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetAmbientCubeMaterialTool_NM( strFX );
	//	//g_spAmbientCubeMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterialTool_NM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetDecalMaterialTool_NM( strFX );
	//	//g_spDecalMaterialTool_NM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDecalMaterialTool_NM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	////-----------------------------------------------------------------------------------
	//	//// Base
	//	//SetDefaultFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spDefaultMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spDefaultMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	//SetVoxelColorFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spVoxelColorMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spVoxelColorMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	//SetVoxelColorVertexColorFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spVoxelColorVCMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spVoxelColorVCMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	//SetAmbientCubeFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spAmbientCubeMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spAmbientCubeMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	//SetAmbientCubeVertexColorFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spAmbientCubeVCMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spAmbientCubeVCMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	//SetDecalFX( strFX );
	//	//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
	//	//{
	//	//	g_spDecalMaterial[i] = std::tr1::shared_ptr<Material>(new Material);
	//	//	g_spDecalMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
	//	//}

	//	////-----------------------------------------------------------------------------------
	//	//// // FX - [Albedo] ShaderModel.3.0	
	//	//SetDefault_PS30_FX( strFX );
	//	//g_spDefaultMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDefaultMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetVoxelColor_PS30_FX( strFX );
	//	//g_spVoxelColorMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetVoxelColorVertexColor_PS30_FX( strFX );
	//	//g_spVoxelColorVCMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorVCMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetAmbientCube_PS30_FX( strFX );
	//	//g_spAmbientCubeMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetAmbientCubeVertexColor_PS30_FX( strFX );
	//	//g_spAmbientCubeVCMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeVCMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	//SetDecal_PS30_FX( strFX );
	//	//g_spDecalMaterial_PS30 = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDecalMaterial_PS30->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf0 );

	//	////-----------------------------------------------------------------------------------
	//	//// // FX - [Albedo] ShaderModel.3.0.NormalMap	
	//	//SetDefaultNM_FX( strFX );
	//	//g_spDefaultMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDefaultMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetVoxelColorNM_FX( strFX );
	//	//g_spVoxelColorMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetVoxelColorVertexColorNM_FX( strFX );
	//	//g_spVoxelColorVCMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorVCMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetAmbientCubeNM_FX( strFX );
	//	//g_spAmbientCubeMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetAmbientCubeVertexColorNM_FX( strFX );
	//	//g_spAmbientCubeVCMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeVCMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	//SetDecalNM_FX( strFX );
	//	//g_spDecalMaterialNM = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spDecalMaterialNM->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

	//	////-----------------------------------------------------------------------------------
	//	//// FX - [Shading] ShaderModel.3.0
	//	//SetLightMapMaterial_PS30_Shading( strFX );
	//	//g_spLightMapMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spLightMapMaterial_PS30_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	//SetVoxelColorMaterial_PS30_Shading( strFX );
	//	//g_spVoxelColorMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterial_PS30_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	//SetAmbientCubeFX_PS30_Shading( strFX );
	//	//g_spAmbientCubeMaterial_PS30_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterial_PS30_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	////-----------------------------------------------------------------------------------
	//	//// FX - [Shading] ShaderModel.3.0 CascasdeShadow
	//	//SetLightMapMaterial_POS_NOR_TEX2_PS30_CSM( strFX );
	//	//g_spLightMapMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spLightMapMaterial_PS30_CSM_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	//SetVoxelColorMaterial_PS30_CSM( strFX );
	//	//g_spVoxelColorMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spVoxelColorMaterial_PS30_CSM_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	//SetAmbientCubeMaterial_PS30_CSM( strFX );
	//	//g_spAmbientCubeMaterial_PS30_CSM_Shading = std::tr1::shared_ptr<Material>(new Material);
	//	//g_spAmbientCubeMaterial_PS30_CSM_Shading->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0 );

	//	//-----------------------------------------------------------------------------------
	//	// Custom
	//	for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
	//	{
	//		(*citer).second->OnResetDevice_Reload( pd3dDevice, TRUE, strFX, 0, 0xf );
	//	}
	//}

	//void Reload( LPDIRECT3DDEVICEQ pd3dDevice )
	//{
	//	CreateMaterials( pd3dDevice );

	//	//// 여기를 들어왔을 시점의 FX 참조카운트는 2이다.
	//	//// 그래서 OnLostDevice 가 필요하다.
	//	//// Delete 에서도 Release 한다.
	//	//OnLostDevice();
	//	//Delete();

	//	// 참조 카운트를 2개 만들어준다.
	//	//CreateMaterials( pd3dDevice );
	//	//OnResetDevice_Reload( pd3dDevice );
	//}

	void SimpleReload( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.begin();
		for ( ; iter!=g_mapCustomFX.end(); ++iter )
		{
			(*iter).second->OnLostDevice();
			SAFE_RELEASE( (*iter).second );
		}
		g_mapCustomFX.clear();

		// Lost
		//for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		//{
		//	(*citer).second->OnLostDevice();
		//}
		g_mapMaterial.clear();

		// Delete
		g_fileTree.CreateTree(g_strPath);
		g_mapFileFind.clear();

		// Create
		VEC_TSTRING vecListString;
		g_fileTree.FindListInExt( vecListString, _T("mat") );
		for ( DWORD i=0; i<vecListString.size(); ++i )
		{
			std::tr1::shared_ptr<Material> spMaterial(std::tr1::shared_ptr<Material>(new Material));

			std::string strFullName = g_fileTree.FindPathName( vecListString[i].c_str() );
			spMaterial->LuaLoad( pd3dDevice, strFullName, vecListString[i], g_fileTree );

			//fun( strFullName.c_str() );

			CString strName;
			STRUTIL::ChangeExt( vecListString[i].c_str(), strName, _T("") );
			g_mapMaterial.insert( MAP_MATERIAL_VALUE( strName.GetString(), spMaterial) );
		}

		// Reset
		std::string strFX;
		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->OnResetDevice_Reload( pd3dDevice, TRUE, strFX, 0, 0xf );
		}
	}

	void OnlyVersionUpForTool()
	{
		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spDefaultMaterialTool_Deffered->OnlyVersionUpForTool();		

		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spLightMapMaterialTool->OnlyVersionUpForTool();
		g_spVoxelColorMaterialTool->OnlyVersionUpForTool();
		//g_spVoxelColorVCMaterialTool->OnlyVersionUpForTool();
		g_spAmbientCubeMaterialTool->OnlyVersionUpForTool();
		g_spDecalMaterialTool->OnlyVersionUpForTool();

		//-----------------------------------------------------------------------------------
		// FX - Tool. NormalMap
		g_spLightMapMaterialTool_NM->OnlyVersionUpForTool();
		g_spVoxelColorMaterialTool_NM->OnlyVersionUpForTool();
		//g_spVoxelColorVCMaterialTool_NM->OnlyVersionUpForTool();
		g_spAmbientCubeMaterialTool_NM->OnlyVersionUpForTool();
		g_spDecalMaterialTool_NM->OnlyVersionUpForTool();

		//-----------------------------------------------------------------------------------
		// Base
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i]->OnlyVersionUpForTool();
			g_spVoxelColorMaterial[i]->OnlyVersionUpForTool();
			g_spVoxelColorVCMaterial[i]->OnlyVersionUpForTool();
			g_spAmbientCubeMaterial[i]->OnlyVersionUpForTool();
			g_spAmbientCubeVCMaterial[i]->OnlyVersionUpForTool();
			g_spDecalMaterial[i]->OnlyVersionUpForTool();
		}

		//-----------------------------------------------------------------------------------
		// // FX - [Albedo] ShaderModel.3.0
		g_spDefaultMaterial_PS30->OnlyVersionUpForTool();
		g_spVoxelColorMaterial_PS30->OnlyVersionUpForTool();
		g_spVoxelColorVCMaterial_PS30->OnlyVersionUpForTool();
		g_spAmbientCubeMaterial_PS30->OnlyVersionUpForTool();
		g_spAmbientCubeVCMaterial_PS30->OnlyVersionUpForTool();
		g_spDecalMaterial_PS30->OnlyVersionUpForTool();

		//-----------------------------------------------------------------------------------
		// // FX - [Albedo] ShaderModel.3.0.NormalMap
		g_spDefaultMaterialNM->OnlyVersionUpForTool();
		g_spVoxelColorMaterialNM->OnlyVersionUpForTool();
		g_spVoxelColorVCMaterialNM->OnlyVersionUpForTool();
		g_spAmbientCubeMaterialNM->OnlyVersionUpForTool();
		g_spAmbientCubeVCMaterialNM->OnlyVersionUpForTool();
		g_spDecalMaterialNM->OnlyVersionUpForTool();
	}

	void Delete()
	{
		std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.begin();
		for ( ; iter!=g_mapCustomFX.end(); ++iter )
		{
			//(*iter).second->OnLostDevice();
			SAFE_RELEASE( (*iter).second );
		}
		g_mapCustomFX.clear();

		for ( DWORD i=0; i<g_vecBaseFX.size(); ++i )
		{
			//g_vecBaseFX[i]->OnLostDevice();
			SAFE_RELEASE( g_vecBaseFX[i] );
		}
		g_vecBaseFX.clear();

		g_mapMaterial.clear();
		g_mapFileFind.clear();

		////-----------------------------------------------------------------------------------
		//// FX - Tool
		//g_spDefaultMaterialTool_Deffered.reset();

		////-----------------------------------------------------------------------------------
		//// FX - Game
		//g_spDefaultMaterial_Deffered.reset();		

		////-----------------------------------------------------------------------------------
		//// FX - Tool
		//g_spLightMapMaterialTool.reset();
		//g_spVoxelColorMaterialTool.reset();
		////g_spVoxelColorVCMaterialTool.reset();
		//g_spAmbientCubeMaterialTool.reset();
		//g_spDecalMaterialTool.reset();

		////-----------------------------------------------------------------------------------
		//// FX - Tool. NormalMap
		//g_spLightMapMaterialTool_NM.reset();
		//g_spVoxelColorMaterialTool_NM.reset();
		////g_spVoxelColorVCMaterialTool_NM.reset();
		//g_spAmbientCubeMaterialTool_NM.reset();
		//g_spDecalMaterialTool_NM.reset();

		////-----------------------------------------------------------------------------------
		//// Base
		//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		//{
		//	g_spDefaultMaterial[i].reset();
		//	g_spVoxelColorMaterial[i].reset();
		//	g_spVoxelColorVCMaterial[i].reset();
		//	g_spAmbientCubeMaterial[i].reset();
		//	g_spAmbientCubeVCMaterial[i].reset();
		//	g_spDecalMaterial[i].reset();
		//}

		////-----------------------------------------------------------------------------------
		//// // FX - [Albedo] ShaderModel.3.0
		//g_spDefaultMaterial_PS30.reset();
		//g_spVoxelColorMaterial_PS30.reset();
		//g_spVoxelColorVCMaterial_PS30.reset();
		//g_spAmbientCubeMaterial_PS30.reset();
		//g_spAmbientCubeVCMaterial_PS30.reset();
		//g_spDecalMaterial_PS30.reset();

		////-----------------------------------------------------------------------------------
		//// // FX - [Albedo] ShaderModel.3.0.NormalMap
		//g_spDefaultMaterialNM.reset();
		//g_spVoxelColorMaterialNM.reset();
		//g_spVoxelColorVCMaterialNM.reset();
		//g_spAmbientCubeMaterialNM.reset();
		//g_spAmbientCubeVCMaterialNM.reset();
		//g_spDecalMaterialNM.reset();

		////-----------------------------------------------------------------------------------
		//// FX - [Shading] ShaderModel.3.0
		//g_spLightMapMaterial_PS30_Shading.reset();
		//g_spVoxelColorMaterial_PS30_Shading.reset();
		//g_spAmbientCubeMaterial_PS30_Shading.reset();

		////-----------------------------------------------------------------------------------
		//// FX - [Shading] ShaderModel.3.0 CascasdeShadow
		//g_spLightMapMaterial_PS30_CSM_Shading.reset();
		//g_spVoxelColorMaterial_PS30_CSM_Shading.reset();
		//g_spAmbientCubeMaterial_PS30_CSM_Shading.reset();
	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.begin();
		for ( ; iter!=g_mapCustomFX.end(); ++iter )
		{
			(*iter).second->OnResetDevice();
		}

		for ( DWORD i=0; i<g_vecBaseFX.size(); ++i )
		{
			g_vecBaseFX[i]->OnResetDevice();
		}

		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spDefaultMaterialTool_Deffered->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// FX - Game
		g_spDefaultMaterial_Deffered->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spLightMapMaterialTool->ResetTechniqueHandle();
		g_spVoxelColorMaterialTool->ResetTechniqueHandle();
		//g_spVoxelColorVCMaterialTool->ResetTechniqueHandle();
		g_spAmbientCubeMaterialTool->ResetTechniqueHandle();
		g_spDecalMaterialTool->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// FX - Tool. NormalMap
		g_spLightMapMaterialTool_NM->ResetTechniqueHandle();
		g_spVoxelColorMaterialTool_NM->ResetTechniqueHandle();
		//g_spVoxelColorVCMaterialTool_NM->ResetTechniqueHandle();
		g_spAmbientCubeMaterialTool_NM->ResetTechniqueHandle();
		g_spDecalMaterialTool_NM->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// Base
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i]->ResetTechniqueHandle();
			g_spVoxelColorMaterial[i]->ResetTechniqueHandle();
			g_spVoxelColorVCMaterial[i]->ResetTechniqueHandle();
			g_spAmbientCubeMaterial[i]->ResetTechniqueHandle();
			g_spAmbientCubeVCMaterial[i]->ResetTechniqueHandle();
			g_spDecalMaterial[i]->ResetTechniqueHandle();
		}

		//-----------------------------------------------------------------------------------
		// // FX - [Albedo] ShaderModel.3.0
		g_spDefaultMaterial_PS30->ResetTechniqueHandle();
		g_spVoxelColorMaterial_PS30->ResetTechniqueHandle();
		g_spVoxelColorVCMaterial_PS30->ResetTechniqueHandle();
		g_spAmbientCubeMaterial_PS30->ResetTechniqueHandle();
		g_spAmbientCubeVCMaterial_PS30->ResetTechniqueHandle();
		g_spDecalMaterial_PS30->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// // FX - [Albedo] ShaderModel.3.0.NormalMap
		g_spDefaultMaterialNM->ResetTechniqueHandle();
		g_spVoxelColorMaterialNM->ResetTechniqueHandle();
		g_spVoxelColorVCMaterialNM->ResetTechniqueHandle();
		g_spAmbientCubeMaterialNM->ResetTechniqueHandle();
		g_spAmbientCubeVCMaterialNM->ResetTechniqueHandle();
		g_spDecalMaterialNM->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// FX - [Shading] ShaderModel.3.0
		g_spLightMapMaterial_PS30_Shading->ResetTechniqueHandle();
		g_spVoxelColorMaterial_PS30_Shading->ResetTechniqueHandle();
		g_spAmbientCubeMaterial_PS30_Shading->ResetTechniqueHandle();

		//-----------------------------------------------------------------------------------
		// FX - [Shading] ShaderModel.3.0 CascasdeShadow
		g_spLightMapMaterial_PS30_CSM_Shading->ResetTechniqueHandle();
		g_spVoxelColorMaterial_PS30_CSM_Shading->ResetTechniqueHandle();
		g_spAmbientCubeMaterial_PS30_CSM_Shading->ResetTechniqueHandle();

		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->ResetTechniqueHandle();
		}

		//	Note : White 텍스쳐 만들기
		D3DFORMAT colorFormat = D3DFMT_A8R8G8B8;
		HRESULT hr = pd3dDevice->CreateTexture ( 1, 1, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
												colorFormat, D3DPOOL_DEFAULT, &g_pWhiteTex, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ	pWhiteSuf(NULL);
			g_pWhiteTex->GetSurfaceLevel( 0, &pWhiteSuf );

			HRESULT hr(S_OK);
			hr = pd3dDevice->SetRenderTarget( 0, pWhiteSuf );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255,255,255,255), 1.0f, 0L );

			SAFE_RELEASE ( pWhiteSuf );
		}

		//	Note : Black 텍스쳐 만들기
		hr = pd3dDevice->CreateTexture ( 1, 1, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
			colorFormat, D3DPOOL_DEFAULT, &g_pBlackTex, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ	pSuf(NULL);
			g_pBlackTex->GetSurfaceLevel( 0, &pSuf );

			HRESULT hr(S_OK);
			hr = pd3dDevice->SetRenderTarget( 0, pSuf );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0,0,0), 1.0f, 0L );

			SAFE_RELEASE ( pSuf );
		}

		//	Note : Mask 텍스쳐 만들기
		hr = pd3dDevice->CreateTexture ( 1, 1, 1, D3DUSAGE_RENDERTARGET,			// 4. A8R8G8B8
			colorFormat, D3DPOOL_DEFAULT, &g_pMaskTex, NULL );
		if( FAILED(hr) )
		{
			return;
		}

		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( pd3dDevice );

			LPDIRECT3DSURFACEQ	pMaskSuf(NULL);
			g_pMaskTex->GetSurfaceLevel( 0, &pMaskSuf );

			HRESULT hr(S_OK);
			hr = pd3dDevice->SetRenderTarget( 0, pMaskSuf );
			pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(128,255,128,255), 1.0f, 0L );

			SAFE_RELEASE ( pMaskSuf );
		}
	}

	void OnLostDevice()
	{
		std::map<TSTRING,ID3DXEffect*>::iterator iter = g_mapCustomFX.begin();
		for ( ; iter!=g_mapCustomFX.end(); ++iter )
		{
			(*iter).second->OnLostDevice();
		}

		for ( DWORD i=0; i<g_vecBaseFX.size(); ++i )
		{
			g_vecBaseFX[i]->OnLostDevice();
		}

		////-----------------------------------------------------------------------------------
		//// FX - Tool
		//g_spDefaultMaterialTool_Deffered->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// FX - Game
		//g_spDefaultMaterial_Deffered->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// FX - Tool
		//g_spLightMapMaterialTool->OnLostDevice();
		//g_spVoxelColorMaterialTool->OnLostDevice();
		////g_spVoxelColorVCMaterialTool->OnLostDevice();
		//g_spAmbientCubeMaterialTool->OnLostDevice();
		//g_spDecalMaterialTool->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// FX - Tool. NormalMap
		//g_spLightMapMaterialTool_NM->OnLostDevice();
		//g_spVoxelColorMaterialTool_NM->OnLostDevice();
		////g_spVoxelColorVCMaterialTool_NM->OnLostDevice();
		//g_spAmbientCubeMaterialTool_NM->OnLostDevice();
		//g_spDecalMaterialTool_NM->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// Base
		//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		//{
		//	g_spDefaultMaterial[i]->OnLostDevice();
		//	g_spVoxelColorMaterial[i]->OnLostDevice();
		//	g_spVoxelColorVCMaterial[i]->OnLostDevice();
		//	g_spAmbientCubeMaterial[i]->OnLostDevice();
		//	g_spAmbientCubeVCMaterial[i]->OnLostDevice();
		//	g_spDecalMaterial[i]->OnLostDevice();
		//}

		////-----------------------------------------------------------------------------------
		//// // FX - [Albedo] ShaderModel.3.0
		//g_spDefaultMaterial_PS30->OnLostDevice();
		//g_spVoxelColorMaterial_PS30->OnLostDevice();
		//g_spVoxelColorVCMaterial_PS30->OnLostDevice();
		//g_spAmbientCubeMaterial_PS30->OnLostDevice();
		//g_spAmbientCubeVCMaterial_PS30->OnLostDevice();
		//g_spDecalMaterial_PS30->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// // FX - [Albedo] ShaderModel.3.0.NormalMap
		//g_spDefaultMaterialNM->OnLostDevice();
		//g_spVoxelColorMaterialNM->OnLostDevice();
		//g_spVoxelColorVCMaterialNM->OnLostDevice();
		//g_spAmbientCubeMaterialNM->OnLostDevice();
		//g_spAmbientCubeVCMaterialNM->OnLostDevice();
		//g_spDecalMaterialNM->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// FX - [Shading] ShaderModel.3.0
		//g_spLightMapMaterial_PS30_Shading->OnLostDevice();
		//g_spVoxelColorMaterial_PS30_Shading->OnLostDevice();
		//g_spAmbientCubeMaterial_PS30_Shading->OnLostDevice();

		////-----------------------------------------------------------------------------------
		//// FX - [Shading] ShaderModel.3.0 CascasdeShadow
		//g_spLightMapMaterial_PS30_CSM_Shading->OnLostDevice();
		//g_spVoxelColorMaterial_PS30_CSM_Shading->OnLostDevice();
		//g_spAmbientCubeMaterial_PS30_CSM_Shading->OnLostDevice();

		//for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		//{
		//	(*citer).second->OnLostDevice();
		//}

		SAFE_RELEASE ( g_pWhiteTex );
		SAFE_RELEASE ( g_pBlackTex );
		SAFE_RELEASE ( g_pMaskTex );
	}

	LPDIRECT3DTEXTUREQ GetWhiteTexture()
	{
		return g_pWhiteTex;
	}

	LPDIRECT3DTEXTUREQ GetBlackTexture()
	{
		return g_pBlackTex;
	}

	LPDIRECT3DTEXTUREQ GetMaskTexture()
	{
		return g_pMaskTex;
	}

	const LPD3DXEFFECT GetDefferedFX_Tool()
	{
		const Material* pMaterial = g_spDefaultMaterialTool_Deffered.get();
		if ( !pMaterial )
			return NULL;

		return pMaterial->m_sFXnType.m_rFX;
	}

	const LPD3DXEFFECT GetDefferedFX()
	{
		const Material* pMaterial = g_spDefaultMaterial_Deffered.get();
		if ( !pMaterial )
			return NULL;

		return pMaterial->m_sFXnType.m_rFX;
	}

	//EMBLEND_OPTION GetBlendType( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return EMBLEND_OPAQUE;

	//	return (*citer).second->m_emBlendOption;
	//}

	//int IsDecalRenderType( const TSTRING& strMaterialName )	// return EMDECAL_BLEND_TYPE
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return -1;

	//	return (*citer).second->m_nDecalBlendType;
	//}

	//int IsDecalOrder( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return -1;

	//	return (*citer).second->m_nDecalOrder;
	//}

	//int IsCastShadowLightMap( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return -1;

	//	return (*citer).second->m_bCastShadowLightMap ? 1 : 0;
	//}

	//BOOL IsReceiveShadowLM( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return TRUE;

	//	return (*citer).second->m_bReceiveShadowLM;
	//}

	//BOOL Is2SideMesh( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return FALSE;

	//	return (*citer).second->m_b2SideMesh;
	//}

	BOOL IsWater( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer == g_mapMaterial.end() )
			return FALSE;

		return (*citer).second->m_emSurfaceProp == EMSURFACEPROP_WATER ? TRUE : FALSE;
	}

	BOOL IsCustomSoftAlpha( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer == g_mapMaterial.end() )
			return FALSE;

		return (*citer).second->m_emSurfaceProp == EMSURFACEPROP_CUSTOM_SOFT_ALPHA ? TRUE : FALSE;
	}

	BOOL IsCustomUNLIT_UNFOG( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer == g_mapMaterial.end() )
			return FALSE;

		return (*citer).second->m_emSurfaceProp == EMSURFACEPROP_CUSTOM_UNLIT_UNFOG ? TRUE : FALSE;
	}

	int GetLightMapIndex( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer == g_mapMaterial.end() )
			return 1;

		return (*citer).second->m_nLightMapIndex;
	}

	//BOOL ExistMaterial( const TSTRING& strMaterialName )
	//{
	//	MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
	//	if ( citer == g_mapMaterial.end() )
	//		return FALSE;

	//	return TRUE;
	//}

	#define POINT_LIGHT_MAX 12		// ConeLight 없이 PointLight만 썼을 시 16개까지 가능했었다.
	#define CONE_LIGHT_MAX 3

	D3DXVECTOR4 g_vLightDiffuse;
	D3DXVECTOR4 g_vLightAmbient;
	D3DXVECTOR4 g_vLightDir;
	D3DXVECTOR4 g_vLightDirViewSpace;
	D3DXVECTOR4 g_vLightPosWorldSpace;
	D3DXVECTOR4 g_vLightPosViewSpace;
	D3DXVECTOR4 g_vCameraFrom;
	D3DXVECTOR3 g_vCameraLookatPt;
	D3DXVECTOR4 g_vPos_Range_PL_TOOL[ POINT_LIGHT_MAX ];
	D3DXVECTOR4 g_vDiff_OverLighting_PL_TOOL[ POINT_LIGHT_MAX ];
	D3DXVECTOR4 g_vAtt_PL_TOOL[ POINT_LIGHT_MAX ];
	D3DXVECTOR4 g_vPos_Range_CL_TOOL[ CONE_LIGHT_MAX ];
	D3DXVECTOR4 g_vDiff_CosPhiHalf_CL_TOOL[ CONE_LIGHT_MAX ];
	D3DXVECTOR4 g_vAtt_CosHalfThetaSubPhi_CL_TOOL[ CONE_LIGHT_MAX ];
	D3DXVECTOR4 g_vDirect_Falloff_CL_TOOL[ CONE_LIGHT_MAX ];
	float		g_fAttEndLineValue_CL_TOOL[ CONE_LIGHT_MAX ];
	float		g_fOverLighting_CL_TOOL[ CONE_LIGHT_MAX ];
	D3DXVECTOR4 g_vFog;
	D3DXVECTOR4 g_vFogColor;
	D3DXVECTOR4 g_vWindowSize;
	D3DXMATRIX	g_matView;
	D3DXMATRIX	g_matProj;
	D3DXMATRIX	g_matVP;
	D3DXMATRIX	g_matInverseView;
	float		g_EditLightMapPower(3.f);
	float		g_fOverLightPowerLM(3.f);
	float		g_fOverLightPower_PS_2_0(1.f);
	float		g_fVS_1_1_ColorMulti(0.75f);	// 이전버젼 호환성을 위해 존재한다.
	float		g_fTime(0.f);

	LPDIRECT3DTEXTUREQ	g_rBackBufferTexture(NULL);
	LPDIRECT3DTEXTUREQ	g_rBackBufferTexture_2nd(NULL);
	LPDIRECT3DTEXTUREQ	g_rBackBufferTexture_3rd(NULL);
	LPDIRECT3DTEXTUREQ	g_rBackBufferTexture_4th(NULL);

	void SetWindowSize( DWORD dwWidth, DWORD dwHeight )
	{
		g_vWindowSize = D3DXVECTOR4( 0.5f/dwWidth, 0.5f/dwHeight, 1.f, 1.f );
	}

	void SetEditLightMapPower( float fPower )
	{
		g_EditLightMapPower = fPower;
	}

	float GetEditLightMapPower()
	{
		return g_EditLightMapPower;
	}

	void SetFogParam( float fNearPlane, float fFarPlane )
	{
		// Fog
		g_vFog = D3DXVECTOR4
		(
			fFarPlane, 
			fFarPlane - fNearPlane,
			1.f,
			1.f
		);
	}

	void SetOverLighting_Values( BOOL bOldMaterialMap_2_0 )
	{
		if ( bOldMaterialMap_2_0 )
		{
			g_fVS_1_1_ColorMulti = 0.5f;
			g_fOverLightPowerLM = 2.f;
		}
		else
		{
			g_fVS_1_1_ColorMulti = 0.75f;
			g_fOverLightPowerLM = 3.f;
		}
	}

	void SetTime( float fTime )
	{
		g_fTime = fTime;
	}

	void FrameMove( const D3DXMATRIX& matView, 
					const D3DXMATRIX& matProj, 
					LPDIRECT3DTEXTUREQ pBackBufferTex, 
					LPDIRECT3DTEXTUREQ pBackBufferTex_2nd, 
					LPDIRECT3DTEXTUREQ pBackBufferTex_3rd,
					LPDIRECT3DTEXTUREQ pBackBufferTex_4th )
	{
		DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
		if ( !pLight )
			return;

		DWORD dwWidth = DxSurfaceTex::GetInstance().GetWidth();
		DWORD dwHeight = DxSurfaceTex::GetInstance().GetHeight();
		g_vWindowSize = D3DXVECTOR4( 0.5f/dwWidth, 0.5f/dwHeight, 1.f, 1.f );

		const D3DLIGHT9& sLight = pLight->m_Light;

		D3DXVECTOR3 vLightDir3;

		g_vLightDiffuse = D3DXVECTOR4(sLight.Diffuse.r,sLight.Diffuse.g,sLight.Diffuse.b,1.f);
		g_vLightAmbient = D3DXVECTOR4(sLight.Ambient.r,sLight.Ambient.g,sLight.Ambient.b,1.f);
		vLightDir3.x = -sLight.Direction.x;
		vLightDir3.y = -sLight.Direction.y;
		vLightDir3.z = -sLight.Direction.z;
		D3DXVec3Normalize( &vLightDir3, &vLightDir3 );
		g_vLightDir.x = vLightDir3.x;
		g_vLightDir.y = vLightDir3.y;
		g_vLightDir.z = vLightDir3.z;
		g_vLightDir.w = 1.f;

		D3DXVec3TransformNormal( &vLightDir3, &vLightDir3, &matView );
		D3DXVec3Normalize( &vLightDir3, &vLightDir3 );
		g_vLightDirViewSpace.x = vLightDir3.x;
		g_vLightDirViewSpace.y = vLightDir3.y;
		g_vLightDirViewSpace.z = vLightDir3.z;
		g_vLightDirViewSpace.w = 1.f;

		// 위치값
		g_vLightPosWorldSpace.x = DxViewPort::GetInstance().GetLookatPt().x - sLight.Direction.x*1500.f;
		g_vLightPosWorldSpace.y = DxViewPort::GetInstance().GetLookatPt().y - sLight.Direction.y*1500.f;
		g_vLightPosWorldSpace.z = DxViewPort::GetInstance().GetLookatPt().z - sLight.Direction.z*1500.f;
		//g_vLightPosWorldSpace.x = -sLight.Direction.x*1500.f;
		//g_vLightPosWorldSpace.y = -sLight.Direction.y*1500.f;
		//g_vLightPosWorldSpace.z = -sLight.Direction.z*1500.f;
		g_vLightPosWorldSpace.w = 1.f;
		D3DXVec4Transform( &g_vLightPosViewSpace, &g_vLightPosWorldSpace, &matView );
		


		// Diffuser Color Power 적용
		{
			if ( DxLightMan::GetInstance()->IsNightAndDay() )
			{
				// 낮 밤 일 경우
				float fLM_DDPower = (DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower()-1.f)*GLPeriod::GetInstance().GetBlendFact();
				g_vLightDiffuse += g_vLightDiffuse*fLM_DDPower;
			}
			else
			{
				// 낮으로 고정
				g_vLightDiffuse += g_vLightDiffuse*(DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower()-1.f);
			}
		}

		// PointLight 설정
		{
			DWORD dwSize = NSNearPointLight::GetPointLightDataSize();
			if ( dwSize > POINT_LIGHT_MAX )
				dwSize = POINT_LIGHT_MAX;

			for ( DWORD i=0; i<dwSize && i<POINT_LIGHT_MAX; ++i )
			{
				const NSNearPointLight::PointLightData* pPointLight = NSNearPointLight::GetPointLightData(i);
				D3DXVECTOR3 vPos( pPointLight->m_vPosition.x, pPointLight->m_vPosition.y, pPointLight->m_vPosition.z );
				D3DXVec3TransformCoord( &vPos, &vPos, &matView );
				g_vPos_Range_PL_TOOL[i] = D3DXVECTOR4( vPos, pPointLight->m_fRange );
				g_vDiff_OverLighting_PL_TOOL[i] = D3DXVECTOR4( pPointLight->m_cDiffuse.r, pPointLight->m_cDiffuse.g, pPointLight->m_cDiffuse.b, pPointLight->m_fOverLighting );
				g_vAtt_PL_TOOL[i] = D3DXVECTOR4( pPointLight->m_vAttenuation.x, pPointLight->m_vAttenuation.y, pPointLight->m_vAttenuation.z, 1.f );
			}
		}

		// ConeLight 설정
		{
			DWORD dwNumber = NSShadowLight::GetLightNumber();
			if ( dwNumber > CONE_LIGHT_MAX )
				dwNumber = CONE_LIGHT_MAX;

			for ( DWORD i=0; i<dwNumber && i<CONE_LIGHT_MAX; ++i )
			{
				const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(i);
				const D3DLIGHTQ& sLight = pSpotLight->m_sLight;
				D3DXVECTOR3 vPos(pSpotLight->m_matWorld._41,pSpotLight->m_matWorld._42,pSpotLight->m_matWorld._43);
				D3DXVec3TransformCoord( &vPos, &vPos, &matView );
				g_vPos_Range_CL_TOOL[i] = D3DXVECTOR4( vPos, sLight.Range );
				g_vDiff_CosPhiHalf_CL_TOOL[i] = D3DXVECTOR4( sLight.Diffuse.r, sLight.Diffuse.g, sLight.Diffuse.b, cosf(D3DX_PI*sLight.Phi/180.f*0.5f) );
				g_vAtt_CosHalfThetaSubPhi_CL_TOOL[i] = D3DXVECTOR4( sLight.Attenuation0, sLight.Attenuation1, sLight.Attenuation2, cosf(D3DX_PI*sLight.Theta/180.f*0.5f)-cosf(D3DX_PI*sLight.Phi/180.f*0.5f) );
				D3DXVECTOR3 vDir(-pSpotLight->m_sLight.Direction.x,-pSpotLight->m_sLight.Direction.y,-pSpotLight->m_sLight.Direction.z);
				D3DXVec3TransformNormal( &vDir, &vDir, &matView );
				g_vDirect_Falloff_CL_TOOL[i] = D3DXVECTOR4(vDir, sLight.Falloff );
				g_fAttEndLineValue_CL_TOOL[i] = pSpotLight->m_fAttEndLineValue;
				g_fOverLighting_CL_TOOL[i] = pSpotLight->m_fOverLighting;
			}
		}

		// LightMap Power 설정.
		g_fOverLightPower_PS_2_0 = 1.f;
		switch( NSLightMapFX::g_emRenderType )
		{
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
			g_fOverLightPower_PS_2_0 = g_EditLightMapPower;
			break;

		default:
			break;
		};


		D3DXMatrixMultiply( &g_matVP, &matView, &matProj );
		g_matView = matView;
		g_matProj = matProj;
		D3DXMatrixInverse( &g_matInverseView, NULL, &g_matView );

		const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
		g_vCameraFrom.x = vFromPt.x;
		g_vCameraFrom.y = vFromPt.y;
		g_vCameraFrom.z = vFromPt.z;
		g_vCameraFrom.w = 1.f;

		g_vCameraLookatPt = DxViewPort::GetInstance().GetLookatPt();

		g_vFogColor.x = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff0000) >> 16) / 255;
		g_vFogColor.y = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff00) >> 8) / 255;
		g_vFogColor.z = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff)) / 255;
		g_vFogColor.w = 1.f;

		if ( pBackBufferTex )
		{
			g_rBackBufferTexture = pBackBufferTex;
			g_rBackBufferTexture_2nd = pBackBufferTex_2nd;
			g_rBackBufferTexture_3rd = pBackBufferTex_3rd;
			g_rBackBufferTexture_4th = pBackBufferTex_4th;
		}

		SetFX_CommonParameter_Reset();
	}

	void SetViewProjection( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		pd3dDevice->GetTransform( D3DTS_VIEW, &g_matView );
		pd3dDevice->GetTransform( D3DTS_PROJECTION, &g_matProj );
		D3DXMatrixMultiply( &g_matVP, &g_matView, &g_matProj );
		D3DXMatrixInverse( &g_matInverseView, NULL, &g_matView );
	}

	void SetOverLightPower_PS_2_0( float fOverLightPower )
	{
		g_fOverLightPower_PS_2_0 = fOverLightPower;
	}

	void SetFogColorForEditor( DWORD dwColor )
	{
		g_vFogColor.x = static_cast<float>((dwColor & 0xff0000) >> 16) / 255;
		g_vFogColor.y = static_cast<float>((dwColor & 0xff00) >> 8) / 255;
		g_vFogColor.z = static_cast<float>((dwColor & 0xff)) / 255;
		g_vFogColor.w = 1.f;
	}

	// Active, ActiveDeffered4Tool 공통으로 쓰이는 부분
	BOOL ActiveCommonForTool( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								RENDER_MODE emRenderMode, 
								UNLM_RENDER_MODE emUnLMRenderMode,
								MATERIAL_DATA& sMaterial_Data,
								BOOL bDecal,
								const D3DXMATRIX& matWorld, 
								float fAlpha,
								BOOL bVertexColor,
								const MaterialValue& sMaterialValue,
								BOOL bNormalMap )
	{
		sMaterial_Data.m_rMaterial->CreateData( pd3dDevice );

		LPD3DXEFFECT pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return FALSE;

		D3DXMATRIX matWV, matWVP;
		D3DXMatrixMultiply( &matWV, &matWorld, &g_matView );
		D3DXMatrixMultiply( &matWVP, &matWorld, &g_matVP );

		// NSOcclusionQueryFastRender를 할 경우 필요한 작업만 한 후 return 한다.
		if ( NSOcclusionQueryFastRender::g_bRendering )
		{
			NSOcclusionQueryFastRender::SetWVP( matWVP );

			switch ( emRenderMode )
			{
			case EMRM_TOOL:
				pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pDCRT ); 
				break;

			case EMRM_MATERIAL:
			case EMRM_TEST:
			case EMRM_TEST_TOOL:
			default:
				pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT ); 
				break;
			};

			return TRUE;
		}

		pFX->SetFloat( "g_fTime", g_fTime );
		pFX->SetFloat( "g_fCubeMapValue", sMaterialValue.m_fCubeMapValue );
		pFX->SetFloat( "g_fSpecularPower", sMaterialValue.m_fSpotSpecPower );
		pFX->SetFloat( "g_fSpecularIntensity", sMaterialValue.m_fSpotSpecMulti );

		pFX->SetMatrixTranspose( "g_matWorld",			&matWorld );
		pFX->SetMatrixTranspose( "g_matWV",				&matWV );
		pFX->SetMatrixTranspose( "g_matWVP",			&matWVP );
		pFX->SetMatrixTranspose( "g_matInverseView",	&g_matInverseView );

		pFX->SetVector( "g_vLightDiffuse",		&g_vLightDiffuse );
		pFX->SetVector( "g_vLightAmbient",		&g_vLightAmbient );
		pFX->SetVector( "g_vLightDir",			&g_vLightDir );
		pFX->SetVector( "g_vLightDirViewSpace",	&g_vLightDirViewSpace );
		pFX->SetVector( "g_vCameraFrom",		&g_vCameraFrom );

		D3DXVECTOR4 vVoxelColor(1.f,1.f,1.f,1.f);
		pFX->SetVector( "g_vVoxelColor",		&vVoxelColor );

		if ( emUnLMRenderMode == NSMaterialManager::EMUNLM_RM_VOXELCOLOR )
		{
			pFX->SetFloat( "g_fVoxel", 1.f );
		}
		else
		{
			pFX->SetFloat( "g_fVoxel", 0.f );
		}

		DWORD dwSize = NSNearPointLight::GetPointLightDataSize();
		if ( dwSize > POINT_LIGHT_MAX )
			dwSize = POINT_LIGHT_MAX;

		pFX->SetInt( "g_nPointLightNum_TOOL", dwSize );
		pFX->SetVectorArray( "g_vPos_Range_PL_TOOL", &g_vPos_Range_PL_TOOL[0], POINT_LIGHT_MAX );
		pFX->SetVectorArray( "g_vDiff_OverLighting_PL_TOOL", &g_vDiff_OverLighting_PL_TOOL[0], POINT_LIGHT_MAX );
		pFX->SetVectorArray( "g_vAtt_PL_TOOL", &g_vAtt_PL_TOOL[0], POINT_LIGHT_MAX );

		DWORD dwConeNumber = NSShadowLight::GetLightNumber();
		if ( dwConeNumber > CONE_LIGHT_MAX )
			dwConeNumber = CONE_LIGHT_MAX;

		pFX->SetInt( "g_nConeLightNum_TOOL", dwConeNumber );
		pFX->SetVectorArray( "g_vPos_Range_CL_TOOL", &g_vPos_Range_CL_TOOL[0], CONE_LIGHT_MAX );
		pFX->SetVectorArray( "g_vDiff_CosPhiHalf_CL_TOOL", &g_vDiff_CosPhiHalf_CL_TOOL[0], CONE_LIGHT_MAX );
		pFX->SetVectorArray( "g_vAtt_CosHalfThetaSubPhi_CL_TOOL", &g_vAtt_CosHalfThetaSubPhi_CL_TOOL[0], CONE_LIGHT_MAX );
		pFX->SetVectorArray( "g_vDirect_Falloff_CL_TOOL", &g_vDirect_Falloff_CL_TOOL[0], CONE_LIGHT_MAX );
		pFX->SetFloatArray( "g_fAttEndLineValue_CL_TOOL", &g_fAttEndLineValue_CL_TOOL[0], CONE_LIGHT_MAX );
		pFX->SetFloatArray( "g_fOverLighting_CL_TOOL", &g_fOverLighting_CL_TOOL[0], CONE_LIGHT_MAX );

		pFX->SetTexture( "g_BackBufferTexture", g_rBackBufferTexture );
		pFX->SetTexture( "g_BackBufferTexture_2nd", g_rBackBufferTexture_2nd );
		pFX->SetTexture( "g_BackBufferTexture_3rd", g_rBackBufferTexture_3rd );
		pFX->SetTexture( "g_BackBufferTexture_4th", g_rBackBufferTexture_4th );

		// Fog
		{
			D3DXVECTOR4 vFog;
			if ( g_bWIREFRAME_VIEW )
			{
				D3DXVECTOR4 vFog
					(
					2, 
					1,
					1.f,
					1.f
					);
				pFX->SetVector( "g_vFOG", &vFog );
			}
			else
			{
				pFX->SetVector( "g_vFOG", &g_vFog );
			}

			pFX->SetVector( "g_vFogColor", &g_vFogColor );
		}

		pFX->SetVector( "g_vWindowSize",	&g_vWindowSize );

		pFX->SetFloat( "g_fAlpha", fAlpha );

		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecConstants.size(); ++i )
		{
			pFX->SetVector( sMaterial_Data.m_rMaterial->m_vecConstants[i].m_strParameter.c_str(), &sMaterial_Data.m_rMaterial->m_vecConstants[i].m_vValue );
		}

		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecTextures.size(); ++i )
		{
			if ( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.Update() )
			{
				pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.GetTexture() );
			}
		}

		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecCubeTextures.size(); ++i )
		{
			if ( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.Update() )
			{
				pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.GetCubeTexture() );
			}
		}

		pFX->SetTexture( "g_GlobalCubeTexture", NSCubeMapUtil::NSCubeMapForPSF::GetCubeMapTexture() );

		D3DXHANDLE hTechniqueHandle(NULL);
		switch ( emRenderMode )
		{
		case EMRM_TOOL:
			sMaterial_Data.m_bGameMode = FALSE;
			pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialTOOL.m_pDCRT ); 
			hTechniqueHandle = pFX->GetTechniqueByName("tool_deffered");
			break;

		case EMRM_TEST:
			sMaterial_Data.m_bGameMode = TRUE;
			pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT ); 
			hTechniqueHandle = pFX->GetTechniqueByName("lightmaptest");
			{
				// EMRM_TEST(EMRM_MATERIAL) 의 LMRT_CHECKER 의 경우는 다른곳에서 g_vLightMapUV_Multiply 값을 셋팅한다.
				D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
				pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );
			}
			break;

		case EMRM_TEST_TOOL:
			sMaterial_Data.m_bGameMode = TRUE;
			pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT ); 
			hTechniqueHandle = pFX->GetTechniqueByName("lightmaptest");
			{
				// Piece에서 Export를 안하고 Checker 를 보기 위해 셋팅값을 가져온다.
				D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
				NSLightMapFX::GetLightMapUV_Multiply( vUV_Multiply );
				pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );

				LPDIRECT3DTEXTUREQ pTextureCombined(NULL);
				NSLightMapFX::GetLightMapCheker( pTextureCombined, vUV_Multiply );

				if ( pTextureCombined )
				{
					pFX->SetTexture( "g_LightMapDayTex", pTextureCombined );
					//pFX->SetTexture( "g_LightMapCombined", pTextureCombined );
				}
			}
			break;

		case EMRM_MATERIAL:
		default:
			sMaterial_Data.m_bGameMode = TRUE;
			pd3dDevice->SetVertexDeclaration( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pDCRT ); 

			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
			{
				hTechniqueHandle = pFX->GetTechniqueByName("runtime_2");
			}
			if ( !hTechniqueHandle && RENDERPARAM::g_emGroundQulity >= TnL_PS_1_1 )
			{
				hTechniqueHandle = pFX->GetTechniqueByName("runtime_1");
			}

			break;
		};

		if ( !hTechniqueHandle )
			return FALSE;

		pFX->SetTechnique(hTechniqueHandle);

		return TRUE;
	}

	// [shhan][2013.06.04] 이 함수가 쓰이는 경우는 현재 DxTextureEffMan 곳 밖에는 없다.
	//						이 말은 psf 모드에서만 쓰인다는 말임.
	//						PSF 쪽에서는 CubeMap 이 기본적으로 쓰인다.
	//						처음 안 보이는 이유는 fCubeMapValue 값이 Zero 이기 때문임.
	BOOL Active( LPDIRECT3DDEVICEQ pd3dDevice, 
				const TSTRING& strMaterialName, 
				RENDER_MODE emRenderMode, 
				UNLM_RENDER_MODE emUnLMRenderMode,
				MATERIAL_DATA& sMaterial_Data,
				BOOL bDecal,
				const D3DXMATRIX& matWorld, 
				float fAlpha,
				BOOL bVertexColor,
				const MaterialValue& sMaterialValue,
				BOOL bNormalMap )
	{
		if ( emRenderMode == EMRM_NULL )
			return FALSE;

		// LightMap 이 있을 경우
		if ( emUnLMRenderMode == EMUNLM_RM_LIGHTMAP )
		{
			if ( emRenderMode == EMRM_MATERIAL )
			{
				// 다 표현됨.
				switch( NSLightMapFX::g_emRenderType )
				{
				case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
				case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
				case NSLIGHTMAPDEF::LMRT_CHECKER:
					{
						// 기본 렌더링이 가능하도록 작업함.
						sMaterial_Data.m_bDefaultMaterial = TRUE;
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial[0].get();
						emRenderMode = EMRM_TEST;
					}
					break;
				}
			}
			else
			{
				// 체커만 표시됨.
				switch( NSLightMapFX::g_emRenderType )
				{
				case NSLIGHTMAPDEF::LMRT_CHECKER:
					{
						// 기본 렌더링이 가능하도록 작업함.
						//sMaterial_Data.m_bDefaultMaterial = TRUE;
						//sMaterial_Data.m_rMaterial = g_spDefaultMaterial[0].get();
						emRenderMode = EMRM_TEST_TOOL;
					}
					break;
				}
			}
		}

		// LMRT_TEXTURE_LIGHTMAP 모드일 경우는 상황에 맞게 셋팅한다.
		if ( !sMaterial_Data.m_rMaterial )
		{
			MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
			if ( citer == g_mapMaterial.end() )
			{
				// 기본 렌더링이 가능하도록 작업함.
				sMaterial_Data.m_bDefaultMaterial = TRUE;

				if ( bNormalMap )
				{
					if ( bDecal )
					{
						sMaterial_Data.m_rMaterial = g_spDecalMaterialTool_NM.get();
					}
					else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
					{
						sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialTool_NM.get();
					}
					else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
					{
						sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialTool_NM.get();
					}
					else
					{
						sMaterial_Data.m_rMaterial = g_spLightMapMaterialTool_NM.get();
					}
				}
				else
				{
					if ( bDecal )
					{
						sMaterial_Data.m_rMaterial = g_spDecalMaterialTool.get();
					}
					else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
					{
						sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialTool.get();
					}
					else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
					{
						sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialTool.get();
					}
					else
					{
						sMaterial_Data.m_rMaterial = g_spLightMapMaterialTool.get();
					}
				}
			}
			else
			{
				sMaterial_Data.m_bDefaultMaterial = FALSE;
				sMaterial_Data.m_rMaterial = (*citer).second.get();
			}
		}

		if ( !ActiveCommonForTool ( pd3dDevice, 
									strMaterialName, 
									emRenderMode, 
									emUnLMRenderMode,
									sMaterial_Data,
									bDecal,
									matWorld, 
									fAlpha,
									bVertexColor,
									sMaterialValue,
									bNormalMap ) )
		{
			return FALSE;
		}

		return TRUE;
	}

	// Tool 에서 사용되고, Deffered Render 용이다.
	BOOL ActiveDefferedForTool( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								RENDER_MODE emRenderMode, 
								UNLM_RENDER_MODE emUnLMRenderMode,
								MATERIAL_DATA& sMaterial_Data,
								BOOL bDecal,
								const D3DXMATRIX& matWorld, 
								float fAlpha,
								BOOL bVertexColor,
								const MaterialValue& sMaterialValue,
								BOOL bNormalMap )
	{
		if ( emRenderMode == EMRM_NULL )
			return FALSE;

		// LMRT_TEXTURE_LIGHTMAP 모드일 경우는 상황에 맞게 셋팅한다.
		if ( !sMaterial_Data.m_rMaterial )
		{
			MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
			if ( citer == g_mapMaterial.end() )
			{
				// 기본 렌더링이 가능하도록 작업함.
				sMaterial_Data.m_bDefaultMaterial = TRUE;

				sMaterial_Data.m_rMaterial = g_spDefaultMaterialTool_Deffered.get();
			}
			else
			{
				sMaterial_Data.m_bDefaultMaterial = FALSE;
				sMaterial_Data.m_rMaterial = (*citer).second.get();
			}
		}

		if ( !ActiveCommonForTool ( pd3dDevice, 
									strMaterialName, 
									emRenderMode, 
									emUnLMRenderMode,
									sMaterial_Data,
									bDecal,
									matWorld, 
									fAlpha,
									bVertexColor,
									sMaterialValue,
									bNormalMap ) )
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								UNLM_RENDER_MODE emUnLMRenderMode,
								MATERIAL_DATA& sMaterial_Data,
								BOOL bDecal,
								BOOL bVertexColor,
								int nRenderMode,	// nRenderMode = 0-Default, 1-CubeMap
								BOOL bNormalMap )
	{
		RENDER_MODE emRenderMode = EMRM_MATERIAL;

		// LightMap 이 있을 경우
		if ( emUnLMRenderMode == EMUNLM_RM_LIGHTMAP )
		{
			// 다 표현됨.
			switch( NSLightMapFX::g_emRenderType )
			{
			case NSLIGHTMAPDEF::LMRT_LIGHTMAP:
			case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
			case NSLIGHTMAPDEF::LMRT_CHECKER:
				{
					// 기본 렌더링이 가능하도록 작업함.
					sMaterial_Data.m_bDefaultMaterial = TRUE;
					sMaterial_Data.m_rMaterial = g_spDefaultMaterial[nRenderMode].get();
					emRenderMode = EMRM_TEST;
				}
				break;
			}
		}

		// LMRT_TEXTURE_LIGHTMAP 모드일 경우는 상황에 맞게 셋팅한다.
		if ( !sMaterial_Data.m_rMaterial )
		{
			MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
			if ( citer == g_mapMaterial.end() )
			{
				// 기본 렌더링이 가능하도록 작업함.
				sMaterial_Data.m_bDefaultMaterial = TRUE;

				if ( !sMaterial_Data.m_bGameMode )
				{
					sMaterial_Data.m_rMaterial = g_spDefaultMaterialTool_Deffered.get();

					//if ( bNormalMap )
					//{
					//	if ( bDecal )
					//	{
					//		sMaterial_Data.m_rMaterial = g_spDecalMaterialTool_NM.get();
					//	}
					//	else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
					//	{
					//		sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialTool_NM.get();
					//	}
					//	else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
					//	{
					//		if ( bVertexColor )
					//		{
					//			sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterialTool_NM.get();
					//		}
					//		else
					//		{
					//			sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialTool_NM.get();
					//		}
					//	}
					//	else
					//	{
					//		sMaterial_Data.m_rMaterial = g_spLightMapMaterialTool_NM.get();
					//	}
					//}
					//else
					//{
					//	if ( bDecal )
					//	{
					//		sMaterial_Data.m_rMaterial = g_spDecalMaterialTool.get();
					//	}
					//	else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
					//	{
					//		sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialTool.get();
					//	}
					//	else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
					//	{
					//		if ( bVertexColor )
					//		{
					//			sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterialTool.get();
					//		}
					//		else
					//		{
					//			sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialTool.get();
					//		}
					//	}
					//	else
					//	{
					//		sMaterial_Data.m_rMaterial = g_spLightMapMaterialTool.get();
					//	}
					//}
				}
				else
				{
					if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
					{
						if ( bNormalMap )
						{
							if ( bDecal )
							{
								sMaterial_Data.m_rMaterial = g_spDecalMaterialNM.get();
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeVCMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_CSM_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_CSM_Shading->m_sFXnType;
								}
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_CSM_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_CSM_Shading->m_sFXnType;
								}
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spDefaultMaterialNM.get();
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_CSM_Shading->m_sFXnType;
							}
						}
						else
						{
							if ( bDecal )
							{
								sMaterial_Data.m_rMaterial = g_spDecalMaterial_PS30.get();
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeVCMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_CSM_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_CSM_Shading->m_sFXnType;
								}
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_CSM_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_CSM_Shading->m_sFXnType;
								}
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spDefaultMaterial_PS30.get();
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_CSM_Shading->m_sFXnType;
							}
						}
					}
					else if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_NM )
					{
						if ( bNormalMap )
						{
							if ( bDecal )
							{
								sMaterial_Data.m_rMaterial = g_spDecalMaterialNM.get();
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeVCMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_Shading->m_sFXnType;
								}
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorMaterialNM.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_Shading->m_sFXnType;
								}
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spDefaultMaterialNM.get();
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_Shading->m_sFXnType;
							}
						}
						else
						{
							if ( bDecal )
							{
								sMaterial_Data.m_rMaterial = g_spDecalMaterial_PS30.get();
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeVCMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spAmbientCubeMaterial_PS30_Shading->m_sFXnType;
								}
							}
							else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
							{
								if ( bVertexColor )
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_Shading->m_sFXnType;
								}
								else
								{
									sMaterial_Data.m_rMaterial = g_spVoxelColorMaterial_PS30.get();
									sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spVoxelColorMaterial_PS30_Shading->m_sFXnType;
								}
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spDefaultMaterial_PS30.get();
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_Shading->m_sFXnType;
							}
						}
					}
					else
					{
						if ( bDecal )
						{
							sMaterial_Data.m_rMaterial = g_spDecalMaterial[nRenderMode].get();
						}
						else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
						{
							if ( bVertexColor )
							{
								sMaterial_Data.m_rMaterial = g_spAmbientCubeVCMaterial[nRenderMode].get();
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spAmbientCubeMaterial[nRenderMode].get();
							}							
						}
						else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
						{
							if ( bVertexColor )
							{
								sMaterial_Data.m_rMaterial = g_spVoxelColorVCMaterial[nRenderMode].get();
							}
							else
							{
								sMaterial_Data.m_rMaterial = g_spVoxelColorMaterial[nRenderMode].get();
							}
						}
						else
						{
							sMaterial_Data.m_rMaterial = g_spDefaultMaterial[nRenderMode].get();
						}
					}

					//if ( RENDERPARAM::g_bFORCE_TnL_PS_3_0_DF && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
					if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF )
					{
						if ( bDecal )
						{
							sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spDecalMaterial[nRenderMode]->m_sFXnType;
						}
						else if ( emUnLMRenderMode == EMUNLM_RM_AMBIENTCUBE )
						{
							if ( bVertexColor )
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spAmbientCubeVCMaterial[nRenderMode]->m_sFXnType;
							}
							else
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spAmbientCubeMaterial[nRenderMode]->m_sFXnType;
							}
						}
						else if ( emUnLMRenderMode == EMUNLM_RM_VOXELCOLOR )
						{
							if ( bVertexColor )
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spVoxelColorVCMaterial[nRenderMode]->m_sFXnType;
							}
							else
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spVoxelColorMaterial[nRenderMode]->m_sFXnType;
							}
						}
						else
						{
							sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &g_spDefaultMaterial[nRenderMode]->m_sFXnType;
						}
					}
				}
			}
			else
			{
				sMaterial_Data.m_bDefaultMaterial = FALSE;
				sMaterial_Data.m_rMaterial = (*citer).second.get();

				LPD3DXEFFECT pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
				if ( !pFX )
					return FALSE;

				if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == EMSURFACEPROP_NULL )
				{
					// dwFVF 를 파악한다.
					DWORD dwFVF(0L);
					for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
					{
						if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Stream == 0x00ff )
							break;

						if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Method == D3DDECLTYPE_UNUSED )
							break;

						if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Usage == D3DDECLUSAGE_POSITION )
						{
							dwFVF |= D3DFVF_XYZ;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Usage == D3DDECLUSAGE_NORMAL )
						{
							dwFVF |= D3DFVF_NORMAL;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Usage == D3DDECLUSAGE_COLOR )
						{
							dwFVF |= D3DFVF_DIFFUSE;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialGAME.m_pELMT[i].Usage == D3DDECLUSAGE_TEXCOORD )
						{
							dwFVF += D3DFVF_TEX1;
						}
					}

					sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = NULL;
					sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = NULL;
					if ( sMaterial_Data.m_rMaterial->m_bLightMap )
					{
						if ( dwFVF == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX2) )
						{
							if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_CSM )
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &sMaterial_Data.m_rMaterial->m_sFXnType;
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_CSM_Shading->m_sFXnType;
							}
							else if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_NM )
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &sMaterial_Data.m_rMaterial->m_sFXnType;
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = &g_spLightMapMaterial_PS30_Shading->m_sFXnType;
							}
							else
							{
								sMaterial_Data.m_rMaterial->m_rFXnType_PS30_DF = &sMaterial_Data.m_rMaterial->m_sFXnType;
								sMaterial_Data.m_rMaterial->m_rFXnType_LBuffer = NULL;
							}
						}
						else
						{
							TSTRING strName = sMaterial_Data.m_rMaterial->m_strFX;
							strName += _T(" - PixelShader.3.0 버젼이 동작되지 않습니다.");
							AfxMessageBox( strName.c_str() );
						}
					}
					else
					{
						TSTRING strName = sMaterial_Data.m_rMaterial->m_strFX;
						strName += _T(" - PixelShader.3.0 버젼이 동작되지 않습니다.");
						AfxMessageBox( strName.c_str() );
					}
				}
				else if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == EMSURFACEPROP_DECAL )
				{
				}
			}
		}

		sMaterial_Data.m_rMaterial->CreateData( pd3dDevice );

		LPD3DXEFFECT pFX = sMaterial_Data.m_rMaterial->m_sFXnType.m_rFX;
		if ( !pFX )
			return FALSE;

		return TRUE;
	}

	std::set<ID3DXEffect*>	g_setCommonFX;	// 참조로만. 새로운 것이 들어온다면 그 때 SetFX_CommonParameter 를 호출하기 위해 존재함.

	void SetFX_CommonParameter_Common( LPD3DXEFFECT pFX, FX_TYPE emFxType )
	{
		switch( emFxType )
		{
		case FX_TYPE_2_0_LM:
			{
				D3DXVECTOR4 vUV_Multiply(1.f,1.f,1.f,1.f);
				pFX->SetVector( "g_vLightMapUV_Multiply", &vUV_Multiply );
			}
			break;

		default:
			break;
		}

		switch( emFxType )
		{
		case FX_TYPE_2_0_LM:
		case FX_TYPE_2_0_VC:
		case FX_TYPE_2_0_AC:
		case FX_TYPE_2_0_DECAL:
			{
				pFX->SetVector( "g_vFOG",					&g_vFog );												// 2.0 버전에 포함된 fx 만
				pFX->SetFloat( "g_fOverLightPowerLM",		g_fOverLightPowerLM );									// 2.0 버전에 포함된 fx 만
				pFX->SetFloat( "g_fOverLightPower_PS_2_0",	g_fOverLightPower_PS_2_0 );								// 2.0 버전에 포함된 fx 만
				pFX->SetFloat( "g_fVS_1_1_ColorMulti",		g_fVS_1_1_ColorMulti );									// 2.0 버전에 포함된 fx 만
				pFX->SetVector( "g_vCameraFrom",			&g_vCameraFrom );										// 2.0 버전에 포함된 fx 만
				pFX->SetTexture( "g_GlobalCubeTexture",		NSCubeMapUtil::NSCubeMapBlend::GetCubeMapTexture() );	// 2.0 버전에 포함된 fx 만
			}
			break;

		case FX_TYPE_3_0_LM:
		case FX_TYPE_3_0_AC:
			pFX->SetVector( "g_vLightDiffuse",				&g_vLightDiffuse );
			break;

		case FX_TYPE_LBUFFER:
			{
				pFX->SetTexture( "g_BackBufferTexture_2nd", g_rBackBufferTexture_2nd );			// LBuffer 시 쓰임.
				pFX->SetTexture( "g_BackBufferTexture_3rd", g_rBackBufferTexture_3rd );			// LBuffer 시 쓰임.
			}
			break;

		case FX_TYPE_3_0_DECAL:
			{
				pFX->SetVector( "g_vFOG",					&g_vFog );							//  TnL_PS_3_0_NM 이상 Decal에서 쓰임.
			}
			break;

		case FX_TYPE_CUSTOM:
			{
				pFX->SetFloat( "g_fTime",					g_fTime );
				pFX->SetFloat( "g_fOverLightPowerLM",		g_fOverLightPowerLM );									// 2.0 버전에 포함된 fx 만
				pFX->SetFloat( "g_fOverLightPower_PS_2_0",	g_fOverLightPower_PS_2_0 );								// 2.0 버전에 포함된 fx 만
				pFX->SetFloat( "g_fVS_1_1_ColorMulti",		g_fVS_1_1_ColorMulti );									// 2.0 버전에 포함된 fx 만
				pFX->SetVector( "g_vCameraFrom",			&g_vCameraFrom );
				pFX->SetVector( "g_vFOG",					&g_vFog );
				pFX->SetVector( "g_vFogColor",				&g_vFogColor );
			}
			break;
		}

		pFX->SetVector( "g_vWindowSize",		&g_vWindowSize );					// All
		pFX->SetTexture( "g_pTextureLodBlend",	NSDeffered::g_pTextureLodBlend );	// All
	}

	void SetFX_CommonParameter( LPD3DXEFFECT pFX, FX_TYPE emFxType )
	{
		// SetFX_CommonParameter 을 한번만 호출하기 위해 존재함.
		std::set<ID3DXEffect*>::iterator iter = g_setCommonFX.find( pFX );
		if ( iter==g_setCommonFX.end() )
		{
			SetFX_CommonParameter_Common( pFX, emFxType );
			g_setCommonFX.insert( pFX );
		}
	}

	void SetFX_CommonParameter_Field( LPD3DXEFFECT pFX, FX_TYPE emFxType, D3DXVECTOR4* pvAmbientCube, D3DXVECTOR4& vVoxelColor )
	{
		// SetFX_CommonParameter 을 한번만 호출하기 위해 존재함.
		std::set<ID3DXEffect*>::iterator iter = g_setCommonFX.find( pFX );
		if ( iter==g_setCommonFX.end() )
		{
			// 필드에서는 한번만 해주는 방향으로 한다.
			switch( emFxType )
			{
			case FX_TYPE_2_0_LM:
			case FX_TYPE_3_0_LM:
			case FX_TYPE_LBUFFER:
				{
					D3DXVECTOR4 vLightMapUV_Offset4( 0.f, 0.f, 1.f, 1.f );
					pFX->SetVector( "g_vLightMapUV_Offset", &vLightMapUV_Offset4 );
				}
				break;

			case FX_TYPE_2_0_VC:
				{
					pFX->SetVector( "g_vVoxelColor", &vVoxelColor );
				}
				break;

			case FX_TYPE_3_0_VC:
				{
					pFX->SetVector( "g_vVoxelColor", &vVoxelColor );
					pFX->SetFloat( "g_fSpotShadowLight1_VoxelColorPower", 0.f );
					pFX->SetFloat( "g_fSpotShadowLight2_VoxelColorPower", 0.f );
				}
				break;

			case FX_TYPE_2_0_AC:
				{
					pFX->SetVectorArray( "g_vAmbientCube", &pvAmbientCube[0], LightMapBoxColor::TEST_COUNT );
				}
				break;

			case FX_TYPE_3_0_AC:
				{
					float arrayTemp[6] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
					pFX->SetFloatArray( "g_arrayfAmbientCubePL1", &arrayTemp[0], LightMapBoxColor::TEST_COUNT );
					pFX->SetFloatArray( "g_arrayfAmbientCubePL2", &arrayTemp[0], LightMapBoxColor::TEST_COUNT );
					pFX->SetVectorArray( "g_vAmbientCube", &pvAmbientCube[0], LightMapBoxColor::TEST_COUNT );
				}
				break;
			};

			SetFX_CommonParameter_Common( pFX, emFxType );
			g_setCommonFX.insert( pFX );
		}
	}

	void SetFX_CommonParameter_Reset()
	{
		g_setCommonFX.clear();
	}

	void SetFX_CommonParameter_Small( LPD3DXEFFECT pFX, float fAlpha, const D3DXMATRIX& matWorld )
	{
		pFX->SetFloat( "g_fAlpha", fAlpha );

		D3DXMATRIX matWV, matWVP;
		D3DXMatrixMultiply( &matWV, &matWorld, &g_matView );
		D3DXMatrixMultiply( &matWVP, &matWorld, &g_matVP );

		pFX->SetMatrixTranspose( "g_matWorld",	&matWorld );
		pFX->SetMatrixTranspose( "g_matWV",		&matWV );
		pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
		pFX->SetMatrix( "g_matView",			&g_matView );
		pFX->SetMatrix( "g_matProj",			&g_matProj );

		//// Fog
		//if ( g_bWIREFRAME_VIEW )
		//{
		//	D3DXVECTOR4 vFog
		//		(
		//		2, 
		//		1,
		//		1.f,
		//		1.f
		//		);
		//	pFX->SetVector( "g_vFOG", &vFog );
		//}
	}

	void SetFX_CommonParameter_Small_Fog( LPD3DXEFFECT pFX )
	{
		pFX->SetVector( "g_vFOG", &g_vFog );
		pFX->SetVector( "g_vFogColor", &g_vFogColor );
	}

	void SetFX_Deffered( LPD3DXEFFECT pFX )
	{
		pFX->SetVector( "g_vLightDiffuse",			&g_vLightDiffuse );
		pFX->SetVector( "g_vLightDirViewSpace",		&g_vLightDirViewSpace );
		pFX->SetVector( "g_vLightPosViewSpace",		&g_vLightPosViewSpace );
		pFX->SetVector( "g_vLightPosWorldSpace",	&g_vLightPosWorldSpace );

		D3DXMATRIX matInverseProj;
		D3DXMatrixInverse ( &matInverseProj, NULL, &DxViewPort::GetInstance().GetMatProj() );
		pFX->SetMatrixTranspose( "g_matInverseProj",		&matInverseProj );

		D3DXMATRIX matInverseView;
		D3DXMatrixInverse ( &matInverseView, NULL, &DxViewPort::GetInstance().GetMatView() );
		pFX->SetMatrixTranspose( "g_matInverseView",		&matInverseView );


		D3DXMATRIX matWVP;
		D3DXMatrixMultiply( &matWVP, &matInverseView, &DxShadowMap::GetInstance().GetMatrix_D() );

		D3DXMATRIX matLightViewProjClip2Tex;
		DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, matWVP );

		pFX->SetMatrixTranspose( "g_matInvView_Shadow_Clip2Tex",	&matLightViewProjClip2Tex );


		D3DXMatrixMultiply( &matWVP, &matInverseView, &DxShadowMap::GetInstance().GetMatrix_D_2nd() );

		DxShadowMap::GetInstance().GetMultiply_Clip2Tex( matLightViewProjClip2Tex, matWVP );

		pFX->SetMatrixTranspose( "g_matInvView_Shadow_2nd_Clip2Tex",	&matLightViewProjClip2Tex );


		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM && NSShadowLight::IsCascadeShadow() )
		{
			//D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.x, 0.5f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.y );	// CSM - x(Size), y(Offset), z(1/Size), w(1/CSM2Size)
			D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.x, 0.001f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.x, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.y );	// CSM - x(Size), y(Offset), z(1/Size), w(1/CSM2Size)
			pFX->SetVector( "g_vShadow", &vShadow );
			pFX->SetTexture( "g_ShadowTexture_CSM",			DxSurfaceTex::GetInstance().m_pShadowTexR32F_CSM );
			pFX->SetTexture( "g_ShadowTexture_CSM_2",		DxSurfaceTex::GetInstance().m_pShadowTexG32R32F_VSM_CSM2 );
		}
		else	// TnL_PS_3_0_NM 이다.
		{
			//if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_3_0 )
			//{
			//	D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.5f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.f );	// x-SIZE(해상도)
			//	pFX->SetVector( "g_vShadow", &vShadow );
			//	pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
			//}
			//else
			{
				D3DXVECTOR4 vShadow( DxSurfaceTex::CASCADESHADOWSIZE.z, 0.001f, 1.f/DxSurfaceTex::CASCADESHADOWSIZE.z, 0.f );	// x-SIZE(해상도), y-EPSILON(Z-Fighting때문에)
				pFX->SetVector( "g_vShadow", &vShadow );
				pFX->SetTexture( "g_ShadowTexture",	DxSurfaceTex::GetInstance().m_pShadowTexR32F );
			}
		}

		// Global CubeMap
		pFX->SetTexture( "g_GlobalCubeTexture", NSCubeMapUtil::NSCubeMapBlend::GetCubeMapTexture() );
	}

	void SetFX_DefferedTool( LPD3DXEFFECT pFX )
	{
		pFX->SetVector( "g_vLightAmbient",		&g_vLightAmbient );
	}

	void SetFX_SpotLight( const WORDWORD& wwLightID, const D3DXMATRIX& matWorld, LPD3DXEFFECT pFX )
	{
		if ( wwLightID.dwData == WORDWORD(-1,-1).dwData )
			return;

		for ( DWORD i=0; i<2; ++i )
		{
			const DxShadowSpotLight* pSpotLight(NULL);
			if ( i == 0 )
			{
				pSpotLight = NSShadowLight::GetShadowSpotLightALL( wwLightID.wA );
			}
			else
			{
				pSpotLight = NSShadowLight::GetShadowSpotLightALL( wwLightID.wB );
			}

			if ( pSpotLight )
			{
				const D3DLIGHTQ& sLight = pSpotLight->m_sLight;
				g_vPos_Range_CL_TOOL[i] = D3DXVECTOR4( pSpotLight->m_vPositionViewSpace, sLight.Range );
				g_vDiff_CosPhiHalf_CL_TOOL[i] = D3DXVECTOR4( sLight.Diffuse.r, sLight.Diffuse.g, sLight.Diffuse.b, cosf(D3DX_PI*sLight.Phi/180.f*0.5f) );
				g_vAtt_CosHalfThetaSubPhi_CL_TOOL[i] = D3DXVECTOR4( sLight.Attenuation0, sLight.Attenuation1, sLight.Attenuation2, cosf(D3DX_PI*sLight.Theta/180.f*0.5f)-cosf(D3DX_PI*sLight.Phi/180.f*0.5f) );
				g_vDirect_Falloff_CL_TOOL[i] = D3DXVECTOR4( pSpotLight->m_vDirectionViewSpace, sLight.Falloff );
				//g_fOverLighting_CL_TOOL[i] = pSpotLight->m_fOverLighting;

				D3DXMATRIX matWVP;
				if ( pSpotLight->m_dwID == DxShadowMap::GetInstance().GetSpotLightID_PL1() )
				{
					if ( i==0 )
					{
						pFX->SetTexture( "g_ShadowTexture_PL1",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL1 );
						D3DXMATRIX matWVP;
						D3DXMatrixMultiply( &matWVP, &matWorld, &DxShadowMap::GetInstance().GetMatrix_PL1() );
						pFX->SetMatrixTranspose( "g_matShadow_PL1",	&matWVP );
						pFX->SetFloat( "g_fSpotShadowLightAlpha1",	pSpotLight->m_fAlpha );
						pFX->SetFloat( "g_fAttEndLineValue1",		pSpotLight->m_fAttEndLineValue );
						pFX->SetFloat( "g_fOverLighting1",			pSpotLight->m_fOverLighting );
					}
					else
					{
						pFX->SetTexture( "g_ShadowTexture_PL2",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL1 );
						D3DXMATRIX matWVP;
						D3DXMatrixMultiply( &matWVP, &matWorld, &DxShadowMap::GetInstance().GetMatrix_PL1() );
						pFX->SetMatrixTranspose( "g_matShadow_PL2",	&matWVP );
						pFX->SetFloat( "g_fSpotShadowLightAlpha2",	pSpotLight->m_fAlpha );
						pFX->SetFloat( "g_fAttEndLineValue2",		pSpotLight->m_fAttEndLineValue );
						pFX->SetFloat( "g_fOverLighting2",			pSpotLight->m_fOverLighting );
					}
				}
				else if ( pSpotLight->m_dwID == DxShadowMap::GetInstance().GetSpotLightID_PL2() )
				{
					if ( i==0 )
					{
						pFX->SetTexture( "g_ShadowTexture_PL1",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL2 );
						D3DXMATRIX matWVP;
						D3DXMatrixMultiply( &matWVP, &matWorld, &DxShadowMap::GetInstance().GetMatrix_PL2() );
						pFX->SetMatrixTranspose( "g_matShadow_PL1",	&matWVP );
						pFX->SetFloat( "g_fSpotShadowLightAlpha1",	pSpotLight->m_fAlpha );
						pFX->SetFloat( "g_fAttEndLineValue1",		pSpotLight->m_fAttEndLineValue );
						pFX->SetFloat( "g_fOverLighting1",			pSpotLight->m_fOverLighting );
					}
					else
					{
						pFX->SetTexture( "g_ShadowTexture_PL2",	DxSurfaceTex::GetInstance().m_pShadowTexR32F_PL2 );
						D3DXMATRIX matWVP;
						D3DXMatrixMultiply( &matWVP, &matWorld, &DxShadowMap::GetInstance().GetMatrix_PL2() );
						pFX->SetMatrixTranspose( "g_matShadow_PL2",	&matWVP );
						pFX->SetFloat( "g_fSpotShadowLightAlpha2",	pSpotLight->m_fAlpha );
						pFX->SetFloat( "g_fAttEndLineValue2",		pSpotLight->m_fAttEndLineValue );
						pFX->SetFloat( "g_fOverLighting2",			pSpotLight->m_fOverLighting );
					}
				}
			}
		}

		pFX->SetVectorArray( "g_vPos_Range_CL_TOOL", &g_vPos_Range_CL_TOOL[0], 2 );
		pFX->SetVectorArray( "g_vDiff_CosPhiHalf_CL_TOOL", &g_vDiff_CosPhiHalf_CL_TOOL[0], 2 );
		pFX->SetVectorArray( "g_vAtt_CosHalfThetaSubPhi_CL_TOOL", &g_vAtt_CosHalfThetaSubPhi_CL_TOOL[0], 2 );
		pFX->SetVectorArray( "g_vDirect_Falloff_CL_TOOL", &g_vDirect_Falloff_CL_TOOL[0], 2 );
		//pFX->SetFloat( "g_fOverLighting1", g_fOverLighting_CL_TOOL[0] );
		//pFX->SetFloat( "g_fOverLighting2", g_fOverLighting_CL_TOOL[1] );
	}

	void SetFX_SpotLightEDITMODE( LPD3DXEFFECT pFX )
	{
		DWORD dwConeNumber = NSShadowLight::GetLightNumber();
		if ( dwConeNumber > CONE_LIGHT_MAX )
			dwConeNumber = CONE_LIGHT_MAX;

		if ( dwConeNumber > 0 )
		{
			pFX->SetInt( "g_nConeLightNum_TOOL", dwConeNumber );
			pFX->SetVectorArray( "g_vPos_Range_CL_TOOL", &g_vPos_Range_CL_TOOL[0], dwConeNumber );
			pFX->SetVectorArray( "g_vDiff_CosPhiHalf_CL_TOOL", &g_vDiff_CosPhiHalf_CL_TOOL[0], dwConeNumber );
			pFX->SetVectorArray( "g_vAtt_CosHalfThetaSubPhi_CL_TOOL", &g_vAtt_CosHalfThetaSubPhi_CL_TOOL[0], dwConeNumber );
			pFX->SetVectorArray( "g_vDirect_Falloff_CL_TOOL", &g_vDirect_Falloff_CL_TOOL[0], dwConeNumber );
			pFX->SetFloatArray( "g_fAttEndLineValue_CL_TOOL", &g_fAttEndLineValue_CL_TOOL[0], dwConeNumber );			
			pFX->SetFloatArray( "g_fOverLighting_CL_TOOL", &g_fOverLighting_CL_TOOL[0], dwConeNumber );
		}
	}

	void SetFX_Parameter( const MATERIAL_DATA& sMaterial_Data, LPD3DXEFFECT pFX )
	{
		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecConstants.size(); ++i )
		{
			pFX->SetVector( sMaterial_Data.m_rMaterial->m_vecConstants[i].m_strParameter.c_str(), &sMaterial_Data.m_rMaterial->m_vecConstants[i].m_vValue );
		}

		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecTextures.size(); ++i )
		{
			if ( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.Update() )
			{
				pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.GetTexture() );
			}
		}

		for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecCubeTextures.size(); ++i )
		{
			if ( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.Update() )
			{
				pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.GetCubeTexture() );
			}
		}
	}

	void ChangeSimpleFullPath( TSTRING& strSrc )
	{
		TSTRING strLeft;
		TSTRING strRight;
		while ( 1 )
		{
			size_t idx = strSrc.find('/');
			if( idx == TSTRING::npos )
				break;

			strLeft = strSrc.substr( 0, idx );
			strRight = strSrc.substr( idx+1 );

			strSrc = strLeft + "\\" + strRight;
		};

		// c:\data\..\texture\abc.dds 와 같이 \.. 을 인식하지 못한다.
		while ( 1 )
		{
			// \\..나 /.. 이 없다면 할 필요는 없다.
			size_t idx = strSrc.find("\\..");
			if ( idx == TSTRING::npos )
			{
				break;
			}

			strLeft = strSrc.substr( 0, idx );

			size_t nRightStartPos = idx+3;		// "\\.." 의 사이즈 3
			strRight = strSrc.substr( nRightStartPos, strSrc.size()-nRightStartPos );

			idx = strLeft.rfind("\\");
			if ( idx == TSTRING::npos )
			{
				break;
			}

			strLeft = strSrc.substr( 0, idx );

			strSrc = strLeft + strRight;
		}
	}

	void Archive( const TSTRING& strName, boost::function<void (const TCHAR*)> pArchive )
	{
		if ( strName.empty() )
			return;

		TSTRING strLowerName(strName);
		strLowerName += _T(".mat");
		sc::string::lowerString( strLowerName );

		pArchive( strLowerName.c_str() );

		// 내부 정보.
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strName );
		if ( citer == g_mapMaterial.end() )
			return;

		strLowerName = (*citer).second->m_strFX;
		sc::string::lowerString( strLowerName );
		pArchive( strLowerName.c_str() );

		for ( DWORD i=0; i<(*citer).second->m_vecTextures.size(); ++i )
		{
			strLowerName = (*citer).second->m_vecTextures[i].m_strTexture;
			sc::string::lowerString( strLowerName );
			//strLowerName = NSMaterialManager::g_strPath + strLowerName;

			// 풀 경로를 사용하는 상황임.
			// 일반적으로는 파일 이름만 넘김.
			//ChangeSimpleFullPath( strLowerName );
			pArchive( strLowerName.c_str() );
		}

		for ( DWORD i=0; i<(*citer).second->m_vecCubeTextures.size(); ++i )
		{
			strLowerName = (*citer).second->m_vecCubeTextures[i].m_strTexture;
			sc::string::lowerString( strLowerName );
			//strLowerName = NSMaterialManager::g_strPath + strLowerName;

			// 풀 경로를 사용하는 상황임.
			// 일반적으로는 파일 이름만 넘김.
			//ChangeSimpleFullPath( strLowerName );
			pArchive( strLowerName.c_str() );
		}
	}

	// pExt 로 확장자를 변경 후, 파일을 찾아보고 있으면 그 파일명을 넘기고,
	// 없다면 기존 NSMaterialManager::g_strPath + strName 을 쓰도록 한다.
	void FindFullNameChangeExt( TSTRING& strName, const TCHAR* pExt )
	{
		// 정상적인 이름으로 만들어줌.
		TSTRING strFullName = NSMaterialManager::g_strPath + strName;
		NSMaterialManager::ChangeSimpleFullPath( strFullName );

		// 해당 폴더명과 파일명을 찾는다.
		TSTRING strFolderName;
		TSTRING strFileName;
		{
			size_t idx = strFullName.rfind("\\");
			if( idx == TSTRING::npos )
				return;

			strFolderName = strFullName.substr( 0, idx );
			strFileName = strFullName.substr( idx+1 );
		}

		// 해당 폴더의 파일 Tree가 없다면 만들도록 한다.
		std::tr1::shared_ptr<sc::CFileFindTree> spFileFind;
		{
			MAP_FILE_FIND_ITER iter = g_mapFileFind.find( strFolderName );
			if ( iter==g_mapFileFind.end() )
			{
				spFileFind = std::tr1::shared_ptr<sc::CFileFindTree>(new sc::CFileFindTree);
				spFileFind->CreateTree( strFolderName, FALSE );
				g_mapFileFind.insert( std::make_pair(strFolderName,spFileFind) );
			}
			else
			{
				spFileFind = (*iter).second;
			}
		}

		// Tree 에 mtf 파일이 있는지 확인 후 있다면 mtf 로 이름을 변경해준다.
		{
			CString strChangeFileExt;
			STRUTIL::ChangeExt( strFileName.c_str(), strChangeFileExt, pExt );

			TSTRING strChangeName = spFileFind->FindPathName( strChangeFileExt.GetString() );
			if ( strChangeName.empty() )
				strName = strFullName;		// 원래 이름을 사용한다.
			else
				strName = strChangeName;	// 변환된 이름을 사용한다.
		}
	}

	BOOL LoadTexture_NonThread( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer != g_mapMaterial.end() )
		{
			(*citer).second.get()->LoadTexture( false );
		}

		return TRUE;
	}
};


//	--------------------------------------------------------------------------------------------------------------------
//													MaterialFXnTYPE
MaterialFXnTYPE::MaterialFXnTYPE()
	: m_rFX(NULL)
	, m_emFxType(NSMaterialManager::FX_TYPE_DEFAULT)
	, m_hTechnique_runtime_1(NULL)
	, m_hTechnique_runtime_2(NULL)
	, m_hTechnique_runtime_3(NULL)
	, m_hTechnique_Instancing(NULL)
	, m_pInstancingDCRT(NULL)
{
}

MaterialFXnTYPE::~MaterialFXnTYPE()
{
	//SAFE_RELEASE( m_rFX );
	SAFE_RELEASE( m_pInstancingDCRT );

	m_hTechnique_runtime_1 = NULL;
	m_hTechnique_runtime_2 = NULL;
	m_hTechnique_runtime_3 = NULL;
	m_hTechnique_Instancing = NULL;
}


//	--------------------------------------------------------------------------------------------------------------------
//													Material
Material::Material()
	: m_sMaterialGAME(0x1)
	, m_sMaterialTOOL(0x1ffff)
	, m_rFXnType_PS30_DF(NULL)
	, m_rFXnType_LBuffer(NULL)
	, m_bLoad(FALSE)
	, m_bLightMap(FALSE)
	, m_bLightMap_float4(FALSE)
	, m_nLightMapIndex(1)
	//, m_nDecalBlendType(-1)
	//, m_nDecalOrder(1)
	//, m_bCastShadowLightMap(TRUE)
	//, m_bReceiveShadowLM(TRUE)
	//, m_b2SideMesh(FALSE)
	, m_emSurfaceProp(NSMaterialManager::EMSURFACEPROP_NULL)
	, m_dwVerSection(0x1)
	//, m_emBlendOption(EMBLEND_OPAQUE)
{
}

Material::~Material()
{
	m_rFXnType_PS30_DF = NULL;		// 참조만 하기때문에 NULL 로 셋팅한다.
	m_rFXnType_LBuffer = NULL;		// 참조만 하기때문에 NULL 로 셋팅한다.
}

void Material::RefreshFX( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwVerSection )
{
	TSTRING strFullFX = DXShaderMan::GetInstance().GetPath() + m_strFX;

	std::map<TSTRING,ID3DXEffect*>::iterator iter = NSMaterialManager::g_mapCustomFX.find( m_strFX );
	if ( iter != NSMaterialManager::g_mapCustomFX.end() )
	{
		// 기존것 사용.
		m_sFXnType.m_rFX = (*iter).second;
	}
	else
	{
		// 새로운 것 만든다.
		ID3DXEffect* pFX(NULL);
		HRESULT hr = D3DXCreateEffectFromFile( pd3dDevice, strFullFX.c_str(), NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );
		if( FAILED( hr ) )
		{
			CDebugSet::ToLogFile( sc::string::format( "Error : %1%", strFullFX.c_str() ) );
			return;
		}

		NSMaterialManager::g_mapCustomFX.insert( std::make_pair(m_strFX,pFX) );
		m_sFXnType.m_rFX = pFX;
	}

	if ( m_sFXnType.m_rFX )
	{
		m_sFXnType.m_hTechnique_runtime_1 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_1");
		m_sFXnType.m_hTechnique_runtime_2 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_2");
		m_sFXnType.m_hTechnique_runtime_3 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_3");
		m_sFXnType.m_hTechnique_Instancing = m_sFXnType.m_rFX->GetTechniqueByName("runtime_Instancing");
	}

	BOOL bDummy(FALSE);
	int nDummy(0);
	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_sFXnType.m_rFX, 
		"SEMANTIC_VS_INPUT", 
		m_sMaterialGAME.m_dwFVF, 
		m_sMaterialGAME.m_pDCRT, 
		&m_sMaterialGAME.m_pELMT, 
		m_bLightMap, 
		m_bLightMap_float4,
		m_nLightMapIndex, 
		m_sMaterialGAME.m_bTangent, 
		m_sMaterialGAME.m_bBinormal 
	);

	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_sFXnType.m_rFX, 
		"SEMANTIC_VS_INPUT_TOOL", 
		m_sMaterialTOOL.m_dwFVF, 
		m_sMaterialTOOL.m_pDCRT, 
		&m_sMaterialTOOL.m_pELMT, 
		bDummy, 
		bDummy,
		nDummy, 
		m_sMaterialTOOL.m_bTangent, 
		m_sMaterialTOOL.m_bBinormal 
	);

	// 새로 읽었으면 버젼을 올린다.(회전시킨다.)
	m_dwVerSection = dwVerSection;
	OnlyVersionUpForTool();

	m_sFXnType.m_emFxType = NSMaterialManager::FX_TYPE_CUSTOM;
}

void Material::RefreshFX_Default( LPDIRECT3DDEVICEQ pd3dDevice, std::string& strFX, int nRenderMode, DWORD dwVerSection, NSMaterialManager::FX_TYPE emFxType )
{
	m_bLoad = TRUE;

	TSTRING strName[4];
	D3DXMACRO sShaderMacros[3];

	if ( nRenderMode == 0 )			// 기본
	{
		sShaderMacros[0].Name = NULL;
		sShaderMacros[0].Definition = NULL;
	}
	else if ( nRenderMode == 1 )	// CubeMap
	{
		strName[0] = _T("USE_CUBEMAP");
		strName[1] = _T("Doesn`t matter");
		sShaderMacros[0].Name = strName[0].c_str();
		sShaderMacros[0].Definition = strName[1].c_str();
		sShaderMacros[1].Name = NULL;
		sShaderMacros[1].Definition = NULL;
	}
	else if ( nRenderMode == 2 )	// Specular
	{
		strName[0] = _T("USE_SPECULAR");
		strName[1] = _T("Doesn`t matter");
		sShaderMacros[0].Name = strName[0].c_str();
		sShaderMacros[0].Definition = strName[1].c_str();
		sShaderMacros[1].Name = NULL;
		sShaderMacros[1].Definition = NULL;
	}
	else if ( nRenderMode == 3 )	// CubeMap + Specular
	{
		strName[0] = _T("USE_CUBEMAP");
		strName[1] = _T("Doesn`t matter");
		sShaderMacros[0].Name = strName[0].c_str();
		sShaderMacros[0].Definition = strName[1].c_str();
		strName[2] = _T("USE_SPECULAR");
		strName[3] = _T("Doesn`t matter");
		sShaderMacros[1].Name = strName[2].c_str();
		sShaderMacros[1].Definition = strName[3].c_str();
		sShaderMacros[2].Name = NULL;
		sShaderMacros[2].Definition = NULL;
	}

	{
		// 새로운 것 만든다.
		ID3DXEffect* pFX(NULL);
		UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
		HRESULT hr = D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, &sShaderMacros[0], NULL, D3DXFX_NOT_CLONEABLE, NULL, &pFX, NULL );
		if( FAILED( hr ) )
		{
			//writeLogError( string::format( "Error : %1%", strFullFX.c_str() ) );
			return;
		}

		NSMaterialManager::g_vecBaseFX.push_back( pFX );
		m_sFXnType.m_rFX = pFX;
	}

	if ( m_sFXnType.m_rFX )
	{
		m_sFXnType.m_hTechnique_runtime_1 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_1");
		m_sFXnType.m_hTechnique_runtime_2 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_2");
		m_sFXnType.m_hTechnique_runtime_3 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_3");
		m_sFXnType.m_hTechnique_Instancing = m_sFXnType.m_rFX->GetTechniqueByName("runtime_Instancing");
	}

	BOOL bDummy(FALSE);
	int nDummy(0);
	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_sFXnType.m_rFX, 
		"SEMANTIC_VS_INPUT", 
		m_sMaterialGAME.m_dwFVF, 
		m_sMaterialGAME.m_pDCRT, 
		&m_sMaterialGAME.m_pELMT, 
		m_bLightMap, 
		m_bLightMap_float4,
		m_nLightMapIndex, 
		m_sMaterialGAME.m_bTangent, 
		m_sMaterialGAME.m_bBinormal 
	);

	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_sFXnType.m_rFX, 
		"SEMANTIC_VS_INPUT_TOOL", 
		m_sMaterialTOOL.m_dwFVF, 
		m_sMaterialTOOL.m_pDCRT, 
		&m_sMaterialTOOL.m_pELMT, 
		bDummy, 
		bDummy, 
		nDummy, 
		m_sMaterialTOOL.m_bTangent, 
		m_sMaterialTOOL.m_bBinormal 
	);

	// CreateVertexDeclaration For Instancing
	if ( (emFxType==NSMaterialManager::FX_TYPE_2_0_LM) || (emFxType==NSMaterialManager::FX_TYPE_2_0_VC) || (emFxType==NSMaterialManager::FX_TYPE_2_0_AC) || (emFxType==NSMaterialManager::FX_TYPE_2_0_DECAL) ||
		(emFxType==NSMaterialManager::FX_TYPE_3_0_LM) || (emFxType==NSMaterialManager::FX_TYPE_3_0_VC) || (emFxType==NSMaterialManager::FX_TYPE_3_0_AC) || (emFxType==NSMaterialManager::FX_TYPE_3_0_DECAL) )
	{
		DWORD dwCount(0);
		for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
		{
			if ( m_sMaterialGAME.m_pELMT[i].Stream == 0xff )
			{
				dwCount = i;
				break;
			}
		}

		DWORD dwCountNEW = dwCount + 7;

		D3DVERTEXELEMENT9* pVertElements = new D3DVERTEXELEMENT9[dwCountNEW+1];

		int nTexCount(1);
		int nOffset(0);
		for ( DWORD i=0; i<dwCountNEW; ++i )
		{
			if ( i < dwCount )
			{
				pVertElements[i] = m_sMaterialGAME.m_pELMT[i];
			}
			else
			{
				pVertElements[i].Stream	= 1;
				pVertElements[i].Offset = nOffset;
				pVertElements[i].Type = D3DDECLTYPE_FLOAT4;
				pVertElements[i].Method = D3DDECLMETHOD_DEFAULT;
				pVertElements[i].Usage = D3DDECLUSAGE_TEXCOORD;
				pVertElements[i].UsageIndex = nTexCount;

				++nTexCount;
				nOffset += 16;
			}
		}

		pVertElements[dwCountNEW].Stream		= 0xff;
		pVertElements[dwCountNEW].Offset		= 0;
		pVertElements[dwCountNEW].Type			= D3DDECLTYPE_UNUSED;
		pVertElements[dwCountNEW].Method		= 0;
		pVertElements[dwCountNEW].Usage			= 0;
		pVertElements[dwCountNEW].UsageIndex	= 0;

		SAFE_RELEASE ( m_sFXnType.m_pInstancingDCRT );
		pd3dDevice->CreateVertexDeclaration( &pVertElements[0], &m_sFXnType.m_pInstancingDCRT );
		SAFE_DELETE_ARRAY ( pVertElements );
	}

	// 새로 읽었으면 버젼을 올린다.
	m_dwVerSection = dwVerSection;
	OnlyVersionUpForTool();

	m_sFXnType.m_emFxType = emFxType;
}

void Material::OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFile, std::string& strFX, int nRenderMode, DWORD dwVerSection )
{
	if ( bFile )
	{
		RefreshFX( pd3dDevice, dwVerSection );
	}
	else
	{
		// m_emFxType 이것은 초기 셋팅 했던걸 사용하도록 한다.
		//RefreshFX_Default( pd3dDevice, strFX, nRenderMode, dwVerSection, m_sFXnType.m_emFxType );

		// System 에서 만들어진 것은 작업할 필요가 없다.
		return;
	}

	if( m_sFXnType.m_rFX )	
		m_sFXnType.m_rFX->OnResetDevice();

	ResetTechniqueHandle();
}

void Material::ResetTechniqueHandle()
{
	if ( m_sFXnType.m_rFX )
	{
		m_sFXnType.m_hTechnique_runtime_1 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_1");
		m_sFXnType.m_hTechnique_runtime_2 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_2");
		m_sFXnType.m_hTechnique_runtime_3 = m_sFXnType.m_rFX->GetTechniqueByName("runtime_3");
		m_sFXnType.m_hTechnique_Instancing = m_sFXnType.m_rFX->GetTechniqueByName("runtime_Instancing");
	}
}

//void Material::OnResetDevice()
//{
//	//if( m_sFXnType.m_rFX )	
//	//	m_sFXnType.m_rFX->OnResetDevice();
//}
//
//void Material::OnLostDevice()
//{
//	//if( m_sFXnType.m_rFX )	
//	//	m_sFXnType.m_rFX->OnLostDevice();
//}

// m_dwVerSection 이 0xf 라면 0x1->0x2->0x3->0x4->...->0xf->0x1->0x2 로 된다.
// m_dwVerSection 이 0xf0 라면 0x10->0x20->0x30->0x40->...->0xf0->0x10->0x20 로 된다.
void Material::OnlyVersionUpForTool()
{
	// 새로 읽었으면 버젼을 올린다.
	DWORD dwAddVerGame = m_dwVerSection & 0x1111;
	m_sMaterialGAME.m_dwVer += dwAddVerGame;
	m_sMaterialGAME.m_dwVer = ( m_sMaterialGAME.m_dwVer & m_dwVerSection );
	if ( m_sMaterialGAME.m_dwVer == 0 )
		m_sMaterialGAME.m_dwVer = dwAddVerGame;
	m_sMaterialTOOL.m_dwVer = m_sMaterialGAME.m_dwVer << 16;
}

void Material::CreateData( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_bLoad )
		return;

	m_bLoad = TRUE;

	//RefreshFX( pd3dDevice, m_dwVerSection );

	LoadTexture( true );
}

void Material::LoadTexture( bool bThreadLoading )
{
	for ( DWORD i=0; i<m_vecTextures.size(); ++i )
	{
		m_vecTextures[i].LoadTexture( TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D, bThreadLoading );
	}

	for ( DWORD i=0; i<m_vecCubeTextures.size(); ++i )
	{
		m_vecCubeTextures[i].LoadTexture( TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE, bThreadLoading );
	}
}

bool Material::ParseCommentLua( LPDIRECT3DDEVICEQ pd3dDevice, const sc::CFileFindTree& sfileTree )
{
	//==========================================================================

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "shader" ) );
		if( ob.IsTable() )
		{
			LuaPlus::LuaTableIterator Iter(ob);
			if ( Iter )
			{   
				std::string strName((const char*) Iter.GetValue().GetString());
				m_strFX = strName;
			}
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "shader" ), MB_OK );
	}

	//==========================================================================

	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "surfaceprop" ) );
		if( ob.IsTable() )
		{
			LuaPlus::LuaTableIterator Iter(ob);
			if ( Iter )
			{   
				std::string strName((const char*) Iter.GetValue().GetString());
				m_strSurfaceProp = strName;

				if ( m_strSurfaceProp == "water" )
				{
					m_emSurfaceProp = NSMaterialManager::EMSURFACEPROP_WATER;
				}
				else if ( m_strSurfaceProp == "stream" )
				{
					m_emSurfaceProp = NSMaterialManager::EMSURFACEPROP_CUSTOM_SOFT_ALPHA;
				}
				else if ( m_strSurfaceProp == "decal" )
				{
					m_emSurfaceProp = NSMaterialManager::EMSURFACEPROP_DECAL;
				}
				else if ( m_strSurfaceProp == "unlit_unfog" )
				{
					m_emSurfaceProp = NSMaterialManager::EMSURFACEPROP_CUSTOM_UNLIT_UNFOG;
				}
			}
		}
	}

	////==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "Decal" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_bDecal = Iter.GetValue().GetBoolean();
	//		}
	//	}
	//}

	//==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "CastShadowLightMap" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_bCastShadowLightMap = Iter.GetValue().GetBoolean();
	//		}
	//	}
	//}

	////==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "ReceiveShadowLM" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_bReceiveShadowLM = Iter.GetValue().GetBoolean();
	//		}
	//	}
	//}

	////==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "TwoSideMesh" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_b2SideMesh = Iter.GetValue().GetBoolean();
	//		}
	//	}
	//}

	////==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "DecalBlendType" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_nDecalBlendType = static_cast<NSMaterialManager::EMDECAL_BLEND_TYPE>( Iter.GetValue().GetInteger() );
	//		}
	//	}
	//}

	////==========================================================================

	//{
	//	LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "DecalOrder" ) );
	//	if( ob.IsTable() )
	//	{
	//		LuaPlus::LuaTableIterator Iter(ob);
	//		if ( Iter )
	//		{   
	//			m_nDecalOrder = Iter.GetValue().GetInteger();
	//		}
	//	}
	//}

	//==========================================================================

	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "textures" ) );
		if( ob.IsTable() )
		{
			for (LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next())
			{
				std::string strKey( (const char*) Iter.GetKey().GetString() );

				LuaPlus::LuaTableIterator IterA(Iter.GetValue());
				std::string strName((const char*) IterA.GetValue().GetString());

				// mtf를 사용할 수 있는지 확인 후, 있다면 mtf 로 이름을 변경해준다.
				NSMaterialManager::FindFullNameChangeExt( strName, _T(".mtf") );

				m_vecTextures.push_back( Textures(strKey,strName) );
			}
		}
	}

	//==========================================================================

	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "cubetextures" ) );
		if( ob.IsTable() )
		{
			for (LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next())
			{
				std::string strKey( (const char*) Iter.GetKey().GetString() );

				LuaPlus::LuaTableIterator IterA(Iter.GetValue());
				std::string strName((const char*) IterA.GetValue().GetString());

				// mtf를 사용할 수 있는지 확인 후, 있다면 mtf 로 이름을 변경해준다.
				NSMaterialManager::FindFullNameChangeExt( strName, _T(".mtf") );

				m_vecCubeTextures.push_back( Textures(strKey,strName) );
			}
		}
	}

	//==========================================================================

	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "parameters" ) );
		if( ob.IsTable() )
		{
			for (LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next())
			{
				std::string strKey( (const char*) Iter.GetKey().GetString() );

				int nIndex(0);
				float afValue[4];
				for ( LuaPlus::LuaTableIterator IterA(Iter.GetValue()); IterA && nIndex<4; IterA.Next() )
				{
					afValue[nIndex++] = IterA.GetValue().GetFloat();
				}

				m_vecConstants.push_back( Constants(strKey,&afValue[0]) );
			}
		}
	}

	return true;
}

HRESULT Material::LuaLoad( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& FullPath, const std::string& _FileName, const sc::CFileFindTree& sfileTree )
{
	std::auto_ptr< sc::BaseStream > pStream
	(
		GLOGIC::openfile_basestream
		(
			FALSE,
			TSTRING(""),
			FullPath.c_str(), 
			_FileName.c_str()
		) 
	);

	if( !pStream.get() )		
	{
		CDebugSet::ToLogFile(
			sc::string::format(
			"NSMaterialManager::LuaLoad(), File Open %1%", FullPath ) );

		return false;
	}

	const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
	void* pBuffer = ::malloc(nSize);
	pStream->ReadBuffer(pBuffer, 0);
	CTypeSmartPtr _pSmartPtr(pBuffer);

	try
	{
		sc::lua_destroyState();
		sc::lua_init();

		if( !sc::lua_doBuffer( static_cast< char* >( pBuffer ), nSize, NULL ) )
		{
			MessageBox( NULL, sc::lua_getError().c_str(), FullPath.c_str(), MB_OK );

			return false;
		}

		return ParseCommentLua( pd3dDevice, sfileTree );
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), FullPath.c_str(), MB_OK );

		return false;
	}

	return false;
}