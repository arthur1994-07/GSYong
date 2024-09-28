#include "stdafx.h"

#include "../InternalCommonLib/Interface/DxInputDeviceInterface.h"
#include "../InternalCommonLib/Interface/SerialFileInterface.h"

#include "./Command_EX/DxVMCommand_EX.h"
#include "./ViewPort/DxViewPortVM.h"
#include "./Preview/NSVM_PREVIEW.h"
#include "./HLSL/DxVMCommandFX.h"
#include "./Command/DxVMCommand.h"
#include "./Manager/DxVMManager.h"

#include "DxVisualMaterial.h"

// ----------------------------------------------------------------------------
#include "../InternalCommonLib/debuginclude.h"
// ----------------------------------------------------------------------------


BOOL        DxVisualMaterial::g_bLineCurve(TRUE);           // Line을 직선 / Curve

DxVisualMaterial::DxVisualMaterial(void)
    : m_pd3dDevice(NULL)
    , m_fTime(0.f)
    , m_strFileName(_T("Hello.vm"))
    , m_emEditMode(VMEM_ZERO)
    , m_vSelectBoxMin(0.f,0.f)
    , m_vSelectBoxMax(0.f,0.f)
    , m_bStartMoveVMCommand(false)
    , m_vPreMoveMousePos(0.0f, 0.0f, 0.0f)
{
}

DxVisualMaterial::~DxVisualMaterial(void)
{
    CleanUp();
}

void DxVisualMaterial::CleanUp()
{
    m_mapVM_Command.clear();

    m_setSelectCommand.clear();
}

//---------------------------------------------------------------------------------- 
void DxVisualMaterial::Create( LPDIRECT3DDEVICEQ pd3dDevice )
{
    m_pd3dDevice = pd3dDevice;

    if ( !m_mapVM_Command.empty() )
        return;

    std::tr1::shared_ptr<DxVMCommand> spNew = DxVMCommand::NEW_COMMAND_STATIC( VMC_CORE );
    if( spNew->Create( m_pd3dDevice, D3DXVECTOR2(0.f,0.f) ) )
    {
        spNew->SetID( 0 );
        m_mapVM_Command.insert( std::make_pair( 0, spNew ) );
    }
}

void DxVisualMaterial::OnResetDevice()
{
    MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
    for( ; iter!=m_mapVM_Command.end(); ++iter )
    {
        (*iter).second->OnResetDevice();
    }
}
 
void DxVisualMaterial::OnLostDevice()
{
    MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
    for( ; iter!=m_mapVM_Command.end(); ++iter )
    {
        (*iter).second->OnLostDevice();
    }
}

void DxVisualMaterial::FrameMove( float fElapsedTime )
{
    m_fTime += fElapsedTime;

    // 기본적은 형상들을 FrameMove
    {
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            (*iter).second->FrameMove( m_fTime );
        }
    }

    // Command를 선택하기 위한 작업 _ 박스형식
    {
        if ( Action_SelectBox() )
            return;
    }

    // Small Box 의 동작들
    {
        if ( Action_SelectBoxForLink() )
            return;

        if ( Action_LinkBoxToBox( m_sSelectIndexBox ) )
            return;

        if ( Action_DeleteBoxForLink( m_sSelectIndexBox ) )
            return;
    }

    // 선택된 Command의 이동작업.
    {
        if ( Action_MoveCommand() )
            return;
    }

    // Command를 선택하기 위한 작업 _ 기본 선택
    {
        if ( Action_SelectCommand( FALSE ) )
            return;
    }

    // 키보드 관련 작업
    KeyProcess();
}

void DxVisualMaterial::Render()
{
    // 초기화
    m_sCanSelectIndex = VM_CAN_SELECT_INDEX();

    // 마우스 위치를 얻어옴.
    D3DXVECTOR3 vMousePt( -1.f,-1.f, 0.f );
    GetMousePointWorld( vMousePt );

    // 기본적은 형상들을 렌더한다.
    {
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            if ( (*iter).first == 0 )
            {
                // FX 파일을 만든다.
                (*iter).second->MakeFX( NSVM_PREVIEW::g_spVMCommandFX.get(), 
                                        m_mapVM_Command, 
                                        NSVM_PREVIEW::g_vecTextureResource, 
                                        NSVM_PREVIEW::g_vecCubeTextureResource, 
                                        NSVM_PREVIEW::g_vecPixelScalar, 
                                        NSVM_PREVIEW::g_vecPixelVector,
                                        VMFX_PIECE );

                // 렌더링 한다.
                (*iter).second->Render( m_sCanSelectIndex, D3DXVECTOR2(vMousePt.x,vMousePt.y) );
            }
            else
            {
                // FX 파일을 만든다.
                (*iter).second->MakeFX( m_mapVM_Command );

                // 렌더링 한다.
                (*iter).second->Render( m_sCanSelectIndex, D3DXVECTOR2(vMousePt.x,vMousePt.y) );

                // FX 파일을 만든다.
                (*iter).second->ClearFX();
            }
        }
    }

    // 선택가능한 Box가 있으면 노란색으로 렌더할 수 있도록 셋팅함
    {
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            (*iter).second->SetYellowBox( m_sCanSelectIndex );
        }
    }

    // 선택한 BaseBox가 있으면 노란색으로 렌더할 수 있도록 셋팅함
    {
        MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
        for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
        {
            (*iterVM).second->SetSelectBaseBox( FALSE );

            SET_DWORD_ITER iter = m_setSelectCommand.begin();
            for( ; iter!=m_setSelectCommand.end(); ++iter )
            {
                if ( (*iterVM).first == 0 )
                    continue;

                if ( (*iterVM).first == (*iter) )
                {
                    (*iterVM).second->SetSelectBaseBox( TRUE );
                }
            }
        }
    }

    // 현재 마우스와 연결된 Line을 그린다.
    {
        RenderLineNow();
        RenderLine();
    }

    // SelectBox 용 Line을 그린다.
    Render_SelectBoxLine();
}

