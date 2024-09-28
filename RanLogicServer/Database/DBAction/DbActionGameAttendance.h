#ifndef _DB_ACTION_GAME_ATTENDANCE_H_
#define _DB_ACTION_GAME_ATTENDANCE_H_

#pragma once
//
//mjeon.attendance
//

#include "DbAction.h"
#include "../../Attendance/AttendanceManager.h"



class CAttendanceLoad : public db::DbAction
{
public:	
	CAttendanceLoad(UINT nChaNum, OUT std::tr1::shared_ptr<AttendanceManager> &pAttendanceMan);
	virtual ~CAttendanceLoad();
	virtual int Execute(NetServer* m_pServer) override;

protected:

	std::tr1::shared_ptr<AttendanceManager>	m_pAttendanceMan;
	UINT				m_nChaNum;
};


class CAttendanceSave : public db::DbAction
{
public:
	CAttendanceSave(DWORD dwClient, UINT nChaNum, LONGLONG nChaCurExp, IN std::tr1::shared_ptr<AttendanceManager> &pAttendanceMan, BOOL bLastOne4Maintenance = FALSE);
	virtual ~CAttendanceSave();
	virtual int Execute(NetServer* m_pServer) override;

protected:
	std::tr1::shared_ptr<AttendanceManager>	m_pAttendanceMan;
	
	UINT				m_nChaNum;
	LONGLONG			m_nChaCurExp;

	//
	//m_bLastOne4Maintenance
	//TRUE: This DbAction is the last one that put into DbAction queue for AttendanceMaintenance.
	//		And the last one will send a message to AgentS to notify all characters finished Attendance::Save().
	//
	BOOL				m_bLastOne4Maintenance;
};


class CAttendanceMaintenance : public db::DbAction
{
public:
	CAttendanceMaintenance();
	virtual ~CAttendanceMaintenance() {};
	virtual int Execute(NetServer* m_pServer) override;

protected:	
};

class AttendanceLogInsert : public db::DbAction
{
public:
    AttendanceLogInsert(int ChaNum, int Type, const std::string& LoadType, int DayN, int Status);
    virtual ~AttendanceLogInsert();
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_ChaNum;
    int m_Type;
    std::string m_LoadType;
    int m_DayN;
    int m_Status;
};

class AttendanceTaskLogInsert : public db::DbAction
{
public:
    AttendanceTaskLogInsert(
        int ChaNum, LONGLONG ChaCurExp, int TaskN, int Type, int Status, int Value,
        int ValueBase, int RewardPoint, int RewardExp, int RewardMoney, int RewardItems, 
        int RewardItem1, int RewardItem2, int RewardItem3, int TookRewards);
    virtual ~AttendanceTaskLogInsert();
    virtual int Execute(NetServer* pServer) override;

protected:
    int m_ChaNum;
	LONGLONG m_ChaCurExp;
    int m_TaskN;
    int m_Type;
    int m_Status;
    int m_Value;
    int m_ValueBase;
    int m_RewardPoint;
    int m_RewardExp;
    int m_RewardMoney;
    int m_RewardItems;
    int m_RewardItem1;
    int m_RewardItem2;
    int m_RewardItem3;
    int m_TookRewards;
};

#endif // _DB_ACTION_GAME_ATTENDANCE_H_