#include "pch.h"
#include "../../EngineLib/G-Logic/GLOGIC.h"
#include "../Character/GLCharAG.h"
#include "../AgentServer/GLAgentServer.h"
#include "./GLMapLayerMan.h"

GLMapLayerMan::GLMapLayerMan(GLAgentServer* pServer)
: m_pAgentServer(pServer)
{
}
GLMapLayerMan::~GLMapLayerMan(void)
{
}

const bool GLMapLayerMan::messageProcedure(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
{
	GLCharAG* const _pGM(m_pAgentServer->GetChar(_dwGaeaID));
	if ( _pGM == NULL )
		return false;
	if ( _pGM->m_dwUserLvl < USER_GM3 )
		return false;

	const unsigned int _nClientSlot(_pGM->ClientSlot());

	const GLMSG::SNET_GM_MAP_LAYER* const _pMapLayerMessage((const GLMSG::SNET_GM_MAP_LAYER* const)_pMessage);
	switch ( _pMapLayerMessage->nSubType )
	{
	case GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_LIST:
		return _messageProcedureList(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_NEW:
		return _messageProcedureNew(_pMessage, _dwGaeaID, _nClientSlot);
	case GLMSG::SNET_GM_MAP_LAYER::EMMAP_LAYER_DEL:
		return _messageProcedureDel(_pMessage, _dwGaeaID, _nClientSlot);
	}
    return false;
}

const bool GLMapLayerMan::_messageProcedureList(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID)
{
	const GLMSG::SNET_GM_MAP_LAYER* const _pListLayerMessage((const GLMSG::SNET_GM_MAP_LAYER* const)_pMessage);

	GLCharAG* const _pGM(m_pAgentServer->GetChar(_dwGaeaID));
	const MapID& _currentMapID(_pGM->GetCurrentMap());

	SNATIVEID _targetMapID;
	if ( _pListLayerMessage->nParameter0 == unsigned int(-1) )
		_targetMapID = _currentMapID.getBaseMapID();
	else
		_targetMapID = SNATIVEID(WORD(_pListLayerMessage->nParameter0), WORD(_pListLayerMessage->nParameter1));

	const SMAPNODE* const _pMapNode(m_pAgentServer->FindMapNode(_targetMapID));
	if ( _pMapNode == NULL )
	{
		m_pAgentServer->SENDTOCLIENT(_dwClientID, &GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY(_targetMapID, GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_WRONG_MAP_ID));
		return false;
	}

	const DWORD _dwFieldServerNum(_pGM->GetCurrentFieldServer());
	if ( _dwFieldServerNum != _pMapNode->GetFieldServerNum() )
	{
		m_pAgentServer->SENDTOCLIENT(_dwClientID, &GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY(_targetMapID, GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_DIFFERENT_FIELD_SERVER));
		return false;
	}
	
	GLMSG::SNET_GM_MOB_LEV_LIST NetMsg;
	NetMsg.dwGaeaID = _dwGaeaID;
	NetMsg.wMAP_MID = _targetMapID.Mid();
	NetMsg.wMAP_SID = _targetMapID.Sid();

	const DWORD _dwChannel(_pGM->GetChannel());
	m_pAgentServer->SENDTOFIELDSVR(_dwChannel, _dwFieldServerNum, &NetMsg);

	return true;
}
const bool GLMapLayerMan::_messageProcedureNew(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID)
{
	const GLMSG::SNET_GM_MAP_LAYER* const _pNewLayerMessage((const GLMSG::SNET_GM_MAP_LAYER* const)_pMessage);

	GLCharAG* const _pGM(m_pAgentServer->GetChar(_dwGaeaID));
	const DWORD _nLayer(_pNewLayerMessage->nParameter2);

	SNATIVEID _targetMapID;
	if ( _pNewLayerMessage->nParameter0 == unsigned int(-1) )
		_targetMapID = _pGM->GetCurrentMap().getBaseMapID();
	else
		_targetMapID = SNATIVEID(WORD(_pNewLayerMessage->nParameter0), WORD(_pNewLayerMessage->nParameter1));

	const SMAPNODE* const _pMapNode(m_pAgentServer->FindMapNode(_targetMapID));
	if ( _pMapNode == NULL )
	{
		m_pAgentServer->SENDTOCLIENT(_dwClientID, &GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY(_targetMapID, GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_WRONG_MAP_ID));
		return false;
	}

	GLMSG::SNET_GM_MOB_LEV_LAYER NetMsg;
	NetMsg.wMAP_MID = _targetMapID.Mid();
	NetMsg.wMAP_SID = _targetMapID.Sid();
	NetMsg.unLayer = (WORD)_nLayer;

	m_pAgentServer->SENDTOALLCHANNEL(&NetMsg);

	GLMSG::SNET_GM_MAP_LAYER_NEW_NOTIFY _newLayerMessage(_targetMapID, _nLayer);
	m_pAgentServer->SENDTOCLIENT(_dwClientID, &_newLayerMessage);

	return true;
}
const bool GLMapLayerMan::_messageProcedureDel(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID)
{
	const GLMSG::SNET_GM_MAP_LAYER* const _pDelLayerMessage((const GLMSG::SNET_GM_MAP_LAYER* const)_pMessage);

	GLCharAG* const _pGM(m_pAgentServer->GetChar(_dwGaeaID));
	const DWORD _nLayer(_pDelLayerMessage->nParameter2);

	SNATIVEID _targetMapID;
	if ( _pDelLayerMessage->nParameter0 == unsigned int(-1) )
		_targetMapID = _pGM->GetCurrentMap().getBaseMapID();
	else
		_targetMapID = SNATIVEID(WORD(_pDelLayerMessage->nParameter0), WORD(_pDelLayerMessage->nParameter1));

	const SMAPNODE* const _pMapNode(m_pAgentServer->FindMapNode(_targetMapID));
	if ( _pMapNode == NULL )
	{
		m_pAgentServer->SENDTOCLIENT(_dwClientID, &GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY(_targetMapID, GLMSG::SNET_GM_MAP_LAYER_ERROR_NOTIFY::EMERROR_WRONG_MAP_ID));
		return false;
	}

	GLMSG::SNET_GM_MOB_LEV_CLEAR NetMsg;
	NetMsg.wMAP_MID = _targetMapID.Mid();
	NetMsg.wMAP_SID = _targetMapID.Sid();

	m_pAgentServer->SENDTOALLCHANNEL(&NetMsg);

	GLMSG::SNET_GM_MAP_LAYER_DEL_NOTIFY _delLayerMessage(_targetMapID);
	m_pAgentServer->SENDTOCLIENT(_dwClientID, &_delLayerMessage);

	return true;
}
