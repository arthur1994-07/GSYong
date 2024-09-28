

#pragma once

class BaseMacro;

class NearestTargetTraceMacro : public BaseMacro
{
private:
	D3DXVECTOR3 m_vTargetPosition;
	enum {
		TRACE_NONE = -1,
		TRACE_READY,		// ������ ��û�������� ����;
		TRACE_ACTION,		// �̵���;
		TRACE_END,			// ������;
	};
	DWORD m_dwTraceState;
	DWORD m_dwTraceStartState;

public:
	NearestTargetTraceMacro(GLGaeaClient* pGaeaClient);
	virtual ~NearestTargetTraceMacro(void);

public:
	// Ŭ����ü���� ��ų�� ����ϱ����� Ÿ������ �����ϴٰ�;
	// ������Ʈ�� �ɷ� �̵����� ���ϴ� �����̴�;
	// Ÿ�� ��ġ�� �������� ���ϱ�����;
	void SetTraceState_ClientOnCall(void) {m_dwTraceStartState=TRACE_READY;}

private:
	virtual void DoInit(void);
	virtual void DoUpdate(float fTime, float fElapsedTime);
	virtual void DoApply(void);
	virtual void DoEndProcedure(void);
	virtual bool CheckTransferable(void);
	virtual bool CheckEnd(void);
};