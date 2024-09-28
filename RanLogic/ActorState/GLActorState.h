#pragma once

#include "../Character/GLCharDefine.h"
// �̵� ���;
// m_bActionType : GLogicEx.h;
// m_Action : GLogicEx.h;
// m_Action : Child class(material, crow, char, summon); �ߺ� ����;


class ActorState{
public:	
	virtual void SetActState(const DWORD dwState);
	virtual void ReSetActState(const DWORD dwState);
	virtual void SetAction(const EMACTIONTYPE emAction);

	virtual const bool IsActState(const DWORD dwState) const;
	virtual const bool isAction(const EMACTIONTYPE emAction) const;
	virtual const bool isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1) const;
	virtual const bool isAction(const EMACTIONTYPE emAction0, const EMACTIONTYPE emAction1, const EMACTIONTYPE emAction2) const;
	virtual const bool IsValidBody(void) const;
	virtual const bool IsDie(void) const; // ���־� ��;
	virtual const bool IsIdle(void) const;

	virtual const DWORD GetActState(void) const;
	virtual const EMACTIONTYPE& GetAction(void) const;

	void RESET(void);

public:
	ActorState(void);
	virtual ~ActorState(void);

private:
	EMACTIONTYPE m_emAction; // ���� �ൿ.
	DWORD m_dwActState; //	�ൿ ����.		
};

class ActorStateCharacterClient : public ActorState
{
public:
	virtual const bool IsValidBody(void) const override;
	virtual const bool IsDie(void) const override;
};

class ActorStateCharacterServer : public ActorState
{
public:
	virtual const bool IsValidBody(void) const override;
	virtual const bool IsDie(void) const override;
};