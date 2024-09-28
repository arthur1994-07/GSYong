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
#include "../../DxLand/LightMap/NSCOMBINETEXTOOLS.h"
#include "../../DxLand/LightMap/LightMapBoxColor.h"
#include "../../DxLand/LightMap/NSLightMapFX.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../G-Logic/GLogic.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"

#include "../Occlusion/NSOcclusionQueryFastRender.h"
#include "../Material/NSVertexDeclaration.h"
#include "../Material/NSMaterialManager.h"
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

#include "./MaterialSkinDef.h"

#include "NSMaterialSkinManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

namespace NSMaterialSkinManager
{
	TSTRING				g_strPath;
	sc::CFileFindTree	g_fileTree;		// mat 정보를 알아놓기 위함.

	// 특정 폴더에 있는 파일들만 검사를 하기위함.
	typedef std::tr1::unordered_map<TSTRING,std::tr1::shared_ptr<sc::CFileFindTree>>	MAP_FILE_FIND;
	typedef MAP_FILE_FIND::iterator														MAP_FILE_FIND_ITER;

	MAP_FILE_FIND	g_mapFileFind;


	typedef std::tr1::unordered_map<TSTRING,std::tr1::shared_ptr<MaterialSkin>>	MAP_MATERIAL;
	typedef MAP_MATERIAL::iterator												MAP_MATERIAL_ITER;
	typedef MAP_MATERIAL::const_iterator										MAP_MATERIAL_CITER;
	typedef MAP_MATERIAL::value_type											MAP_MATERIAL_VALUE;

	//////////////////////////////////////////////////////////////////////////
	// int nRenderMode,
	//////////////////////////////////////////////////////////////////////////
	// 0-Default, 
	// 1-CubeMap
	// 2-Multi
	// 3-TexUpDown
	// 4-CubeMap+Multi
	// 5-CubeMap+TexUpDown
	// 6-Multi+TexUpDown
	// 7-CubeMap+Multi+TexUpDown
	//
	// 8-Level, 
	// 9-Level+CubeMap
	// 10-Level+Multi
	// 11-Level+TexUpDown
	// 12-Level+CubeMap+Multi
	// 13-Level+CubeMap+TexUpDown
	// 14-Level+Multi+TexUpDown
	// 15-Level+CubeMap+Multi+TexUpDown
	//
	// 16-Level+Flow, 
	// 17-Level+Flow+CubeMap
	// 18-Level+Flow+Multi
	// 19-Level+Flow+TexUpDown
	// 20-Level+Flow+CubeMap+Multi
	// 21-Level+Flow+CubeMap+TexUpDown
	// 22-Level+Flow+Multi+TexUpDown
	// 23-Level+Flow+CubeMap+Multi+TexUpDown
	//////////////////////////////////////////////////////////////////////////

	enum
	{
		FX_RENDER_TYPE_NUM = 24,
	};

	// FX - Tool
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial_SM1;					// ShaderModel_1_1
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial_Glow;					// ShaderModel_2_0 Glow
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial_Shadow_R32F;			// ShaderModel_2_0 Shadow
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial_Shadow_G32R32F;			// ShaderModel_2_0 Shadow
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial_Kajiya_Kay_Hair;		// ShaderModel_2_0 Kajiya-Kay Hair
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterial[FX_RENDER_TYPE_NUM];	// ShaderModel_2_0 
	std::tr1::shared_ptr<MaterialSkin>	g_spDefaultMaterialNM[FX_RENDER_TYPE_NUM];	// NormalMap

	MAP_MATERIAL					g_mapMaterial;

	void CreateMaterials( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		g_fileTree.CreateTree(g_strPath);

		VEC_TSTRING vecListString;
		g_fileTree.FindListInExt( vecListString, _T("mat_s") );

		for ( DWORD i=0; i<vecListString.size(); ++i )
		{
			std::tr1::shared_ptr<MaterialSkin> spMaterial(std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin));

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
		SetDefaultFX_SM1( strFX );
		g_spDefaultMaterial_SM1 = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_SM1->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf );

