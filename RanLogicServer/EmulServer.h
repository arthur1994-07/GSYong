#ifndef _EMUL_SERVER_H_
#define _EMUL_SERVER_H_

#include "../RanLogic/EmulBase.h"

class GLGaeaServer;

class EmulServerField : public EmulBase
{
public:
	EmulServerField( GLGaeaServer* pServer );
	virtual ~EmulServerField() {}

protected:
	GLGaeaServer* m_pServer;

public:
	virtual void _update_emul_msg(
		DWORD dwClientID,
		DWORD dwGaeaID,
		float fElapsTime ) override;

}; // class EmulServerField

class GLAgentServer;

class EmulServerAgent : public EmulBase
{
public:
	EmulServerAgent( GLAgentServer* pServer );
	virtual ~EmulServerAgent() {}

protected:
	GLAgentServer* m_pServer;

public:
	virtual void _update_emul_msg(
		DWORD dwClientID,
		DWORD dwGaeaID,
		float fElapsTime ) override;

}; // class EmulServerAgent

#endif // _EMUL_SERVER_H_