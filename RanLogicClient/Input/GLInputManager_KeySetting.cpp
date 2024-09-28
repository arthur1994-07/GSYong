#include "../pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/Common/gltexfile.h"

#include "./GLInputManager_KeySetting.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/tokenizer.hpp>

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void SSHORTCUTS::ParseFormula( const std::string& strFormulaKey )
{
	vecKey.clear();

	std::string strInfix(strFormulaKey);
	boost::trim(strInfix);

	std::string strPostfix = InfixToPostfixNotation( strInfix );

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	static boost::char_separator<char> s_sep("\t ");
	tokenizer tokens(strPostfix, s_sep);

	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		PushKey( SKEYSTRINGBINDER::GetInstance().GetKey((*tok_iter))  );
	}

	strKey = strFormulaKey;
}

std::string SSHORTCUTS::InfixToPostfixNotation( IN const std::string& strInfixKey )
{
	std::string strSep;
	strSep += SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::OPERATOR_AND() );
	strSep += SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::OPERATOR_OR() );
	strSep += SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::OPERATOR_BRACKET_CLOSE() );
	strSep += SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::OPERATOR_BRACKET_OPEN() );
	strSep += SKEYSTRINGBINDER::GetInstance().GetString( SSHORTCUTS::OPERATOR_NEGATIVE_OPERATOR() );

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	static boost::char_separator<char> s_sep("", strSep.c_str(), boost::keep_empty_tokens );
	tokenizer tokens(strInfixKey, s_sep);

	static std::vector<std::string>		s_vecOutfix;
	static std::vector<SSHORTCUTS::KEY> s_vecOPStack;
	static std::string					s_strBuild;

	s_vecOutfix.clear();
	s_vecOPStack.clear();
	s_strBuild.clear();

	for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
	{
		const SSHORTCUTS::KEY& sKey = SKEYSTRINGBINDER::GetInstance().GetKey(*tok_iter);

		if ( sKey.IsKeyType() )
		{
			//! 임시 스트임에 저장중이라면 계속 넣는다.
			if ( !s_strBuild.empty() )
			{
				s_strBuild += (*tok_iter);
				continue;
			}

			s_vecOutfix.push_back(*tok_iter);
		}
		else
		{
			switch ( sKey.emTYPE )
			{
			/* " " */
			case EMKEYTYPE_NEGATIVE_OPERATOR:
				{
					//! 부정연산자를 만나면 다시 만날때까지 스트링에 담는다.
					if ( s_strBuild.empty() )
					{
						s_strBuild += (*tok_iter);
					}
					//! 다시 만났다면 임시 스트링에 저장한 스트링을 푸쉬한다.
					else
					{
						s_vecOutfix.push_back(s_strBuild);
						s_strBuild.clear();
					}
				}
				break;

			case EMKEYTYPE_AND:
			case EMKEYTYPE_OR:
				{
					while ( !s_vecOPStack.empty() )
					{
						SSHORTCUTS::KEY& sPopKey = s_vecOPStack.back();

						if ( sPopKey < sKey )
						{
							break;
						}

						s_vecOutfix.push_back( SKEYSTRINGBINDER::GetInstance().GetString( sPopKey ) );
						s_vecOPStack.pop_back();
					}

					s_vecOPStack.push_back( sKey );
				}
				break;

			case EMKEYTYPE_BRACKET_OPEN:
				{
					s_vecOPStack.push_back(sKey);
				}
				break;

			case EMKEYTYPE_BRACKET_CLOSE:
				{
					const SSHORTCUTS::OPERATOR_BRACKET_OPEN sKeyOpenBracket;

					while ( !s_vecOPStack.empty() )
					{
						SSHORTCUTS::KEY& sPopKey = s_vecOPStack.back();

						if ( sPopKey == sKeyOpenBracket )
						{
							s_vecOPStack.pop_back();
							break;
						}

						s_vecOutfix.push_back( SKEYSTRINGBINDER::GetInstance().GetString( sPopKey ) );
						s_vecOPStack.pop_back();
					}
				}
				break;
			};
		}
	}

	while( !s_vecOPStack.empty() )
	{
		SSHORTCUTS::KEY& sPopKey = s_vecOPStack.back();
		s_vecOutfix.push_back( SKEYSTRINGBINDER::GetInstance().GetString( sPopKey ) );
		s_vecOPStack.pop_back();
	}

	std::string strOutfix;
	std::vector<std::string>::iterator iterOufix = s_vecOutfix.begin();
	for ( ; iterOufix != s_vecOutfix.end(); ++iterOufix )
	{
		strOutfix += (*iterOufix);
		strOutfix += " ";
	}

	return strOutfix;
}

