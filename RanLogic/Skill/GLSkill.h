

/* 
 * [9/28/2016 gbgim];
 * changeSet #39568 ���� ������׺��� ������ �ٲ�;
 * SKILL::SBASIC, SKILL::SAPPLY, SKILL::SLEARN, SKILL::SEXT_DATA, SKILL::SSPECIAL_SKILL 
 * �� �ֿ� ��ų�� �����͵��� �����ϴ� �ڷ������� �ʹ����� GLSkill.h, cpp�� �����Ǿ��־;
 * ���� �����ͺ� �и���Ŵ;
 * �� �ش� ��ü�� ��� ������ ������ �ڷ����� ������ h, cpp(ex- GLSkill_Apply.h, cpp)��;
 * �ش� �ڷ����� �ʿ���ϴ� �ΰ� �ڷ����� h, cpp(ex- GLSkill_ApplyData.h, cpp)�� ����;
 * �� 1������������ ���ּ� �����ɰŰ���;
 */

#ifndef GLSKILL_H_
#define GLSKILL_H_

#pragma once

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAnimationManager.h"

#include "../Item/GLItemDef.h"
#include "../Character/GLCharDefine.h"

#include "./GLSkillDef.h"
#include "./GLSkill_SubData.h"
#include "./GLSkill_Basic.h"
#include "./GLSkill_Apply.h"
#include "./GLSkill_Learn.h"
#include "./GLSkill_ExtData.h"
#include "./GLSkill_SpecialSkill.h"

#define CHECK_FLAG_SKILL(dwBitFlag, iRealValue) (dwBitFlag & (1 << iRealValue) ? true : false)

namespace sc
{
    class Excel;
}

class SUBPATH;

struct GLSKILL
{
	enum
	{
		VERSION			= 0x0100,

		FILE_SBASIC			= 1,
		FILE_SAPPLY			= 2,
		FILE_SLEARN			= 3,
		FILE_SEXT_DATA		= 4,
		FILE_SPECIAL_SKILL  = 5,

		FILE_END_DATA	= 0xEDEDEDED,

		// Note : Desc Format�� Enum�Դϴ�. 
		// ���� �������� �ʴ´ٸ� �ƹ��͵� ������� �ʰ� ���� ���� �˻��մϴ�.
		DESC_FORMAT_NONE   = 0x0000, // Desc �׳� ����
		DESC_FORMAT_BASIC  = 0x0001, // Desc Format �⺻ ���� �� ����
		DESC_FORMAT_BLOW   = 0x0002, // Desc Format blow   �� ����
		DESC_FORMAT_IMPACT = 0x0004, // Desc Format Impact �� ����
		DESC_FORMAT_SPEC   = 0x0008, // Desc Format Spec   �� ����
	};

	SKILL::SBASIC		  m_sBASIC;
	SKILL::SAPPLY		  m_sAPPLY;
	SKILL::SLEARN		  m_sLEARN;
	SKILL::SEXT_DATA	  m_sEXT_DATA;
	SKILL::SSPECIAL_SKILL m_sSPECIAL_SKILL;

	GLSKILL(void);

	BOOL SaveFile ( sc::SerialFile &SFile );
	BOOL LoadFile ( sc::BaseStream &SFile, bool bPastLoad );

	static VOID SaveCsvHead ( std::fstream &SFile );
	VOID SaveCsv ( std::fstream &SFile );
	VOID LoadCsv ( CStringArray &StrArray );

private:
	VOID SaveCsvAnimIfo ( std::fstream &SFile );

public:
	GLSKILL& operator= (const GLSKILL& value);

	bool	IsSkillFact ();		//���Ӽ� ��ų
	bool	CheckSpecialaAddon( EMSPEC_ADDON eAddon )	const;
	bool	IsRebirth() const;	//��Ȱ��ų���� Ȯ��
	bool	IsBuff()	const;	//�̷ο� �ֹ��ΰ�? (Heal�� ����)
	bool	IsHarm()	const;	//�طο� �ֹ��ΰ�?
	bool	IsLimit() const;
	bool	m_bMultiSkill;
	void	SetMultiSkillFlag( bool bBool ) { m_bMultiSkill = bBool; };
	const char* GetName();
    const std::string GetNameStr() const;
	const char* GetDesc();

    inline SNATIVEID	GetId() const { return m_sBASIC.GetId(); };