void DxVisualMaterial::CreateDataFX()
{
    // 기본적은 형상들을 렌더한다.
    {
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            if ( (*iter).first == 0 )
            {
                // FX 파일을 만든다.
                (*iter).second->MakeFX( NSVM_PREVIEW::g_spVMCommandFX.get(), 
                                        m_mapVM_Command, 
                                        NSVM_PREVIEW::g_vecTextureResource, 
                                        NSVM_PREVIEW::g_vecCubeTextureResource, 
                                        NSVM_PREVIEW::g_vecPixelScalar, 
                                        NSVM_PREVIEW::g_vecPixelVector,
                                        VMFX_PIECE );
            }
            else
            {
                // FX 파일을 만든다.
                (*iter).second->MakeFX( m_mapVM_Command );

                // FX 파일을 만든다.
                (*iter).second->ClearFX();
            }
        }
    }
}

void DxVisualMaterial::RenderLineNow()
{
    if ( m_emEditMode != VMEM_SELECT_BOX_FOR_LINE )
        return;

    // 마우스 왼쪽 버튼을 누르고 있어야 한다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_DOWNED ) )
	{
        return;
    }

    // 
    D3DXVECTOR3 vPoint(0.f,0.f,0.f);
    GetMousePointWorld( vPoint );

    // Note : SetRenderState()
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

    // Note : SetTextureStageState()
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );

    m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );

    // Line Render
    if ( m_sSelectIndexBox.m_emMode == CAN_SELECT_INPUT )
    {
        RenderHermiteLine( vPoint, m_vMouseDownPoint, 0xff000000 );
    }
    else if ( m_sSelectIndexBox.m_emMode == CAN_SELECT_OUTPUT )
    {
        RenderHermiteLine( m_vMouseDownPoint, vPoint, 0xff000000 );
    }
}

void DxVisualMaterial::RenderLine()
{
    // 초기화
    vm::VERTEXCOLOR vLine[2];
    vm::VERTEXCOLOR vTriangle[3];
    {
        // Note : SetRenderState()
	    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

        // Note : SetTextureStageState()
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
        
        vLine[0].dwColor = 0xff000000;
        vLine[1].dwColor = 0xff000000;

        vTriangle[0].dwColor = 0xff000000;
        vTriangle[1].dwColor = 0xff000000;
        vTriangle[2].dwColor = 0xff000000;

        m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );
    }

    // 기본적은 형상들을 렌더한다.
    {
        DWORD dwLineColor(0L);
        D3DXVECTOR3 vUp(0.f,0.f,-1.f);
        D3DXVECTOR3 vDir(0.f,0.f,0.f);
        D3DXVECTOR3 vCross(0.f,0.f,0.f);
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            const VEC_VMLINEDATA& vecLineData = (*iter).second->GetVMLineData();
            for( DWORD j=0; j<vecLineData.size(); ++j )
            {
                // Color를 얻도록 한다.
                dwLineColor = GetLineColor( (*iter).first, 
                                            vecLineData[j]->m_nIndexBox_Input, 
                                            vecLineData[j]->m_dwID_Output, 
                                            vecLineData[j]->m_nIndexBox_Output );

                vLine[0].dwColor = dwLineColor;
                vLine[1].dwColor = dwLineColor;
                vTriangle[0].dwColor = dwLineColor;
                vTriangle[1].dwColor = dwLineColor;
                vTriangle[2].dwColor = dwLineColor;

                // Point Input
                GetPointInput( vLine[0].vPos, (*iter).first, vecLineData[j]->m_nIndexBox_Input );

                // Point Output
                GetPointOutput( vLine[1].vPos, vecLineData[j]->m_dwID_Output, vecLineData[j]->m_nIndexBox_Output );

                // Line Render
                RenderHermiteLine( vLine[1].vPos, vLine[0].vPos, dwLineColor );

                // Triangle Render
                {
                    // Line Direction을 구한다.
                    vDir = GetHermiteLastDir( vLine[1].vPos, vLine[0].vPos );

                    D3DXVec3Cross( &vCross, &vDir, &vUp );
                    D3DXVec3Normalize( &vCross, &vCross );
                    D3DXVec3Normalize( &vDir, &vDir );

                    vTriangle[0].vPos = vLine[0].vPos;
                    vTriangle[1].vPos = vLine[0].vPos - vDir - (vCross*0.3f);
                    vTriangle[2].vPos = vLine[0].vPos - vDir + (vCross*0.3f);
                    m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 1, vTriangle, sizeof(vm::VERTEXCOLOR) );
                }
            }
        }
    }
}

