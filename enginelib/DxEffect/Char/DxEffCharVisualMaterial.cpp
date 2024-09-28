#include "pch.h"

#include "../../../VisualMaterialLib/Command_EX/DxVMCommand_EX.h"
#include "../../../VisualMaterialLib/Manager/VisualMaterialFX.h"
#include "../../../VisualMaterialLib/Manager/DxVMManager.h"
#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"
#include "../../../VisualMaterialLib/Util/DxTangentMesh.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/DxLoadShader.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_NORMAL.h"
#include "../../DxMeshs/SkinMesh/_new/DxSkinMesh9_HLSL.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkinEffResult.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"
#include "../../DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxRenderStates.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/DxSurfaceTex.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/editmeshs.h"
#include "../../DxTools/DxFogMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "./DxEffCharAmbient.h"
#include "./DxEffCharHLSL.h"

#include "DxEffCharVisualMaterial.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//                                  EFFCHAR_PROPERTY_VISUAL_MATERIAL
EFFCHAR_PROPERTY_VISUAL_MATERIAL::EFFCHAR_PROPERTY_VISUAL_MATERIAL()
{
    m_strVisualMaterial = _T("");
}

EFFCHAR_PROPERTY_VISUAL_MATERIAL::~EFFCHAR_PROPERTY_VISUAL_MATERIAL()
{
    CleanUp();
}

void EFFCHAR_PROPERTY_VISUAL_MATERIAL::CleanUp()
{
    m_vecTextureResource.clear();
    m_mapParameter.clear();
}

void EFFCHAR_PROPERTY_VISUAL_MATERIAL::CloneDummy( const EFFCHAR_PROPERTY_VISUAL_MATERIAL& sSrc )
{
    CleanUp();

    m_mapParameter      = sSrc.m_mapParameter;
    m_strVisualMaterial	= sSrc.m_strVisualMaterial;

    for ( DWORD i=0; i<sSrc.m_vecTextureResource.size(); ++i )
    {
        vm::NAME_TEXTURE sNameTexture;
        sNameTexture.m_strName = sSrc.m_vecTextureResource[i].m_strName;
        m_vecTextureResource.push_back( sNameTexture );
    }

    m_spVMFX = sSrc.m_spVMFX;
}

void EFFCHAR_PROPERTY_VISUAL_MATERIAL::Save( sc::BaseStream& SFile )
{
    SFile << m_strVisualMaterial;

    DWORD dwSize = static_cast<DWORD> (m_vecTextureResource.size());
    SFile << dwSize;

    SFile.BeginBlock();
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            SFile << m_vecTextureResource[i].m_strName;
        }
    }
    SFile.EndBlock();
    
    SFile.BeginBlock();
    {
        dwSize = static_cast<DWORD> (m_mapParameter.size());
        SFile << dwSize;

        MAP_PARAMETER_NAME_DATA2_ITER iter = m_mapParameter.begin();
        for ( ; iter!=m_mapParameter.end(); ++iter )
        {
            SFile << (*iter).first;
            SFile << (*iter).second.m_bNight;
            SFile << (*iter).second.m_vParameter;
            SFile << (*iter).second.m_vNight;
        }
    }
    SFile.EndBlock();
}

void EFFCHAR_PROPERTY_VISUAL_MATERIAL::Load( LPDIRECT3DDEVICEQ pd3dDevice, sc::BaseStream& SFile, BOOL bSkinMesh )
{
    CleanUp();

	SFile >> m_strVisualMaterial;

    if ( bSkinMesh )
    {
        m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_VS );
    }
    else
    {
        m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_OBJECT );
    }

    DWORD dwSize(0), dwBufferSizeMINI(0);
    SFile >> dwSize;
    SFile >> dwBufferSizeMINI;

    if ( dwSize == m_spVMFX->GetTextureSize() )
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            SFile >> sNameTexture.m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
                false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }
    else
    {
        // ��Ȯ�� ��ġ�� �̵�
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSizeMINI );

        // ���ο� �����ͷ� ������ �����Ѵ�.
        for ( DWORD i=0; i<m_spVMFX->GetTextureSize(); ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_spVMFX->GetTextureName( i );

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
                false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }

    SFile >> dwBufferSizeMINI;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        TSTRING strName;
        ParameterData sData;
        SFile >> strName;
        SFile >> sData.m_bNight;
        SFile >> sData.m_vParameter;
        SFile >> sData.m_vNight;

        m_mapParameter.insert( std::make_pair( strName.c_str(), sData ) );
    }

	// ������ �����ϴ�.
	if ( !m_spVMFX->GetFX() && !m_strVisualMaterial.empty() )
	{
		std::string ErrorMsg(
			sc::string::format(
			"VisualMaterial file not exist : %1%", m_strVisualMaterial));

		CDebugSet::ToLogFile( ErrorMsg );
	}
}

