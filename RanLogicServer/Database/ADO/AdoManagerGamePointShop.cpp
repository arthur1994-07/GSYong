#include "../../pch.h"
#include "./AdoManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace db
{

//! Character 의 PointShop Point 를 가져온다
//! ChaDbNum : 캐릭터 DB 번호
//! PointRefundable : 환불가능 포인트
//! PointNotRefundable : 환불불가 포인트
int AdoManager::GetChaPoint(int ChaDbNum, __int64& PointRefundable, __int64& PointNotRefundable)
{
    sc::db::AdoExt Ado(m_GameDBConnString);
    APPEND_IPARAM_INT(Ado, "@ChaNum", ChaDbNum);
    if (!Ado.Execute4Cmd("dbo.sp_GetChaPoint", adCmdStoredProc))
        return sc::db::DB_ERROR;

    if (Ado.GetEOF())
    {
        Ado.PRINT("AdoManager::GetChaPoint no data. RecordSet is Empty.");
        return sc::db::DB_ERROR;
    }

    do
    {            
        Ado.GetCollect(0, PointRefundable);
        Ado.GetCollect(1, PointNotRefundable);        
    } while (Ado.Next());

    return sc::db::DB_OK;
}

//! Character 의 PointShop Point 를 설정한다
//! ChaDbNum : 캐릭터 DB 번호
//! PointRefundable : 환불가능 포인트
//! PointNotRefundable : 환불불가 포인트
int AdoManager::SetChaPoint(int ChaDbNum, const __int64& PointRefundable, const __int64& PointNotRefundable)
{   
    sc::db::AdoExt AdoGame(m_GameDBConnString);
    AdoGame.AppendIParamInteger   ("@ChaNum", ChaDbNum);
    AdoGame.AppendIParamBigint("@ChaPointRefundable", PointRefundable);
    AdoGame.AppendIParamBigint("@ChaPointNotRefundable", PointNotRefundable);

    return AdoGame.ExecuteStoredProcedure("dbo.sp_SetChaPoint");
}

}