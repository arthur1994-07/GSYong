#pragma once

#include <boost/logic/tribool.hpp>
#include "../../RanLogic/Club/GLClub.h"

class GLGaeaServer;

class GLClubField : public GLClub
{
public:
    GLClubField(GLGaeaServer* pServer);
    virtual ~GLClubField();
    const GLClubField& operator = (const GLClubField& value);
    const GLClubField& operator = (const GLClub& value);

private:
    GLGaeaServer* m_pServer;
    LONGLONG m_ProfitMoeny; //! ������ ���ͱ�
    //float m_fTIMER_POS; //! Ŭ���� ��ġ ���� Ÿ�̸�
    float m_fTIMER_MONEY; //! Ŭ�� ���� ���� Ÿ�̸�

public:
    //void SAVESTORAGE2DB();
    virtual void RESET() override;

    //! ������ ���ͱ�
    inline LONGLONG GetProfitMoney() const { return m_ProfitMoeny; }
    inline void ResetProfitMoeny() { m_ProfitMoeny = 0; }
    inline void AddProfitMoeny(LONGLONG Money) 
    {
        if (Money > 0)
            m_ProfitMoeny += Money;
    }

    inline void AddProfitTimer(float AddTime)
    {
        if (AddTime > 0.0f)
            m_fTIMER_MONEY += AddTime;
    }

//  inline void AddPosTimer(float AddTime)
//  {
//      if (AddTime > 0.0f)
//          m_fTIMER_POS += AddTime;
//  }

//  inline void SetPosTimer(float Time)
//  {
//      m_fTIMER_POS = Time;
//  }

//    inline float GetPosTimer() const { return m_fTIMER_POS; }

    inline float GetProfitTimer() const { return m_fTIMER_MONEY; }
    inline void ResetProfitTimer() { m_fTIMER_MONEY = 0; }

    void SendClubClient(NET_MSG_GENERIC* pMsg);
};