void EFFCHAR_PROPERTY_VISUAL_MATERIAL::Load_100( LPDIRECT3DDEVICEQ pd3dDevice, sc::BaseStream& SFile, BOOL bSkinMesh )
{
    CleanUp();

	SFile >> m_strVisualMaterial;

    if ( bSkinMesh )
    {
        m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_VS );
    }
    else
    {
        m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_OBJECT );
    }

    DWORD dwSize(0), dwBufferSizeMINI(0);
    SFile >> dwSize;
    SFile >> dwBufferSizeMINI;

    if ( dwSize == m_spVMFX->GetTextureSize() )
    {
        for ( DWORD i=0; i<dwSize; ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            SFile >> sNameTexture.m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
                false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }
    else
    {
        // ��Ȯ�� ��ġ�� �̵�
        SFile.SetOffSet( SFile.GetfTell() + dwBufferSizeMINI );

        // ���ο� �����ͷ� ������ �����Ѵ�.
        for ( DWORD i=0; i<m_spVMFX->GetTextureSize(); ++i )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = m_spVMFX->GetTextureName( i );

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
                false,
				FALSE );

            m_vecTextureResource.push_back( sNameTexture );
        }
    }

	// ������ �����ϴ�.
	if ( !m_spVMFX->GetFX() )
	{
		std::string ErrorMsg(
			sc::string::format(
			"VisualMaterial file not exist. : %1%", m_strVisualMaterial));
		
		CDebugSet::ToLogFile( ErrorMsg );
	}
}


//-----------------------------------------------------------------------------------------------------------
//                                  DxEffCharVisualMaterial
DWORD		DxEffCharVisualMaterial::TYPEID			= EMEFFCHAR_VISUAL_MATERIAL;
const DWORD	DxEffCharVisualMaterial::VERSION		= 0x0101;
char		DxEffCharVisualMaterial::NAME[MAX_PATH]	= "1.Visual Material";

extern BOOL		g_bCHAR_EDIT_RUN;
extern BOOL		g_bCHAR_EDIT_PIECE_RENDER_FIXED;

DxEffCharVisualMaterial::DxEffCharVisualMaterial(void)
    : DxEffChar()
    , m_pd3dDevice(NULL)
	, m_bPossibleEffect(TRUE)
    , m_fTime(0.f)
    , m_rSKINEFFDATA(NULL)
    , m_vUserColor1(0.f,0.f,0.f)
    , m_vUserColor2(0.f,0.f,0.f)
	, m_pSkinMesh(NULL)
	, m_pmcMesh(NULL)
{
}

DxEffCharVisualMaterial::~DxEffCharVisualMaterial(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharVisualMaterial::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	if ( RENDERPARAM::g_emCharacterQulity <= TnL_CHAR_FIXED_HIGH )
	{
		return NULL;
	}

	// [shhan][2015.04.28] ����翡�� crash �� ���� �� ���Ƽ� ��ȿ�� �˻� �߰���.
	if ( !m_bPossibleEffect )
		return NULL;

	HRESULT hr;
	DxEffCharVisualMaterial *pEffChar = new DxEffCharVisualMaterial;
	pEffChar->SetFileName ( GetFileName() );
	pEffChar->SetLinkObj ( pCharPart, pSkinPiece );
	pEffChar->SetProperty ( &m_sProperty );	

	hr = pEffChar->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffChar);
		return NULL;
	}

    pEffChar->SetPropertyNEW( pd3dDevice, m_vecProperty );

	return pEffChar;
}

HRESULT DxEffCharVisualMaterial::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

    m_pd3dDevice = pd3dDevice;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	if ( !DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader() )		// PS.2.0 �ϵ���� ������ ���� �� ��츸 �ε�
	{
		m_bPossibleEffect = FALSE;
		return S_OK;
	}

	// m_pmcMesh
	IsExist_Copy_pmcMesh( pd3dDevice );

	return S_OK;
}

HRESULT DxEffCharVisualMaterial::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT DxEffCharVisualMaterial::InvalidateDeviceObjects ()
{
	return S_OK;
}

HRESULT DxEffCharVisualMaterial::DeleteDeviceObjects ()
{
	return S_OK;
}

void DxEffCharVisualMaterial::SetProperty( EFFCHAR_PROPERTY *pProp )
{
}

void DxEffCharVisualMaterial::SetPropertyNEW( LPDIRECT3DDEVICEQ pd3dDevice, const VEC_PROPERTY& vecSrc )
{
    m_vecProperty.clear();

    for ( DWORD i=0; i<vecSrc.size(); ++i )
    {
        std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);

        spProperty->m_strVisualMaterial = vecSrc[i]->m_strVisualMaterial;
        spProperty->m_mapParameter      = vecSrc[i]->m_mapParameter;

        for ( DWORD j=0; j<vecSrc[i]->m_vecTextureResource.size(); ++j )
        {
            vm::NAME_TEXTURE sNameTexture;
            sNameTexture.m_strName = vecSrc[i]->m_vecTextureResource[j].m_strName;

            sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                sNameTexture.m_strName,
                false,
                TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                0,
                D3DFMT_UNKNOWN,
                TEXTURE_RESIZE_NONE,
                TEXTURE_VISUALMATERIAL,
                false,
				FALSE );

            spProperty->m_vecTextureResource.push_back( sNameTexture );
        }

        if ( m_pmcMesh->pSkinInfo )
        {
            spProperty->m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, spProperty->m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_VS );
        }
        else
        {
            spProperty->m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, spProperty->m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_OBJECT );
        }

        m_vecProperty.push_back( spProperty );
    } 
}