void SSHORTCUTS::PushKey( const SSHORTCUTS::KEY& sKey )
{
	if ( sKey.IsNull() )
	{
		std::string strError = "SKEYSTRINGBINDER::PushKey() - sKey.IsNull()";
		GASSERT_MSG(!sKey.IsNull(), strError);
		sc::writeLogError( strError );
		return;
	}

	if ( sKey.IsKeyType() )
	{
		if ( vecKey.empty() )
		{
			++nMinimumKeyNum;
		}
	}
	else
	{
		if ( vecKey.empty() )
		{
			//! 첨부터 연산자를 넣을 수 없다.
			std::string strError = "SKEYSTRINGBINDER::PushKey() - vecKey.empty()";
			GASSERT_MSG(!vecKey.empty(), strError);
			sc::writeLogError( strError );
			return;

		}
		else
		{
			if ( sKey.emTYPE == EMKEYTYPE_AND )
			{
				++nMinimumKeyNum;
			}
		}
	}

	vecKey.push_back(sKey);
}

void SSHORTCUTS::BuildString()
{
	static std::vector<std::string> s_vStack;
	s_vStack.clear();

	strKey = "";

	if ( vecKey.empty() )
	{
		return;
	}

	VEC_KEY_CITER iter = vecKey.begin();
	for ( ; iter!=vecKey.end(); ++iter )
	{
		const KEY& sKey = (*iter);

		if ( sKey.IsKeyType() )
		{
			s_vStack.push_back( SKEYSTRINGBINDER::GetInstance().GetString( sKey ) );
		}
		else
		{
			switch ( sKey.emTYPE )
			{
			default:
				{
					std::string strError = sc::string::format( "SSHORTCUTS::BuildString() - invalid key type '%1%'", sKey.emTYPE );
					GASSERT_MSG(0, strError);
					sc::writeLogError( strError );
					return;
				}
				break;

			case EMKEYTYPE_AND:
			case EMKEYTYPE_OR:
				{
					GASSERT( 2 <= static_cast<UINT>(s_vStack.size()) );

					std::string strFormula("");
					strFormula += s_vStack.back();
					s_vStack.pop_back();

					strFormula += SKEYSTRINGBINDER::GetInstance().GetString( sKey ); //AND 연산자

					strFormula += s_vStack.back();
					s_vStack.pop_back();

					if ( !s_vStack.empty() )
					{
						std::string strBrackets;
						strBrackets += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_OPEN() );
						strBrackets += strFormula;
						strBrackets += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_CLOSE() );

						strFormula = strBrackets;
					}

					s_vStack.push_back( strFormula );
				}
				break;
	
			};
		}
	}

	GASSERT( !s_vStack.empty() && (s_vStack.size() == 1) );
	strKey = s_vStack[0];
}

void SSHORTCUTS::AND_SynthesisKey( const SSHORTCUTS::KEY& sKey )
{
	vecKey.push_back( sKey );
	vecKey.push_back( OPERATOR_AND() );
	++nMinimumKeyNum;

	strKey += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_AND() );
	strKey += SKEYSTRINGBINDER::GetInstance().GetString( sKey );
}

void SSHORTCUTS::OR_SynthesisKey( const SSHORTCUTS::KEY& sKey )
{
	vecKey.push_back( sKey );
	vecKey.push_back( OPERATOR_OR() );

	strKey += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_OR() );
	strKey += SKEYSTRINGBINDER::GetInstance().GetString( sKey );
}

void SSHORTCUTS::AND_SynthesisShortcuts( const SSHORTCUTS& rhs )
{
	std::string strLhs(strKey);
	if ( 2 <= static_cast<UINT>(vecKey.size()) )
	{
		strLhs = SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_OPEN() );
		strLhs += strKey;
		strLhs += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_CLOSE() );
	}

	std::string strRhs(rhs.GetString());
	if ( 2 <= static_cast<UINT>(rhs.vecKey.size()) )
	{
		strRhs = SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_OPEN() );
		strRhs += rhs.GetString();
		strRhs += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_CLOSE() );
	}

	strKey  = strLhs;
	strKey += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_AND() );
	strKey += strRhs;

	vecKey.insert( vecKey.end(), rhs.vecKey.begin(), rhs.vecKey.end() );
	vecKey.push_back( OPERATOR_AND() );
	nMinimumKeyNum += rhs.GetMinimumKeyNum();
}