	inline BOOL			CanShowSelfZone3Eff( SKILL::EMEFFECTIME eIncTime, int nStep )
	{
		return ( m_sEXT_DATA.emSELFZONE03 == eIncTime ||
				 ( m_sEXT_DATA.emSELFZONE03 == SKILL::EMTIME_EFFECT &&
				   //m_sAPPLY.nEffectFrame != 0 && 
				   m_sAPPLY.nEffectFrame == nStep ) );
	} //CanShowSelfZone3Eff

	inline BOOL			CanShowTargetZone3Eff( SKILL::EMEFFECTIME eIncTime, int nStep )
	{
		return ( m_sEXT_DATA.emTARGZONE03 == eIncTime ||
				 ( m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT &&
				   //m_sAPPLY.nEffectFrame != 0 &&
				   m_sAPPLY.nEffectFrame == nStep ) );
	} //CanShowTargetZone3Eff

	GLSKILL*		GetSecondSkill();

	inline bool			HavePassiveFunc()
	{
		if( m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE )
			return true;

		GLSKILL*	pSecondSkill = GetSecondSkill();
		if( pSecondSkill &&
			pSecondSkill->m_sBASIC.emROLE == SKILL::EMROLE_PASSIVE ) 
		{
			return true;			
		} //if

		return false;
	} //HavePassiveFunc

	const bool HaveSpec(const EMSPEC_ADDON emSPEC);
	const bool HaveSpec(const EMSPEC_ADDON emSPEC, SKILL::SSPEC_ADDON& resSpecAddon);

	//! ���� : ������ ��ų ���� ���� ������ ���佺Ʈ���� ����ؼ� �о���� �Լ��Դϴ�.
	void GetDescFormatByLevel(CString& strDest, WORD wLevel, DWORD dwFormatFlag);

    //! "��" (�� ������ �ڱ���, �� ������ �����)
    EMIMPACT_SIDE BasicImpacktSide() const { return m_sBASIC.ImpactSide(); }

    const SKILL::SLEARN& GetLearnData() const { return  m_sLEARN; }

    //! ���밪 Ÿ��
    inline SKILL::EMTYPES ApplyType() const { return m_sAPPLY.BasicType(); }

};
typedef GLSKILL* PGLSKILL;

class GLSkillMan
{
public:
	enum
	{
		ENCODE_VER = 0x0101,

		VERSION = 0x0101,
		MAX_CLASSSKILL = 128,
	};

	static const char* _FILEHEAD;
	static const char* _LOGFILE;
	static const char* _STRINGTABLE;
public:
    GLSkillMan();
    ~GLSkillMan();
    static GLSkillMan& GetInstance();

protected:
	char				m_szFileName[MAX_PATH];	
	bool				m_bModify;
	PGLSKILL			m_pSkills[EMSKILLCLASS_NSIZE][MAX_CLASSSKILL];
	LPDIRECT3DDEVICEQ	m_pd3dDevice;

public:
	GLSKILL* GetData(const WORD wClass, const WORD Index);
	GLSKILL* GetData(const SNATIVEID& sID) { return GetData(sID.wMainID,sID.wSubID); }
    void	SetData ( WORD wClass, WORD Index, const PGLSKILL SkillData, bool binner=false );

    const std::string GetSkillName(const SNATIVEID& SkillId) const;
    const std::string GetSkillName(WORD MainId, WORD SubId) const;
    const std::string GetSkillName(DWORD SkillId) const;

public:
	WORD	FindFreeSkillIndex ( WORD wClass );
	BOOL	DeleteSkill ( WORD wClass, WORD Index );
	void	GetMaxSkill ( WORD& wClass, WORD& rIndex );	

	bool	ValidData ();
	bool	IsModify ()							{ return m_bModify; }

	HRESULT OneTimeSceneInit ();
	HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT DeleteDeviceObjects ();
	HRESULT FinalCleanup ();

    HRESULT	SyncStringTable();
    HRESULT SyncUpdateData (const SUBPATH* pPath);

    HRESULT LoadFile(const std::string& FileName, bool bPastLoad);
	HRESULT LoadCsvFile( CWnd* pWnd );	

    HRESULT SaveFile(const char* szFile);
	HRESULT SaveCsvFile( CWnd* pWnd );
    void SaveSkillDataSimple(sc::Excel& Xls, int Sheet, const std::string& SheetName);

	std::string SaveSqlFile();

	HRESULT LoadStrTblExcelFile(const char* szPath);
	HRESULT SaveStrTblExcelFile(std::string& strSavePath);
};

#endif // GLSKILL_H_
