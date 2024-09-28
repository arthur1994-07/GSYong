#pragma once

class GLAgentServer;
struct NET_MSG_GENERIC;

class GLMapLayerMan
{
public:
	const bool messageProcedure(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID);

private:
	const bool _messageProcedureList(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID);
	const bool _messageProcedureNew(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID);
	const bool _messageProcedureDel(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID, const DWORD _dwClientID);

public:
	GLMapLayerMan(GLAgentServer* pServer);
	~GLMapLayerMan(void);

private:
	GLAgentServer* m_pAgentServer;
};