void DxVisualMaterial::RenderHermiteLine( const D3DXVECTOR3& vLine1, const D3DXVECTOR3& vLine2, DWORD dwColor )
{
    if ( g_bLineCurve )
    {
        D3DXVECTOR3 vDir = vLine1 - vLine2;
        float fLength = D3DXVec3Length( &vDir );
        fLength *= 0.5f;

        vm::VERTEXCOLOR vLine[VM_HERMITE_LINE_VERT];
        D3DXVECTOR3 vTangent(-fLength,0.f,0.f);
        for( int i=0; i<VM_HERMITE_LINE_VERT; ++i )
        {
            D3DXVec3Hermite
            ( 
                &vLine[i].vPos, 
                &vLine1, 
                &vTangent, 
                &vLine2, 
                &vTangent, 
                static_cast<float>(i)/(VM_HERMITE_LINE_VERT-1) 
            );

            vLine[i].dwColor = dwColor;
        }

        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, VM_HERMITE_LINE_VERT-1, vLine, sizeof(vm::VERTEXCOLOR) );
    }
    else
    {
        vm::VERTEXCOLOR vLine[2];
        vLine[0].vPos = vLine1;
        vLine[1].vPos = vLine2;
        vLine[0].dwColor = dwColor;
        vLine[1].dwColor = dwColor;
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, vLine, sizeof(vm::VERTEXCOLOR) );
    }
}

D3DXVECTOR3 DxVisualMaterial::GetHermiteLastDir( const D3DXVECTOR3& vLine1, const D3DXVECTOR3& vLine2 )
{
    if ( g_bLineCurve )
    {
        D3DXVECTOR3 vDir = vLine1 - vLine2;
        float fLength = D3DXVec3Length( &vDir );
        float fRate = 1.f / fLength;
        fLength *= 0.5f;

        D3DXVECTOR3 vHermitePos(0.f,0.f,0.f);
        D3DXVECTOR3 vTangent(-fLength,0.f,0.f);
        D3DXVec3Hermite
        ( 
            &vHermitePos, 
            &vLine1, 
            &vTangent, 
            &vLine2, 
            &vTangent, 
            1.f-fRate
        );

        vDir = vLine2 - vHermitePos;
        fLength = D3DXVec3Length( &vDir );
        if ( fLength <= 0.f )
        {
            return D3DXVECTOR3( -1.f, 0.f, 0.f );
        }
        else
        {
            return vDir;
        }
    }
    else
    {
        return vLine2 - vLine1;
    }
}

void DxVisualMaterial::GetPointInput( D3DXVECTOR3& vPoint, DWORD dwID, int nIndexBox )
{
    MAP_VM_COMMAND_ITER iter = m_mapVM_Command.find( dwID );
    if ( iter == m_mapVM_Command.end() )
    {
        return;
    }

    (*iter).second->GetPointInput( vPoint, nIndexBox );
}

void DxVisualMaterial::GetPointOutput( D3DXVECTOR3& vPoint, DWORD dwID, int nIndexBox )
{
    MAP_VM_COMMAND_ITER iter = m_mapVM_Command.find( dwID );
    if ( iter == m_mapVM_Command.end() )
    {
        return;
    }

    (*iter).second->GetPointOutput( vPoint, nIndexBox );
}

DWORD DxVisualMaterial::GetLineColor( DWORD dwInputID, int nInputIndexBox, DWORD dwOutputID, int nOutputIndexBox )
{
    switch( m_sCanSelectIndex.m_emMode )
    {
    case CAN_SELECT_INPUT:
        if ( m_sCanSelectIndex.m_dwID == dwInputID &&
            m_sCanSelectIndex.m_nIndexBox == nInputIndexBox )
        {
            return 0xffff6666;
        }
        break;
    case CAN_SELECT_OUTPUT:
        if ( m_sCanSelectIndex.m_dwID == dwOutputID &&
            m_sCanSelectIndex.m_nIndexBox == nOutputIndexBox )
        {
            return 0xffff6666;
        }
        break;
    };

    return 0L;
}

//-------------------------------------------------------------------------------------------------- Edit
//-------------------------------------------------------------------------------------------------- Mouse Up ( 단축키 )
void DxVisualMaterial::MouseUp()
{
    // Note : Add Command

    // Constant를 만든다.
    if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_1 ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_CONSTANT );
	}
    // Constant2Vector을 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_2 ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_CONSTANT2VECTOR );
	}
    // Constant3Vector를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_3 ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_CONSTANT3VECTOR );
	}
    // Constant4Vector를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_4 ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_CONSTANT4VECTOR );
	}
    // Add를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_A ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_ADD );
	}
    // LinearInterpolate를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_L ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_LINEAR_INTERPOLATE );
	}
    // Multiply를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_M ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_MULTIPLY );
	}
    // Panner를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_P ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_PANNER );
	}
    // ReflectionVector를 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_R ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_REFLECTION_VECTOR );
	}
    // TextureSample을 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_T ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_TEXTURESAMPLE );
	}
    // VectorParameter을 만든다.
    else if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_V ) & ic::DXKEY_DOWNED )
	{
        CreateCommand( VMC_VECTOR_PARAMETER );
	}
    
    
}