HRESULT DxEffCharVisualMaterial::FrameMove ( float fTime, float fElapsedTime )
{
    m_fTime = fTime;

	return S_OK;
}

void DxEffCharVisualMaterial::RenderVM( LPDIRECT3DDEVICEQ pd3dDevice, const SKINEFFDATA* pSKINEFFDATA, TEXTURE_TYPE_FLAG emTexType )
{
	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

	if( !m_pSkinMesh )		return;

	D3DXMATRIX matStaticLinkBone;
	D3DXMatrixIdentity( &matStaticLinkBone );

	const D3DXMATRIXA16*	pBoneMatrices(NULL);
	if ( m_pCharPart )
	{
		pBoneMatrices = m_pCharPart->GetBoneMatrices();
		if ( !pBoneMatrices )
			return;

		matStaticLinkBone = m_pCharPart->GetStaticLinkBone();
	}


	// [shhan][2014.06.30] VisualMaterial �� ���۵��� ���� ���, �׸��⸦ ���⼭ ���� �ʴ´�.
	//				GetFlag () �� EMECF_VISUAL_MATERIAL; ���� �ѱ��� �ʵ��� �Ͽ�, ����Ʈ ������ �ǵ��� �Ѵ�.
	//
  //  // fx �� �� ��������� ��� ���⼭ �������Ѵ�.
  //  // �������� Base�� ������ ���ϱ� ������ �̷��� �Ѵ�.
  //  if( !m_bPossibleEffect || (!RENDERPARAM::bUseNormalMap && !g_bCHAR_EDIT_RUN) )
  //  {
  //      CHARSETTING sCharSetting;
		//sCharSetting.pMeshContainerBase = m_pmcMesh;
		//sCharSetting.pmtrlPiece			= pMaterialPiece;
		//sCharSetting.pmtrlSpecular		= NULL;
		//sCharSetting.emRDOP				= CTOP_BASE;
		//m_pSkinMesh->SetDrawState ( TRUE, FALSE, FALSE, FALSE );
  //      m_pSkinMesh->DrawMeshContainer( pd3dDevice, sCharSetting, m_rSkeleton );
  //      return;
  //  }

    //const D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView();
    //const D3DXMATRIX& matProj = DxViewPort::GetInstance().GetMatProj();

	//D3DXMATRIX matView;
	//D3DXMATRIX matProj;
	//pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
	//pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );
    //D3DXMatrixMultiply( &m_matVP, &matView, &matProj );
	m_matVP = NSMaterialSkinManager::GetMatrixViewProjection();

	if ( pSKINEFFDATA )
		m_rSKINEFFDATA = pSKINEFFDATA;

    m_pSkinMesh->DrawMeshVisualMaterial( pd3dDevice, m_pmcMesh, pBoneMatrices, this, m_rSkeleton, emTexType, matStaticLinkBone );
}

