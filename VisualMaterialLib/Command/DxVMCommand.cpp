#include "stdafx.h"

#include <sstream>

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"

#include "../Command_EX/DxVMCommand_EX.h"
#include "../Preview/NSVM_PREVIEW.h"
#include "../HLSL/DxVMCommandFX.h"
#include "../Edit/DxEditVM.h"
#include "./DxVMCommand_Constants.h"
#include "./DxVMCommand_Coordinates.h"
#include "./DxVMCommand_Math.h"
#include "./DxVMCommand_Parameters.h"
#include "./DxVMCommand_Texture.h"
#include "./DxVMCommand_Utility.h"
#include "./DxVMCommand_VectorOps.h"
#include "./DxVMCommand_Vectors.h"
#include "./DxVMCommand_Core.h"
#include "./NSVMCommand.h"

#include "DxVMCommand.h"

// ----------------------------------------------------------------------------
#include "../../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------- DxVMCommand
//----------------------------------------------------------------------------------------------------------------------------
DxVMCommand::DxVMCommand(void)
    : m_pd3dDevice(NULL)
    , m_fTime(0.f)
    , m_dwID(0)
    , m_vPos(0.f,0.f)
    , m_strDesc(_T(""))
{
    m_spEditVM_Title = std::tr1::shared_ptr<DxEditVM>(new DxEditVM);
    m_spEditVM_Command = std::tr1::shared_ptr<DxEditVM>(new DxEditVM);
    m_spVMCommandFX = std::tr1::shared_ptr<DxVMCommandFX>(new DxVMCommandFX);
}

DxVMCommand::~DxVMCommand(void)
{
    CleanUp();
}

std::tr1::shared_ptr<DxVMCommand> DxVMCommand::NEW_COMMAND_STATIC( VMC_TYPE emType )
{
    // Note : Add Command
    std::tr1::shared_ptr<DxVMCommand> spNew;
    switch ( emType )
    {
    case VMC_CORE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Core);
        }
        break;

    case VMC_CONSTANT:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Constant);
        }
        break;

    case VMC_CONSTANT2VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Constant2Vector);
        }
        break;

    case VMC_CONSTANT3VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Constant3Vector);
        }
        break;

    case VMC_CONSTANT4VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Constant4Vector);
        }
        break;

    case VMC_PANNER:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Panner);
        }
        break;

    case VMC_ROTATOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Rotator);
        }
        break;

    case VMC_TEXTURE_COORDINATE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_TextureCoordinate);
        }
        break;

    case VMC_ADD:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Add);
        }
        break;

    case VMC_MULTIPLY:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Multiply);
        }
        break;

    case VMC_SINE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Sine);
        }
        break;

    case VMC_SUBTRACT:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Subtract);
        }
        break;

    case VMC_VECTOR_PARAMETER:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_VectorParameter);
        }
        break;

    case VMC_TEXTURESAMPLE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_TextureSample);
        }
        break;

	case VMC_TEXTURE_REFRACT:
		{
			spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_TextureRefract);
		}
		break;

    case VMC_CLAMP:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Clamp);
        }
        break;

    case VMC_LINEAR_INTERPOLATE:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_LinearInterpolate);
        }
        break;

    case VMC_TIME:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Time);
        }
        break;

    case VMC_FRESNEL:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_Fresnel);
        }
        break;

    case VMC_REFLECTION_VECTOR:
        {
            spNew = std::tr1::shared_ptr<DxVMCommand>(new DxVMCommand_ReflectionVector);
        }
        break;
    };

    return spNew;
}

void DxVMCommand::CleanUp()
{
    OnLostDevice();

    m_vecLineData.clear();
}

BOOL DxVMCommand::Create( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXVECTOR2& vPos )
{
    m_vPos = vPos;
    m_pd3dDevice = pd3dDevice;

    return OnResetDevice();
}

BOOL DxVMCommand::OnResetDevice()
{
    OnLostDevice();

    m_spVMCommandFX->OnResetDevice();

    return TRUE;
}

void DxVMCommand::OnLostDevice()
{
    m_spVMCommandFX->OnLostDevice();
}

void DxVMCommand::FrameMove( float fTime )
{
    m_fTime = fTime;
}

