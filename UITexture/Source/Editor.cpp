#include "stdafx.h"

#include <boost/algorithm/string.hpp>

#include "../../InternalCommonLib/dxstdafx.h"

#include "../../EngineLib/DxBase/RenderInterface.h"
#include "../../EngineLib/DxBase/RenderDevice.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/TextTexture/TextUtil.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./UITexture.h"
#include "./Frame.h"
#include "./Editor.h"

#include "../../SigmaCore/DebugInclude.h"

CEditor* g_pEditor = NULL;

CEditor::CEditor()
    : m_bisLoad( false )
{
    m_pEngineDevice = new DxResponseMan( SP_OFFICE_TEST, theApp.GetSubPath() );
}

CEditor::~CEditor()
{
	SAFE_DELETE( m_pEngineDevice );
}

void CEditor::Reset()
{
	m_strSelectedID.clear();
	m_mapControl.clear();
    m_bisLoad = false;
}

bool CEditor::LoadXml( const std::string& strFilePath )
{
	Reset();

	UITextureList::CXmlLoader cLoader;
    m_bisLoad = true;
	return cLoader.Load( strFilePath, m_mapControl );
}

bool CEditor::AddXml( const std::string& strFilePath )
{
    if( m_bisLoad == FALSE )
        return FALSE;

    UITextureList::CPsdXmlToUiXml cToUIXML;
    return cToUIXML.AddPsdXml( strFilePath, m_mapControl );
   
}

bool CEditor::SaveXml( const std::string& strFilePath )
{
	UITextureList::CXmlSaver cSaver;

	return cSaver.Save( strFilePath, m_mapControl );
}

bool CEditor::IsLoaded()
{
    return m_bisLoad;
}

HRESULT CEditor::OnDeviceCreated(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    CTextUtil::m_bUsage = TRUE;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		m_pEngineDevice->OneTimeSceneInit(
			theApp.m_strPath.GetString(),
			theApp.GetFrame(),
			CGameTextMan::GetInstance().GetCommentText("FONTSYS", 1).GetString(),
			false,
			language::KOREAN,
			g_pRI->DefaultFontName().c_str() );
	}
	else
	{
		m_pEngineDevice->OneTimeSceneInit(
			theApp.m_strPath.GetString(),
			theApp.GetFrame(),
			COMMENT::FONTSYS[ 1 ].c_str(),
			false,
			language::KOREAN,
			g_pRI->DefaultFontName().c_str() );
	}

    DxFontMan::GetInstance().InitDeviceObjects( pd3dDevice );

    m_pEngineDevice->InitDeviceObjects( pd3dDevice, FALSE );

    return S_OK;
}

VOID CEditor::OnDeviceDestroyed()
{
    m_pEngineDevice->DeleteDeviceObjects();

    m_pEngineDevice->FinalCleanup();
}

HRESULT CEditor::OnDeviceReset(
    IDirect3DDevice9* pd3dDevice,
    const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    m_pEngineDevice->RestoreDeviceObjects();

    return S_OK;
}

VOID CEditor::OnDeviceLost()
{
    m_pEngineDevice->InvalidateDeviceObjects();
}

VOID CEditor::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    m_pEngineDevice->FrameMove( FLOAT( fTime ), fElapsedTime, FALSE );
}

VOID CEditor::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    CTextUtil::Get()->FrameMove();
}

void CEditor::AddNewTemp()
{
    static int nIdCount = 0;
    
    CString strID;
    strID.Format( "!%03dTEMP", nIdCount++ );

    m_mapControl.insert( UITextureList::ControlMapValue( strID.GetString(), UITextureList::SControl() ) );
}

bool CEditor::DelControl()
{
    if( GetSelectedID().empty() == TRUE )
    {
        return false;
    }

    UITextureList::ControlMapIter itFind = m_mapControl.find( GetSelectedID().c_str() );
    if( itFind != m_mapControl.end() )
    {
        m_mapControl.erase( itFind );
        return true;
    }

    return false;
}


bool CEditor::FindControl( D3DXVECTOR2 vPos, std::string& strOutItem )
{
    const std::string& strSelectedID = g_pEditor->GetSelectedID();
    UITextureList::ControlMap& mapControl = g_pEditor->GetControlMap();

    UITextureList::ControlMapIter itControl = mapControl.find( strSelectedID );
    if( itControl == mapControl.end() )
        return false;

    std::string strTextureFilename = itControl->second.m_strFileName;
    
    

    // 선택한 좌표에 UI를 찾는다.
    UITextureList::ControlMapIter it;
    for( it = m_mapControl.begin() ; it != m_mapControl.end() ; it++ )
    {
        //if( strcmp( itControl->second.m_strFileName.c_str() ,strTextureFilename.c_str() ) == 0 )
        UITextureList::SControl& sControl = it->second;
        const std::string& strFindControlId = sControl.m_strFileName;
        if( boost::equal( strFindControlId ,strTextureFilename ) )
        {
            if( sControl.m_vPos.x < vPos.x && vPos.x < sControl.m_vPos.x + sControl.m_vPos.z
                && sControl.m_vPos.y < vPos.y && vPos.y < sControl.m_vPos.y + sControl.m_vPos.w )
            {
                strOutItem = it->first;
                return true;

            }
        }
    }

    return false;
}