void DxEffCharVisualMaterial::PreSettingFX( std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>& spProperty, ID3DXEffect* pFX, float fAlpha )
{
    DXLIGHT* pLight = DxLightMan::GetInstance()->GetDirectLight();
    const D3DLIGHT9& sLight = pLight->m_Light;

    D3DXVECTOR4 vLightDir;
    D3DXVECTOR3 vHalfDir3;
    D3DXVECTOR4 vHalfDir4;
    D3DXVECTOR4 vCameraFrom4;
    {
        D3DXVECTOR4 vLightDiffuse(sLight.Diffuse.r,sLight.Diffuse.g,sLight.Diffuse.b,1.f);
        D3DXVECTOR4 vLightAmbient(sLight.Ambient.r,sLight.Ambient.g,sLight.Ambient.b,1.f);
        D3DXVECTOR4 vLightSpecular(sLight.Specular.r,sLight.Specular.g,sLight.Specular.b,1.f);
        m_vLightDir.x = -sLight.Direction.x;
        m_vLightDir.y = -sLight.Direction.y;
        m_vLightDir.z = -sLight.Direction.z;
        D3DXVec3Normalize( &m_vLightDir, &m_vLightDir );
        vLightDir.x = m_vLightDir.x;
        vLightDir.y = m_vLightDir.y;
        vLightDir.z = m_vLightDir.z;
        vLightDir.w = 1.f;

        const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
        vCameraFrom4.x = vFromPt.x;
        vCameraFrom4.y = vFromPt.y;
        vCameraFrom4.z = vFromPt.z;
        vCameraFrom4.w = 1.f;

        D3DXVECTOR4 vLightLevel(1.f,1.f,1.f,1.f);
        D3DXVECTOR4 vLevelFlowColor(1.f,1.f,1.f,1.f);
        if ( m_rSKINEFFDATA->m_rLevelData )
        {
            vLightLevel = D3DXVECTOR4( m_rSKINEFFDATA->m_rLevelData->m_vLevelColor.x,
                                        m_rSKINEFFDATA->m_rLevelData->m_vLevelColor.y,
                                        m_rSKINEFFDATA->m_rLevelData->m_vLevelColor.z,
                                        1.f );

            vLevelFlowColor = D3DXVECTOR4( m_rSKINEFFDATA->m_rLevelData->m_vLevelFlowColor.x,
                                            m_rSKINEFFDATA->m_rLevelData->m_vLevelFlowColor.y,
                                            m_rSKINEFFDATA->m_rLevelData->m_vLevelFlowColor.z,
                                            1.f );
        }

        D3DXVECTOR4 vUserColor1(0.f,0.f,0.f,0.f);
        D3DXVECTOR4 vUserColor2(0.f,0.f,0.f,0.f);

        if( !g_bCHAR_EDIT_RUN )
	    {
            if ( m_pCharPart )
            {
		        D3DXCOLOR cColor1  = m_pCharPart->GetHairColor();
		        D3DXCOLOR cColor2  = m_pCharPart->GetSubColor();
		        vUserColor1 = D3DXVECTOR4( cColor1.r, cColor1.g, cColor1.b, cColor1.a );
		        vUserColor2 = D3DXVECTOR4( cColor2.r, cColor2.g, cColor2.b, cColor2.a );
            }
	    }
        else
        {
            vUserColor1 = D3DXVECTOR4( m_vUserColor1.x, m_vUserColor1.y, m_vUserColor1.z, 1.f );
		    vUserColor2 = D3DXVECTOR4( m_vUserColor2.x, m_vUserColor2.y, m_vUserColor2.z, 1.f );
        }

        // Ambient ȿ���� CharPart�� ����� ���.. Ambient �� ����.
        // ȿ���� ���� �ȵǾ ����Ʈ�� m_pCharPart->m_cEffAmbientType == USETEXCOLOR
        //                              m_pCharPart->m_cEffAmbientColor = 1.f,1.f,1.f,1.f 
        //                              �̶� ������ �Ѽ����� �ʴ´�.
        if ( m_pCharPart )
        {
            if ( m_pCharPart->m_cEffAmbientType == USETEXCOLOR )
            {
                vLightAmbient.x *= m_pCharPart->m_cEffAmbientColor.r;
                vLightAmbient.y *= m_pCharPart->m_cEffAmbientColor.g;
                vLightAmbient.z *= m_pCharPart->m_cEffAmbientColor.b;
            }
            else
            {
                vLightAmbient.x = m_pCharPart->m_cEffAmbientColor.r;
                vLightAmbient.y = m_pCharPart->m_cEffAmbientColor.g;
                vLightAmbient.z = m_pCharPart->m_cEffAmbientColor.b; 
            }
        }

        // Diffuser Color Power ����
        {
            float fLM_DDPower = (DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower()-1.f)*GLPeriod::GetInstance().GetBlendFact();
            vLightDiffuse += vLightDiffuse*fLM_DDPower;
			vLightDiffuse.w = fAlpha;
        }

		// ����Ʈ �� ȿ���� �ֱ� ����
		DWORD dwLighting(0);
		m_pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
		if ( dwLighting == 0 )
		{
			vLightDiffuse = D3DXVECTOR4( 0.f, 0.f, 0.f, fAlpha );
			vLightAmbient = D3DXVECTOR4( 1.f, 1.f, 1.f, 1.f );
			vLightSpecular = D3DXVECTOR4( 0.f, 0.f, 0.f, 0.f );
		}

		pFX->SetVector( "g_vLightDiffuse", &vLightDiffuse );
		pFX->SetVector( "g_vLightAmbient", &vLightAmbient );
        pFX->SetVector( "g_vLightSpecular", &vLightSpecular );
        pFX->SetVector( "g_vLightDir", &vLightDir );
        pFX->SetVector( "g_vCameraFrom", &vCameraFrom4 );
		pFX->SetVector( "g_vLightLevel", &vLightLevel );
		pFX->SetVector( "g_vLevelFlowColor", &vLevelFlowColor );
        pFX->SetVector( "g_vUserColor1", &vUserColor1 );
        pFX->SetVector( "g_vUserColor2", &vUserColor2 );

        pFX->SetTexture( "ShininessTex", NS_VM_MANAGER::g_textureResShininess.GetTexture() );
        pFX->SetTexture( "LevelFlowTex", DxSurfaceTex::GetInstance().m_pFlowTex );

		// Fog
		D3DXVECTOR4 vFog
		(
			DxFogMan::GetInstance().GetFogEnd_RealTime(), 
			DxFogMan::GetInstance().GetFogEnd_RealTime() - DxFogMan::GetInstance().GetFogStart_RealTime(),
			1.f,
			1.f
		);
		pFX->SetVector( "g_vFog", &vFog );

        // Light ����
        {
            // ĳ���Ϳ����� �ִ� 4������ PointLight�� �����ȴ�.
            #define POINT_LIGHT_MAX 4

            D3DXVECTOR4 vPos_Range_PL[ POINT_LIGHT_MAX ];
            D3DXVECTOR4 vDiff_PL[ POINT_LIGHT_MAX ];
            DWORD dwSize = NSNearPointLight::GetPointLightDataSize();
            if ( dwSize > POINT_LIGHT_MAX )
                dwSize = POINT_LIGHT_MAX;

            for ( DWORD i=0; i<dwSize && i<POINT_LIGHT_MAX; ++i )
            {
                const NSNearPointLight::PointLightData* pPointLight = NSNearPointLight::GetPointLightData(i);
                vDiff_PL[i] = D3DXVECTOR4( pPointLight->m_cDiffuse.r, pPointLight->m_cDiffuse.g, pPointLight->m_cDiffuse.b, 1.f );
                vPos_Range_PL[i] = D3DXVECTOR4( pPointLight->m_vPosition.x, pPointLight->m_vPosition.y, pPointLight->m_vPosition.z, pPointLight->m_fRange );
            }

            pFX->SetInt( "g_nPointLightNum", dwSize );
            pFX->SetVectorArray( "g_vPos_Range_PL", &vPos_Range_PL[0], POINT_LIGHT_MAX );
            pFX->SetVectorArray( "g_vDiff_PL", &vDiff_PL[0], POINT_LIGHT_MAX );
        }

        // Texture_0 ... ����
        {
            char c1='0';
            std::string strTextureORIG = "Texture_";
            std::string strTexture = "";
            for ( size_t i=0; i<spProperty->m_vecTextureResource.size(); ++i )
            {
                strTexture = strTextureORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTexture.c_str(), spProperty->m_vecTextureResource[i].m_textureRes.GetTexture() );
            }
        }

        // TextureCube_0 ... ����
        {
            char c1='0';
            std::string strTextureCubeORIG = "CubeTexture_";
            std::string strTextureCube = "";
            for ( size_t i=0; i<spProperty->m_spVMFX->GetCubeTextureSize(); ++i )
            {
                strTextureCube = strTextureCubeORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTextureCube.c_str(), spProperty->m_spVMFX->GetCubeTexture(i) );
            }
        }

        // g_fUniformPixelScalar_0 ... ����.
        {
            vm::MAP_PARAMETER_NAME_DATA mapParamaterNameData;
            MAP_PARAMETER_NAME_DATA2_CITER iter = spProperty->m_mapParameter.begin();
            for ( ; iter!=spProperty->m_mapParameter.end(); ++iter )
            {
                // ��
                if ( (!GLPeriod::GetInstance().IsOffLight() && (*iter).second.m_bNight) && DxLightMan::GetInstance()->IsNightAndDay() == TRUE )
                {
                    // ��
                    mapParamaterNameData.insert( std::make_pair( (*iter).first, (*iter).second.m_vNight ) );
                }
                else
                {
                    // ��
                    mapParamaterNameData.insert( std::make_pair( (*iter).first, (*iter).second.m_vParameter ) );
                }                
            }

            spProperty->m_spVMFX->FrameMove( mapParamaterNameData, m_fTime );
        }
     }
}

