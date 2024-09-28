#ifndef _GL_FREE_PK_H_
#define _GL_FREE_PK_H_

#pragma once

class GLAgentServer;

class GLFreePK
{
public:
    GLFreePK(GLAgentServer* pServer);
    ~GLFreePK();

protected:
    GLAgentServer* m_pServer;
	float	m_fTIMER;
	float	m_fPKTIME;
	float	m_fUPDATETIMER;

public:
	bool IsFREEPKMODE() const { return m_fPKTIME > 0.0f; }
	void RESET() { m_fTIMER = 0.0f; m_fPKTIME = 0.0f; }
	void StartFREEPKMODE(DWORD dwTIME);
	void EndFREEPKMODE();
	bool FrameMove(float fElapsedAppTime);
};

#endif // _GL_FREE_PK_H_