#include "../../pch.h"

#include "../../Server/s_CFieldServer.h"
#include "../DbManager.h"
#include "./DbActionGameVehicle.h"



// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



/**
* ���ο� Ż���� �����Ѵ�.
* \param nChaNum  Ż�� ������ ĳ���� ��ȣ
* \param szVehicleName Ż�� �̸�
* \param nVehicleType Ż���� Ÿ��
* \param nVehicleMID Ż�� MID
* \param nVehicleSID Ż�� SID
* \param nVehicleCardMID Ż���� ������ MID
* \param nVehicleCardSID Ż���� ������ SID
* \return sc::db::DB_ERROR �Ǵ� ������������ Ż���� ������ȣ
*/
CCreateVehicle::CCreateVehicle(
	DWORD dwClientID,
	DWORD ChaDbNum,
	const TCHAR* szVehicleName,
	int nVehicleType,
	int nVehicleCardMID,
	int nVehicleCardSID)
	: m_ChaDbNum(ChaDbNum)
	, m_nVehicleType(nVehicleType)
	, m_nVehicleCardMID(nVehicleCardMID)
	, m_nVehicleCardSID(nVehicleCardSID)
{
	m_ClientSlot = dwClientID;
	m_strVehicleName = szVehicleName;
	m_strVehicleName.Trim(_T(" ")); // �յ� ��������
}

int CCreateVehicle::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CCreateVehicle::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult = m_pDbManager->CreateVehicle( m_ChaDbNum,
		m_strVehicleName.GetString(),
		m_nVehicleType,
		m_nVehicleCardMID, 
		m_nVehicleCardSID);	
	
	GLMSG::SNET_VEHICLE_CREATE_FROMDB_FB InternalMsg;
    InternalMsg.m_ChaDbNum = m_ChaDbNum;

	// ������������ ���� �޽��� �߼�
	if ( nResult != sc::db::DB_ERROR )
	{
        InternalMsg.emFB = EMVEHICLE_CREATE_FROMDB_FB_OK;
		InternalMsg.emTYPE = (VEHICLE_TYPE)m_nVehicleType;
		InternalMsg.sVehicleID.wMainID = m_nVehicleCardMID;
		InternalMsg.sVehicleID.wSubID = m_nVehicleCardSID;
		InternalMsg.m_VehicleDbNum = nResult;
		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(DBACTION_CLIENT_ID, &InternalMsg);
	}
	else
	{
    
        InternalMsg.emFB = EMVEHICLE_CREATE_FROMDB_FB_FAIL;
		InternalMsg.emTYPE = (VEHICLE_TYPE)m_nVehicleType;
		InternalMsg.sVehicleID.wMainID = m_nVehicleCardMID;
		InternalMsg.sVehicleID.wSubID = m_nVehicleCardSID;
		InternalMsg.m_VehicleDbNum = nResult;
		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg(DBACTION_CLIENT_ID, &InternalMsg);
	}
	return nResult;
}


/**
* Ż���� ������ �����´�.
*/
CGetVehicle::CGetVehicle(
	GLVEHICLE* pVehicle,
	DWORD dwVehicleNum,
	DWORD dwClientID,
	DWORD dwCharNum,
	bool  bLMTItemCheck,
	bool  bCardInfo,
	bool  bTrade
	)
	: m_pVehicle( pVehicle )
	, m_dwVehicleNum ( dwVehicleNum )
	, m_dwCharNum ( dwCharNum )
	, m_bLMTItemCheck ( bLMTItemCheck )
	, m_bCardInfo ( bCardInfo )
	, m_bTrade ( bTrade )

{
	m_ClientSlot = dwClientID;
}

int CGetVehicle::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CGetVehicle::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult = m_pDbManager->GetVehicle(m_pVehicle, m_dwVehicleNum, m_dwCharNum);

	if ( nResult == sc::db::DB_OK )
	{
		GLMSG::SNET_VEHICLE_GET_FROMDB_FB InternalMsg;
		InternalMsg.pVehicle = m_pVehicle;
		InternalMsg.m_VehicleDbNum = m_dwVehicleNum;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo = m_bCardInfo;
		InternalMsg.bTrade = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );
	}
	else if ( nResult == sc::db::DB_ROWCNT_ERROR )
	{

		GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR InternalMsg;		

        InternalMsg.emFB          = EMVEHICLE_GET_FROMDB_ERROR_FB_NODATA;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo	  = m_bCardInfo;
		InternalMsg.bTrade		  = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

		SAFE_DELETE ( m_pVehicle );		
	}
	else
	{

		GLMSG::SNET_VEHICLE_GET_FROMDB_ERROR InternalMsg;		

        InternalMsg.emFB          = EMVEHICLE_GET_FROMDB_ERROR_FB_DBFAIL;
		InternalMsg.bLMTItemCheck = m_bLMTItemCheck;
		InternalMsg.bCardInfo	  = m_bCardInfo;
		InternalMsg.bTrade		  = m_bTrade;

		CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
		pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

		SAFE_DELETE ( m_pVehicle );
	}

	

	return nResult;
}