//-------------------------------------------------------------------------------------------------- CreateCommand
void DxVisualMaterial::CreateCommand( VMC_TYPE emType )
{
    D3DXVECTOR3 vMousePt(0.f,0.f,0.f);
    if ( !GetMousePointWorld( vMousePt ) )
        return;

    DWORD dwIndex = GetEmptyIndex_Command();
    if ( dwIndex == -1 )
        return;

    std::tr1::shared_ptr<DxVMCommand> spNew = DxVMCommand::NEW_COMMAND_STATIC( emType );
    if( spNew->Create( m_pd3dDevice, D3DXVECTOR2(vMousePt.x,vMousePt.y) ) )
    {
        spNew->SetID( dwIndex );
        m_mapVM_Command.insert( std::make_pair( dwIndex, spNew ) );
    }
}

//-------------------------------------------------------------------------------------------------- Is Camera Move
BOOL DxVisualMaterial::IsCameraMove()
{
    switch( m_emEditMode )
    {
    case VMEM_SELECT_BOX:
    case VMEM_SELECT_BOX_FOR_LINE:
        return FALSE;
    }

    BOOL bKeyLCtrlDowned(FALSE);
    if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LCONTROL ) & ic::DXKEY_DOWNED )
        bKeyLCtrlDowned = TRUE;

    if ( !m_setSelectCommand.empty() && bKeyLCtrlDowned )
        return FALSE;
    else
        return TRUE;
}