void SSHORTCUTS::OR_SynthesisShortcuts( const SSHORTCUTS& rhs )
{
	std::string strLhs(strKey);
	if ( 2 <= static_cast<UINT>(vecKey.size()) )
	{
		strLhs = SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_OPEN() );
		strLhs += strKey;
		strLhs += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_CLOSE() );
	}

	std::string strRhs(rhs.GetString());
	if ( 2 <= static_cast<UINT>(rhs.vecKey.size()) )
	{
		strRhs = SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_OPEN() );
		strRhs += rhs.GetString();
		strRhs += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_BRACKET_CLOSE() );
	}

	strKey  = strLhs;
	strKey += SKEYSTRINGBINDER::GetInstance().GetString( OPERATOR_OR() );
	strKey += strRhs;

	vecKey.insert( vecKey.end(), rhs.vecKey.begin(), rhs.vecKey.end() );
	vecKey.push_back( OPERATOR_OR() );
	nMinimumKeyNum = max(nMinimumKeyNum, rhs.GetMinimumKeyNum());
}

// ---------------------------------------------------------------------------- //

SKEYSTRINGBINDER& SKEYSTRINGBINDER::GetInstance()
{
	static SKEYSTRINGBINDER sInstance;
	return sInstance;
}

// ---------------------------------------------------------------------------- //

CKeySetting::~CKeySetting()
{
	//Blank
}

const bool CKeySetting::Load( gltexfile& cFile )
{
	MAP_KEYBINDING_NAME_ITER iter = m_mName.begin();
	for ( ; iter!=m_mName.end(); ++iter )
	{
		const std::string& strName	   = iter->first;
		EMSHORTCUTS		   emShortCuts = iter->second;

		const SSHORTCUTS& sKey = GetShortcuts( emShortCuts );

		if ( sKey.IsNull() )
		{
			continue;
		}

		if ( sKey.IsHide() )
		{
			continue;
		}

        std::string strShortcuts;
        if ( !cFile.getflag( strName, 1, 1, strShortcuts ) )
        {
            continue;
        }

        SetShortcuts( emShortCuts, SSHORTCUTS(strShortcuts, false) );

	}

	return true;
}

const bool CKeySetting::Save( FILE* pFile )
{
	if ( !pFile )
	{
		return false;
	}

	MAP_KEYBINDING_NAME_ITER iter = m_mName.begin();
	for ( ; iter!=m_mName.end(); ++iter )
	{
		const std::string& strName	   = iter->first;
		EMSHORTCUTS		   emShortCuts = iter->second;

		const SSHORTCUTS& sShortcuts = GetShortcuts(emShortCuts);
		
		if ( sShortcuts.IsHide() )
		{
			continue;
		}

        fprintf ( pFile, strName.c_str() );
        fprintf ( pFile, "\t" );
        fprintf ( pFile, "%s",  sShortcuts.GetString().c_str() );
        fprintf ( pFile, "\n" );

        //SSHORTCUTS::VEC_KEY_CITER keyIter    = sShortcuts.IterBegin();
        //SSHORTCUTS::VEC_KEY_CITER keyIterEnd = sShortcuts.IterEnd();

        //for ( UINT i=0; keyIter!=keyIterEnd; ++i, ++keyIter )
        //{
        //	fprintf ( pFile, "," );

        //	const SSHORTCUTS::KEY& sKey = (*keyIter);
        //	std::string	strFormat = sc::string::format("%1%,%2%", static_cast<UINT>(sKey.emTYPE), sKey.nVALUE);

        //	fprintf ( pFile, strFormat.c_str() );
        //}

        //fprintf ( pFile, "}\n" );

	}

	return true;
}

