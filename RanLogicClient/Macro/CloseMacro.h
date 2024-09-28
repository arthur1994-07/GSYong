
#pragma once

class BaseMacro;

class CloseMacro : public BaseMacro
{
private:
public:
	CloseMacro(GLGaeaClient* pGaeaClient);
	virtual ~CloseMacro(void);

private:
	virtual void DoInit(void);
	virtual void DoUpdate(float fTime, float fElapsedTime);
	virtual void DoApply(void);
	virtual void DoEndProcedure(void);
	virtual bool CheckTransferable(void);
	virtual bool CheckEnd(void);
};