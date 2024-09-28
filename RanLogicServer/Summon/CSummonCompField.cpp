#include "pch.h"

#include "./CSummonCompField.h"

#include "../FieldServer/GLGaeaServer.h"

#include "../SigmaCore/DebugInclude.h"

namespace SummonSystem
{
	CSummonCompField::CSummonCompField (
		GLActor* pOwner )
		: ICrowCompField( pOwner )
	{
		m_pServerOwner = static_cast< ServerActor* >( GetOwner() );
	}

	CSummonCompField::~CSummonCompField () { }

	const ComponentSystem::EMCOMP_ID CSummonCompField::GetComponentID () const
	{
		return ComponentSystem::EMCOMP_SUMMON;
	}

	const ComponentSystem::EMFAMILY_ID CSummonCompField::GetFamilyID () const
	{
		return ComponentSystem::EMFAMILY_CROW;
	}

	void CSummonCompField::Initialize ()
	{
		m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
		m_wRangeTracing = 0;
		m_dwReactCount = 0;
	}

	void CSummonCompField::Destroy ()
	{

	}

	void CSummonCompField::Reset ()
	{
		m_dwAtypeAfterTracing = SCROWDATA::EMMAXATTACK;
		m_wRangeTracing = 0;
		m_dwReactCount = 0;

		GLGaeaServer* pServer = m_pServerOwner->GetGaeaServer();
		if ( !pServer )
		{
			sc::writeLogError( "CSummonCompField::Reset - pServer is NULL." );
			return;
		}

		GLLandMan* pLand = m_pServerOwner->GetLandMan();
		if ( !pLand )
		{
			sc::writeLogError( "CSummonCompField::Reset - pLand is NULL." );
			return;
		}

		ServerActor* pSummoner = pServer->GetActor( pLand, m_sSummmonerID );
		if ( !pSummoner )
		{
			sc::writeLogError( "CSummonCompField::Reset - pSummoner is NULL." );
			return;
		}

		pSummoner->RemoveChildCrow( m_pServerOwner->GetActorID() );
	}

	void CSummonCompField::BeginUpdate ()
	{

	}

	void CSummonCompField::Update ( float fElapsedTime )
	{

	}

	void CSummonCompField::EndUpdate ()
	{

	}

	void CSummonCompField::SetCrowData ( const SCROWDATA& sCrowData )
	{
		m_fCurSpeed = sCrowData.m_sAction.m_fRunVelo;
	}

	void CSummonCompField::SetSummonData ( const GLSUMMON& sSummonData )
	{
		m_sSummonData.ASSIGN( sSummonData );
	}

	void CSummonCompField::SetSummoner ( const Faction::GLActorID& sSummonID )
	{
		m_sSummmonerID = sSummonID;
	}

	void CSummonCompField::IdleProcess ( float fElapsedTime )
	{

	}

	void CSummonCompField::MoveProcess ( float fElapsedTime )
	{
		// 속도 관련;
		/*float fMoveVelo = m_pServerOwner->GetMoveVelo();

		// 이속이 틀려지면 속도 갱신을 한다;
		if ( m_bOwnerSpeed && (m_fCurSpeed != fMoveVelo) )
		{
			// Note : 주변의 클라이언트에게 상태변화를 알림
			GLMSG::SNET_SUMMON_UPDATE_STATE	NetMsgBrd(GetGaeaID(), GetActState(), m_dwNowHP, m_emGestureState, GetMoveVelo());
			SendMsgViewAround(&NetMsgBrd);
		}

		m_fCurSpeed = fMoveVelo;*/
	}

	DWORD CSummonCompField::GETCLIENTID ()
	{
		GLGaeaServer* pServer = m_pServerOwner->GetGaeaServer();
		if ( !pServer )
		{
			sc::writeLogError( "CSummonCompField::Reset - pServer is NULL." );
			return 0;
		}

		GLLandMan* pLand = m_pServerOwner->GetLandMan();
		if ( !pLand )
		{
			sc::writeLogError( "CSummonCompField::Reset - pLand is NULL." );
			return 0;
		}

		ServerActor* pSummoner = pServer->GetActor( pLand, m_sSummmonerID );
		if ( !pSummoner )
		{
			sc::writeLogError( "CSummonCompField::Reset - pSummoner is NULL." );
			return 0;
		}

		return pSummoner->GETCLIENTID();
	}

	WORD CSummonCompField::GetSchool () const
	{
		GLGaeaServer* pServer = m_pServerOwner->GetGaeaServer();
		if ( !pServer )
		{
			sc::writeLogError( "CSummonCompField::Reset - pServer is NULL." );
			return 0;
		}

		GLLandMan* pLand = m_pServerOwner->GetLandMan();
		if ( !pLand )
		{
			sc::writeLogError( "CSummonCompField::Reset - pLand is NULL." );
			return 0;
		}

		ServerActor* pSummoner = pServer->GetActor( pLand, m_sSummmonerID );
		if ( !pSummoner )
		{
			sc::writeLogError( "CSummonCompField::Reset - pSummoner is NULL." );
			return 0;
		}

		return pSummoner->GetSchool();
	}

	SCONFTING* CSummonCompField::GETCONFTING ()
	{
		GLGaeaServer* pServer = m_pServerOwner->GetGaeaServer();
		if ( !pServer )
		{
			sc::writeLogError( "CSummonCompField::Reset - pServer is NULL." );
			return NULL;
		}

		GLLandMan* pLand = m_pServerOwner->GetLandMan();
		if ( !pLand )
		{
			sc::writeLogError( "CSummonCompField::Reset - pLand is NULL." );
			return NULL;
		}

		ServerActor* pSummoner = pServer->GetActor( pLand, m_sSummmonerID );
		if ( !pSummoner )
		{
			sc::writeLogError( "CSummonCompField::Reset - pSummoner is NULL." );
			return NULL;
		}

		return pSummoner->GETCONFTING();
	}

}