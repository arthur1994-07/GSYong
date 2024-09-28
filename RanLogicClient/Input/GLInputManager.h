//bjju.InputManager
//

#pragma once

#include "./GLInputManager_KeySetting.h"

#include <boost/noncopyable.hpp>

// ----------------------------------------------------------------------------

class GLGaeaClient;
class GLControlBase;

class GLInputManager : public boost::noncopyable
{
public:
	typedef std::map<EM_CONTROLTYPE, CKeySetting>   MAP_DEFAULT_KEYSETTING;
	typedef MAP_DEFAULT_KEYSETTING::iterator		MAP_DEFAULT_KEYSETTING_ITER;

public:

	const bool LoadOpt( gltexfile& cFile );
	const bool SaveOpt( FILE* pFile );
    const bool LoadOpt120( gltexfile& cFile );

	const bool LoadDefaultSetting( const EM_CONTROLTYPE emControlType, const std::string& strNutFilePath );

public:
	void InitEnvironment( GLGaeaClient* pGaeaClient );
	void InitControlType( const EM_CONTROLTYPE emControlType );

public:
	void FrameMove( float fTime, float fElapsedTime );
	//void SetKeyProc( const EMSHORTCUTS emShortcuts, )

public:
	const EM_CONTROLTYPE GetCurrentControlType() const { return m_emControlType; }
	const SSHORTCUTS& GetShortcuts( const EMSHORTCUTS emShortcuts );
	const DWORD GetKeyState ( const EMSHORTCUTS emShortcuts );

public:
	const std::string& GetShortcutsToString( const EMSHORTCUTS emShortcuts );
	const std::string& GetDefaultShortcutsToString( const EM_CONTROLTYPE emControlType, const EMSHORTCUTS emShortcuts );
	const std::string& GetDxKeyboardValueToString( const DWORD dwDIK );
	const std::string& GetNullKeyString();
	const SSHORTCUTS::KEY& GetStringToKey( const std::string& strKey );

public:
	void SetShortcutsDataFromString( const EMSHORTCUTS emShortcuts, const std::string& strKey );

	void InitControlTypeForWidget( WORD wControlType );
	void SetShortcutsDataFromStringForWidget( WORD wShortcutType, const char* strKey );
	std::string GetShortcutsToStringForWidget( WORD wShortcutType );
	std::string GetDefaultShortcutsToStringForWidget( WORD wControlType, WORD wShortcutType );

public:
	const bool IsShortcutsBlock();

public:
	const UINT GetNumManagedSelectTarget();

public:
	void ExternEvent_SkillTrayLeftClickUp(const WORD wQUICKSKILLSLOT);
	void ExternEvent_SkillTrayLeftClickUp( const SNATIVEID& sID, const WORD wQUICKSKILLSLOT );
	void ExternEvent_SelectAround();

private:
	EM_CONTROLTYPE	m_emControlType;
	GLControlBase*	m_pControl;

private:
	CKeySetting				m_CurrentKeySetting;
	MAP_DEFAULT_KEYSETTING	m_mDefaultKeySetting;

private:
	GLGaeaClient*  m_pGaeaClient;

public:
	static GLInputManager& GetInstance();

private:
	GLInputManager();
	~GLInputManager();

public :
	void DropHoldItem();
};
