#pragma once

#include "afxcmn.h"
#include "../RanLogic/Skill/GLSkill.h"

class SkillIconImage;

struct GLCHARLOGIC;

// Skill Viewer
class SkillViewDlg : public CDialog
{
	DECLARE_DYNAMIC( SkillViewDlg );

public:
	enum SKILL_INFOR_TYPE
	{
		INFOR_NONE_TYPE,

		NORMAL_DURATION_PLUS,
		NORMAL_DURATION_MINUS,

		NORMAL_NONDURATION_PLUS,
		NORMAL_NONDURATION_MINUS,

		PASSIVE_DURATION_PLUS,
		PASSIVE_DURATION_MINUS,

		PASSIVE_NONDURATION_PLUS,
		PASSIVE_NONDURATION_MINUS,
	};

private:
	enum MAIN_SKILL_COLUMN
	{
		MAIN_SKILL_NAME,
		MAIN_SKILL_GNA,
		MAIN_SKILL_TYPE,
		MAIN_SKILL_WEAPON,
		MAIN_SKILL_CONSUME,
		MAIN_SKILL_DELAY,
		MAIN_SKILL_COLUMN_COUNT
	};

	enum MULTI_SKILL_COLUMN
	{
		MULTI_SKILL_NAME,
		MULTI_SKILL_GNA,
		MULTI_SKILL_TYPE,
		MULTI_SKILL_COLUMN_COUNT
	};

public:
	SkillViewDlg( CWnd* pParent = NULL );
	virtual ~SkillViewDlg();

	// Dialog Resource ID
	enum { IDD = IDD_DIALOG_SKILLVIEW };

public:
	// Setting Skill
	void SetSkill( GLSKILL* pSkill, UINT skillLevel = 0 );

	// Update List
	void UpdateList( GLSKILL* pSkill );

private:
	// Initialize
	void InitializeSkillView();
	void InitSkillImage();
	void InitListMainSkill();
	void InitListMultiTargetSkill();

	// Update List
	void UpdateMainSkillList( GLSKILL* pSkill );
	void UpdateMultiTargetSkillList( GLSKILL* pSkill );

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual void PostNcDestroy();

	// Message
	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnTimer( UINT_PTR nIDEvent );
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeComboLevel();
	DECLARE_MESSAGE_MAP()

private:
	CString			m_strAppPath;
	char			m_szAppPath[ MAX_PATH ];

	// Information
	UINT			m_skillLevel;			// Skill Level
	GLCHARLOGIC*	m_pDefaultCharacter;	// Default Character
	GLSKILL			m_skill;				// Skill Data

	// List Control
	CListCtrl		m_ListMainSkill;		// Main Skill
	CListCtrl		m_ListMultiTargetSkill;	// Multi Target Skill

	// Combo Box
	CComboBox		m_ComboBoxLevel;		// Level

	// Static
	CStatic			m_StaticMainExplain;	// Main Skill Explain
	CStatic			m_StaticMultiExplain;	// Multi Skill Explain

	// Edit
	CEdit			m_EditMainScope;		// Main Skill Scope
	CEdit			m_EditMainEffect;		// Main Skill Effect
	CEdit			m_EditMainCondition;	// Main Skill Unusual Condition
	CEdit			m_EditMultiScope;		// Multi Skill Scope
	CEdit			m_EditMultiEffect;		// Multi Skill Effect
	CEdit			m_EditMultiCondition;	// Multi Skill Unusual Condition

	// Skill Image
	SkillIconImage*	m_MainSkillImage;		// Main Skill Image
	SkillIconImage*	m_MultiSkillImage;		// Multi Skill Image
};