//--------------------------------------------------------------------------------------------------- 단 하나만 선택되었을 경우, 선택된 Command를 보내준다.
//                                                                                                    선택안되어 있을 경우는 Core를 보낸다.
DxVMCommand* DxVisualMaterial::GetCommand()
{
    if ( m_mapVM_Command.empty() )
        return NULL;

    MAP_VM_COMMAND_ITER iter;
    size_t nSelectSize = m_setSelectCommand.size();
    if ( nSelectSize == 0 )
    {
        iter = m_mapVM_Command.begin();
        return (*iter).second.get();
    }
    else if ( nSelectSize == 1 )
    {
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.find( (*m_setSelectCommand.begin()) );
        if ( iter != m_mapVM_Command.end() )
        {
            return (*iter).second.get();
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

//--------------------------------------------------------------------------------------------------- 파일이름을 가져온다.
const TSTRING& DxVisualMaterial::GetFileName()
{
    return m_strFileName;
}

//--------------------------------------------------------------------------------------------------- Select Box For Link
BOOL DxVisualMaterial::Action_SelectBoxForLink()
{
    if ( m_emEditMode != VMEM_ZERO )
        return FALSE;

    if ( m_sCanSelectIndex.m_emMode == CAN_SELECT_ZERO )
        return FALSE;

    // 마우스 왼쪽 버튼을 눌렀다.
    if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_DOWN )
	{
        GetMousePointWorld( m_vMouseDownPoint );
        m_sSelectIndexBox = m_sCanSelectIndex;
        SetEditMode( VMEM_SELECT_BOX_FOR_LINE );

        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------------------- Link Box To Box
BOOL DxVisualMaterial::Action_LinkBoxToBox( const VM_CAN_SELECT_INDEX& sSelectIndexBox )
{
    if ( m_emEditMode != VMEM_SELECT_BOX_FOR_LINE )
        return FALSE;

    // 마우스 왼쪽 버튼을 눌렀다.
    if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_DOWNED )
	{
        return FALSE;
    }

    // 초기화 시켜놓는다.
    SetEditMode( VMEM_ZERO );

    // 라인을 연결하도록 한다.
    if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_UP )
	{
        if ( sSelectIndexBox.m_emMode != CAN_SELECT_ZERO &&
            m_sCanSelectIndex.m_emMode != CAN_SELECT_ZERO )
        {
            // 같지만 않으면 된다.
            if ( sSelectIndexBox.m_emMode != m_sCanSelectIndex.m_emMode )
            {
                if ( sSelectIndexBox.m_emMode == CAN_SELECT_INPUT )
                {
                    if ( SetLineLink2( sSelectIndexBox.m_dwID, sSelectIndexBox.m_nIndexBox, m_sCanSelectIndex ) )
                        return TRUE;
                }
                else if ( m_sCanSelectIndex.m_emMode == CAN_SELECT_INPUT )
                {
                    if ( SetLineLink2( m_sCanSelectIndex.m_dwID, m_sCanSelectIndex.m_nIndexBox, sSelectIndexBox ) )
                        return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL DxVisualMaterial::SetLineLink2( DWORD dwID, int nIndexBox, const VM_CAN_SELECT_INDEX& sCanSelectIndex )
{
    MAP_VM_COMMAND_ITER iter = m_mapVM_Command.find( dwID );
    if ( iter == m_mapVM_Command.end() )
    {
        return FALSE;
    }

    (*iter).second->LineLink( nIndexBox, sCanSelectIndex );

    return TRUE;
}

//--------------------------------------------------------------------------------------------------- Delete Box For Link
BOOL DxVisualMaterial::Action_DeleteBoxForLink( const VM_CAN_SELECT_INDEX& sSelectIndexBox )
{
    // 마우스 왼쪽 버튼을 눌렀다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_UP ) )
	{
        return FALSE;
    }

    if ( sSelectIndexBox.m_emMode == CAN_SELECT_ZERO )
        return FALSE;

    if ( sSelectIndexBox.m_dwID != m_sCanSelectIndex.m_dwID || 
        sSelectIndexBox.m_emMode != m_sCanSelectIndex.m_emMode ||
        sSelectIndexBox.m_nIndexBox != m_sCanSelectIndex.m_nIndexBox )
        return FALSE;

    if ( ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LCONTROL ) & ic::DXKEY_DOWNED ) ||
        ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LMENU ) & ic::DXKEY_DOWNED ) )
	{
        MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
        for( ; iter!=m_mapVM_Command.end(); ++iter )
        {
            (*iter).second->DeleteLineLink( sSelectIndexBox );
        }
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------------------- Move Command
BOOL DxVisualMaterial::Action_MoveCommand()
{
    // 선택된 것이 있어야 이동시킬수 있다.
    if ( m_setSelectCommand.empty() )
    {
        return FALSE;
    }

    // 마우스 왼쪽 버튼을 누르고 있어야 한다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_DOWNED ) )
	{
        m_bStartMoveVMCommand = false;

        return FALSE;
    }

    // LCtrl키를 누르고 있어야 한다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LCONTROL ) & ic::DXKEY_DOWNED ) )
	{
        m_bStartMoveVMCommand = false;

        return FALSE;
    }

    if ( !m_bStartMoveVMCommand )
    {
        m_bStartMoveVMCommand = true;

        MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
        for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
        {
            SET_DWORD_ITER iter = m_setSelectCommand.find( (*iterVM).first );
            if ( iter != m_setSelectCommand.end() )
            {
                GetMousePointWorld( m_vPreMoveMousePos );
            }
        }

    }

    // 마우스 이동
    int         dx(0), dy(0), dz(0);
    ic::DxInputDeviceInterface::GetInstance()->GetMouseMove ( dx, dy, dz );

    // 선택된 Command들 이동시킨다.
    if ( ( dx != 0 ) || ( dy != 0 ) || ( dz != 0 ) )
    {
        MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
        for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
        {
            SET_DWORD_ITER iter = m_setSelectCommand.find( (*iterVM).first );
            if ( iter != m_setSelectCommand.end() )
            {
                D3DXVECTOR3 vec3MousePosWorld( 0.0f, 0.0f, 0.0f );
                if ( GetMousePointWorld( vec3MousePosWorld ) )
                {
                    D3DXVECTOR2 vec2ChangeMousePos;
                    vec2ChangeMousePos.x = vec3MousePosWorld.x - m_vPreMoveMousePos.x;
                    vec2ChangeMousePos.y = vec3MousePosWorld.y - m_vPreMoveMousePos.y;

                    (*iterVM).second->Move( vec2ChangeMousePos );
                }

            }
        }
        GetMousePointWorld( m_vPreMoveMousePos );

        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------------------------------------- Select Command
BOOL DxVisualMaterial::Action_SelectCommand( BOOL bBox )
{
    if ( m_emEditMode == VMEM_ZERO )
    {
        // 마우스 왼쪽 버튼을 눌렀다.
        if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_DOWN )
	    {
            if ( bBox )
            {
                SetEditMode( VMEM_SELECT_BOX );
            }
            else
            {
                SetEditMode( VMEM_SELECT_POINT );
            }

            GetMousePointWorld( m_vMouseDownPoint );

            return TRUE;
        }
    }
    else if ( m_emEditMode == VMEM_SELECT_BOX )
    {
        // 마우스 왼쪽 버튼을 뗐다.
        if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_UP )
	    {       
            // 현재의 범위로 선택되는 녀석들을 구해본다.
            {
                ClearSelectCommand();

                MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
                for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
                {
                    if ( (*iterVM).first == 0 )
                        continue;

                    if ( (*iterVM).second->IsSelectCommandAtBox( m_vSelectBoxMin, m_vSelectBoxMax ) )
                    {
                        InsertSelectCommand( (*iterVM).first );
                    }
                }
            }

            // 행동 완료
            SetEditMode( VMEM_ZERO );
        }

        return TRUE;
    }
    else if ( m_emEditMode == VMEM_SELECT_POINT )
    {
        Action_SelectCommand_Point();
        return TRUE;
    }

    return FALSE;
}

void DxVisualMaterial::Action_SelectCommand_Point()
{
    // 마우스 왼쪽 버튼을 뗐다.
    if ( ic::DxInputDeviceInterface::GetInstance()->GetMouseState( ic::DXMOUSE_LEFT ) & ic::DXKEY_UP )
    {       
        D3DXVECTOR3 vMousePoint;
        GetMousePointWorld( vMousePoint );

        if ( m_vMouseDownPoint.x + 0.01f > vMousePoint.x && m_vMouseDownPoint.x < vMousePoint.x + 0.01f &&
            m_vMouseDownPoint.y + 0.01f > vMousePoint.y && m_vMouseDownPoint.y < vMousePoint.y + 0.01f )
        {
            // LCtrl키를 누르고 있다면, 현 Command는 추가 선택혹은 선택취소가 된다.
            if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LCONTROL ) & ic::DXKEY_DOWNED )
            {
                DWORD dwSelectID(0);
                D3DXVECTOR2 vPoint( vMousePoint.x, vMousePoint.y );
                MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
                for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
                {
                    if ( (*iterVM).second->IsSelectCommandAtBox( vPoint, vPoint ) )
                    {
                        dwSelectID = (*iterVM).first;
                        break;
                    }
                }

                // 0 만 아니면 선택혹은 취소로 만들어준다.
                if ( dwSelectID != 0 )
                {
                    Action_SelectCommand_Point2( dwSelectID );
                }
            }
            else
            {
                ClearSelectCommand();

                D3DXVECTOR2 vPoint( vMousePoint.x, vMousePoint.y );
                MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
                for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
                {
                    if ( (*iterVM).second->IsSelectCommandAtBox( vPoint, vPoint ) )
                    {
                        InsertSelectCommand( (*iterVM).second->GetID() );
                        break;
                    }
                }
            }
        }

        // 행동 완료
        SetEditMode( VMEM_ZERO );
    }
}

void DxVisualMaterial::Action_SelectCommand_Point2( DWORD dwSelectID )
{
    SET_DWORD_ITER iter = m_setSelectCommand.find( dwSelectID );
    if ( iter == m_setSelectCommand.end() )
    {
        // 같은 값이 없으므로 추가한다.
        InsertSelectCommand( dwSelectID );
    }
    else
    {
        // 같은 값이 있으므로 삭제한다.
        DeleteSelectCommand( iter );
    }
}

//--------------------------------------------------------------------------------------------------- 함수를 이용해서 m_setSelectCommand 변수에 추가, 삭제함
void DxVisualMaterial::InsertSelectCommand( DWORD dwSelectID )
{
    m_setSelectCommand.insert( dwSelectID );
}

void DxVisualMaterial::DeleteSelectCommand( SET_DWORD_ITER& iter )
{
    m_setSelectCommand.erase( iter );
}

void DxVisualMaterial::ClearSelectCommand()
{
    m_setSelectCommand.clear();
}

//--------------------------------------------------------------------------------------------------- Select Command At Box
BOOL DxVisualMaterial::Action_SelectBox()
{
    // LAlt키를 누르고 있어야 한다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LMENU ) & ic::DXKEY_DOWNED ) )
	{
        return FALSE;
    }

    // LCtrl키를 누르고 있어야 한다.
    if ( !( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_LCONTROL ) & ic::DXKEY_DOWNED ) )
	{
        return FALSE;
    }

    return Action_SelectCommand( TRUE );
}

void DxVisualMaterial::Render_SelectBoxLine()
{
    if ( m_emEditMode != VMEM_SELECT_BOX )
    {
        return;
    }

    // 초기화
    vm::VERTEXCOLOR vLine[2];
    {
        // Note : SetRenderState()
	    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,		    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,	    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,		    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	0xffffffff );

        // Note : SetTextureStageState()
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,	D3DTA_TFACTOR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE );

        D3DXMATRIX matIdentity;
        D3DXMatrixIdentity( &matIdentity );
        m_pd3dDevice->SetTransform( D3DTS_WORLD, &matIdentity );
        
        vLine[0].vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
        vLine[1].vPos = D3DXVECTOR3( 0.f, 0.f, 0.f );
        vLine[0].dwColor = 0xffff0000;
        vLine[1].dwColor = 0xffff0000;

        m_pd3dDevice->SetFVF( vm::VERTEXCOLOR::FVF );
    }

    // SelectBox Min Max 를 얻는다.
    {
        D3DXVECTOR3 vMousePoint;
        GetMousePointWorld( vMousePoint );
        m_vSelectBoxMin = D3DXVECTOR2(m_vMouseDownPoint.x,m_vMouseDownPoint.y);
        m_vSelectBoxMax = D3DXVECTOR2(m_vMouseDownPoint.x,m_vMouseDownPoint.y);

        if ( m_vSelectBoxMin.x > vMousePoint.x )        m_vSelectBoxMin.x = vMousePoint.x;
        if ( m_vSelectBoxMin.y > vMousePoint.y )        m_vSelectBoxMin.y = vMousePoint.y;
        if ( m_vSelectBoxMax.x < vMousePoint.x )        m_vSelectBoxMax.x = vMousePoint.x;
        if ( m_vSelectBoxMax.y < vMousePoint.y )        m_vSelectBoxMax.y = vMousePoint.y;

        if ( m_vSelectBoxMin.x > m_vMouseDownPoint.x )  m_vSelectBoxMin.x = m_vMouseDownPoint.x;
        if ( m_vSelectBoxMin.y > m_vMouseDownPoint.y )  m_vSelectBoxMin.y = m_vMouseDownPoint.y;
        if ( m_vSelectBoxMax.x < m_vMouseDownPoint.x )  m_vSelectBoxMax.x = m_vMouseDownPoint.x;
        if ( m_vSelectBoxMax.y < m_vMouseDownPoint.y )  m_vSelectBoxMax.y = m_vMouseDownPoint.y;
    }

    // Line Render // 상
    vLine[0].vPos.x = m_vSelectBoxMin.x;
    vLine[0].vPos.y = m_vSelectBoxMax.y;
    vLine[1].vPos.x = m_vSelectBoxMax.x;
    vLine[1].vPos.y = m_vSelectBoxMax.y;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

    // 하
    vLine[0].vPos.x = m_vSelectBoxMin.x;
    vLine[0].vPos.y = m_vSelectBoxMin.y;
    vLine[1].vPos.x = m_vSelectBoxMax.x;
    vLine[1].vPos.y = m_vSelectBoxMin.y;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

    // 좌
    vLine[0].vPos.x = m_vSelectBoxMin.x;
    vLine[0].vPos.y = m_vSelectBoxMin.y;
    vLine[1].vPos.x = m_vSelectBoxMin.x;
    vLine[1].vPos.y = m_vSelectBoxMax.y;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );

    // 우
    vLine[0].vPos.x = m_vSelectBoxMax.x;
    vLine[0].vPos.y = m_vSelectBoxMin.y;
    vLine[1].vPos.x = m_vSelectBoxMax.x;
    vLine[1].vPos.y = m_vSelectBoxMax.y;
    m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, vLine, sizeof(vm::VERTEXCOLOR) );
}

