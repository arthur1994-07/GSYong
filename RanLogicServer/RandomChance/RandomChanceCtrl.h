#ifndef _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_
#define _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_

#pragma once

#include <set>

struct SNATIVEID;

//! Ȯ��/������ ����Ʈ ���� ��� : sckim, Redmine #4544
namespace randomchance
{

//! Ȯ��/������ ����Ʈ ���� ��� : RandomChanceCtrlSys.lua
class RandomChanceCtrl
{
    typedef std::set<DWORD>                        RANDOMCHANCE_ITEM_DATA;
    typedef RANDOMCHANCE_ITEM_DATA::iterator       RANDOMCHANCE_ITEM_DATA_ITER;
    typedef RANDOMCHANCE_ITEM_DATA::const_iterator RANDOMCHANCE_ITEM_DATA_CITER;

public:
    RandomChanceCtrl();
    ~RandomChanceCtrl();

protected:
    bool m_bActive;

    //! �ý��� �޼��� ��¿� Ȯ��(�ش� Ȯ�� ������ ���� �ý��� �޽��� �ⷰ)
    float m_fGlobalRC;

    //! �ý��� �޽��� ��¿� ItemList
    RANDOMCHANCE_ITEM_DATA m_RCData;

public:
    //! lua file loading
    bool LoadFile(const std::string& strName);

    //! �ý��� �޽��� ��¿� ������ ����
    void Reset();

    void AddItem(WORD ItemMid, WORD ItemSid);
    void AddItem(DWORD ItemID);
    void AddItem(const SNATIVEID& ItemID);
    
    float GetGlobalRC() const;

    bool IsGlobalRC(float fRate) const;
    bool IsRCItem(WORD ItemMid, WORD ItemSid) const;
    bool IsRCItem(DWORD ItemID) const;
    bool IsRCItem(const SNATIVEID& ItemID) const;
};

} // namespace Randomchance

#endif // _LOGIC_SERVER_RANDOMCHANCE_CTRL_H_