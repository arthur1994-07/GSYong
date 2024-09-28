#include "../pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/gassert.h"

#include "../../EngineLib/DxTools/DxInputDevice.h"

#include "../../RanLogic/Script/SquirrelMan.h"
#include "../../RanLogicClient/Char/GLCharacter.h"
#include "../Macro/MacroManager.h"

#include "./GLInputManager_KeySetting.h"
#include "./GLInputManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSKeySettingScriptMan
{
	class SQTargetID : public Sqrat::SqObjectType
	{
	public:
		SQTargetID()
		{
		}

		SQTargetID( const STARGETID& _sTargetID )
			: sTargetID(_sTargetID)
		{
		}

		virtual ~SQTargetID()
		{
		}

	public:
		operator STARGETID() const
		{
			return sTargetID;
		}

		const SQTargetID& operator=( const STARGETID& rhs )
		{
			sTargetID = rhs;
			return (*this);
		}

	private:
		STARGETID sTargetID;
	};

	class SQCharacterPtr : public Sqrat::SqObjectType
	{
	public:
		SQCharacterPtr()
		{
		}

		SQCharacterPtr( GLCharacter* _pCharacter )
			: pCharacter(_pCharacter)
		{
			GASSERT(pCharacter);
		}

		virtual ~SQCharacterPtr()
		{
		}

	public:
		const bool IsMoving() const
		{
			return pCharacter->isAction(GLAT_MOVE) ? true : false;
		}

		const bool IsMovingKeyDown() const
		{
			DWORD dwState = GLInputManager::GetInstance().GetKeyState(EMSHORTCUTS_MOVING);
			return !(dwState == DXKEY_IDLE || dwState == 0);
		}

		const SQTargetID GetSelectTargetBodyAround( const UINT nFinderType, const float fRange, const UINT nDegree )
		{
			return SQTargetID(pCharacter->GetSelectTargetBodyAround(static_cast<EMFIND_TAR>(nFinderType), fRange, nDegree));
		}

		const SQTargetID GetSelectTargetMouseAround( const UINT nFinderType, const float fRange )
		{
			return SQTargetID(pCharacter->GetSelectTargetMouseAround(static_cast<EMFIND_TAR>(nFinderType), fRange));
		}

	private:
		GLCharacter* pCharacter;
	};

	struct SQTableBinder : public Sqrat::ForEachFunction<std::string, Sqrat::Object>
	{
	private:
		typedef std::map<std::string,Sqrat::Function> MFUNCTION;
		typedef MFUNCTION::iterator                   MFUNCTION_ITER;

		typedef std::map<std::string,SQInteger>       MINTEGER;
		typedef MINTEGER::iterator                    MINTEGER_ITER;

	public:
		SQTableBinder( CKeySetting& _KeySetting )
			: cKeySetting(_KeySetting)
		{
		}

	public:
		virtual void operator()( const std::string& key, const Sqrat::Object& value )
		{
			switch( value.GetType() )
			{
			case OT_STRING:
				{
					std::string strValue(value.Cast<Sqrat::string>());

					EMSHORTCUTS emType = cKeySetting.GetShortCutsType(key);

					if ( emType == EMSHORTCUTS_NULL )
					{
						sc::writeLogError( "SQTableBinder::operator() - emType == EMSHORTCUTS_NULL" );
						GASSERT( !"SQTableBinder::operator() - emType == EMSHORTCUTS_NULL" );
						break;
					}

					const SSHORTCUTS& sShortcuts = cKeySetting.GetShortcuts(key);
					cKeySetting.SetShortcuts( emType, SSHORTCUTS(strValue, sShortcuts.IsHide()) );
				}
				break;

			default:
				{
					sc::writeLogError( "SQTableBinder::operator() - value.GetType() is not string type" );
					GASSERT( !"SQTableBinder::operator() - value.GetType() is not string type" );
				}
				break;
			};
		}

	private:
		CKeySetting& cKeySetting;

	};

	class SQKeySettingScriptMan : public SquirrelMan
	{
	public:
		SQKeySettingScriptMan( CKeySetting& _KeySetting )
			: cKeySetting(_KeySetting)
			, nManagedSelectTarget(0)
		{
			using namespace Sqrat;
			Sqrat::ClassType<SQTargetID>::Initialized()		= false;
			Sqrat::ClassType<SQCharacterPtr>::Initialized() = false;

			Sqrat::RootTable(m_vm).Bind( "SQTargetID", Sqrat::Class<SQTargetID>(m_vm)
			);

			Sqrat::RootTable(m_vm).Bind( "SQCharacterPtr", Sqrat::Class<SQCharacterPtr>(m_vm)
				.Func( "IsMoving", &SQCharacterPtr::IsMoving )
				.Func( "IsMovingKeyDown", &SQCharacterPtr::IsMovingKeyDown )
				.Func( "GetSelectTargetBodyAround",  &SQCharacterPtr::GetSelectTargetBodyAround )
				.Func( "GetSelectTargetMouseAround", &SQCharacterPtr::GetSelectTargetMouseAround )
			);

			Sqrat::ConstTable(m_vm)
				.Const( "ENFIND_TAR_NULL",		ENFIND_TAR_NULL )
				.Const( "EMFIND_TAR_ANY",		EMFIND_TAR_ANY )
				.Const( "EMFIND_TAR_ENEMY",		EMFIND_TAR_ENEMY )
				.Const( "EMFIND_TAR_OUR",		EMFIND_TAR_OUR )
				.Const( "EMFIND_TAR_OUR_DIE",	EMFIND_TAR_OUR_DIE )
				.Const( "EMFIND_TAR_ANY_DIE",	EMFIND_TAR_ANY_DIE );
		}

		virtual ~SQKeySettingScriptMan()
		{
		}

	public:
		virtual void ToDo()
		{
			using namespace Sqrat;

			try
			{
				//---------------------------------------------------------------------------
				//! 컨트롤 바인딩 관련
				Sqrat::Object ObjControlType = RootTable(m_vm).GetSlot( "ControlType" );

				if ( ObjControlType.IsNull() )
				{
					sc::writeLogError( sc::string::format( 
						"CKeySettingScriptMan::ToDo() - At least 'ControlType' table must exist (%1%)", 
						m_strFilePath) 
					);

					GASSERT( !"CKeySettingScriptMan::ToDo() - At least 'ControlType' table must exist" );
					return;
				}

				//! 테이블인지 체크용
				Sqrat::Table sqIsTable(ObjControlType);
				
				//! 키값들을 바인딩
				SQTableBinder sBinder(cKeySetting);
				Sqrat::ForEach(ObjControlType, sBinder);

				//---------------------------------------------------------------------------
				//! 컨트롤 로직 관련
				Sqrat::Object ObjControlLogic = RootTable(m_vm).GetSlot( "ControlLogic" );

				if ( !ObjControlLogic.IsNull() )
				{
					Sqrat::Table sqLogicTable(ObjControlLogic);
					sqFnSelectAround = sqLogicTable.GetFunction( "SelectAround" );
				}

				//---------------------------------------------------------------------------
				//! 컨트롤 상수 관련
				Sqrat::Object ObjControlConst = RootTable(m_vm).GetSlot( "ControlConst" );

				if ( !ObjControlConst.IsNull() )
				{
					Sqrat::Table sqConstTable(ObjControlConst);

					Sqrat::Object TempObj;

					TempObj = sqConstTable.GetSlot( "NumManagedSelectTarget" );
					if ( !TempObj.IsNull() )
					{
						nManagedSelectTarget = TempObj.Cast<UINT>();
					}

				}
			}
			catch( Sqrat::Exception ex )
			{
				sc::writeLogError( static_cast<std::string>(ex.Message()) );
				GASSERT_MSG(0, static_cast<std::string>(ex.Message()) );
			}
		}

	public:
		const STARGETID DoLogic_SelectAround( GLCharacter* pCharacter )
		{
			if ( sqFnSelectAround.IsNull() )
			{
				return TARGETID_NULL;
			}

			return sqFnSelectAround.Evaluate<SQTargetID, SQCharacterPtr>( SQCharacterPtr(pCharacter) );
		}

	private:
		Sqrat::Function sqFnSelectAround;

	public:
		UINT nManagedSelectTarget;
		const UINT GetManagedSelectTarget() const { return nManagedSelectTarget; }

	private:
		CKeySetting& cKeySetting;
	};

};