//--------------------------------------------------------------------------------------------------- Key Process
void DxVisualMaterial::KeyProcess()
{
    // 삭제
    if ( ic::DxInputDeviceInterface::GetInstance()->GetKeyState( DIK_DELETE ) & ic::DXKEY_UP )
	{
        DeleteCommand();
	}
}

//--------------------------------------------------------------------------------------------------- Delete Command
void DxVisualMaterial::DeleteCommand()
{
    SET_DWORD_ITER iter = m_setSelectCommand.begin();
    for( ; iter!=m_setSelectCommand.end(); ++iter )
    {
        // 첫번째 것이 선택되어 있으면 DeleteCommand 작업은 하지 못한다.
        if ( (*iter) == 0 )
            return;

        MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.find( (*iter) );
        if ( iterVM != m_mapVM_Command.end() )
        {
            // LineLink도 지워준다.
            DeleteCommand2( (*iterVM).first );

            // Command를 지운다.
            //SAFE_DELETE( (*iterVM).second );
            iterVM = m_mapVM_Command.erase( iterVM );
        }
    }

    // 선택한 정보는 모두 삭제해 준다.
    m_setSelectCommand.clear();
}

void DxVisualMaterial::DeleteCommand2( DWORD dwID )
{
    MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
    for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
    {
        (*iterVM).second->DeleteLineLink( dwID );
    }
}

