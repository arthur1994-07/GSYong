#include "pch.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../Character/GLChar.h"
#include "../Util/GLItemLimit.h"
#include "../FieldServer\GLGaeaServer.h"
#include "../Database/DBAction/DbActionGameItem.h"
#include "./GLVehicleField.h"

#include "../../SigmaCore/DebugInclude.h"

namespace GLVEHICLE_FIELD
{
    void UpdateTimeLmtItem( GLGaeaServer* pGLGaeaServer, GLChar* pChar, GLVEHICLE* pVehicle )
    {
		if ( !pGLGaeaServer || !pChar || !pVehicle )
		{
			sc::writeLogError( std::string( "GLVEHICLE_FIELD::UpdateTimeLmtItem !pGLGaeaServer || !pChar || !pVehicle") );
			return;
		}

        const CTime cTIME_CUR = CTime::GetCurrentTime();

        for ( WORD i = 0; i < ACCE_TYPE_SIZE; ++i )
        {
            SITEMCUSTOM sVehicleItem = pVehicle->m_PutOnItems[i];
            if ( sVehicleItem.GetNativeID() == NATIVEID_NULL () ) continue;

            const SITEM* pITEM = GLogicData::GetInstance().GetItem( sVehicleItem.GetNativeID() );
            if ( !pITEM )
                continue;

            // ���Ѻ� ������
			if ( sVehicleItem.IsTimeLimit( SITEMCUSTOM::CHECKFLAGOP_NORMAL ) )
            {
                CTimeSpan cSPAN( pITEM->sDrugOp.tTIME_LMT );
                CTime cTIME_LMT( sVehicleItem.tBORNTIME );
                cTIME_LMT += cSPAN + sVehicleItem.GetPeriodExBasic();

                if ( cTIME_CUR > cTIME_LMT )
                {
                    // �ð� �������� ������ ���� �α� ����.
                    GLItemLimit* pItemLimit = pGLGaeaServer->GetItemLimit();
					// ��� �� �α� ����
                    pItemLimit->LogItemExchange( sVehicleItem, gamelog::ID_CHAR, pChar->m_CharDbNum, gamelog::ID_CHAR, 0, ITEM_ROUTE_DELETE, 0, ( pChar == NULL ) ? MapID( SNATIVEID( false ), SNATIVEID( false ) ) : pChar->GetCurrentMap(), LOG_ITEM_DEL_ITEM_BY_TIMEOUT, false );

					// ������ ����.
					{
						pVehicle->m_PutOnItems[i].SetDbState( db::DB_DELETE );

						switch ( pVehicle->m_PutOnItems[i].GetDbState() )
						{
						case db::DB_DELETE:
							{
								pGLGaeaServer->AddGameAdoJob(
									db::DbActionPtr( 
									new db::ItemDelete( pVehicle->m_PutOnItems[i].GetGuid(), pVehicle->m_PutOnItems[i].GetDbState(), DELLTE_VEHICLE_INVEN_TIMELIMIT_ITEM ) ) );
							}
							break;
						default:
							{
								sc::writeLogError( std::string( "GLVEHICLE_FIELD::UpdateTimeLmtItem ItemDelete db state error" ) );
							}
							break;
						}

						pVehicle->m_PutOnItems[i] = SITEMCUSTOM();
					}

                    // [�ڽſ���] �ش� �������� �ι꿡�� ����.
                    GLMSG::SNET_VEHICLE_ACCESSORY_DELETE NetMsg;
                    NetMsg.m_VehicleDbNum	= pVehicle->m_DbNum;
                    NetMsg.accetype = ( VEHICLE_ACCESSORYTYPE )i;
                    pGLGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsg );

                    // �ֺ��� �˸�.
                    GLMSG::SNET_VEHICLE_ACCESSORY_DELETE_BRD NetMsgBrd;
                    NetMsgBrd.dwGaeaID = pChar->GetGaeaID();
                    NetMsgBrd.m_VehicleDbNum = pVehicle->m_DbNum;
                    NetMsgBrd.accetype = ( VEHICLE_ACCESSORYTYPE )i;
                    pChar->SendMsgViewAround( &NetMsgBrd );

                    // �ð� �������� ������ ���� �˸�.
                    GLMSG::SNET_INVEN_DEL_ITEM_TIMELMT NetMsgInvenDelTimeLmt;
                    NetMsgInvenDelTimeLmt.nidITEM = sVehicleItem.GetNativeID();
                    pGLGaeaServer->SENDTOCLIENT( pChar->ClientSlot(), &NetMsgInvenDelTimeLmt );
                }
            }
        }
    }
}