/**
* Ż���� ���͸��� �����Ѵ�. ���͸��� 0-1000 ����
* \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
* \param nVehicleBttery ���͸�
*/
CSetVehicleBattery::CSetVehicleBattery(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,
	int nVehicleBttery
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
	, m_nVehicleBttery( nVehicleBttery )
{
	m_ClientSlot = dwClientID;
}


int CSetVehicleBattery::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CSetVehicleBattery::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetVehicleBattery( m_nVehicleNum, m_dwCharNum, m_nVehicleBttery );
}


CGetVehicleBattery::CGetVehicleBattery( 
	DWORD dwClientID, 
	DWORD dwCharNum,
	int	  nVehicleNum, 
	SNATIVEID	sItemID,
	WORD  wCureVolume, 
	BOOL  bRatio, 
	SNATIVEID sBatteryID,
	VEHICLE_TYPE emType,
    WORD HoldPosX,
    WORD HoldPosY )
	: m_dwCharNum ( dwCharNum )
	, m_VehicleDbNum( nVehicleNum )	
	, m_sItemID ( sItemID )
	, m_wCureVolume( wCureVolume )
	, m_bRatio ( bRatio )
	, m_sBatteryID ( sBatteryID ) 
	, m_emType( emType )
    , m_HoldPosX( HoldPosX )
    , m_HoldPosY( HoldPosY )
{
	m_ClientSlot = dwClientID;
}


int CGetVehicleBattery::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CGetVehicleBattery::Execute. Use ADO");
        return NET_ERROR;
    }

	int nResult = m_pDbManager->GetVehicleBattery( m_VehicleDbNum, m_dwCharNum );

	GLMSG::SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB InternalMsg;

	if ( nResult != sc::db::DB_ERROR )
	{
		InternalMsg.m_VehicleDbNum	 = m_VehicleDbNum;
		InternalMsg.sItemID		 = m_sItemID;	
		InternalMsg.nFull		 = nResult;
		InternalMsg.wCureVolume  = m_wCureVolume;
		InternalMsg.bRatio		 = m_bRatio;
		InternalMsg.sBatteryID   = m_sBatteryID;
		InternalMsg.emFB		 = EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK;
		InternalMsg.emType		 = m_emType;
        InternalMsg.wHoldPosX    = m_HoldPosX;
        InternalMsg.wHoldPosY    = m_HoldPosY;
	}
	else
	{
		InternalMsg.emFB		 = EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL;
	}

	CFieldServer* pTemp = reinterpret_cast<CFieldServer*> (pServer);
	pTemp->InsertMsg ( m_ClientSlot, (char*) &InternalMsg );

	
	return 0;
}




/**
* Ż���� �����Ѵ�.
* \param nVehicle ������ Ż���� ��ȣ
*/
CDeleteVehicle::CDeleteVehicle(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
{
	m_ClientSlot = dwClientID;
}

int CDeleteVehicle::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CDeleteVehicle::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->DeleteVehicle( m_nVehicleNum, m_dwCharNum );
}

CRebuyVehicle::CRebuyVehicle(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
{
	m_ClientSlot = dwClientID;
}

int CRebuyVehicle::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CRebuyVehicle::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->RebuyVehicle( m_nVehicleNum, m_dwCharNum );
}

/**
* Ż���� �κ��丮 ������Ʈ
*/
CSetVehicleInven::CSetVehicleInven(		
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,	
	const GLVEHICLE* pVehicle)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum(nVehicleNum)
{	
	m_ClientSlot = dwClientID;
	//pVehicle->GETPUTONITEMS_BYBUF( m_ByteStream );
	pVehicle->GETPUTONITEMS_BYVECTOR( m_vecItem );
}

int CSetVehicleInven::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CSetVehicleInven::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SaveVehicleInven(
		m_nVehicleNum,
		m_dwCharNum,
		m_vecItem);
}

/**
* Ż���� �÷� ������Ʈ
*/
CSetVehicleColor::CSetVehicleColor(		
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,	
	const GLVEHICLE* pVehicle)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum(nVehicleNum)
{	
	m_ClientSlot = dwClientID;
	pVehicle->GETITEMSCOLOR_BYBUF( m_ByteStream );
}

int CSetVehicleColor::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CSetVehicleColor::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetVehicleColor(
		m_nVehicleNum,
		m_dwCharNum,
		m_ByteStream);
}

/**
* Ż���� �ν��� ��ų ������Ʈ
* \param m_nVehicleBooster ��� �ν��� ����
*/
CSetVehicleBooster::CSetVehicleBooster(
	DWORD dwClientID,
	DWORD dwCharNum,
	int nVehicleNum,
	DWORD dwVehicleBooster
	)
	: m_dwCharNum ( dwCharNum )
	, m_nVehicleNum( nVehicleNum )
	, m_dwVehicleBooster( dwVehicleBooster )
{
	m_ClientSlot = dwClientID;
}

int CSetVehicleBooster::Execute(NetServer* pServer)
{
    if (m_pDbManager->GetConnectionType() != db::ADO)
    {
        sc::writeLogError("CSetVehicleBooster::Execute. Use ADO");
        return NET_ERROR;
    }

	return m_pDbManager->SetVehicleBooster( m_nVehicleNum, m_dwCharNum, m_dwVehicleBooster );
}