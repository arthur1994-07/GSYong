
#pragma once

#include "./MacroDefine.h"
#include "./BaseMacro.h"

class GLGaeaClient;

class GLMacroManager
{
private:
	GLGaeaClient*	m_pGaeaClient;
	GLCharacter*	m_pCharacter;
	std::vector<BaseMacro*>	m_vecMacroModel;
	BaseMacro*				m_pActiveMacro;

	MACRO::MSG_QUEUE	m_MsgQueue;
	DWORD m_dwMacroState;

	std::deque<INT> m_deqMacroRemainTimeAlram;

private:
	GLMacroManager(void);
	virtual ~GLMacroManager(void);

public:
	static GLMacroManager& GetInstance(void);

public:
	void OneTimeSceneInit(void);
	void InitMacro(void);
	void Active(void);
	void FrameMove(float fTime, float fElapsedTime);
	void Update(float fTime, float fElapsedTime);
	void UpdateAlarm(void);
	void ResetMacro(void);
	
public:
	void MsgProcedure(void);
	bool AddMessage(DWORD type, DWORD dwMsg=0);
	bool PopMessage(DWORD* type, DWORD* dwMsg);

public:
	inline bool IsActive(void) const;
	inline bool IsLockByUserInput(void) const;
	bool IsMacroTransferable(DWORD dwMacroType) const;
	inline bool IsState( DWORD dwState ) const	{ return m_dwMacroState&dwState; }
	void SetState( DWORD dwState )		{ m_dwMacroState |= dwState; }
	void ReSetState( DWORD dwState )	{ m_dwMacroState &=~dwState; }
	DWORD GetActiveType(void);

	void SetGaeaClient(GLGaeaClient* const pGaeaClient)	{ m_pGaeaClient = pGaeaClient; }

public: // Scaleform
	void MacroOnOff(bool bOn);
	// ReturnByOption-Macro
	void SetCheckRemainDrugCount(bool bCheck);
	void SetCheckRemainMacroTime(bool bCheck);
	bool IsCheckRemainDrugCount(void);
	bool IsCheckRemainMacroTime(void);
	// AutomaticSkill-Macro
	void SetSkillTabEnable(WORD wSlotNumber, bool bEnable);
	bool IsSkillTabEnable(WORD wSlotNumber);

	LuaTable GetAllOptionState(void);
};