		SetDefaultFX_Glow( strFX );
		g_spDefaultMaterial_Glow = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Glow->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf );

		SetDefaultFX_Shadow_R32F( strFX );
		g_spDefaultMaterial_Shadow_R32F = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Shadow_R32F->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf );

		SetDefaultFX_Shadow_G32R32F( strFX );
		g_spDefaultMaterial_Shadow_G32R32F = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Shadow_G32R32F->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf );	

		SetDefaultFX_Kajiya_Kay_Hair( strFX );
		g_spDefaultMaterial_Kajiya_Kay_Hair = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Kajiya_Kay_Hair->RefreshFX_Default( pd3dDevice, strFX, 0, 0xf00 );				

		SetDefaultFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i] = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
			g_spDefaultMaterial[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf );
		}

		SetDefaultFX_NM( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterialNM[i] = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
			g_spDefaultMaterialNM[i]->RefreshFX_Default( pd3dDevice, strFX, i, 0xf0 );
		}

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

	void OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::string strFX;

		//-----------------------------------------------------------------------------------
		// FX - Tool
		SetDefaultFX_SM1( strFX );
		g_spDefaultMaterial_SM1 = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_SM1->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf );

		SetDefaultFX_Glow( strFX );
		g_spDefaultMaterial_Glow = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Glow->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf );		

		SetDefaultFX_Shadow_R32F( strFX );
		g_spDefaultMaterial_Shadow_R32F = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Shadow_R32F->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf );

		SetDefaultFX_Shadow_G32R32F( strFX );
		g_spDefaultMaterial_Shadow_G32R32F = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Shadow_G32R32F->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf );	

		SetDefaultFX_Kajiya_Kay_Hair( strFX );
		g_spDefaultMaterial_Kajiya_Kay_Hair = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
		g_spDefaultMaterial_Kajiya_Kay_Hair->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, 0, 0xf00 );

		SetDefaultFX( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i] = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
			g_spDefaultMaterial[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf );
		}

		SetDefaultFX_NM( strFX );
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterialNM[i] = std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin);
			g_spDefaultMaterialNM[i]->OnResetDevice_Reload( pd3dDevice, FALSE, strFX, i, 0xf0 );
		}

		//-----------------------------------------------------------------------------------
		// Custom
		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->OnResetDevice_Reload( pd3dDevice, TRUE, strFX, 0, 0xf );
		}
	}

	void Reload( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		OnLostDevice();
		Delete();

		Init( pd3dDevice );
		OnResetDevice_Reload( pd3dDevice );
	}

	void SimpleReload( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		// Lost
		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->OnLostDevice();
		}
		g_mapMaterial.clear();

		// Delete
		g_fileTree.CreateTree(g_strPath);
		g_mapFileFind.clear();

		// Create
		VEC_TSTRING vecListString;
		g_fileTree.FindListInExt( vecListString, _T("mat_s") );
		for ( DWORD i=0; i<vecListString.size(); ++i )
		{
			std::tr1::shared_ptr<MaterialSkin> spMaterial(std::tr1::shared_ptr<MaterialSkin>(new MaterialSkin));

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
		// 캐릭터 툴을 처음 켜면 CsheetWithTabChar::SetActivePageTab 부분이 g_spDefaultMaterial_SM1 들이 생성도 안되었는데 들어오는 경우가 있다.
		if ( !g_spDefaultMaterial_SM1.get() )
			return;

		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spDefaultMaterial_SM1->OnlyVersionUpForTool();
		g_spDefaultMaterial_Glow->OnlyVersionUpForTool();
		g_spDefaultMaterial_Shadow_R32F->OnlyVersionUpForTool();
		g_spDefaultMaterial_Shadow_G32R32F->OnlyVersionUpForTool();
		g_spDefaultMaterial_Kajiya_Kay_Hair->OnlyVersionUpForTool();
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i]->OnlyVersionUpForTool();
			g_spDefaultMaterialNM[i]->OnlyVersionUpForTool();
		}
	}

	void Delete()
	{
		g_mapMaterial.clear();
		g_mapFileFind.clear();

		////-----------------------------------------------------------------------------------
		//// FX - Tool
		//g_spDefaultMaterial_SM1.reset();
		//g_spDefaultMaterial_Glow.reset();
		//g_spDefaultMaterial_Shadow_R32F.reset();
		//g_spDefaultMaterial_Shadow_G32R32F.reset();
		//g_spDefaultMaterial_Kajiya_Kay_Hair.reset();
		//for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		//{
		//	g_spDefaultMaterial[i].reset();
		//	g_spDefaultMaterialNM[i].reset();
		//}

	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		HRESULT hr(S_OK);


		////-----------------------------------------------------------------------------------
		// FX - Tool
		g_spDefaultMaterial_SM1->OnResetDevice();
		g_spDefaultMaterial_Glow->OnResetDevice();
		g_spDefaultMaterial_Shadow_R32F->OnResetDevice();
		g_spDefaultMaterial_Shadow_G32R32F->OnResetDevice();
		g_spDefaultMaterial_Kajiya_Kay_Hair->OnResetDevice();
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i]->OnResetDevice();
			g_spDefaultMaterialNM[i]->OnResetDevice();
		}

		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->OnResetDevice();
		}
	}

	void OnLostDevice()
	{
		//-----------------------------------------------------------------------------------
		// FX - Tool
		g_spDefaultMaterial_SM1->OnLostDevice();
		g_spDefaultMaterial_Glow->OnLostDevice();
		g_spDefaultMaterial_Shadow_R32F->OnLostDevice();
		g_spDefaultMaterial_Shadow_G32R32F->OnLostDevice();
		g_spDefaultMaterial_Kajiya_Kay_Hair->OnLostDevice();
		for ( DWORD i=0; i<FX_RENDER_TYPE_NUM; ++i )
		{
			g_spDefaultMaterial[i]->OnLostDevice();
			g_spDefaultMaterialNM[i]->OnLostDevice();
		}

		for ( MAP_MATERIAL_CITER citer=g_mapMaterial.begin(); citer!=g_mapMaterial.end(); ++citer )
		{
			(*citer).second->OnLostDevice();
		}
	}

	BOOL IsCustomSoftAlpha( const TSTRING& strMaterialName )
	{
		MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
		if ( citer == g_mapMaterial.end() )
			return FALSE;

		return (*citer).second->m_emSurfaceProp == EMSURFACEPROP_SKIN_CUSTOM_SOFT_ALPHA ? TRUE : FALSE;
	}

	#define POINT_LIGHT_MAX 12		// ConeLight 없이 PointLight만 썼을 시 16개까지 가능했었다.
	#define CONE_LIGHT_MAX 3
	BOOL		g_bMob(FALSE);		// Mob 일 경우 좀 어둡게 렌더링한다.
	D3DXVECTOR3 g_vCharPos(0.f,0.f,0.f);
	D3DXVECTOR3 g_vDiffuseColor(1.f,1.f,1.f);
	D3DXVECTOR3 g_vPointLightColor(0.f,0.f,0.f);
	D3DXVECTOR4 g_vLightDiffuse;
	D3DXVECTOR4 g_vLightAmbient;
	D3DXVECTOR4 g_vLightDir;
	D3DXVECTOR4 g_vLightDirViewSpace;
	D3DXVECTOR4 g_vSpecularDirViewSpace;
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
	D3DXMATRIX	g_matView;
	D3DXMATRIX	g_matProj;
	D3DXMATRIX	g_matVP;
	D3DXMATRIX	g_matInverseView;
	float		g_fTime(0.f);
	float		g_fColorPower_LookLength(1.f);
	float		g_fSaturationFinal(1.f);

	void SetTime( float fTime )
	{
		g_fTime = fTime;
	}

	const D3DXMATRIX& GetMatrixView()
	{
		return g_matView;
	}

	const D3DXMATRIX& GetMatrixProjection()
	{
		return g_matProj;
	}

	const D3DXMATRIX& GetMatrixViewProjection()
	{
		return g_matVP;
	}

	void FrameMove( const D3DXMATRIX& matView, const D3DXMATRIX& matProj )
	{
		DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
		if ( !pLight )
			return;

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

		D3DXVECTOR3 vCameraDir = DxViewPort::GetInstance().GetLookDir();
		D3DXVec3Normalize( &vCameraDir, &vCameraDir );
		D3DXVECTOR3 vSpecularDir = vCameraDir;	// + (vCross * 0.5f) + (vUp * 0.5f);
		D3DXVec3Normalize( &vSpecularDir, &vSpecularDir );
		D3DXVec3TransformNormal( &vSpecularDir, &vSpecularDir, &g_matView );
		g_vSpecularDirViewSpace = D3DXVECTOR4(-vSpecularDir,1.f);

		SetFX_CommonParameter_Reset();
	}

	//////////////////////////////////////////////////////////////////////////
	// int nRenderMode,
	//////////////////////////////////////////////////////////////////////////
	// 0-Default, 
	// 1-CubeMap
	// 2-Multi
	// 3-TexUpDown
	// 4-CubeMap+Multi
	// 5-CubeMap+TexUpDown
	// 6-Multi+TexUpDown
	// 7-CubeMap+Multi+TexUpDown
	//
	// 8-Level, 
	// 9-Level+CubeMap
	// 10-Level+Multi
	// 11-Level+TexUpDown
	// 12-Level+CubeMap+Multi
	// 13-Level+CubeMap+TexUpDown
	// 14-Level+Multi+TexUpDown
	// 15-Level+CubeMap+Multi+TexUpDown
	//
	// 16-Level+Flow, 
	// 17-Level+Flow+CubeMap
	// 18-Level+Flow+Multi
	// 19-Level+Flow+TexUpDown
	// 20-Level+Flow+CubeMap+Multi
	// 21-Level+Flow+CubeMap+TexUpDown
	// 22-Level+Flow+Multi+TexUpDown
	// 23-Level+Flow+CubeMap+Multi+TexUpDown
	//////////////////////////////////////////////////////////////////////////

	BOOL Active_MATERIAL_DATA( LPDIRECT3DDEVICEQ pd3dDevice, 
								const TSTRING& strMaterialName, 
								MATERIAL_SKIN_DATA& sMaterial_Data,
								BOOL bVertexColor,
								int nRenderMode,
								BOOL bNormalMap,
								int nRenderType )
	{
		// LMRT_TEXTURE_LIGHTMAP 모드일 경우는 상황에 맞게 셋팅한다.
		if ( !sMaterial_Data.m_rMaterial )
		{
			MAP_MATERIAL_CITER citer = g_mapMaterial.find( strMaterialName );
			if ( citer == g_mapMaterial.end() )
			{
				// 기본 렌더링이 가능하도록 작업함.
				sMaterial_Data.m_bDefaultMaterial = TRUE;

				if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_1_1 )
				{
					sMaterial_Data.m_rMaterial = g_spDefaultMaterial_SM1.get();
				}
				else
				{
					if ( nRenderType == 1 )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial_Glow.get();
					}
					else if ( nRenderType == 2 )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial_Shadow_R32F.get();
					}
					else if ( nRenderType == 3 )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial_Shadow_G32R32F.get();
					}
					else if ( nRenderType == 4 )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial_Kajiya_Kay_Hair.get();
					}
					else if ( nRenderType == 5 )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial_SM1.get();	// Ghosting 때는 sm1 을 쓰도록 하자.
					}
					else if ( bNormalMap )
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterialNM[nRenderMode].get();
					}
					else
					{
						sMaterial_Data.m_rMaterial = g_spDefaultMaterial[nRenderMode].get();
					}
				}
			}
			else
			{
				sMaterial_Data.m_bDefaultMaterial = FALSE;
				sMaterial_Data.m_rMaterial = (*citer).second.get();

				LPD3DXEFFECT pFX = sMaterial_Data.m_rMaterial->m_pFX;
				if ( !pFX )
					return FALSE;

				if ( sMaterial_Data.m_rMaterial->m_emSurfaceProp == EMSURFACEPROP_SKIN_NULL )
				{
					// dwFVF 를 파악한다.
					DWORD dwFVF(0L);
					for ( DWORD i=0; i<MAXD3DDECLLENGTH; ++i )
					{
						if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Stream == 0x00ff )
							break;

						if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Method == D3DDECLTYPE_UNUSED )
							break;

						if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Usage == D3DDECLUSAGE_POSITION )
						{
							dwFVF |= D3DFVF_XYZ;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Usage == D3DDECLUSAGE_NORMAL )
						{
							dwFVF |= D3DFVF_NORMAL;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Usage == D3DDECLUSAGE_COLOR )
						{
							dwFVF |= D3DFVF_DIFFUSE;
						}
						else if ( sMaterial_Data.m_rMaterial->m_sMaterialObject.m_pELMT[i].Usage == D3DDECLUSAGE_TEXCOORD )
						{
							dwFVF += D3DFVF_TEX1;
						}
					}
				}
			}
		}

		sMaterial_Data.m_rMaterial->CreateData( pd3dDevice );

		LPD3DXEFFECT pFX = sMaterial_Data.m_rMaterial->m_pFX;
		if ( !pFX )
			return FALSE;

		return TRUE;
	}

	std::set<ID3DXEffect*>	g_setCommonFX;	// 참조로만. 새로운 것이 들어온다면 그 때 SetFX_CommonParameter 를 호출하기 위해 존재함.

	void SetFX_CommonParameter_Common( LPD3DXEFFECT pFX )
	{
		pFX->SetFloat( "g_fInverseNormal",			1.f );
		pFX->SetFloat( "g_fInverseLightIntensity",	0.3f );
		pFX->SetMatrix( "g_matView",				&g_matView );
		pFX->SetMatrix( "g_matViewProj",			&g_matVP );
		pFX->SetMatrix( "g_matInverseView",			&g_matInverseView );
		pFX->SetVector( "g_vlhtDirViewSpace",		&g_vLightDirViewSpace);
		pFX->SetVector( "g_vSpecularDirViewSpace",	&g_vSpecularDirViewSpace );
		pFX->SetVector( "g_vCameraFrom",			&g_vCameraFrom );
		pFX->SetVector( "g_vFOG",					&NSMaterialManager::g_vFog );

		D3DXVECTOR4 vFogColor;
		vFogColor.x = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff0000) >> 16) / 255;
		vFogColor.y = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff00) >> 8) / 255;
		vFogColor.z = static_cast<float>((DxFogMan::GetInstance().GetFogColor() & 0xff)) / 255;
		vFogColor.w = 1.f;
		pFX->SetVector( "g_vFogColor", &vFogColor );
	}

	void SetFX_CommonParameter( LPD3DXEFFECT pFX )
	{
		// SetFX_CommonParameter 을 한번만 호출하기 위해 존재함.
		std::set<ID3DXEffect*>::iterator iter = g_setCommonFX.find( pFX );
		if ( iter==g_setCommonFX.end() )
		{
			SetFX_CommonParameter_Common( pFX );
			g_setCommonFX.insert( pFX );
		}
	}

	void SetFX_CommonParameter_Reset()
	{
		g_setCommonFX.clear();
	}

	void SetMob( BOOL bMob )
	{
		g_bMob = bMob;
	}

	void SetDiffuseColor( const D3DXVECTOR3& vDiffuseColor, const D3DXVECTOR3& vPointLightColor )
	{
		g_vDiffuseColor = vDiffuseColor;
		g_vPointLightColor = vPointLightColor;
	}

	void GetDiffuseColor( D3DXVECTOR3 &vDiffuseColor, D3DXVECTOR3 &vPointLightColor )
	{
		vDiffuseColor = g_vDiffuseColor;
		vPointLightColor = g_vPointLightColor;
	}

	void SetColorPower_LooakLength( float fLength )
	{
		g_fColorPower_LookLength = fLength;
	}

	void SetPosition_NSMaterialSkinManager( const D3DXVECTOR3& vPos )
	{
		g_vCharPos = vPos;
	}

	void ChangeColor( D3DXVECTOR3 &vDiffuse, D3DXVECTOR3 &vAmbient, const D3DXVECTOR3 &vColorAmount )
	{
		// Mob 은 음영을 더욱 살린다.
		if ( g_bMob && RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_2_0 )
		{
			// Ambient 값을 낮추고 40%
			vAmbient = vColorAmount * 0.4f;

			// Diffuse 값을 높이고 60%
			vDiffuse = vAmbient * 1.5f;
		}
		else
		{
			// Ambient 값을 낮추고 40%
			vDiffuse = vColorAmount * 0.4f;

			// Diffuse 값을 높이고 60%
			vAmbient = vDiffuse * 1.5f;
		}
	}

	void SetFX_LightParameter( LPD3DXEFFECT pFX, const EffectMaterialData* pEffectMaterialData, float fMultiply )
	{
		D3DXVECTOR4 vDiffuseDynamic;

		// set material color properties 
		D3DXVECTOR4 vDiffuse;
		vDiffuse.x = g_vDiffuseColor.x;
		vDiffuse.y = g_vDiffuseColor.y;
		vDiffuse.z = g_vDiffuseColor.z;
		vDiffuse.w = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Diffuse.a;


		
		D3DXVECTOR4 vAmbient;
		vAmbient.x = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.r;
		vAmbient.y = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.g;
		vAmbient.z = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.b;
		vAmbient.w = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Ambient.a;
		
		D3DXVECTOR4 vAmbientORG(vAmbient);

		UINT nPass(0);
		{

			//////////////////////////////////////////////////////////////////////////
			// Direction Light 것을 계산한다.
			D3DXVECTOR3 vLightAmount( vDiffuse.x+vAmbient.x, vDiffuse.y+vAmbient.y, vDiffuse.z+vAmbient.z );
			D3DXVECTOR3 vDiffuseA(0.f,0.f,0.f);
			D3DXVECTOR3 vAmbientA(0.f,0.f,0.f);
			ChangeColor( vDiffuseA, vAmbientA, vLightAmount );

			vDiffuse.x = vDiffuseA.x;
			vDiffuse.y = vDiffuseA.y;
			vDiffuse.z = vDiffuseA.z;
			vAmbient.x = vAmbientA.x;
			vAmbient.y = vAmbientA.y;
			vAmbient.z = vAmbientA.z;

			//////////////////////////////////////////////////////////////////////////
			// Point Light 것을 합산한다.
			ChangeColor( vDiffuseA, vAmbientA, g_vPointLightColor );

			vDiffuse.x += vDiffuseA.x;
			vDiffuse.y += vDiffuseA.y;
			vDiffuse.z += vDiffuseA.z;
			vAmbient.x += vAmbientA.x;
			vAmbient.y += vAmbientA.y;
			vAmbient.z += vAmbientA.z;
		}


		//////////////////////////////////////////////////////////////////////////
		// 적당한 색을 구하자.
		// 각각 0.6 가 적당하다.
		// 너무 어두운 맵인데 밝게 나오면 안되니 최대 밝게 할 수 있는 제한수치를 넣어 놓는다.
		float fSaturation = 0.2125f*(vDiffuse.x+vAmbient.x) + 0.7154f*(vDiffuse.y+vAmbient.y) + 0.0721f*(vDiffuse.z+vAmbient.z);	// 채도를 구한다.
		float fGoodGray = 1.2f / fSaturation;
		if ( fGoodGray > 2.0f )		// 어둡게 나오는 것을 조절한다. 밝게 해준다.
			fGoodGray = 2.0f;
		if ( fGoodGray < 0.5f )		// 너무 밝게 나오는 것을 조절한다. 어둡게 해준다.
			fGoodGray = 0.5f;

		// 맵 밝기와 적정 캐릭터 밝기 차이가 너무 많이 나면 안되서 50% 로 두도록 한다.
		// fGoodGray 를 그냥 사용하면 이전필드(야외,NoMaterialMap)에서 캐릭터가 너무 밝게 나와서 약간 수치를 낮추도록 한다.
		fGoodGray = 1.f + (fGoodGray-1.f)*0.5f;	// 수치변경이 적게 되도록 작업함.

		// 적당한 색을 넣는다.
		vDiffuse.x *= fGoodGray;
		vDiffuse.y *= fGoodGray;
		vDiffuse.z *= fGoodGray;
		vAmbient.x *= fGoodGray;
		vAmbient.y *= fGoodGray;
		vAmbient.z *= fGoodGray;


		// 옵션에 따른 DiffuseSpecular 강도 변경.
		// 이유는 SelfShadow 때는 그림자 지는 부분으로 인해 밝은 부분이 약간 적은데 비해
		// SelfShadow 가 꺼져있을 경우 밝은 부분이 많아서 DiffuseSpecular 강도 변경.
		float fOptionDiffuseSpecular(1.f);
		if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_2_0 )
		{
			fOptionDiffuseSpecular *= 0.7f;
		}
		else if ( RENDERPARAM::g_emCharacterQulity == TnL_CHAR_PS_1_1 )	// SM.1.1 상 역광이 사라지면서 밝게 되는 효과가 없어진다. 그래서 값을 크게한다.
		{
			fOptionDiffuseSpecular *= 1.5f;
		}

		// 카메라와 빛의 각도에 따른 Diffuse 강도 변화
		{
			D3DXVECTOR3 vLightDir(DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction);
			D3DXVec3Normalize( &vLightDir, &vLightDir );
			D3DXVECTOR3 vCameraDir = g_vCharPos - DxViewPort::GetInstance().GetFromPt();	// 캐릭터 위치와 계산을 한다.
			vCameraDir.y += 10.f;															// 키가 있으니 10 정도 올려준다.
			D3DXVec3Normalize( &vCameraDir, &vCameraDir );
			float fDot = -D3DXVec3Dot ( &vLightDir, &vCameraDir );
			fDot += 0.5f;
			if ( fDot < 0.f )
				fDot = 0.f;

			// 몹일 경우는 밝게 되는 수치를 좀 낮춘다. 
			// Diffuse 값이 워낙 강해서 좀 줄여도 될듯.
			if ( g_bMob && RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_2_0 )
			{
				vDiffuseDynamic = vDiffuse * (fDot * 0.93f) * g_fColorPower_LookLength * fOptionDiffuseSpecular;
				vDiffuse *= 0.6f;
			}
			else
			{
				vDiffuseDynamic = vDiffuse * (fDot * 1.4f) * g_fColorPower_LookLength * fOptionDiffuseSpecular;
				vDiffuse *= 0.6f;
			}

			// 천 때문에 색을 감쇠시킴.
			vDiffuse *= fMultiply;
			vDiffuseDynamic *= fMultiply;
		}

		// Diffuse, Ambient 값에 변화를 준다.
		if ( pEffectMaterialData && pEffectMaterialData->m_bAmbient )
		{
			vDiffuse.x *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.x;
			vDiffuse.y *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.y;
			vDiffuse.z *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.z;
			vDiffuseDynamic.x *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.x;
			vDiffuseDynamic.y *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.y;
			vDiffuseDynamic.z *= pEffectMaterialData->m_vEffAmbient_DiffuseMulti.z;
			vAmbient.x *= pEffectMaterialData->m_vEffAmbient_AmbientMulti.x;
			vAmbient.y *= pEffectMaterialData->m_vEffAmbient_AmbientMulti.y;
			vAmbient.z *= pEffectMaterialData->m_vEffAmbient_AmbientMulti.z;
		}

		// 라이트의 채도값을 구해서 넣도록 함.
		vDiffuse.w = 0.2125f*(vDiffuse.x+vDiffuseDynamic.x+vAmbient.x) + 
					0.7154f*(vDiffuse.y+vDiffuseDynamic.y+vAmbient.y) + 
					0.0721f*(vDiffuse.z+vDiffuseDynamic.z+vAmbient.z);	// 채도를 구한다.

		// 셋팅.
		pFX->SetVector( "g_vLightDiffuseSaturation", &vDiffuse );		// 값을 조금 낮춘다. - 주변광으로 인해 Diffuse 는 감소한다 생각하자.
		pFX->SetVector( "g_vLightDiffuseDynamic", &vDiffuseDynamic );
		pFX->SetVector( "g_vLightAmbient", &vAmbient );					// 값을 조금 높인다. - 주변광을 얻는다 생각한다.
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

	//void SetFX_Parameter( const MATERIAL_SKIN_DATA& sMaterial_Data, LPD3DXEFFECT pFX )
	//{
	//	for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecConstants.size(); ++i )
	//	{
	//		pFX->SetVector( sMaterial_Data.m_rMaterial->m_vecConstants[i].m_strParameter.c_str(), &sMaterial_Data.m_rMaterial->m_vecConstants[i].m_vValue );
	//	}

	//	for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecTextures.size(); ++i )
	//	{
	//		if ( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.Update() )
	//		{
	//			pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecTextures[i].m_textureRes.GetTexture() );
	//		}
	//	}

	//	for ( DWORD i=0; i<sMaterial_Data.m_rMaterial->m_vecCubeTextures.size(); ++i )
	//	{
	//		if ( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.Update() )
	//		{
	//			pFX->SetTexture( sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_strParameter.c_str(), sMaterial_Data.m_rMaterial->m_vecCubeTextures[i].m_textureRes.GetCubeTexture() );
	//		}
	//	}
	//}

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
		strLowerName += _T(".mat_s");
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
			//strLowerName = NSMaterialSkinManager::g_strPath + strLowerName;

			// 풀 경로를 사용하는 상황임.
			// 일반적으로는 파일 이름만 넘김.
			//ChangeSimpleFullPath( strLowerName );
			pArchive( strLowerName.c_str() );
		}

		for ( DWORD i=0; i<(*citer).second->m_vecCubeTextures.size(); ++i )
		{
			strLowerName = (*citer).second->m_vecCubeTextures[i].m_strTexture;
			sc::string::lowerString( strLowerName );
			//strLowerName = NSMaterialSkinManager::g_strPath + strLowerName;

			// 풀 경로를 사용하는 상황임.
			// 일반적으로는 파일 이름만 넘김.
			//ChangeSimpleFullPath( strLowerName );
			pArchive( strLowerName.c_str() );
		}
	}

	// pExt 로 확장자를 변경 후, 파일을 찾아보고 있으면 그 파일명을 넘기고,
	// 없다면 기존 NSMaterialSkinManager::g_strPath + strName 을 쓰도록 한다.
	void FindFullNameChangeExt( TSTRING& strName, const TCHAR* pExt )
	{
		// 정상적인 이름으로 만들어줌.
		TSTRING strFullName = NSMaterialSkinManager::g_strPath + strName;
		NSMaterialSkinManager::ChangeSimpleFullPath( strFullName );

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
};


