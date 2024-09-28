#include "../pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/Common/gltexfile.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../GLGaeaClient.h"

#include "./GLInputManager.h"
#include "./GLControlTypeA.h"
#include "./GLControlTypeB.h"

#include <boost/bind.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLInputManager::GLInputManager()
	: m_emControlType(EM_CONTROLTYPE_DEFAULT)
	, m_pGaeaClient(NULL)
	, m_pControl(NULL)
{
}

GLInputManager::~GLInputManager()
{
	SAFE_DELETE(m_pControl);
}

void GLInputManager::InitEnvironment( GLGaeaClient* pGaeaClient )
{
	m_pGaeaClient = pGaeaClient;
}

void GLInputManager::InitControlType( const EM_CONTROLTYPE emControlType )
{
	GASSERT(m_pGaeaClient);

	// 선택 초기화
	GfxInterfaceBase::GetInstance()->ResetTargetInfoSelect();

	if ( m_emControlType == EM_CONTROLTYPE_FLYCAMERA )
		m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_ACTIVE, 0);

	if ( GLUseFeatures::GetInstance().IsUsingNewControlType() )
	{
		m_emControlType = emControlType;
	}
	else
	{
		m_emControlType = EM_CONTROLTYPE_DEFAULT;
	}

	//! 컨트롤 생성
	SAFE_DELETE(m_pControl);


	//! 기본 키 세팅 복사
	MAP_DEFAULT_KEYSETTING_ITER finditer = m_mDefaultKeySetting.find( m_emControlType );
	if ( finditer != m_mDefaultKeySetting.end() )
	{
		m_CurrentKeySetting = finditer->second;
	}
	else
	{
		GASSERT( !"GLInputManager::InitControlType() - not found default key setting" );
		sc::writeLogError( "GLInputManager::InitControlType() - not found default key setting" );
	}


	switch ( m_emControlType )
	{
	case EM_CONTROLTYPE_A:
		{
			m_pControl = new GLControlTypeA(m_pGaeaClient);
		}
		break;

	case EM_CONTROLTYPE_B:
		{
			m_pControl = new GLControlTypeB(m_pGaeaClient);
		}
		break;

	case EM_CONTROLTYPE_FLYCAMERA:
		{
			m_pControl = new GLControlTypeFlyCamera(m_pGaeaClient);
			//m_CurrentKeySetting.SetCameraMoveShortcuts();
			m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_ACTIVE, 1);
		}
		break;
	};

	if ( m_pControl )
	{
		m_pControl->Init( m_CurrentKeySetting );
	}
}

const bool GLInputManager::LoadOpt( gltexfile& cFile )
{
	int nValue = 0;
	if ( cFile.getflag ( "emControlType", 1, 1, nValue ) )
	{
		m_emControlType = static_cast<EM_CONTROLTYPE>(nValue);
		
		if( m_emControlType >= EM_CONTROLTYPE_SIZE ) 
			m_emControlType = EM_CONTROLTYPE_DEFAULT;

		InitControlType( m_emControlType );
	}

	if ( !m_CurrentKeySetting.Load( cFile ) )
	{
		GASSERT( !"GLInputManager::Load( gltexfile ) - failed load" );
		return false;
	}

	return true;
}

const bool GLInputManager::SaveOpt( FILE* pFile )
{
	if ( !pFile )
	{
		GASSERT( !"GLInputManager::Save( FILE* ) - pFile is null" );
		return false;
	}

	int nValue = static_cast<int>(m_emControlType);
	// FlyCamera 옵션은 저장시키지 않는다;
	if ( nValue == (int)EM_CONTROLTYPE_FLYCAMERA )
		nValue = (int)EM_CONTROLTYPE_A;

	fprintf ( pFile, "emControlType\t%d\n", nValue);

	if ( !m_CurrentKeySetting.Save( pFile ) )
	{
		GASSERT( !"GLInputManager::Save( FILE* ) - failed save" );
		return false;
	}
	
	return true;
}

const bool GLInputManager::LoadOpt120( gltexfile& cFile )
{
    int nValue = 0;
    if ( cFile.getflag ( "emControlType", 1, 1, nValue ) )
    {
        m_emControlType = static_cast<EM_CONTROLTYPE>(nValue);

        if( m_emControlType >= EM_CONTROLTYPE_SIZE ) 
            m_emControlType = EM_CONTROLTYPE_DEFAULT;

        InitControlType( m_emControlType );
    }

    if ( !m_CurrentKeySetting.Load120( cFile ) )
    {
        GASSERT( !"GLInputManager::Load( gltexfile ) - failed load" );
        return false;
    }

    //m_pInterface->ResetKeyBinding();

    return true;
}