void DxVMCommand::Render( VM_CAN_SELECT_INDEX& sCanSelectIndex, const D3DXVECTOR2& vMousePt )
{
    vm::MAP_PARAMETER_NAME_DATA mapParamaterNameData;

    // SetTexture 작업
    ID3DXEffect* pFX = m_spVMCommandFX->GetFX();
    if ( pFX )
    {
        // Texture_0 ... 설정
        {
            char c1='0';
            std::string strTextureORIG = "Texture_";
            std::string strTexture = "";
            for ( size_t i=0; i<m_vecTextureResource.size(); ++i )
            {
                strTexture = strTextureORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTexture.c_str(), m_vecTextureResource[i].m_textureRes.GetTexture() );
            }
        }

        // TextureCube_0 ... 설정
        {
            char c1='0';
            std::string strTextureCubeORIG = "CubeTexture_";
            std::string strTextureCube = "";
            for ( size_t i=0; i<m_vecCubeTextureResource.size(); ++i )
            {
                strTextureCube = strTextureCubeORIG + static_cast<char>( c1 + i );
                pFX->SetTexture( strTextureCube.c_str(), m_vecCubeTextureResource[i].m_textureRes.GetCubeTexture() );
            }
        }

        // g_fUniformPixelScalar_0 ... 설정
        {
            char c1='0';
            std::string strUniformPixelScalarORIG = "g_fUniformPixelScalar_";
            std::string strUniformPixelScalar = "";
            for ( size_t i=0; i<m_vecPixelScalar.size(); ++i )
            {
                m_vecPixelScalar[i]->FrameMove( m_fTime );

                strUniformPixelScalar = strUniformPixelScalarORIG + static_cast<char>( c1 + i );
                pFX->SetFloat( strUniformPixelScalar.c_str(), fabsf( m_vecPixelScalar[i]->GetPixelVector( mapParamaterNameData ).x ) );
            }
        }

        // g_vUniformPixelVector_0 ... 설정
        {
            char c1='0';
            std::string strParameterORIG = "g_vUniformPixelVector_";
            std::string strParameter = "";
            for ( size_t i=0; i<m_vecPixelVector.size(); ++i )
            {
                m_vecPixelVector[i]->FrameMove( m_fTime );

                strParameter = strParameterORIG + static_cast<char>( c1 + i );
                pFX->SetVector( strParameter.c_str(), &m_vecPixelVector[i]->GetPixelVector( mapParamaterNameData ) );
            }
        }
    }

    // Width를 구한다.
    float fWidth(m_fBaseWidth);
    fWidth = m_spEditVM_Title->GetCalculateWidth( fWidth );
    fWidth = m_spEditVM_Command->GetCalculateWidth( fWidth );

    D3DXVECTOR2 vPos(m_vPos);
    m_spEditVM_Title->Render
    ( 
        m_dwID,
        sCanSelectIndex,
        vPos, 
        fWidth, 
        m_fBaseHeight, 
        DEVM_CENTER_V | DEVM_CENTER_H,
        vMousePt,
        NULL
    );

    vPos.y -= m_fBaseHeight;
    vPos.y -= NSVISUAL_MATERIAL::TITLE_HEIGHT_SMALL;
    m_spEditVM_Command->Render
    ( 
        m_dwID,
        sCanSelectIndex,
        vPos, 
        fWidth, 
        m_fBaseHeight, 
        DEVM_RIGHT,
        vMousePt,
        pFX
    );
}

const VEC_VMLINEDATA& DxVMCommand::GetVMLineData()
{
    return m_vecLineData;
}

void DxVMCommand::SetID( DWORD dwID )
{
    m_dwID = dwID;
}

DWORD DxVMCommand::GetID()
{
    return m_dwID;
}

VMC_TYPE DxVMCommand::GetType()
{
    return m_emType;
}

void DxVMCommand::LineLink( int nIndexBox, const VM_CAN_SELECT_INDEX& sCanSelectIndex )
{    
    for ( VEC_VMLINEDATA_ITER iter = m_vecLineData.begin(); iter!=m_vecLineData.end(); )
    {
        // 현재 Input에 들어오는 것이 있는가~?
        // 현재 들어오는 것이 있다면 삭제시켜놓는다.
        if ( (*iter)->m_nIndexBox_Input == nIndexBox )
        {
            //SAFE_DELETE( (*iter) );
            iter = m_vecLineData.erase( iter );
        }
        else
        {
            ++iter;
        }
    }

    std::tr1::shared_ptr<DxVMLineData> spNew = std::tr1::shared_ptr<DxVMLineData>(new DxVMLineData);
    spNew->m_nIndexBox_Input     = nIndexBox;
    spNew->m_dwID_Output         = sCanSelectIndex.m_dwID;
    spNew->m_nIndexBox_Output    = sCanSelectIndex.m_nIndexBox;
    m_vecLineData.push_back( spNew );
}