void DxEffCharVisualMaterial::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIXA16* pBoneMatrices, LPD3DXBONECOMBINATION pBoneCur, int nPaletteEntries, int nInfl, TEXTURE_TYPE_FLAG emTexType )
{
	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

    if ( !m_rSKINEFFDATA )
        return;

    if ( pBoneCur->AttribId >= m_vecProperty.size() )
        return;

    std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = m_vecProperty[pBoneCur->AttribId];

    if ( !spProperty->m_spVMFX.get() )
        return;

	if ( (emTexType == EMTTF_OPAQUE) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_OPAQUE) )
		return;
	else if ( (emTexType == EMTTF_ALPHA_HARD) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_MASKED) )
		return;
	else if ( (emTexType == EMTTF_ALPHA_SOFT) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_TRANSLUCENT) )
		return;

    ID3DXEffect* pFX = spProperty->m_spVMFX->GetFX();
    if ( !pFX )
        return;

    // [�������]
    // SetMatrix ���� SetMatrixTranspose ����
    // HLSL ���� ���Ǵ� ��ġ�� �ٸ���.
    //pFX->SetMatrix( "g_matVP",	&matVP );           // Out.Position = mul( float4(vPos.xyz, 1.0f), g_matVP ); 
    //pFX->SetMatrixTranspose( "g_matVP",	&matVP );   // Out.Position = mul( g_matVP, float4(vPos.xyz, 1.0f) ); 
    pFX->SetMatrix( "g_matVP",	&m_matVP );

    pFX->SetMatrixArray( "g_matWorldMatrixArray", pBoneMatrices, nPaletteEntries );
    pFX->SetInt( "g_nCurNumBones", nInfl-1 );

    LPDIRECT3DVERTEXBUFFERQ pVB = NULL;
    LPDIRECT3DINDEXBUFFERQ	pIB = NULL;
    m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetVertexBuffer( &pVB );
    m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh->GetIndexBuffer( &pIB );

    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_SKIN );

    pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(vm::VERTEXNORTEXTANGENT_VS_COMPRESS) );
	pd3dDevice->SetIndices ( pIB );

	float fAlpha(1.f);
	DWORD dwAlphaTestEnable, dwAlphaBlendEnable, dwSrcBlend, dwDestBlend;
	if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_TRANSLUCENT )
	{
		fAlpha = m_pmcMesh->pMaterials[pBoneCur->AttribId].MatD3D.Diffuse.a;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetRenderState( D3DRS_SRCBLEND,			&dwSrcBlend );
		pd3dDevice->GetRenderState( D3DRS_DESTBLEND,		&dwDestBlend );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	}
	else if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_MASKED )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	}
    
	//--------------------------------------------------- Draw
    {
        PreSettingFX( spProperty, pFX, fAlpha );
        Render( pd3dDevice, pFX, pBoneCur );
    }
	//--------------------------------------------------- Draw

	if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_TRANSLUCENT )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			dwSrcBlend );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		dwDestBlend );
	}
	else if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_MASKED )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	}

    if ( spProperty->m_spVMFX->GetTwoSideSeperatePass() )
    {
        DWORD dwCullMode;
        pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
        pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

        D3DXVECTOR4 vLightDir;
        vLightDir.x = m_vLightDir.x;
        vLightDir.y = m_vLightDir.y;
        vLightDir.z = m_vLightDir.z;
        vLightDir.w = -1.f;
        pFX->SetVector( "g_vLightDir", &vLightDir );

        Render( pd3dDevice, pFX, pBoneCur );

        pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );
    }

    // ���� ����
    pd3dDevice->SetFVF( VERTEX::FVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );

    SAFE_RELEASE( pVB );
    SAFE_RELEASE( pIB );
}