const bool CKeySetting::LoadSquirrel( const std::string& strFilePath )
{
	using namespace NSKeySettingScriptMan;

	m_spSqScript.reset(new SQKeySettingScriptMan((*this)));
	m_spSqScript->LoadScript( strFilePath.c_str() );

	if ( !m_spSqScript->IsLoaded() )
	{
		sc::writeLogError( sc::string::format( 
			"CKeySetting::LoadSquirrel() - '%1%' Load Failed", 
			strFilePath) 
		);

		GASSERT( !"CKeySetting::LoadSquirrel() - Load Failed" );
		return false;
	}

	return true;
}

const UINT CKeySetting::GetNumManagedSelectTarget() const
{
	if ( m_spSqScript )
	{
		return m_spSqScript->nManagedSelectTarget;
	}

	return 0;
}

const STARGETID CKeySetting::DoLogic_SelectAround_Squirrel( GLCharacter* pCharacter )
{
	if ( pCharacter->IsActState(EM_ACT_CONTINUEMOVE) == true )
		return pCharacter->GetSelectTargetBodyAround( EMFIND_TAR_ENEMY, 200.0, 160 );

	if ( GLMacroManager::GetInstance().IsActive() )
		return pCharacter->GetSelectTargetBodyAround( EMFIND_TAR_ENEMY, 100.0f, 360.f );
	else
		return pCharacter->GetSelectTargetMouseAround( EMFIND_TAR_ENEMY, 100.0 );
// 	if ( m_spSqScript )
// 	{
// 		return m_spSqScript->DoLogic_SelectAround(pCharacter);
// 	}
// 
// 	return TARGETID_NULL;
}

