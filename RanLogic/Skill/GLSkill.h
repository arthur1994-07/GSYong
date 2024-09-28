

/* 
 * [9/28/2016 gbgim];
 * changeSet #39568 이후 변경사항부터 다음과 바뀜;
 * SKILL::SBASIC, SKILL::SAPPLY, SKILL::SLEARN, SKILL::SEXT_DATA, SKILL::SSPECIAL_SKILL 
 * 위 주요 스킬의 데이터들이 참조하는 자료형들이 너무많이 GLSkill.h, cpp에 응집되어있어서;
 * 각각 데이터별 분리시킴;
 * 각 해당 개체의 모든 버전을 포함한 자료형이 구성된 h, cpp(ex- GLSkill_Apply.h, cpp)와;
 * 해당 자료형이 필요로하는 부가 자료형들 h, cpp(ex- GLSkill_ApplyData.h, cpp)로 구성;
 * 한 1년정도지나면 이주석 지우면될거같음;
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

		// Note : Desc Format용 Enum입니다. 
		// 값이 존재하지 않는다면 아무것도 적용되지 않고 다음 값을 검색합니다.
		DESC_FORMAT_NONE   = 0x0000, // Desc 그냥 읽음
		DESC_FORMAT_BASIC  = 0x0001, // Desc Format 기본 적용 값 읽음
		DESC_FORMAT_BLOW   = 0x0002, // Desc Format blow   값 읽음
		DESC_FORMAT_IMPACT = 0x0004, // Desc Format Impact 값 읽음
		DESC_FORMAT_SPEC   = 0x0008, // Desc Format Spec   값 읽음
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

	bool	IsSkillFact ();		//지속성 스킬
	bool	CheckSpecialaAddon( EMSPEC_ADDON eAddon )	const;
	bool	IsRebirth() const;	//부활스킬인지 확인
	bool	IsBuff()	const;	//이로운 주문인가? (Heal도 포함)
	bool	IsHarm()	const;	//해로운 주문인가?
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

	//! 병진 : 레벨별 스킬 설명에 대한 정보를 포멧스트링을 사용해서 읽어오는 함수입니다.
	void GetDescFormatByLevel(CString& strDest, WORD wLevel, DWORD dwFormatFlag);

    //! "편" (그 주위의 자기편, 그 주위의 상대편)
    EMIMPACT_SIDE BasicImpacktSide() const { return m_sBASIC.ImpactSide(); }

    const SKILL::SLEARN& GetLearnData() const { return  m_sLEARN; }

    //! 적용값 타입
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
