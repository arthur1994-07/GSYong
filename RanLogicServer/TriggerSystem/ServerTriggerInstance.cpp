#include "pch.h"
#include "ServerTriggerInstance.h"
#include "ServerTriggerFinger.h"

#include "../SigmaCore/DebugInclude.h"

#include "../PVE/PVEInDun.h"
#include "../PVE/PVESystem.h"
#include "../Database/DBAction/DbActionLogInstantDungeon.h"


using namespace TriggerSystem;

GLLandMan* CServerTriggerInstance::GetLandMan()
{
    CServerTriggerFinger* pFinger = 0;

    if (CastFinger(pFinger))
    {
        return pFinger->GetLandMan();
    }

    return 0;
}



void TriggerSystem::CServerTriggerInstance::WriteLogPlay( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage )
{
    CServerTriggerFinger* pFinger = 0;

    GLGaeaServer* pGaeaServer = 0;
    if( CastFinger(pFinger) )
        pGaeaServer = pFinger->GetGaeaServer();

    if( pGaeaServer == NULL )
        return;

    GLLandMan* pLand = GetLandMan();
	if( pLand )
	{
		pGaeaServer->AddLogAdoJob(
			db::DbActionPtr(
			new db::LogInstantInfiniteStaircasePlay(
			pFinger->GetGuid(),
			nPersonnel,
			dwChaDbNum,
			nChrLevel,
			nStage,
			pLand->GetBaseMapID()
			)));

	}


}

void TriggerSystem::CServerTriggerInstance::WriteLogEnd( const int& nPersonnel, const DWORD& dwChaDbNum, const int& nChrLevel, const int& nStage, const bool& bMVP, const LONGLONG& llMoney, const DWORD& dwExp )
{
    CServerTriggerFinger* pFinger = 0;

    GLGaeaServer* pGaeaServer = 0;
    if( CastFinger(pFinger) )
        pGaeaServer = pFinger->GetGaeaServer();

    if( pGaeaServer == NULL )
        return;

	GLLandMan* pLand = GetLandMan();
	if( pLand )
	{
		pGaeaServer->AddLogAdoJob(
			db::DbActionPtr(
			new db::LogInstantInfiniteStaircaseEnd(
			pFinger->GetGuid(),
			nPersonnel,
			dwChaDbNum,
			nChrLevel,
			nStage,
			bMVP,
			llMoney,
			dwExp,
			pLand->GetBaseMapID()
			)));
	}


}

void CServerTriggerInstance::SendMessage(SMessage* msg)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        pLandMan->SendTriggerMessage(msg);
    }
}

void CServerTriggerInstance::ReturnPC(SMessage* msg)
{
	/**
		�⺻������ ��ȯ �޽����� ����
		���� �ð��� 0 �� ���� �ؼ� ���̻� ������
		�Ұ��� �ϰ� ó�� �Ѵ�.

	 */
	SetLifeTime(0.f);
}

