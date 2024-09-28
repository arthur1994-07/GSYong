#ifndef _DB_ACTION_GAME_VEHICLE_H_
#define _DB_ACTION_GAME_VEHICLE_H_

#pragma once

#include "../../../RanLogic/Vehicle/GLVEHICLE.h"
#include "DbAction.h"

/**
* ���ο� Ż���� �����Ѵ�.
* \param nChaNum  Ż�� ������ ĳ���� ��ȣ
* \param szVehicleName Ż�� �̸�
* \param nVehicleType Ż���� Ÿ��
* \param nVehicleCardMID Ż���� ������ MID
* \param nVehicleCardSID Ż���� ������ SID
* \return sc::db::DB_ERROR �Ǵ� ������������ Ż���� ������ȣ
*/
class CCreateVehicle : public db::DbAction
{
public:
	CCreateVehicle( 
		DWORD dwClientID,
		DWORD ChaDbNum,
		const TCHAR* szVehicleName,
		int nVehicleType,
		int nVehicleCardMID,
		int nVehicleCardSID
		);
	virtual ~CCreateVehicle() {};
	virtual int Execute(NetServer* pServer) override;

protected:

	DWORD m_ChaDbNum;
	CString m_strVehicleName;
	int m_nVehicleType;
	int m_nVehicleCardMID;
	int m_nVehicleCardSID;
};

/**
* Ż���� ������ �����´�.
*/
class CGetVehicle : public db::DbAction
{
public:
	CGetVehicle(
		GLVEHICLE* pVehicle,
		DWORD dwVehicleNum,
		DWORD dwClientID,
		DWORD dwCharNum,
		bool bLMTItemCheck=false, 
		bool bCardInfo=false, 
		bool bTrade=false
		);

	virtual ~CGetVehicle() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	GLVEHICLE* m_pVehicle;
	DWORD m_dwVehicleNum;
	DWORD	m_dwCharNum;
	bool  m_bLMTItemCheck;
	bool  m_bCardInfo;
	bool  m_bTrade;
};


/**
* Ż���� ���͸��� �����Ѵ�. ���͸��� 0-1000 ����
* \param nVehicleBttery ���͸��� ������ Ż���� ��ȣ
* \param nVehicleBttery ���͸�
*/
class CSetVehicleBattery : public db::DbAction
{
public:
	CSetVehicleBattery(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,
		int nVehicleBttery
		);
	virtual ~CSetVehicleBattery() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwCharNum;
	int m_nVehicleNum;
	int m_nVehicleBttery;
};


/**
* Ż���� �������� �����´�
* \param nVehicleNum Ż�� ��ȣ
* \return Ż�� ������ or sc::db::DB_ERROR
*/

class CGetVehicleBattery : public db::DbAction
{
public:
	CGetVehicleBattery( 
		DWORD dwClientID, 
		DWORD dwCharNum,
		int	  nVehicleNum, 
		SNATIVEID	sItemID,
		WORD  wCureVolume, 
		BOOL  bRatio, 
		SNATIVEID sBatteryID,
		VEHICLE_TYPE emType,
        WORD HoldPosX,
        WORD HoldPosY );

	virtual ~CGetVehicleBattery() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD		m_dwCharNum;
	int			m_VehicleDbNum;
	SNATIVEID	m_sItemID;
	int			m_nVehicleBttery;
	WORD		m_wCureVolume;
	BOOL		m_bRatio;
	SNATIVEID	m_sBatteryID;
	VEHICLE_TYPE m_emType;
    WORD        m_HoldPosX;
    WORD        m_HoldPosY;
};


/**
* Ż���� �����Ѵ�.
* \param nVehicle ������ Ż���� ��ȣ
*/
class CDeleteVehicle : public db::DbAction
{
public:
	CDeleteVehicle( 
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum
		);

	virtual ~CDeleteVehicle() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwCharNum;
	int		m_nVehicleNum;	
};


class CRebuyVehicle : public db::DbAction
{
public:
	CRebuyVehicle( 
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum
		);

	virtual ~CRebuyVehicle() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwCharNum;
	int		m_nVehicleNum;	
};


/**
* Ż�� �κ��丮 ������Ʈ
*  
*/
class CSetVehicleInven : public db::DbAction
{
public:
	CSetVehicleInven(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,		
		const GLVEHICLE* pVehicle
		);
	virtual ~CSetVehicleInven() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwCharNum;
	int m_nVehicleNum;
	//se::ByteStream m_ByteStream;
	std::vector< SINVENITEM_SAVE > m_vecItem;
};


/**
* Ż�� �÷� ������Ʈ
*  
*/
class CSetVehicleColor : public db::DbAction
{
public:
	CSetVehicleColor(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,		
		const GLVEHICLE* pVehicle);
	virtual ~CSetVehicleColor() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD	m_dwCharNum;
	int m_nVehicleNum;
	se::ByteStream m_ByteStream;
};


/**
* Ż���� �ν��� ��ų ������Ʈ
* \param m_nVehicleBooster ��� �ν��� ����
*/
class CSetVehicleBooster : public db::DbAction
{
public:
	CSetVehicleBooster(
		DWORD dwClientID,
		DWORD dwCharNum,
		int nVehicleNum,
		DWORD dwVehicleBooster
		);
	virtual ~CSetVehicleBooster() {};
	virtual int Execute(NetServer* pServer) override;

protected:
	DWORD m_dwCharNum;
	int m_nVehicleNum;
	DWORD m_dwVehicleBooster;
};


#endif // _DB_ACTION_GAME_VEHICLE_H_