void DxVMCommand::GetPointInput( D3DXVECTOR3& vPoint, int nIndexBox )
{
    m_spEditVM_Command->GetPointInput( vPoint, nIndexBox );
}

void DxVMCommand::GetPointOutput( D3DXVECTOR3& vPoint, int nIndexBox )
{
    m_spEditVM_Command->GetPointOutput( vPoint, nIndexBox );
}

void DxVMCommand::SetYellowBox( VM_CAN_SELECT_INDEX& sCanSelectIndex )
{
    m_spEditVM_Command->SetYellowBox( sCanSelectIndex );
}

void DxVMCommand::DeleteLineLink( const VM_CAN_SELECT_INDEX& sSelectIndexBox )
{
    VEC_VMLINEDATA_ITER iter = m_vecLineData.begin();
    for ( ; iter!=m_vecLineData.end(); )
    {
        switch( sSelectIndexBox.m_emMode )
        {
        case CAN_SELECT_INPUT:
            if ( m_dwID == sSelectIndexBox.m_dwID &&
                (*iter)->m_nIndexBox_Input == sSelectIndexBox.m_nIndexBox )
            {
                //SAFE_DELETE( (*iter) );
                iter = m_vecLineData.erase( iter );
            }
            else
            {
                ++iter;
            }
            break;

        case CAN_SELECT_OUTPUT:
            if ( (*iter)->m_dwID_Output == sSelectIndexBox.m_dwID &&
                (*iter)->m_nIndexBox_Output == sSelectIndexBox.m_nIndexBox )
            {
                //SAFE_DELETE( (*iter) );
                iter = m_vecLineData.erase( iter );
            }
            else
            {
                ++iter;
            }
            break;

        default:
            ++iter;
            break;
        };
    }
}

void DxVMCommand::DeleteLineLink( DWORD dwID )
{
    VEC_VMLINEDATA_ITER iter = m_vecLineData.begin();
    for ( ; iter!=m_vecLineData.end(); )
    {
        if ( m_dwID == dwID )
        {
            iter = m_vecLineData.erase( iter );
        }
        else if ( (*iter)->m_dwID_Output == dwID )
        {
            iter = m_vecLineData.erase( iter );
        }
        else
        {
            ++iter;
        }
    }
}

void DxVMCommand::SetSelectBaseBox( BOOL bSelect )
{
    m_spEditVM_Title->SetSelectBaseBox( bSelect );
    m_spEditVM_Command->SetSelectBaseBox( bSelect );
}

void DxVMCommand::Move( const D3DXVECTOR2& vMove )
{
    m_vPos += vMove;
}

BOOL DxVMCommand::IsSelectCommandAtBox( const D3DXVECTOR2& vMin, const D3DXVECTOR2& vMax )
{
    if( m_spEditVM_Title->IsSelectAtBox( vMin, vMax ) )
        return TRUE;

    if( m_spEditVM_Command->IsSelectAtBox( vMin, vMax ) )
        return TRUE;

    return FALSE;
}

BOOL DxVMCommand::MakeFX( const MAP_VM_COMMAND& mapVM_Command )
{
    return MakeFX( m_spVMCommandFX.get(), mapVM_Command, m_vecTextureResource, m_vecCubeTextureResource, m_vecPixelScalar, m_vecPixelVector, VMFX_COMMAND );
}

