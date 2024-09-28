#include "pch.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../enginelib/GUInterface/UITextControl.h"
#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../enginelib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../GLGaeaClient.h"
#include "../Char/GLCharacter.h"
#include "../Char/GLCharClient.h"
#include "InstanceSystemClient.h"

namespace InstanceSystem
{
    ManagerClient::ManagerClient ( GLGaeaClient* const _pGaeaClient )
        : pGaeaClient ( _pGaeaClient )
		, nCloseButtonParameter ( 0 )
		, m_nRetryInstanceID ( 0 )
    {
		
    }

    ManagerClient::~ManagerClient ( void ) { }

    


	void CALLBACK ManagerClient::OnFrameMove ( float fElapsedTime, const CTime& sCurrentTime )
	{
	}


	



    void ManagerClient::_printGuideLine(void)
    {
        const unsigned int _colorGuideLine(D3DCOLOR_XRGB(239, 132, 84));
        this->pGaeaClient->PrintMsgText(_colorGuideLine, std::string("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"));
    }

    

    const bool ManagerClient::InstanceXMLStringToString(const InstanceXmlString& _xmlString, std::string& _msgString)
    {
        switch ( _xmlString.emXmlType )
        {
        case InstanceXmlString::EMXMLGAME_IN_TEXT:
            {
                std::string stringMessage(ID2GAMEINTEXT(_xmlString.strXmlKey.c_str(), _xmlString.index));
                switch ( _xmlString.XmlParamVector.size() )
                {
                case 0:
                    {
                        _msgString = stringMessage;
                    }
                    break;
                case 1:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0] );
                    }
                    break;
                case 2:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1] );
                    }
                    break;
                case 3:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2]);
                    }
                    break;
                case 4:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3]);
                    }
                    break;
                case 5:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3],
                            _xmlString.XmlParamVector[4]);
                    }
                    break;
                case 6:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3],
                            _xmlString.XmlParamVector[4],
                            _xmlString.XmlParamVector[5]);
                    }
                    break;
                case 7:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3],
                            _xmlString.XmlParamVector[4],
                            _xmlString.XmlParamVector[5],
                            _xmlString.XmlParamVector[6]);
                    }
                    break;
                case 8:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3],
                            _xmlString.XmlParamVector[4],
                            _xmlString.XmlParamVector[5],
                            _xmlString.XmlParamVector[6],
                            _xmlString.XmlParamVector[7]);
                    }
                    break;
                case 9:
                    {
                        _msgString = sc::string::format(stringMessage,
                            _xmlString.XmlParamVector[0],
                            _xmlString.XmlParamVector[1],
                            _xmlString.XmlParamVector[2],
                            _xmlString.XmlParamVector[3],
                            _xmlString.XmlParamVector[4],
                            _xmlString.XmlParamVector[5],
                            _xmlString.XmlParamVector[6],
                            _xmlString.XmlParamVector[7],
                            _xmlString.XmlParamVector[8]);
                    }
                    break;
                
                default:
                    return false;
                }
            }
            break;

        default:
            return false;
        }
        return true;
    }

	void ManagerClient::requestClose(void)
	{
		GLWidgetScript& _widgetScript(GLWidgetScript::GetInstance());
		_widgetScript.LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_Competition_CDM_Close);
		if ( this->nCloseButtonParameter != 0 )
		{
			GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE netMsg(this->nCloseButtonParameter);
			netMsg.emDestType = InstanceSystem::EM_MESSAGE_MY_INSTANCE;
			netMsg.emSrcType = InstanceSystem::EM_MESSAGE_USER;
			netMsg.dwSrcID = pGaeaClient->GetCharacter()->CharDbNum();
			pGaeaClient->NETSENDTOFIELD(&netMsg);
		}
	}

}