//	--------------------------------------------------------------------------------------------------------------------
//													MaterialSkin
MaterialSkin::MaterialSkin()
	: m_sMaterialSkin(0x1)
	, m_sMaterialObject(0x1ffff)
	, m_bLoad(FALSE)
	, m_emSurfaceProp(NSMaterialSkinManager::EMSURFACEPROP_SKIN_NULL)
	, m_dwVerSection(0x1)
	, m_pFX(NULL)
{
}

MaterialSkin::~MaterialSkin()
{
	SAFE_RELEASE( m_pFX );
}

void MaterialSkin::RefreshFX( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwVerSection )
{
	TSTRING strFullFX = DXShaderMan::GetInstance().GetPath() + m_strFX;

	// Note : Effect 파일을 읽어온다.
	SAFE_RELEASE( m_pFX );
	HRESULT hr = D3DXCreateEffectFromFile( pd3dDevice, strFullFX.c_str(), NULL, NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
	if( FAILED( hr ) )
	{
		CDebugSet::ToLogFile( sc::string::format( "Error : %1%", strFullFX.c_str() ) );
		return;
	}

	if ( m_pFX )
	{
		m_hTechnique_runtime_Skin = m_pFX->GetTechniqueByName("runtime_Skin");
		m_hTechnique_runtime_Object = m_pFX->GetTechniqueByName("runtime_Object");
	}

	BOOL bDummy(FALSE);
	int nDummy(0);
	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_pFX, 
		"SEMANTIC_VS_INPUT", 
		m_sMaterialSkin.m_dwFVF, 
		m_sMaterialSkin.m_pDCRT, 
		&m_sMaterialSkin.m_pELMT, 
		bDummy, 
		bDummy,
		nDummy, 
		m_sMaterialSkin.m_bTangent, 
		m_sMaterialSkin.m_bBinormal 
	);

	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_pFX, 
		"SEMANTIC_VS_INPUT_OBJECT", 
		m_sMaterialObject.m_dwFVF, 
		m_sMaterialObject.m_pDCRT, 
		&m_sMaterialObject.m_pELMT, 
		bDummy, 
		bDummy,
		nDummy, 
		m_sMaterialObject.m_bTangent, 
		m_sMaterialObject.m_bBinormal 
	);

	// 새로 읽었으면 버젼을 올린다.(회전시킨다.)
	m_dwVerSection = dwVerSection;
	OnlyVersionUpForTool();
}


