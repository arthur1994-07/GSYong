
#pragma once

class BaseMacro;

class ReturnByOptionMacro : public BaseMacro 
{
private:
	// ���� ����, ���ѽð� �������� ��ȯ �ɼ��� üũ�Ǿ��� ���;
	bool m_bCheckDrug;
	bool m_bCheckLimitTime;

	enum EMRETURNOP_TYPE
	{
		TYPE_NONE = -1,
		TYPE_DRUG,
		TYPE_TIME,
	};
	EMRETURNOP_TYPE m_emReturnType;

public:
	ReturnByOptionMacro(GLGaeaClient* pGaeaClient);
	virtual ~ReturnByOptionMacro(void);

private:
	virtual void DoInit(void);
	virtual void DoUpdate(float fTime, float fElapsedTime);
	virtual void DoApply(void);
	virtual void DoEndProcedure(void);
	virtual bool CheckTransferable(void);
	virtual bool CheckEnd(void);

public:
	void CheckDrug(bool bCheck)	{ m_bCheckDrug = bCheck;}
	void CheckTime(bool bCheck)	{ m_bCheckLimitTime = bCheck; }
	bool IsCheckDrug(void)	{ return m_bCheckDrug; }
	bool IsCheckTime(void)	{ return m_bCheckLimitTime; }
};