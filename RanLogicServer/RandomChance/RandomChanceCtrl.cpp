#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./RandomChanceCtrl.h"

#include "../../SigmaCore/DebugInclude.h"

//! 확률/아이템 리스트 제어 기능 : sckim, Redmine #4544
namespace randomchance
{
    RandomChanceCtrl::RandomChanceCtrl()
        : m_bActive(false)
        , m_fGlobalRC(0.0f)
    {   
    }

    RandomChanceCtrl::~RandomChanceCtrl()
    {
    }

    bool RandomChanceCtrl::LoadFile(const std::string& strName)
    {
        try
        {
		    sc::lua_init();

		    if (!sc::lua_doFile(strName))
            {
                std::string strErrMsg(sc::string::format("%1% file missing.(error 9421)", strName));
                sc::writeLogError( strErrMsg );

                return false;
            }

            // 전체 확률
    		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName("GrobalRC");
		    if(ob.IsNil())
		    {
                sc::writeLogError(_T("LUA Object missing(GrobalRC)..."));

                m_fGlobalRC = 0.0f;
		    }
            m_fGlobalRC = ob.GetFloat();	

#ifdef _RELEASED
	        sc::writeLogInfo(sc::string::format("RandomChanceCtrl GlobalRC Value = %1%", m_fGlobalRC));
#endif

            // 아이템 리스트
		    WORD wMID = 0;
            WORD wSID = 0;

	        LuaPlus::LuaObject obItemList = sc::lua_getGlobalFromName("RC_list");
		    for(LuaPlus::LuaTableIterator itrItem(obItemList); itrItem.IsValid(); itrItem.Next())
		    {
			    for ( LuaPlus::LuaTableIterator itrID( itrItem.GetValue() ); itrID.IsValid(); itrID.Next() )
			    {
			        wMID = static_cast<WORD>(itrID.GetValue().GetInteger());

			        itrID.Next();
			        wSID = static_cast<WORD>(itrID.GetValue().GetInteger());

			        AddItem(wMID, wSID);

#ifdef _RELEASED
	                sc::writeLogInfo(sc::string::format("RandomChanceCtrl RC List MID = %1%, SID = %2%", wMID, wSID));
#endif

                }
		    }

            m_bActive = true;
        }
        catch ( const LuaPlus::LuaException& e )
        {
            std::string strErrMsg(sc::string::format("%1%, %2%", strName, e.GetErrorMessage()));
            sc::writeLogError( strErrMsg );

            return false;
        }

        return true;
    }

    void RandomChanceCtrl::Reset()
    {
        m_bActive = false;

        m_fGlobalRC = 0.0f;

        m_RCData.clear();
    }

    void RandomChanceCtrl::AddItem(WORD ItemMid, WORD ItemSid)
    {
        SNATIVEID ItemID(ItemMid, ItemSid);
        AddItem(ItemID);    
    }

    void RandomChanceCtrl::AddItem(const SNATIVEID& ItemID)
    {
        AddItem(ItemID.Id());
    }

    void RandomChanceCtrl::AddItem(DWORD ItemID)
    {
        m_RCData.insert(ItemID);
    }

    float RandomChanceCtrl::GetGlobalRC() const
    {
        return m_fGlobalRC;
    }

    bool RandomChanceCtrl::IsGlobalRC(float fRate) const
    {
        if (fRate <= m_fGlobalRC)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RandomChanceCtrl::IsRCItem(WORD ItemMid, WORD ItemSid) const
    {
        SNATIVEID ItemID(ItemMid, ItemSid);
        return IsRCItem(ItemID);
    }

    bool RandomChanceCtrl::IsRCItem(const SNATIVEID& ItemID) const
    {
        return IsRCItem(ItemID.Id());
    }

    bool RandomChanceCtrl::IsRCItem(DWORD ItemID) const
    {
        if (!m_bActive)
        {
            return false;
        }

        RANDOMCHANCE_ITEM_DATA_CITER citer = m_RCData.find(ItemID);
        if (citer != m_RCData.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
} // namespace randomchance

