#ifndef _DB_ACTION_GAME_VEHICLE_H_
#define _DB_ACTION_GAME_VEHICLE_H_

#pragma once

#include "../../../RanLogic/Vehicle/GLVEHICLE.h"
#include "DbAction.h"

/**
* 새로운 탈것을 생성한다.
* \param nChaNum  탈것 소유자 캐릭터 번호
* \param szVehicleName 탈것 이름
* \param nVehicleType 탈것의 타입
* \param nVehicleCardMID 탈것의 아이템 MID
* \param nVehicleCardSID 탈것의 아이템 SID
* \return sc::db::DB_ERROR 또는 성공했을때는 탈것의 고유번호
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
* 탈것의 정보를 가져온다.
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
* 탈것의 배터리를 설정한다. 배터리는 0-1000 사이
* \param nVehicleBttery 배터리를 설정할 탈것의 번호
* \param nVehicleBttery 배터리
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
* 탈것의 포만감을 가져온다
* \param nVehicleNum 탈것 번호
* \return 탈것 포만감 or sc::db::DB_ERROR
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
* 탈것을 삭제한다.
* \param nVehicle 삭제할 탈것의 번호
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
* 탈것 인벤토리 업데이트
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
* 탈것 컬러 업데이트
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
* 탈것의 부스터 스킬 업데이트
* \param m_nVehicleBooster 배운 부스터 종류
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