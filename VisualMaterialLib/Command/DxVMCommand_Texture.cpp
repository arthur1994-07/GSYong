#include "stdafx.h"

#include "../../InternalCommonLib/Interface/TextureManagerInterface.h"
#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Edit/DxEditVM.h"
#include "./NSVMCommand.h"

#include "DxVMCommand_Texture.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------



//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureSample
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_TextureSample::DxVMCommand_TextureSample(void)
    : m_strTexture(_T(""))
    , m_bCubeTexture(FALSE)
{ 
    m_emType = VMC_TEXTURESAMPLE;
    m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
    m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_TextureSample::~DxVMCommand_TextureSample(void)
{
}

BOOL DxVMCommand_TextureSample::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    if( !DxVMCommand::Create( pd3dDevice, vPos ) )
        return FALSE;

    m_spEditVM_Title->Create( pd3dDevice, TRUE );
    m_spEditVM_Title->AddChannel( _T("TextureSample") );

    m_spEditVM_Command->Create( pd3dDevice, FALSE );
    m_spEditVM_Command->AddChannel(_T("UVs"));
    m_spEditVM_Command->AddOutputColor();
    m_spEditVM_Command->AddImage();

	Refresh();

    return TRUE;
}

void DxVMCommand_TextureSample::Refresh()
{
    // CCustomItemFileBox::OnValueChanged() 에서 호출 되면,
    // 이름이 바뀐상태이고, m_texrureRes2D는 이전꺼라서 ReleaseTexture()하면 엉뚱한것을 삭제하게 된다.
    //ic::TextureManagerInterface::GetInstance()->ReleaseTexture( m_strTexture.c_str(), m_texrureRes2D );
    //ic::TextureManagerInterface::GetInstance()->ReleaseTexture( m_strTexture.c_str(), m_textureResCube );

    if ( m_bCubeTexture )
    {
        m_textureResCube = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            m_strTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_CUBE,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
            false,
			FALSE );
    }
    else
    {
        m_texrureRes2D = ic::TextureManagerInterface::GetInstance()->LoadTexture(
            m_strTexture,
            false,
            TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
            0,
            D3DFMT_UNKNOWN,
            TEXTURE_RESIZE_NONE,
            TEXTURE_VISUALMATERIAL,
            false,
			FALSE );
    }
}

std::string DxVMCommand_TextureSample::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
                                                    DWORD dwOutputIndex, 
                                                    vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
                                                    vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                                                    vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                                                    vm::VEC_PIXEL_VECTOR& vecPixelVector,
                                                    vm::EM_WHERE_CODE& emWhereCode )
{
    std::string strTexType = "(tex2D(";
    std::string strTextureType = "Texture2D_";
    std::string strStringFX_A = "In.TexCoord.xy";
    if ( m_bCubeTexture )
    {
        strTexType = "(texCUBE(";
        strTextureType = "TextureCube_";
        strStringFX_A = "In.TexCoord.xyz";
    }

    vm::EM_WHERE_CODE emWhereCode_A(vm::EMWC_NEUTRAL);
    MAP_VM_COMMAND_CITER iter_0;
    if ( m_vecLineData.size() == 1 )
    {
        iter_0 = mapVM_Command.find( m_vecLineData[0]->m_dwID_Output );
        if ( iter_0 != mapVM_Command.end() )
        {
            strStringFX_A = (*iter_0).second->MakeStringFX( mapVM_Command, 
                                                            m_vecLineData[0]->m_nIndexBox_Output, 
                                                            vecTextureResource, 
                                                            vecCubeTextureResource,
                                                            vecPixelScalar, 
                                                            vecPixelVector,
                                                            emWhereCode_A );
        }

        // Scalar가 종료된다.
        if ( ( emWhereCode_A == vm::EMWC_SCALAR ) )
        {
            // 스칼라 이름으로 작업
            strStringFX_A = NSVMCommand::GetPixelScalarName( (*iter_0).second.get(), 
                                                                mapVM_Command, 
                                                                vecPixelScalar );
        }
        // Vector가 종료된다.
        else if ( emWhereCode == vm::EMWC_VECTOR )
        {
            // Vector 이름으로 작업
            strStringFX_A = NSVMCommand::GetUniformPixelVectorName( (*iter_0).second.get(), 
                                                                    mapVM_Command,
                                                                    vecPixelVector );
        }
    }

    // emWhereCode 확인
    {
        emWhereCode = vm::EMWC_HLSL;
    }


    if ( m_bCubeTexture )
    {
        // nIndex에 넣을 수치 넣는다.
        size_t nIndex = vecCubeTextureResource.size();
        if ( nIndex > 7 )
            return "";

        // 같은 텍스쳐가 있을 경우 Index를 같은 것으로 설정
        BOOL bSameTexture(FALSE);
        for ( DWORD i=0; i<vecCubeTextureResource.size(); ++i )
        {
            if ( vecCubeTextureResource[i].m_strName == m_strTexture )
            {
                nIndex = i;
                bSameTexture = TRUE;
            }
        }

        // 만약 같은 텍스쳐가 있다면 nIndex가 size 보다 적을것이다.
        if ( nIndex >= vecCubeTextureResource.size() )
        {
            vm::NAME_CUBE_TEXTURE    sNameTexture;
            sNameTexture.m_strName = m_strTexture;
            sNameTexture.m_textureRes = m_textureResCube;
            vecCubeTextureResource.push_back( sNameTexture );
        }

        char c1='0';
        c1 = static_cast<char>( c1 + nIndex );

        strTextureType += c1;
    }
    else
    {
        // nIndex에 넣을 수치 넣는다.
        size_t nIndex = vecTextureResource.size();
        if ( nIndex > 7 )
            return "";

        // 같은 텍스쳐가 있을 경우 Index를 같은 것으로 설정
        BOOL bSameTexture(FALSE);
        for ( DWORD i=0; i<vecTextureResource.size(); ++i )
        {
            if ( vecTextureResource[i].m_strName == m_strTexture )
            {
                nIndex = i;
                bSameTexture = TRUE;
            }
        }

        // 만약 같은 텍스쳐가 있다면 nIndex가 size 보다 적을것이다.
        if ( nIndex >= vecTextureResource.size() )
        {
            vm::NAME_TEXTURE    sNameTexture;
            sNameTexture.m_strName = m_strTexture;
            sNameTexture.m_textureRes = m_texrureRes2D;
            vecTextureResource.push_back( sNameTexture );
        }

        char c1='0';
        c1 = static_cast<char>( c1 + nIndex );

        strTextureType += c1;
    }

    switch ( dwOutputIndex )
    {
    case 0:     return strTexType + strTextureType + "," + strStringFX_A + ")).xyzw";
    case 1:     return strTexType + strTextureType + "," + strStringFX_A + ")).xxxx";
    case 2:     return strTexType + strTextureType + "," + strStringFX_A + ")).yyyy";
    case 3:     return strTexType + strTextureType + "," + strStringFX_A + ")).zzzz";
    case 4:     return strTexType + strTextureType + "," + strStringFX_A + ")).wwww";
    default:    return "";
    };
}