HRESULT DxEffCharVisualMaterial::Render( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, LPD3DXBONECOMBINATION pBoneCur )
{
    HRESULT hr(S_OK);

    DWORD dwPassIndex(0);
    if ( m_rSKINEFFDATA->m_rLevelData )
    {
        if ( m_rSKINEFFDATA->m_rLevelData->m_bLevel )
            ++dwPassIndex;
        if ( m_rSKINEFFDATA->m_rLevelData->m_bLevelFlow )
            ++dwPassIndex;
    }

    UINT numPasses;
    V( pFX->Begin( &numPasses, 0 ) ); 
    if( dwPassIndex < numPasses )
    {
        V( pFX->BeginPass( dwPassIndex ) );
        {
            pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
									            0,
									            pBoneCur->VertexStart,
									            pBoneCur->VertexCount,
									            pBoneCur->FaceStart*3,
									            pBoneCur->FaceCount );
        }
        V( pFX->EndPass() );
    }
    V( pFX->End() );

    return hr;
}

void DxEffCharVisualMaterial::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwAttrib, TEXTURE_TYPE_FLAG emTexType, const D3DXMATRIX& matStaticLinkBone )
{
	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

    DxTangentMesh* pTangentMesh = m_pmcMesh->m_spTangentMesh.get();
    if ( !pTangentMesh )
        return;

    const D3DXATTRIBUTERANGE& sAttributeTable = pTangentMesh->GetAttrib()[dwAttrib];
    DWORD dwAttribID = sAttributeTable.AttribId;

    if ( dwAttribID >= m_vecProperty.size() )
        return;

    std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = m_vecProperty[dwAttribID];

    if ( !spProperty->m_spVMFX.get() )
        return;

	if ( (emTexType == EMTTF_OPAQUE) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_OPAQUE) )
		return;
	else if ( (emTexType == EMTTF_ALPHA_HARD) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_MASKED) )
		return;
	else if ( (emTexType == EMTTF_ALPHA_SOFT) && (spProperty->m_spVMFX->GetBlendMode() != VM_BLENDMODE_TRANSLUCENT) )
		return;

    ID3DXEffect* pFX = spProperty->m_spVMFX->GetFX();
    if ( !pFX )
        return;

    if ( g_bCHAR_EDIT_PIECE_RENDER_FIXED )
    {
        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );

        D3DXMATRIX matWVP;
        D3DXMatrixMultiply( &matWVP, &matIdentity, &m_matVP );

        pFX->SetMatrixTranspose( "g_matWorld",	&matIdentity );
        pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
    }
    else
    {
        D3DXMATRIX matWVP;
        D3DXMatrixMultiply( &matWVP, &matStaticLinkBone, &m_matVP );

        pFX->SetMatrixTranspose( "g_matWorld",	&matStaticLinkBone );
        pFX->SetMatrixTranspose( "g_matWVP",	&matWVP );
    }

    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( NS_VM_MANAGER::g_pDCRT_SKIN_OBJECT );

	pd3dDevice->SetStreamSource( 0, pTangentMesh->GetVB(), 0, sizeof(vm::VERTEXNORTEXTANGENT_COMPRESS) );
	pd3dDevice->SetIndices ( pTangentMesh->GetIB() );

	float fAlpha(1.f);
	DWORD dwAlphaTestEnable, dwAlphaBlendEnable, dwSrcBlend, dwDestBlend;
	if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_TRANSLUCENT )
	{
		fAlpha = m_pmcMesh->pMaterials[dwAttrib].MatD3D.Diffuse.a;
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
		pd3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlendEnable );
		pd3dDevice->GetRenderState( D3DRS_SRCBLEND,			&dwSrcBlend );
		pd3dDevice->GetRenderState( D3DRS_DESTBLEND,		&dwDestBlend );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
	}
	else if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_MASKED )
	{
		pd3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE,	&dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE );
	}

	//--------------------------------------------------- Draw
    {
        PreSettingFX( spProperty, pFX, fAlpha );
        Render( pd3dDevice, pFX, sAttributeTable );
    }
	//--------------------------------------------------- Draw

	if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_TRANSLUCENT )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
		pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	dwAlphaBlendEnable );
		pd3dDevice->SetRenderState( D3DRS_SRCBLEND,			dwSrcBlend );
		pd3dDevice->SetRenderState( D3DRS_DESTBLEND,		dwDestBlend );
	}
	else if ( spProperty->m_spVMFX->GetBlendMode() == VM_BLENDMODE_MASKED )
	{
		pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	dwAlphaTestEnable );
	}

    if ( spProperty->m_spVMFX->GetTwoSideSeperatePass() )
    {
        DWORD dwCullMode;
        pd3dDevice->GetRenderState( D3DRS_CULLMODE, &dwCullMode );
        pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

        D3DXVECTOR4 vLightDir;
        vLightDir.x = m_vLightDir.x;
        vLightDir.y = m_vLightDir.y;
        vLightDir.z = m_vLightDir.z;
        vLightDir.w = -1.f;
        pFX->SetVector( "g_vLightDir", &vLightDir );

        Render( pd3dDevice, pFX, sAttributeTable );

        pd3dDevice->SetRenderState( D3DRS_CULLMODE, dwCullMode );
    }

    // ���� ����
    pd3dDevice->SetFVF( VERTEX::FVF );
    pd3dDevice->SetVertexShader( NULL );
    pd3dDevice->SetPixelShader( NULL );
}

