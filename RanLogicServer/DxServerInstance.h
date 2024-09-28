#ifndef _DX_SERVER_INSTANCE_H_
#define _DX_SERVER_INSTANCE_H_

#pragma once

#include <boost/function.hpp>
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../RanLogic/Network/NetLogicDefine.h"

class DxMsgServer;
class DxConsoleMsg;
class GLAgentServer;
class GLGaeaServer;
class SUBPATH;
class AgentServerConfigXml;
class FieldServerConfigXml;

//
//mjeon.dbman
//
//class GLDBMan;
namespace db {
class IDbManager;
}

struct F_SERVER_INFO;

namespace DxServerInstance
{
    //! Field Server
	HRESULT FieldCreate(
        GLGaeaServer* pGaeaServer,
        const char* _szAppPath,
        DxMsgServer* pMsgServer, /*DxConsoleMsg* pConsoleMsg,*/
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        EMSERVICE_PROVIDER emServiceProvider, 
        SUBPATH* pPath,
        const char* szMapList/*=NULL*/,
        DWORD dwFieldSID/*=FIELDSERVER_MAX*/,
        DWORD dwMaxClient/*=1000*/,
        bool bPK_MODE/*=true*/, 
        int nChannel/*=0*/,
        BOOL bPKLess/*=FALSE*/,
        BYTE ExcessExpProcessType/*=0*/,
        language::LANGFLAG dwLangSet/*=language::DEFAULT*/,
        bool bPKServer/*=false*/,
        bool bToolMode,
		FieldServerConfigXml* pConfig);

	HRESULT FieldFrameMove(GLGaeaServer* pGaeaServer);
	void FieldCleanUp(GLGaeaServer* pGaeaServer);

	//! Agent Server
    HRESULT AgentCreate(
        GLAgentServer* pGLAgentServer,
        const char* szAppPath,
        DxMsgServer* pMsgServer, /*DxConsoleMsg* pConsoleMsg,*/
        db::IDbManager* pDBMan,
		db::IDbManager* pAdoMan,
        EMSERVICE_PROVIDER ServiceProvider,
        SUBPATH* pPath,
        char* szMapList/*=NULL*/,
        DWORD dwMaxClient/*=1000*/,
        F_SERVER_INFO* pFieldInfo/*=NULL*/,
        bool bPK_MODE/*=false*/,
        BOOL bPKLess/*=FALSE*/,
        language::LANGFLAG dwLangSet/*=language::DEFAULT*/,
        bool bToolMode,
		AgentServerConfigXml* pConfig);	
	HRESULT AgentFrameMove(GLAgentServer* pGLAgentServer);
	void AgentCleanUp(GLAgentServer* pGLAgentServer);

    HRESULT CreateBefore(
        const char* _szAppPath,
        SUBPATH* pPath,
        language::LANGFLAG dwLangSet,
        EMSERVICE_PROVIDER ServiceProvider,
        bool bPK_MODE,
        BOOL bPKLess,
        bool bToolMode);

    void CreateAfter();
}

class GMTOOL
{
public:
    typedef boost::function<void (void)> FUNCPROC;

public:
    GMTOOL();
    ~GMTOOL();

public:
    static HRESULT Create(
        const char* szAppPath,
        SUBPATH* pPath,
        EMSERVICE_PROVIDER ServiceProvider,
		const FUNCPROC* pLoadProcedure=NULL,
		boost::function<void (std::string)> PercentCallbackFunc=NULL,
		boost::function<void (unsigned int)> PercentCallbackFunc2=NULL);
	static void CleanUp();
};

#endif // _DX_SERVER_INSTANCE_H_