void DxVMCommand_TextureSample::Save( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Save( pSFile );

    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << m_bCubeTexture;
        (*pSFile) << m_strTexture;
    }
    pSFile->EndBlock();
}
 
void DxVMCommand_TextureSample::Load( ic::SerialFileInterface* pSFile )
{
    DxVMCommand::Load( pSFile );

    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        (*pSFile) >> m_bCubeTexture;
        (*pSFile) >> m_strTexture;
    }
    else if ( dwVer == 0x101 )
    {
        (*pSFile) >> m_bCubeTexture;
        (*pSFile) >> m_strTexture;
        (*pSFile) >> m_strDesc;
    }
    else if ( dwVer == 0x100 )
    {
        (*pSFile) >> m_strTexture;
        (*pSFile) >> m_strDesc;
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }

    Refresh();
}


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand_TextureRefract
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand_TextureRefract::DxVMCommand_TextureRefract(void)
{ 
	m_emType = VMC_TEXTURE_REFRACT;
	m_fBaseWidth = NSVISUAL_MATERIAL::TITLE_COMMAND_WIDTH;
	m_fBaseHeight = NSVISUAL_MATERIAL::TITLE_COMMAND_HEIGHT;
}

DxVMCommand_TextureRefract::~DxVMCommand_TextureRefract(void)
{
}

BOOL DxVMCommand_TextureRefract::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
	if( !DxVMCommand::Create( pd3dDevice, vPos ) )
		return FALSE;

	m_spEditVM_Title->Create( pd3dDevice, TRUE );
	m_spEditVM_Title->AddChannel( _T("TextureRefract") );

	m_spEditVM_Command->Create( pd3dDevice, FALSE );
	m_spEditVM_Command->AddChannel(_T("UVs"));
	m_spEditVM_Command->AddOutputColor();
	m_spEditVM_Command->AddImage();

	Refresh();

	return TRUE;
}

void DxVMCommand_TextureRefract::Refresh()
{
}

std::string DxVMCommand_TextureRefract::MakeStringFX( const MAP_VM_COMMAND& mapVM_Command, 
													DWORD dwOutputIndex, 
													vm::VEC_TEXTURE_RESOURCE& vecTextureResource,
													vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
													vm::VEC_PIXEL_VECTOR& vecPixelScalar,
													vm::VEC_PIXEL_VECTOR& vecPixelVector,
													vm::EM_WHERE_CODE& emWhereCode )
{
	return "";
}

void DxVMCommand_TextureRefract::Save( ic::SerialFileInterface* pSFile )
{
	DxVMCommand::Save( pSFile );

	(*pSFile) << VERSION;
	pSFile->BeginBlock();
	{
	}
	pSFile->EndBlock();
}

void DxVMCommand_TextureRefract::Load( ic::SerialFileInterface* pSFile )
{
	DxVMCommand::Load( pSFile );

	DWORD dwVer, dwBufferSize;
	(*pSFile) >> dwVer;
	(*pSFile) >> dwBufferSize;

	if ( dwVer == VERSION )
	{
	}
	else
	{
		pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
	}

	Refresh();
}