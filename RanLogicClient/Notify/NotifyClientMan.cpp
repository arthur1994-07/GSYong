#include "pch.h"

#include "../../SigmaCore/DebugInclude.h"

#include "./NotifyClientMan.h"
#include "./NotifyClientBase.h"
#include "./NotifyClientFriendAdd.h"

#include "../Widget/GLWidgetScript.h"
#include "../../RanLogic/GLUseFeatures.h"

#include <boost/foreach.hpp>

NotifyClientMan::NotifyClientMan(GLGaeaClient* pGaeaClient)
    : m_pGaeaClient(pGaeaClient)
{
}

NotifyClientMan::~NotifyClientMan()
{
}

void NotifyClientMan::Clear()
{
    m_Data.clear();
}

bool NotifyClientMan::Add( std::tr1::shared_ptr<INotifyClientBase> spData )
{
    if (!spData)
        return false;

    NOTIFY_DATA_ITER iter = m_Data.find(spData->GetGuid());
    if (iter != m_Data.end())
        return false;

    
    BOOST_FOREACH( const NOTIFY_DATA_VALUE& val, m_Data )
    {
        const std::tr1::shared_ptr<INotifyClientBase> spNotify = val.second;
            
        if( spNotify->GetType() == spData->GetType() )
        {
            if( strcmp( spNotify->Description().c_str(), spData->Description().c_str() ) == 0 )
            {
                if( spData->GetDate() - spNotify->GetDate() < DATA_OVERLAP_TIME )
                {
                    return false;
                }
            }
        }

    }

	switch ( spData->GetType() )
	{
	case gamelogic::notify::FRIEND_ADD:
		{
			const sc::MGUID& mGuid = spData->GetGuid();
			LuaTable tbMGUID( GLWidgetScript::GetInstance().GetLuaState() );
			tbMGUID.set( 1, mGuid.Data1);
			tbMGUID.set( 2, mGuid.Data2);
			tbMGUID.set( 3, mGuid.Data3);
			tbMGUID.set( 4, mGuid.Data4[0]);
			tbMGUID.set( 5, mGuid.Data4[1]);
			tbMGUID.set( 6, mGuid.Data4[2]);
			tbMGUID.set( 7, mGuid.Data4[3]);
			tbMGUID.set( 8, mGuid.Data4[4]);
			tbMGUID.set( 9, mGuid.Data4[5]);
			tbMGUID.set(10, mGuid.Data4[6]);
			tbMGUID.set(11, mGuid.Data4[7]);

			// 친구 수락 메시지;
			NotifyClientFriendAdd* pNotifyFriendAdd = static_cast<NotifyClientFriendAdd*>(spData.get());
			/*
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Community_ReqFriend,
				"-s, -b",
				pNotifyFriendAdd->Description().c_str(),
				GLUseFeatures::GetInstance().IsUsingNotifyWindow() );
				*/
			GLWidgetCallFuncValue args[ 3 ];
			args[0].SetString(pNotifyFriendAdd->Description().c_str());
			args[1].SetBool(GLUseFeatures::GetInstance().IsUsingNotifyWindow());
			args[2].SetTable(&tbMGUID);
			GLWidgetScript::GetInstance().LuaCallFunc< void >(
				NSWIDGET_SCRIPT::g_strFunc_Community_ReqFriend, args, 3);
		}
		break;
	case gamelogic::notify::GROUPCHAR_INVITE:
		break;
	case gamelogic::notify::CLUB_INVITE:
		break;
	case gamelogic::notify::PARTY_INVITE:
		break;
	}
    
    m_Data.insert(NOTIFY_DATA_VALUE(spData->GetGuid(), spData));
    m_LatelyGuid = spData->GetGuid();

    //! 알람버튼에게 알림을 왔음을 알린다.
    //m_pInterface->NotifyWindowSetNewAlarm();

    return true;
}

bool NotifyClientMan::Del( sc::MGUID Guid )
{
    NOTIFY_DATA_ITER iter = m_Data.find(Guid);
    if (iter != m_Data.end())
    {
        m_Data.erase(iter);
        return true;
    }
    else
    {
        return false;
    }
}

const std::tr1::shared_ptr<INotifyClientBase> NotifyClientMan::GetData( sc::MGUID Guid ) const
{
    NOTIFY_DATA_CITER iter = m_Data.find( Guid );

    if (iter != m_Data.end())
        return iter->second;

    return std::tr1::shared_ptr<INotifyClientBase>((INotifyClientBase*)NULL);
}

const std::tr1::shared_ptr<INotifyClientBase> NotifyClientMan::GetLatelyData() const
{
    NOTIFY_DATA_CITER iter = m_Data.find( m_LatelyGuid );

    if( iter != m_Data.end() )
        return iter->second;

    return std::tr1::shared_ptr<INotifyClientBase>((INotifyClientBase*)NULL);
}

