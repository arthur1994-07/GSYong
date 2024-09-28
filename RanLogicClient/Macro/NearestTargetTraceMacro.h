

#pragma once

class BaseMacro;

class NearestTargetTraceMacro : public BaseMacro
{
private:
	D3DXVECTOR3 m_vTargetPosition;
	enum {
		TRACE_NONE = -1,
		TRACE_READY,		// 서버에 요청보내놓은 상태;
		TRACE_ACTION,		// 이동중;
		TRACE_END,			// 추적끝;
	};
	DWORD m_dwTraceState;
	DWORD m_dwTraceStartState;

public:
	NearestTargetTraceMacro(GLGaeaClient* pGaeaClient);
	virtual ~NearestTargetTraceMacro(void);

public:
	// 클라자체에서 스킬을 사용하기위해 타겟으로 접근하다가;
	// 오브젝트에 걸려 이동하지 못하는 상태이다;
	// 타겟 위치를 목적지로 셋하기위함;
	void SetTraceState_ClientOnCall(void) {m_dwTraceStartState=TRACE_READY;}

private:
	virtual void DoInit(void);
	virtual void DoUpdate(float fTime, float fElapsedTime);
	virtual void DoApply(void);
	virtual void DoEndProcedure(void);
	virtual bool CheckTransferable(void);
	virtual bool CheckEnd(void);
};