const bool CKeySetting::Load120( gltexfile& cFile )
{
    MAP_KEYBINDING_NAME_ITER iter = m_mName.begin();
    for ( ; iter!=m_mName.end(); ++iter )
    {
        const std::string& strName	   = iter->first;
        EMSHORTCUTS		   emShortCuts = iter->second;

        const SSHORTCUTS& sKey = GetShortcuts( emShortCuts );

        if ( sKey.IsNull() )
        {
            continue;
        }

        if ( sKey.IsHide() )
        {
            continue;
        }

        DWORD dwParamNum = cFile.getparamnum( 0, strName );

        if ( dwParamNum <= 1 )
        {
            continue;
        }

        DWORD dwVer = 0;

        if ( !cFile.getflag( strName, 1, static_cast<int>(dwParamNum-1), dwVer ) )
        {
            continue;
        }

        switch ( dwVer )
        {
        case VERSION:
            {
                UINT nSize = 0;
                if ( !cFile.getflag( strName, 2, static_cast<int>(dwParamNum-1), nSize ) )
                {
                    continue;
                }

                std::string strShortcuts;
                if ( !cFile.getflag( strName, 3, static_cast<int>(dwParamNum-1), strShortcuts ) )
                {
                    continue;
                }

                SetShortcuts( emShortCuts, SSHORTCUTS(strShortcuts, false) );


                //int nKeyNum = static_cast<int>((dwParamNum-1) - 2) / 2;

                //if ( nKeyNum < 1 )
                //{
                //	continue;
                //}

                //SSHORTCUTS sShortcuts;

                //for ( int i=0; i<nKeyNum; ++i )
                //{
                //	EMKEYTYPE emKeyType = EMKEYTYPE_NULL;
                //	UINT	  nValue	= 0;

                //	cFile.getflag( strName, (i*2)+3, static_cast<int>(dwParamNum-1), emKeyType );
                //	cFile.getflag( strName, (i*2)+4, static_cast<int>(dwParamNum-1), nValue );

                //	sShortcuts.PushKey( SSHORTCUTS::KEY(emKeyType,nValue) );
                //}

                //SetShortcuts( emShortCuts, sShortcuts );
            }
            break;
        };
    }

    return true;
}

const DWORD CKeySetting::GetKeyState ( const EMSHORTCUTS emShortcuts, DWORD dwOption ) const
{
	const SSHORTCUTS& sShortcut = GetShortcuts( emShortcuts );
	
	if ( sShortcut.IsNull() )
	{
		return NULL;
	}

	if ( IsBlockKeyState(emShortcuts) )
	{
		return NULL;
	}

	return sShortcut.GetKeyState(dwOption, &m_setBlockKeys);
}

const int CKeySetting::GetKeyAmount( const EMSHORTCUTS emShortcuts ) const
{
	const SSHORTCUTS& sShortcut = GetShortcuts( emShortcuts );

	if ( sShortcut.IsNull() )
	{
		return 0;
	}

	return sShortcut.GetKeyAmount( &m_setBlockKeys );
}

void CKeySetting::GetMouseMove( OUT int& dx, OUT int& dy, OUT int& dz )
{
	DxInputDevice::GetInstance().GetMouseMove(dx,dy,dz);
}

void CKeySetting::BlockKeyState( const EMSHORTCUTS emShortcuts )
{
	const SSHORTCUTS& sShortcut = GetShortcuts( emShortcuts );

	if ( sShortcut.IsNull() )
	{
		return;
	}

	SSHORTCUTS::VEC_KEY_CITER iter = sShortcut.IterBegin();
	for ( ; iter != sShortcut.IterEnd(); ++iter )
	{
		const SSHORTCUTS::KEY& sKey = (*iter);

		if ( sKey.IsKeyType() )
		{
			m_setBlockKeys.insert(sKey);
		}
	}

	m_setBlockShortcuts.insert(emShortcuts);
}

void CKeySetting::ResetBlockKeyState( const EMSHORTCUTS emShortcuts )
{
	const SSHORTCUTS& sShortcut = GetShortcuts( emShortcuts );

	if ( m_setBlockKeys.empty() )
	{
		return;
	}

	if ( sShortcut.IsNull() )
	{
		return;
	}

	SSHORTCUTS::VEC_KEY_CITER iter = sShortcut.IterBegin();
	for ( ; iter != sShortcut.IterEnd(); ++iter )
	{
		const SSHORTCUTS::KEY& sKey = (*iter);

		if ( sKey.IsKeyType() )
		{
			SSHORTCUTS::SET_BLOCK_KEYS_ITER finditer = m_setBlockKeys.find(sKey);
			if ( finditer != m_setBlockKeys.end() )
			{
				m_setBlockKeys.erase(finditer);
			}
		}
	}

	SET_BLOCK_SHORTCUTS_ITER finditer = m_setBlockShortcuts.find(emShortcuts);
	if ( finditer != m_setBlockShortcuts.end() )
	{
		m_setBlockShortcuts.erase(finditer);
	}
}