//--------------------------------------------------------------------------------------------------- Etc
BOOL DxVisualMaterial::GetMousePointWorld( D3DXVECTOR3& vMousePt )
{
    // 마우스 방향을 계산함
    D3DXVECTOR3 vTargetPt, vFromPt;
	vFromPt = DxViewPortVM::GetInstance()->GetFromPt();
	BOOL bTargetted = DxViewPortVM::GetInstance()->GetMouseTargetPosWnd( vTargetPt );
    if ( !bTargetted )
        return FALSE;

    // Plane
    D3DXVECTOR3 vP[3];
    vP[0] = D3DXVECTOR3(0.f,0.f,0.f);
    vP[1] = D3DXVECTOR3(1.f,0.f,0.f);
    vP[2] = D3DXVECTOR3(0.f,1.f,0.f);
    D3DXPLANE sPlane;
    D3DXPlaneFromPoints( &sPlane, &vP[0], &vP[1], &vP[2] );

    // 교차점을 찾는다.
    D3DXPlaneIntersectLine( &vMousePt, &sPlane, &vTargetPt, &vFromPt );

    return TRUE;
}

DWORD DxVisualMaterial::GetEmptyIndex_Command()
{
    DWORD dwIndex(0);
    for ( DWORD i=0; i<10000; ++i )
    {
        BOOL bSameIndex = FALSE;
        MAP_VM_COMMAND_ITER iterVM = m_mapVM_Command.begin();
        for( ; iterVM!=m_mapVM_Command.end(); ++iterVM )
        {
            if ( i == (*iterVM).first )
            {
                bSameIndex = TRUE;
            }
        }

        if ( !bSameIndex )
            return i;
    }
    return -1;
}

void DxVisualMaterial::SetEditMode( VM_EDIT_MODE emMode )
{
    m_emEditMode = emMode;
}