//////////////////////////////////////////////////////////////////////////
// int nRenderMode,
//////////////////////////////////////////////////////////////////////////
// 0-Default, 
// 1-CubeMap
// 2-Multi
// 3-TexUpDown
// 4-CubeMap+Multi
// 5-CubeMap+TexUpDown
// 6-Multi+TexUpDown
// 7-CubeMap+Multi+TexUpDown
//
// 8-Level, 
// 9-Level+CubeMap
// 10-Level+Multi
// 11-Level+TexUpDown
// 12-Level+CubeMap+Multi
// 13-Level+CubeMap+TexUpDown
// 14-Level+Multi+TexUpDown
// 15-Level+CubeMap+Multi+TexUpDown
//
// 16-Level+Flow, 
// 17-Level+Flow+CubeMap
// 18-Level+Flow+Multi
// 19-Level+Flow+TexUpDown
// 20-Level+Flow+CubeMap+Multi
// 21-Level+Flow+CubeMap+TexUpDown
// 22-Level+Flow+Multi+TexUpDown
// 23-Level+Flow+CubeMap+Multi+TexUpDown
//////////////////////////////////////////////////////////////////////////

void MaterialSkin::RefreshFX_Default( LPDIRECT3DDEVICEQ pd3dDevice, std::string& strFX, int nRenderMode, DWORD dwVerSection )
{
	m_bLoad = TRUE;

	TSTRING strName[6];
	D3DXMACRO sShaderMacros[6];

	for ( DWORD i=0; i<6; ++i )
	{
		strName[i] = _T("Doesn`t matter");
		sShaderMacros[i].Name = NULL;
		sShaderMacros[i].Definition = NULL;
	}

	DWORD dwIndex(0);
	int nDivide = nRenderMode/8;
	if ( nDivide >= 2 )
	{
		strName[dwIndex++] = _T("USE_LEVEL");
		strName[dwIndex++] = _T("USE_LEVEL_FLOW");
	}
	else if ( nDivide >= 1 )
	{
		strName[dwIndex++] = _T("USE_LEVEL");
	}

	int nValue = nRenderMode%8;
	if ( nValue == 1 )	// 1-CubeMap, 
	{
		strName[dwIndex++] = _T("USE_CUBEMAP");
	}
	else if ( nValue == 2 )	// 2-Multi
	{
		strName[dwIndex++] = _T("USE_MULTI");
	}
	else if ( nValue == 3 )	// 3-TexUpDown
	{
		strName[dwIndex++] = _T("USE_TEXUPDOWN");
	}
	else if ( nValue == 4 )	// 4-CubeMap+Multi
	{
		strName[dwIndex++] = _T("USE_CUBEMAP");
		strName[dwIndex++] = _T("USE_MULTI");
	}
	else if ( nValue == 5 )	// 5-CubeMap+TexUpDown
	{
		strName[dwIndex++] = _T("USE_CUBEMAP");
		strName[dwIndex++] = _T("USE_TEXUPDOWN");
	}
	else if ( nValue == 6 )	// 6-Multi+TexUpDown
	{
		strName[dwIndex++] = _T("USE_MULTI");
		strName[dwIndex++] = _T("USE_TEXUPDOWN");
	}
	else if ( nValue == 7 )	// 7-CubeMap+Multi+TexUpDown
	{
		strName[dwIndex++] = _T("USE_CUBEMAP");
		strName[dwIndex++] = _T("USE_MULTI");
		strName[dwIndex++] = _T("USE_TEXUPDOWN");
	}

	for ( DWORD i=0; i<dwIndex; ++i )
	{
		sShaderMacros[i].Name = strName[i].c_str();;
		sShaderMacros[i].Definition = strName[5].c_str();
	}

	SAFE_RELEASE( m_pFX );
	UINT dwBufferSize = static_cast<UINT>( strFX.size() ) + 1; 
	HRESULT hr = D3DXCreateEffect( pd3dDevice, strFX.c_str(), dwBufferSize, &sShaderMacros[0], NULL, D3DXFX_NOT_CLONEABLE, NULL, &m_pFX, NULL );
	if( FAILED( hr ) )
	{
		//writeLogError( string::format( "Error : %1%", strFullFX.c_str() ) );
		return;
	}

	if ( m_pFX )
	{
		m_hTechnique_runtime_Skin = m_pFX->GetTechniqueByName("runtime_Skin");
		m_hTechnique_runtime_Object = m_pFX->GetTechniqueByName("runtime_Object");
	}

	BOOL bDummy(FALSE);
	int nDummy(0);
	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_pFX, 
		"SEMANTIC_VS_INPUT", 
		m_sMaterialSkin.m_dwFVF, 
		m_sMaterialSkin.m_pDCRT, 
		&m_sMaterialSkin.m_pELMT, 
		bDummy, 
		bDummy, 
		nDummy,
		m_sMaterialSkin.m_bTangent, 
		m_sMaterialSkin.m_bBinormal 
	);

	NSVertexDeclaration::BuildVertexDeclaration
	( 
		pd3dDevice, 
		m_pFX, 
		"SEMANTIC_VS_INPUT_OBJECT", 
		m_sMaterialObject.m_dwFVF, 
		m_sMaterialObject.m_pDCRT, 
		&m_sMaterialObject.m_pELMT, 
		bDummy, 
		bDummy, 
		nDummy,
		m_sMaterialObject.m_bTangent, 
		m_sMaterialObject.m_bBinormal 
	);

	// 새로 읽었으면 버젼을 올린다.
	m_dwVerSection = dwVerSection;
	OnlyVersionUpForTool();
}