const bool CKeySetting::IsBlockKeyState ( const EMSHORTCUTS emShortcuts ) const
{
	SET_BLOCK_SHORTCUTS_CITER finditer = m_setBlockShortcuts.find(emShortcuts);
	if ( finditer != m_setBlockShortcuts.end() )
	{
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------//

const DWORD SSHORTCUTS::GetKeyState( DWORD dwOption, const SET_BLOCK_KEYS* pBlockKeys ) const
{
	static std::vector<DWORD> s_vStack;
	s_vStack.clear();

	if ( vecKey.empty() )
	{
		return NULL;
	}

	DWORD dwTotalState = 0;

	VEC_KEY_CITER iter = vecKey.begin();
	for ( ; iter!=vecKey.end(); ++iter )
	{
		const KEY& sKey = (*iter);
		DWORD dwState = 0;

		switch ( sKey.emTYPE )
		{
		case EMKEYTYPE_KEYBOARD:
			{
				if ( pBlockKeys )
				{
					SET_BLOCK_KEYS_CITER finditer = pBlockKeys->find(sKey);
					if ( finditer != pBlockKeys->end() )
					{
						s_vStack.push_back(dwState);
						continue;
					}
				}

				if ( !(dwOption&FLAG_GETKEYSTATE_BLOCK_KEYBOARD) )
				{
					dwState = DxInputDevice::GetInstance().GetKeyState(static_cast<BYTE>(sKey.nVALUE));
				}

				s_vStack.push_back(dwState);
				continue;
			}
			break;

		case EMKEYTYPE_MOUSE:
			{
				if ( pBlockKeys )
				{
					SET_BLOCK_KEYS_CITER finditer = pBlockKeys->find(sKey);
					if ( finditer != pBlockKeys->end() )
					{
						s_vStack.push_back(dwState);
						continue;
					}
				}

				if ( !(dwOption&FLAG_GETKEYSTATE_BLOCK_MOUSE) )
				{
					dwState = DxInputDevice::GetInstance().GetMouseState(static_cast<DX_MOUSEKEYS>(sKey.nVALUE));
				}

				s_vStack.push_back(dwState);
				continue;
			}
			break;

		case EMKEYTYPE_AND:
			{
				GASSERT( 2 <= static_cast<UINT>(s_vStack.size()) );
				
				DWORD dwState1 = s_vStack.back();
				s_vStack.pop_back();

				DWORD dwState2 = s_vStack.back();
				s_vStack.pop_back();

				s_vStack.push_back( GetKeyStateAND( dwState1, dwState2 ) );
			}
			break;

		case EMKEYTYPE_OR:
			{
				GASSERT( 2 <= static_cast<UINT>(s_vStack.size()) );

				DWORD dwState1 = s_vStack.back();
				s_vStack.pop_back();

				DWORD dwState2 = s_vStack.back();
				s_vStack.pop_back();

				s_vStack.push_back( GetKeyStateOR( dwState1, dwState2 ) );
			}
			break;
		};
	}

	GASSERT( !s_vStack.empty() && (s_vStack.size() == 1) );
	return s_vStack[0];
}

const DWORD SSHORTCUTS::GetKeyStateAND( DWORD dwState1, DWORD dwState2 ) const
{
	if ( dwState1==0 || dwState2==0 )
	{
		return DXKEY_IDLE;
	}

	if ( dwState1==DXKEY_IDLE || dwState2==DXKEY_IDLE )
	{
		return DXKEY_IDLE;
	}

	DWORD dwResult = dwState1;

	if ((dwState1&DXKEY_DUP) || (dwState2&DXKEY_DUP))
	{
		dwResult = DXKEY_DUP;
	}
	else if ((dwState1&DXKEY_UP) || (dwState2&DXKEY_UP))
	{
		dwResult = DXKEY_UP;
	}
	else if ((dwState1&DXKEY_DOWN) || (dwState2&DXKEY_DOWN))
	{
		dwResult = DXKEY_DOWN;
	}
	else if ((dwState1&DXKEY_PRESSED) || (dwState2&DXKEY_PRESSED))
	{
		dwResult = DXKEY_PRESSED;
	}

	if ((dwState1&DXKEY_IDLE) || (dwState2&DXKEY_IDLE))
	{
		dwResult |= DXKEY_IDLE;
	}

	if ((dwState1&DXKEY_DRAG) || (dwState2&DXKEY_DRAG))
	{
		dwResult |= DXKEY_DRAG;
	}

	return dwResult;
}

const DWORD SSHORTCUTS::GetKeyStateOR( DWORD dwState1, DWORD dwState2 ) const
{
	DWORD dwResult = dwState1;

	if ((dwState1&DXKEY_PRESSED) || (dwState2&DXKEY_PRESSED))
	{
		dwResult = DXKEY_PRESSED;
	}
	else if ((dwState1&DXKEY_DOWN) || (dwState2&DXKEY_DOWN))
	{
		dwResult = DXKEY_DOWN;
	}
	else if ((dwState1&DXKEY_UP) || (dwState2&DXKEY_UP))
	{
		dwResult = DXKEY_UP;
	}
	else if ((dwState1&DXKEY_DUP) || (dwState2&DXKEY_DUP))
	{
		dwResult = DXKEY_DUP;
	}

	if ((dwState1&DXKEY_IDLE) || (dwState2&DXKEY_IDLE))
	{
		dwResult |= DXKEY_IDLE;
	}

	if ((dwState1&DXKEY_DRAG) || (dwState2&DXKEY_DRAG))
	{
		dwResult |= DXKEY_DRAG;
	}

	return dwResult;
}

const int SSHORTCUTS::GetKeyAmount( const SET_BLOCK_KEYS* pBlockKeys ) const
{
	static std::vector<int> s_vStack;
	s_vStack.clear();

	if ( vecKey.empty() )
	{
		return NULL;
	}

	DWORD dwTotalState = 0;

	VEC_KEY_CITER iter = vecKey.begin();
	for ( ; iter!=vecKey.end(); ++iter )
	{
		const KEY& sKey = (*iter);
		int nAmount = 0;

		switch ( sKey.emTYPE )
		{
		default:
			continue;

		case EMKEYTYPE_KEYBOARD:
			{
				if ( pBlockKeys )
				{
					SET_BLOCK_KEYS_CITER finditer = pBlockKeys->find(sKey);
					if ( finditer != pBlockKeys->end() )
					{
						s_vStack.push_back(nAmount);
						continue;
					}
				}

				DWORD dwState = DxInputDevice::GetInstance().GetKeyState(static_cast<BYTE>(sKey.nVALUE));

				if ( dwState&DXKEY_DOWNED )
				{
					nAmount = 1;
				}
				else
				{
					nAmount = 0;
				}

				s_vStack.push_back(nAmount);
				continue;
			}
			break;

		case EMKEYTYPE_MOUSE:
			{
				if ( pBlockKeys )
				{
					SET_BLOCK_KEYS_CITER finditer = pBlockKeys->find(sKey);
					if ( finditer != pBlockKeys->end() )
					{
						s_vStack.push_back(nAmount);
						continue;
					}
				}

				if ( sKey.nVALUE == DXMOUSE_MIDDLE )
				{
					int dx=0,dy=0,dz=0;
					DxInputDevice::GetInstance().GetMouseMove ( dx, dy, dz );

					nAmount = dz;
				}
				else
				{
					DWORD dwState = DxInputDevice::GetInstance().GetMouseState(static_cast<DX_MOUSEKEYS>(sKey.nVALUE));

					if ( dwState&DXKEY_DOWNED )
					{
						nAmount = 1;
					}
					else
					{
						nAmount = 0;
					}
				}

				s_vStack.push_back(nAmount);
				continue;
			}
			break;

		case EMKEYTYPE_AND:
			{
				GASSERT( 2 <= static_cast<UINT>(s_vStack.size()) );

				int nAmount1 = s_vStack.back();
				s_vStack.pop_back();

				int nAmount2 = s_vStack.back();
				s_vStack.pop_back();

				s_vStack.push_back( nAmount1*nAmount2 );
			}
			break;

		case EMKEYTYPE_OR:
			{
				GASSERT( 2 <= static_cast<UINT>(s_vStack.size()) );

				int nAmount1 = s_vStack.back();
				s_vStack.pop_back();

				int nAmount2 = s_vStack.back();
				s_vStack.pop_back();

				s_vStack.push_back( nAmount1+nAmount2 );
			}
			break;
		};
	}

	GASSERT( !s_vStack.empty() && (s_vStack.size() == 1) );
	return s_vStack[0];

}

const STARGETID CKeySetting::DoLogic_SelectAround( GLCharacter* pCharacter )
{
	GASSERT(pCharacter);

	if ( m_spSqScript )
	{
		return DoLogic_SelectAround_Squirrel( pCharacter );
	}

	return TARGETID_NULL;
}