BOOL DxVMCommand::MakeFX( DxVMCommandFX* pVMCommandFX,
                         const MAP_VM_COMMAND& mapVM_Command, 
                         vm::VEC_TEXTURE_RESOURCE& vecTextureResource, 
                         vm::VEC_CUBE_TEXTURE_RESOURCE& vecCubeTextureResource,
                         vm::VEC_PIXEL_VECTOR& vecPixelScalar,
                         vm::VEC_PIXEL_VECTOR& vecPixelVector,
                         VMFX emModeFX )
{
    // 정리는 필요.
    vecTextureResource.clear();
    vecCubeTextureResource.clear();
    vecPixelScalar.clear();
    vecPixelVector.clear();

    COMMENT_FX sCommentFX;
    std::string strCommandFX;

    vm::EM_WHERE_CODE emWhereCode;
    strCommandFX = MakeStringFX( mapVM_Command, 0, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
    if ( !strCommandFX.empty() )
    {
        sCommentFX.m_strDiffuseFX = "    color.xyzw = ";
        sCommentFX.m_strDiffuseFX += strCommandFX;
        sCommentFX.m_strDiffuseFX += "; \r\n";
    }

    // Scalar라면,
    if ( emWhereCode == vm::EMWC_SCALAR )
    {
        // 스칼라 이름으로 작업
        sCommentFX.m_strDiffuseFX = "    color.xyzw = ";
        sCommentFX.m_strDiffuseFX += NSVMCommand::GetPixelScalarName( this, 
                                                                        mapVM_Command,
                                                                        vecPixelScalar );
        sCommentFX.m_strDiffuseFX += "; \r\n";
    }
    // Vector라면,
    else if ( emWhereCode == vm::EMWC_VECTOR )
    {
        // Vector 이름으로 작업
        sCommentFX.m_strDiffuseFX = "    color.xyzw = ";
        sCommentFX.m_strDiffuseFX += NSVMCommand::GetUniformPixelVectorName( this, 
                                                                            mapVM_Command,
                                                                            vecPixelVector );
        sCommentFX.m_strDiffuseFX += "; \r\n";
    }

    // 현재는 DxVMCommand_Core 일 경우만 sCommentFX 에 정보가 셋팅된다.
    {
        SetCommentFX( sCommentFX );
    }

    // Command 일 경우는 연산할 필요가 없다.
	// 여기 1~9 까지의 숫자는 DxVMCommand_Core::Create 에 있는 순서와 같아야한다.
    if ( emModeFX != VMFX_COMMAND )
    {
        strCommandFX = MakeStringFX( mapVM_Command, 1, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strEmissiveFX = "    color = ";
            sCommentFX.m_strEmissiveFX += strCommandFX;
            sCommentFX.m_strEmissiveFX += "; \r\n";
        }

		strCommandFX = MakeStringFX( mapVM_Command, 2, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
		if ( !strCommandFX.empty() )
		{
			sCommentFX.m_strStarGlowFX = "    color = ";
			sCommentFX.m_strStarGlowFX += strCommandFX;
			sCommentFX.m_strStarGlowFX += "; \r\n";
		}

        strCommandFX = MakeStringFX( mapVM_Command, 3, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strSpecularFX = "    Specular = ";
            sCommentFX.m_strSpecularFX += strCommandFX;
            sCommentFX.m_strSpecularFX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 4, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strShininessFX = "    ShininessMap = ";
            sCommentFX.m_strShininessFX += strCommandFX;
            sCommentFX.m_strShininessFX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 5, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strOpacityFX = "    color = ";
            sCommentFX.m_strOpacityFX += strCommandFX;
            sCommentFX.m_strOpacityFX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 6, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strNormalFX = "    color.xyzw = ";
            sCommentFX.m_strNormalFX += strCommandFX;
            sCommentFX.m_strNormalFX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 7, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strLevelFX = "    fLevel = ";
            sCommentFX.m_strLevelFX += strCommandFX;
            sCommentFX.m_strLevelFX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 8, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strUserColor1FX = "    color = ";
            sCommentFX.m_strUserColor1FX += strCommandFX;
            sCommentFX.m_strUserColor1FX += "; \r\n";
        }

        strCommandFX = MakeStringFX( mapVM_Command, 9, vecTextureResource, vecCubeTextureResource, vecPixelScalar, vecPixelVector, emWhereCode );
        if ( !strCommandFX.empty() )
        {
            sCommentFX.m_strUserColor2FX = "    color = ";
            sCommentFX.m_strUserColor2FX += strCommandFX;
            sCommentFX.m_strUserColor2FX += "; \r\n";
        }
    }

    HRESULT hr = pVMCommandFX->LoadFX( m_pd3dDevice, sCommentFX, vecTextureResource.size(), vecCubeTextureResource.size(), vecPixelScalar.size(), vecPixelVector.size(), emModeFX );
    if ( hr == S_OK )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL DxVMCommand::CreateCommand_EX( const MAP_VM_COMMAND& mapVM_Command, std::tr1::shared_ptr<DxVMCommand_EX>& spVMCommandEX )
{
    spVMCommandEX = DxVMCommand_EX::NEW_COMMAND_STATIC( m_emType );
    if ( !spVMCommandEX.get() )
        return FALSE;

    spVMCommandEX->Clone( this );

    for ( DWORD i=0; i<m_vecLineData.size(); ++i )
    {
        MAP_VM_COMMAND_CITER iter = mapVM_Command.find( m_vecLineData[i]->m_dwID_Output );
        if ( iter != mapVM_Command.end() )
        {
            std::tr1::shared_ptr<DxVMCommand_EX> spVMCommandEX_Child;
            if ( (*iter).second->CreateCommand_EX( mapVM_Command, spVMCommandEX_Child ) )
            {
                spVMCommandEX->SetLink( spVMCommandEX_Child );
            }
        }
    }

    return TRUE;
}

void DxVMCommand::ClearFX()
{
    m_spVMCommandFX->CleanUp();
    m_vecTextureResource.clear();
    m_vecCubeTextureResource.clear();
    m_vecPixelScalar.clear();
    m_vecPixelVector.clear();
}

TSTRING* DxVMCommand::GetDescPointer()
{
    return &m_strDesc;
}

void DxVMCommand::Save( ic::SerialFileInterface* pSFile )
{
    (*pSFile) << VERSION;
    pSFile->BeginBlock();
    {
        (*pSFile) << static_cast<DWORD>( m_emType );
        (*pSFile) << m_dwID;
        (*pSFile) << m_vPos.x;
        (*pSFile) << m_vPos.y;
        (*pSFile) << m_strDesc;

        (*pSFile) << static_cast<DWORD>( m_vecLineData.size() );
        for( size_t i=0; i<m_vecLineData.size(); ++i )
        {
            (*pSFile) << m_vecLineData[i]->m_nIndexBox_Input;
            (*pSFile) << m_vecLineData[i]->m_dwID_Output;
            (*pSFile) << m_vecLineData[i]->m_nIndexBox_Output;
        }
    }
    pSFile->EndBlock();
}
 
void DxVMCommand::Load( ic::SerialFileInterface* pSFile )
{
    DWORD dwVer, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION )
    {
        DWORD dwTemp;
        (*pSFile) >> dwTemp;    m_emType = static_cast<VMC_TYPE>( dwTemp );
        (*pSFile) >> m_dwID;
        (*pSFile) >> m_vPos.x;
        (*pSFile) >> m_vPos.y;
        (*pSFile) >> m_strDesc;

        DWORD dwSize(0L);
        (*pSFile) >> dwSize;
        for( DWORD i=0; i<dwSize; ++i )
        {
            std::tr1::shared_ptr<DxVMLineData> spNew = std::tr1::shared_ptr<DxVMLineData>(new DxVMLineData);
            (*pSFile) >> spNew->m_nIndexBox_Input;
            (*pSFile) >> spNew->m_dwID_Output;
            (*pSFile) >> spNew->m_nIndexBox_Output;
            m_vecLineData.push_back( spNew );
        }
    }
	else if ( dwVer == 0x101 )
	{
		DWORD dwTemp;
		(*pSFile) >> dwTemp;    m_emType = static_cast<VMC_TYPE>( dwTemp );
		(*pSFile) >> m_dwID;
		(*pSFile) >> m_vPos.x;
		(*pSFile) >> m_vPos.y;
		(*pSFile) >> m_strDesc;

		DWORD dwSize(0L);
		(*pSFile) >> dwSize;
		for( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<DxVMLineData> spNew = std::tr1::shared_ptr<DxVMLineData>(new DxVMLineData);
			(*pSFile) >> spNew->m_nIndexBox_Input;
			(*pSFile) >> spNew->m_dwID_Output;
			(*pSFile) >> spNew->m_nIndexBox_Output;

			// Version 102 에서 중간에 2번(StarGlow)이 추가되어서 다 밀렸다.
			if ( m_dwID == 0 && spNew->m_nIndexBox_Input>=2 )
			{
				spNew->m_nIndexBox_Input += 1;
			}

			m_vecLineData.push_back( spNew );
		}
	}
    else if ( dwVer == 0x100 )
    {
        DWORD dwTemp;
        (*pSFile) >> dwTemp;    m_emType = static_cast<VMC_TYPE>( dwTemp );
        (*pSFile) >> m_dwID;
        (*pSFile) >> m_vPos.x;
        (*pSFile) >> m_vPos.y;

        DWORD dwSize(0L);
        (*pSFile) >> dwSize;
        for( DWORD i=0; i<dwSize; ++i )
        {
            std::tr1::shared_ptr<DxVMLineData> spNew = std::tr1::shared_ptr<DxVMLineData>(new DxVMLineData);
            (*pSFile) >> spNew->m_nIndexBox_Input;
            (*pSFile) >> spNew->m_dwID_Output;
            (*pSFile) >> spNew->m_nIndexBox_Output;

			// Version 102 에서 중간에 2번(StarGlow)이 추가되어서 다 밀렸다.
			if ( m_dwID == 0 && spNew->m_nIndexBox_Input>=2 )
			{
				spNew->m_nIndexBox_Input += 1;
			}

            m_vecLineData.push_back( spNew );
        }
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
    }
}