HRESULT DxEffCharVisualMaterial::Render( LPDIRECT3DDEVICEQ pd3dDevice, ID3DXEffect* pFX, const D3DXATTRIBUTERANGE& sAttributeTable )
{
    HRESULT hr(S_OK);

    DWORD dwPassIndex(0);
    if ( m_rSKINEFFDATA->m_rLevelData )
    {
        if ( m_rSKINEFFDATA->m_rLevelData->m_bLevel )
            ++dwPassIndex;
        if ( m_rSKINEFFDATA->m_rLevelData->m_bLevelFlow )
            ++dwPassIndex;
    }

    UINT numPasses;
    V( pFX->Begin( &numPasses, 0 ) ); 
    if( dwPassIndex < numPasses )
    {
        V( pFX->BeginPass( dwPassIndex ) );
        {
            pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 
									            0,
									            sAttributeTable.VertexStart,
									            sAttributeTable.VertexCount,
									            sAttributeTable.FaceStart*3,
									            sAttributeTable.FaceCount );
        }
        V( pFX->EndPass() );
    }
    V( pFX->End() );

    return hr;
}

void DxEffCharVisualMaterial::ReloadVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !IsExist_Copy_pmcMesh( pd3dDevice ) )
		return;

    VEC_TSTRING    m_vecTextureName;
    for ( DWORD z=0; z<m_vecProperty.size(); ++z )
    {
        std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = m_vecProperty[z];

        // ���
        m_vecTextureName.clear();
        if ( spProperty->m_spVMFX.get() )
        {
            for ( DWORD i=0; i<spProperty->m_spVMFX->GetTextureSize(); ++i )
            {
                m_vecTextureName.push_back( spProperty->m_spVMFX->GetTextureName(i) );
            }
        }

        // �ε�
        if ( m_pmcMesh->pSkinInfo )
        {
            spProperty->m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, spProperty->m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_VS );
        }
        else
        {
            spProperty->m_spVMFX = NS_VM_MANAGER::Load( pd3dDevice, spProperty->m_strVisualMaterial, NS_VM_MANAGER::EMRT_SKIN_OBJECT );
        }

        // ���õ� �ؽ��� ������ ���� ������, �ؽ��� ������ �����ϵ��� �Ѵ�.
        BOOL bTextureReset = FALSE;
        if ( spProperty->m_vecTextureResource.size() != spProperty->m_spVMFX->GetTextureSize() )
        {
            bTextureReset = TRUE;
        }
        else
        {
            // ���� �ؽ��� �̸��� ����Ǿ��ٸ�, �ؽ��� ������ �����ϵ��� �Ѵ�.
            for ( DWORD i=0; i<spProperty->m_spVMFX->GetTextureSize(); ++i )
            {
                if ( m_vecTextureName[i] != spProperty->m_spVMFX->GetTextureName(i) &&
                    m_vecTextureName[i] == spProperty->m_vecTextureResource[i].m_strName )
                {
                    bTextureReset = TRUE;
                }
            }
        }
        
        // �ؽ��� ����
        if ( bTextureReset )
        {
            // Reload ������ spProperty->CleanUp()�� ���� ����.
            // �� �Լ��� ������ �����µ� ReleaseTexture ������ ������ �����ϰ� �ִ� m_pTexture �����Ͱ� �������� �ִ�.
            // Tool �̶� ������ �������� ���ε��� �Ѵ�.
            spProperty->m_vecTextureResource.clear();

            DWORD dwSize = spProperty->m_spVMFX->GetTextureSize();
            for ( DWORD i=0; i<dwSize; ++i )
            {
                vm::NAME_TEXTURE sNameTexture;
                sNameTexture.m_strName = spProperty->m_spVMFX->GetTextureName(i);

                sNameTexture.m_textureRes = ic::TextureManagerInterface::GetInstance()->LoadTexture(
                    sNameTexture.m_strName,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    0,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_NONE,
                    TEXTURE_VISUALMATERIAL,
                    false,
					FALSE );

                spProperty->m_vecTextureResource.push_back( sNameTexture );
            }
        }
    }
}