void MaterialSkin::OnResetDevice_Reload( LPDIRECT3DDEVICEQ pd3dDevice, BOOL bFile, std::string& strFX, int nRenderMode, DWORD dwVerSection )
{
	if ( bFile )
	{
		RefreshFX( pd3dDevice, dwVerSection );
	}
	else
	{
		// m_emFxType 이것은 초기 셋팅 했던걸 사용하도록 한다.
		RefreshFX_Default( pd3dDevice, strFX, nRenderMode, dwVerSection );
	}

	if( m_pFX )	
		m_pFX->OnResetDevice();

	if ( m_pFX )
	{
		m_hTechnique_runtime_Skin = m_pFX->GetTechniqueByName("runtime_Skin");
		m_hTechnique_runtime_Object = m_pFX->GetTechniqueByName("runtime_Object");
	}
}

void MaterialSkin::OnResetDevice()
{
	if( m_pFX )	
		m_pFX->OnResetDevice();
}

void MaterialSkin::OnLostDevice()
{
	if( m_pFX )	
		m_pFX->OnLostDevice();
}

// m_dwVerSection 이 0xf 라면 0x1->0x2->0x3->0x4->...->0xf->0x1->0x2 로 된다.
// m_dwVerSection 이 0xf0 라면 0x10->0x20->0x30->0x40->...->0xf0->0x10->0x20 로 된다.
void MaterialSkin::OnlyVersionUpForTool()
{
	// 새로 읽었으면 버젼을 올린다.
	DWORD dwAddVerGame = m_dwVerSection & 0x1111;
	m_sMaterialObject.m_dwVer += dwAddVerGame;
	m_sMaterialObject.m_dwVer = ( m_sMaterialObject.m_dwVer & m_dwVerSection );

	if ( m_sMaterialObject.m_dwVer == 0 )
		m_sMaterialObject.m_dwVer = dwAddVerGame;

	m_sMaterialSkin.m_dwVer = m_sMaterialObject.m_dwVer << 16;
}

void MaterialSkin::CreateData( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( m_bLoad )
		return;

	m_bLoad = TRUE;

	RefreshFX( pd3dDevice, m_dwVerSection );

	LoadTexture( true );
}

void MaterialSkin::LoadTexture( bool bThreadLoading )
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

bool MaterialSkin::ParseCommentLua( LPDIRECT3DDEVICEQ pd3dDevice, const sc::CFileFindTree& sfileTree )
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

				if ( m_strSurfaceProp == "custom_soft_alpha" )
				{
					m_emSurfaceProp = NSMaterialSkinManager::EMSURFACEPROP_SKIN_CUSTOM_SOFT_ALPHA;
				}
			}
		}
	}

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
				NSMaterialSkinManager::FindFullNameChangeExt( strName, _T(".mtf") );

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
				NSMaterialSkinManager::FindFullNameChangeExt( strName, _T(".mtf") );

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

HRESULT MaterialSkin::LuaLoad( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& FullPath, const std::string& _FileName, const sc::CFileFindTree& sfileTree )
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
			"NSMaterialSkinManager::LuaLoad(), File Open %1%", FullPath ) );

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