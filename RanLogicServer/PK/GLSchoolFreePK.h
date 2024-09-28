#ifndef _GL_SCHOOL_FREE_PK_
#define _GL_SCHOOL_FREE_PK_

#pragma once

class GLAgentServer;
class GLGaeaServer;

class GLSchoolFreePK
{
public:
    GLSchoolFreePK();
    virtual ~GLSchoolFreePK();

protected:
	bool  m_bON;	

public:
	bool IsON() const;
	void SET(bool bON) { m_bON = bON; }	
};

class GLSchoolFreePKAgent : public GLSchoolFreePK
{
public:
    GLSchoolFreePKAgent(GLAgentServer* pServer);
    virtual ~GLSchoolFreePKAgent();

private:
    GLAgentServer* m_pServer;
    DWORD m_dwLAST_DAY;
    float m_fTIMER;

public:
    void FrameMove(float fElaps);
    bool CheckBegin();
    bool DoProgress(float fElaps);
    bool DoBegin();
    bool DoEnd();
};

class GLSchoolFreePKField : public GLSchoolFreePK
{
public:
    GLSchoolFreePKField();
    virtual ~GLSchoolFreePKField();
};

#endif // _GL_SCHOOL_FREE_PK_