HRESULT DxEffCharVisualMaterial::LoadFile_0101( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    DWORD dwSize;

    BOOL bSkinMesh;
    SFile >> bSkinMesh;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);
	    spProperty->Load( pd3dDevice, SFile, bSkinMesh );
        m_vecProperty.push_back( spProperty );
    }

	return S_OK;
}

HRESULT DxEffCharVisualMaterial::LoadFile_0100( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    DWORD dwSize;

    BOOL bSkinMesh;
    SFile >> bSkinMesh;

    SFile >> dwSize;
    for ( DWORD i=0; i<dwSize; ++i )
    {
        std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL> spProperty = std::tr1::shared_ptr<EFFCHAR_PROPERTY_VISUAL_MATERIAL>(new EFFCHAR_PROPERTY_VISUAL_MATERIAL);
	    spProperty->Load_100( pd3dDevice, SFile, bSkinMesh );
        m_vecProperty.push_back( spProperty );
    }

	return S_OK;
}

HRESULT	DxEffCharVisualMaterial::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case VERSION:	
        LoadFile_0101( SFile, pd3dDevice );	
        break;

    case 0x0100:	
        LoadFile_0100( SFile, pd3dDevice );	
        break;

	default:
		{
			//	Note : ������ Ʋ�� ��쿡�� ���Ͽ� ������ DATA������ �Ǵ� ��� �۾��� ����.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		
			return E_FAIL;
		}
		break;
	};

	//	Note : Device �ڿ��� �����Ѵ�.
	HRESULT hr(S_OK);
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT	DxEffCharVisualMaterial::SaveFile ( sc::BaseStream &SFile )
{
	SFile << TYPEID;
	SFile << VERSION;

	SFile.BeginBlock();
	{
        if ( m_pmcMesh->pSkinInfo )
        {
            SFile << TRUE;
        }
        else
        {
            SFile << FALSE;
        } 

        SFile << static_cast< DWORD >( m_vecProperty.size() );
        for ( DWORD i=0; i<m_vecProperty.size(); ++i )
        {
		    m_vecProperty[i]->Save( SFile );
        }
	}
	SFile.EndBlock();

	return S_OK;
}

DWORD DxEffCharVisualMaterial::GetFlag ()
{
	if ( RENDERPARAM::g_emCharacterQulity >= TnL_CHAR_PS_2_0 )
	{
		return EMECF_VISUAL_MATERIAL;
	}
	else
	{
		return 0;
	}
}

BOOL DxEffCharVisualMaterial::IsExist_Copy_pmcMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// ���� �ִٸ� �� �۾����� �ʴ´�.
	if ( m_pmcMesh )
		return TRUE;

	if ( m_pCharPart )
	{
		m_pmcMesh = m_pCharPart->m_rMeshContainer;
	}
	else if ( m_pSkinPiece )
	{
		m_pmcMesh = m_pSkinPiece->m_pmcMesh;
	}

	if ( !m_pmcMesh )
		return FALSE;

	if ( m_pmcMesh->pSkinInfo )
	{
		if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
		{
			m_pmcMesh->GenerateSkinnedMeshNormalMap( pd3dDevice );

			// m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh NULL �� �߻��ϸ� �ȵ�����, BugTrap �� �߻��ѵ� ���� ��ȿ�� �˻���.
			if ( !m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh )
				return FALSE;

			NS_VM_MANAGER::CreateTangentSkinMeshVS( pd3dDevice, &m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
			NS_VM_MANAGER::CompressTangentSkinMeshVS( pd3dDevice, &m_pmcMesh->m_sMeshDataHLSL.m_MeshData.pMesh );
		}
	}
	else
	{
		if( !m_pmcMesh->m_spTangentMesh.get() )
		{
			m_pmcMesh->m_spTangentMesh = std::tr1::shared_ptr<DxTangentMesh>(new DxTangentMesh);
			m_pmcMesh->m_spTangentMesh->CreateSkinMeshObject( pd3dDevice, m_pmcMesh->MeshData.pMesh );

			m_pmcMesh->m_dwNumAttributeGroups = m_pmcMesh->m_spTangentMesh->GetAttribSize();
		}
	}

	return TRUE;
}