const bool GLInputManager::LoadDefaultSetting( const EM_CONTROLTYPE emControlType, const std::string& strNutFilePath )
{
	return m_mDefaultKeySetting[emControlType].LoadSquirrel(strNutFilePath);
}

GLInputManager& GLInputManager::GetInstance()
{
	static GLInputManager sInstance;
	return sInstance;
}

void GLInputManager::FrameMove( float fTime, float fElapsedTime )
{
	if ( m_pControl )
	{
		m_pControl->FrameMove( fTime, fElapsedTime );
	}
}

const SSHORTCUTS& GLInputManager::GetShortcuts( const EMSHORTCUTS emShortcuts )
{
	return m_CurrentKeySetting.GetShortcuts( emShortcuts );
}

const DWORD GLInputManager::GetKeyState ( const EMSHORTCUTS emShortcuts )
{
	return m_CurrentKeySetting.GetKeyState(emShortcuts);
}

const std::string& GLInputManager::GetShortcutsToString( const EMSHORTCUTS emShortcuts )
{
	return m_CurrentKeySetting.GetString( emShortcuts );
}

const std::string& GLInputManager::GetDefaultShortcutsToString( const EM_CONTROLTYPE emControlType, const EMSHORTCUTS emShortcuts )
{
	switch( emControlType )
	{
	case EM_CONTROLTYPE_A: return m_mDefaultKeySetting[EM_CONTROLTYPE_A].GetString( emShortcuts );
	case EM_CONTROLTYPE_B: return m_mDefaultKeySetting[EM_CONTROLTYPE_B].GetString( emShortcuts );	
	};

	return GetNullKeyString();
}

const std::string& GLInputManager::GetDxKeyboardValueToString( const DWORD dwDIK )
{
	return SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::KEYBOARD(dwDIK) );
}

const std::string& GLInputManager::GetNullKeyString()
{
	return SKEYSTRINGBINDER::GetInstance().GetNullKey();
}

const SSHORTCUTS::KEY& GLInputManager::GetStringToKey( const std::string& strKey )
{
	return SKEYSTRINGBINDER::GetInstance().GetKey(strKey);
}

void GLInputManager::SetShortcutsDataFromString( const EMSHORTCUTS emShortcuts, const std::string& strKey )
{
	{
		const SSHORTCUTS& sShortcuts = m_CurrentKeySetting.GetShortcuts( emShortcuts );
		
		if ( sShortcuts.IsHide() )
		{
			GASSERT(!"GLInputManager::SetShortcutsDataFromString() - sShortcuts.IsNull()");
			return; 
		}
	}

	m_CurrentKeySetting.SetShortcuts( emShortcuts, SSHORTCUTS(strKey, false) );
}

void GLInputManager::InitControlTypeForWidget( WORD wControlType )
{
	InitControlType( EM_CONTROLTYPE( wControlType ) );
}

void GLInputManager::SetShortcutsDataFromStringForWidget( WORD wShortcutType, const char* strKey )
{
	SetShortcutsDataFromString( EMSHORTCUTS( wShortcutType ), strKey );
}

std::string GLInputManager::GetShortcutsToStringForWidget( WORD wShortcutType )
{
	return GetShortcutsToString( EMSHORTCUTS( wShortcutType ) ).c_str();
}

std::string GLInputManager::GetDefaultShortcutsToStringForWidget( WORD wControlType, WORD wShortcutType )
{
	return GetDefaultShortcutsToString( EM_CONTROLTYPE( wControlType ), EMSHORTCUTS( wShortcutType ) ).c_str();
}

const bool GLInputManager::IsShortcutsBlock()
{
	if ( !m_pControl )
	{
		return true;
	}

	return m_pControl->IsShortcutsBlock( 0.0f, 0.0f );
}

const UINT GLInputManager::GetNumManagedSelectTarget()
{
	return m_CurrentKeySetting.GetNumManagedSelectTarget();
}

void GLInputManager::ExternEvent_SkillTrayLeftClickUp( const WORD wQUICKSKILLSLOT )
{
	if ( !m_pControl )
	{
		return;
	}

	m_pControl->ExternEvent_SkillTrayLeftClickUp(wQUICKSKILLSLOT);
}

void GLInputManager::ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT )
{
	if( NULL == m_pControl )
		return;

	m_pControl->ExternEvent_SkillTrayLeftClickUp( sID, wQUICKSKILLSLOT );
}

void GLInputManager::ExternEvent_SelectAround()
{
	if ( !m_pControl )
	{
		return;
	}

	m_pControl->ExternEvent_SelectAround();
}

void GLInputManager::DropHoldItem()
{
	if( NULL == m_pControl )
		return;

	m_pControl->DropHoldItem();
}