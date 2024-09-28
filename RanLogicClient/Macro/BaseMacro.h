
#pragma once

#include "./MacroDefine.h"

class GLGaeaClient;

class BaseMacro
{
protected:
	GLGaeaClient* const m_pGaeaClient;
	DWORD m_MacroType;

	bool m_bUsable;

	// begin���� �켱�����̴�;
	// ������ AI�� ���ϸ� ���̰����� Ÿ�� ������ ���� ��ɵ� �����ϸ� ���� �� ����;
	MACRO::VEC_TYPE m_vecTransferableType;

public:
	BaseMacro(GLGaeaClient* pGaeaClient);
	virtual ~BaseMacro(void);

public:
	void Init(bool bUsable);
	void FrameMove(float fTime, float fElapsedTime);
	void Apply(void);
	void EndProcedure(void);
	bool IsTransferable(void);
	bool IsEnd(void);

private:
	virtual void DoInit(void) = 0;
	virtual void DoUpdate(float fTime, float fElapsedTime) = 0;
	virtual void DoApply(void) = 0;
	virtual void DoEndProcedure(void) = 0;
	virtual bool CheckTransferable(void) = 0;
	virtual bool CheckEnd(void) = 0;

public:
	const DWORD GetMacroType(void)	{ return m_MacroType; }
	const bool IsUsable(void)		{ return m_bUsable; }
	const MACRO::VEC_TYPE& GetTransferableType(void)	{ return m_vecTransferableType; }
};