//--------------------------------------------------------------------------------------------------- Save & Load
void DxVisualMaterial::Save( const TSTRING& strFullName, const TSTRING& strFileName )
{
    m_strFileName = strFileName;

    std::auto_ptr<ic::SerialFileInterface> apSFile = ic::g_funcSerialFile();
    ic::SerialFileInterface* pSFile = apSFile.get();
    if ( !pSFile->OpenFile( ic::FOT_WRITE, strFullName.c_str() ) )		
        return;

    (*pSFile) << static_cast<DWORD>( VERSION );
    pSFile->BeginBlock();
    {
        (*pSFile) << static_cast<DWORD>( VERSION_FX );

        // 이 부분은 Edit에서 쓰이는 곳이다.
        pSFile->BeginBlock();
        {
            (*pSFile) << m_strFileName;          // 이름

            (*pSFile) << static_cast<DWORD>( m_mapVM_Command.size() );
            MAP_VM_COMMAND_ITER iter = m_mapVM_Command.begin();
            for ( ; iter!=m_mapVM_Command.end(); ++iter )
            {
                (*pSFile) << static_cast<DWORD>( (*iter).first );
                (*pSFile) << static_cast<DWORD>( (*iter).second->GetType() );
                (*iter).second->Save( pSFile );
            }
        }
        pSFile->EndBlock();

        // 이 부분은 게임에서 쓰이는 곳이다.
        // 변경되면 VisualMaterialFX::Load 도 동시에 변경시켜주어야 한다.
        {
            // HLSL 관련
            {
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_Piece();
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_PieceColor();
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_PieceColorLM();
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_SKIN_VS();
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetSourceString_SkinObject();
                (*pSFile) << static_cast<DWORD>( NSVM_PREVIEW::g_vecTextureResource.size() );
                for ( DWORD i=0; i<NSVM_PREVIEW::g_vecTextureResource.size(); ++i )
                {
                    (*pSFile) << NSVM_PREVIEW::g_vecTextureResource[i].m_strName;
                }

                (*pSFile) << static_cast<DWORD>( NSVM_PREVIEW::g_vecCubeTextureResource.size() );
                for ( DWORD i=0; i<NSVM_PREVIEW::g_vecCubeTextureResource.size(); ++i )
                {
                    (*pSFile) << NSVM_PREVIEW::g_vecCubeTextureResource[i].m_strName;
                }
            }

            // HLSL 외 - PixelScalar
            {
                (*pSFile) << static_cast<DWORD>( NSVM_PREVIEW::g_vecPixelScalar.size() );
                for ( DWORD i=0; i<NSVM_PREVIEW::g_vecPixelScalar.size(); ++i )
                {
                    (*pSFile) << NSVM_PREVIEW::g_vecPixelScalar[i]->GetType();
                    NSVM_PREVIEW::g_vecPixelScalar[i]->Save( pSFile );
                }
            }

            // HLSL 외 - PixelVector
            {
                (*pSFile) << static_cast<DWORD>( NSVM_PREVIEW::g_vecPixelVector.size() );
                for ( DWORD i=0; i<NSVM_PREVIEW::g_vecPixelVector.size(); ++i )
                {
                    (*pSFile) << NSVM_PREVIEW::g_vecPixelVector[i]->GetType();
                    NSVM_PREVIEW::g_vecPixelVector[i]->Save( pSFile );
                }
            }

            // HLSL 외 - Etc
            {
                (*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->GetTwoSideSeperatePass();
                (*pSFile) << static_cast<DWORD>( NSVM_PREVIEW::g_spVMCommandFX->GetBlendMode() );
				(*pSFile) << NSVM_PREVIEW::g_spVMCommandFX->IsActiveStarGlow();
            }
        }
    }
    pSFile->EndBlock();

    pSFile->CloseFile();
}

// Version이 추가된다면 VisualMaterialFX::Load 도 수정해야 한다.
BOOL DxVisualMaterial::Load( LPDIRECT3DDEVICEQ pd3dDevice, const TSTRING& strName )
{
    CleanUp();

    TCHAR szPathName[MAX_PATH] = _T("");
	StringCchCopy( szPathName, MAX_PATH, NS_VM_MANAGER::g_strFullPath.c_str() );
	StringCchCat( szPathName, MAX_PATH, strName.c_str() );

    std::auto_ptr<ic::SerialFileInterface> spSFile = ic::g_funcSerialFile();
    ic::SerialFileInterface* pSFile = spSFile.get();
	if ( !pSFile->OpenFile( ic::FOT_READ, szPathName ) )		
        return FALSE;

    DWORD dwVer, dwVerFX, dwBufferSize;
    (*pSFile) >> dwVer;
    (*pSFile) >> dwBufferSize;

    if ( dwVer == VERSION || dwVer == 0x104 || dwVer == 0x103 || dwVer == 0x102 )
    {
        (*pSFile) >> dwVerFX;

        DWORD dwJumpBufferSize;
        (*pSFile) >> dwJumpBufferSize;

        (*pSFile) >> m_strFileName;          // 이름

        DWORD dwSize(0L);
        (*pSFile) >> dwSize;
        for ( DWORD i=0; i<dwSize; ++i )
        {
            DWORD dwID, dwType;
            (*pSFile) >> dwID;
            (*pSFile) >> dwType;

            std::tr1::shared_ptr<DxVMCommand> spNew = DxVMCommand::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>(dwType) );
			if ( !spNew )
			{
				pSFile->CloseFile();
				return FALSE;
			}

            if ( spNew->Create( pd3dDevice, D3DXVECTOR2(0.f,0.f) ) )
            {
                spNew->Load( pSFile );
                m_mapVM_Command.insert( std::make_pair( dwID, spNew ) );
            }
        }
    }
    else if ( dwVer == 0x101 || dwVer == 0x100 )
    {
        DWORD dwJumpBufferSize;
        (*pSFile) >> dwJumpBufferSize;

        (*pSFile) >> m_strFileName;          // 이름

        DWORD dwSize(0L);
        (*pSFile) >> dwSize;
        for ( DWORD i=0; i<dwSize; ++i )
        {
            DWORD dwID, dwType;
            (*pSFile) >> dwID;
            (*pSFile) >> dwType;

            std::tr1::shared_ptr<DxVMCommand> spNew = DxVMCommand::NEW_COMMAND_STATIC( static_cast<VMC_TYPE>(dwType) );
			if ( !spNew )
			{
				pSFile->CloseFile();
				return FALSE;
			}

            if ( spNew->Create( pd3dDevice, D3DXVECTOR2(0.f,0.f) ) )
            {
                spNew->Load( pSFile );
                m_mapVM_Command.insert( std::make_pair( dwID, spNew ) );
            }
        }
    }
    else
    {
        pSFile->SetOffSet ( pSFile->GetfTell()+dwBufferSize );
		return FALSE;
    }

    pSFile->CloseFile();

	return TRUE;
}