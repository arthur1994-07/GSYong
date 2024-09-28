#ifndef _CROW_ACTION_H_
#define _CROW_ACTION_H_

#pragma once

#include <string>
#include "../../EngineLib/DxMeshs/SkinMesh/SAnimationDefine.h"
#include "../Character/GLCharDefine.h"
#include "./GLCrowDefine.h"

namespace sc
{
    class BaseStream;
    class SerialFile;
}

struct SCROWACTION_100
{
    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
    char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

    BOOL				m_bTrade;						//	�ŷ���������.
    char				m_szSaleFile[ACF_SZNAME];		//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
    char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    DWORD				m_dwActionUP;					//	�ൿ ���� HP 50%�̻�.
    DWORD				m_dwActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_101
{
    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
    char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    char				m_szSaleFile[ACF_SZNAME];		//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    char				m_szBirthEffect[ACF_SZNAME];	//	ź���� ������ ȿ��.
    char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
    char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_102
{
    enum { SALENUM = 3 };
    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    char				m_szSkinObj[ACF_SZNAME];		//	��Ų ����.
    char				m_szTalkFile[ACF_SZNAME];		//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    char				m_szSaleFile[SALENUM][ACF_SZNAME];		//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    char				m_szBirthEffect[ACF_SZNAME];	//	ź���� ������ ȿ��.
    char				m_szFallingEffect[ACF_SZNAME];	//	�״� ������ ������ ȿ��.
    char				m_szBlowEffect[ACF_SZNAME];		//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_103
{
    enum { VERSION = 0x0103, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.
};

struct SCROWACTION_104
{
    enum { VERSION = 0x0104, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
};

struct SCROWACTION_105
{
    enum { VERSION = 0x0105, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
    float				m_fActionDNRate;				// �ൿ ���� ���� %
};


struct SCROWACTION_106
{
    enum { VERSION = 0x0106, SALENUM = 3 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
    float				m_fActionDNRate;				// �ൿ ���� ���� %

    BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
    SNATIVEID			m_sMobLinkID;					// ����� ������ ID
};

struct SCROWACTION_107
{
    enum { VERSION = 0x0107, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
    float				m_fActionDNRate;				// �ൿ ���� ���� %

    BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
    SNATIVEID			m_sMobLinkID;					// ����� ������ ID

    SCROWPATTERN		m_Pattern;						
    std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION_108
{
    enum { VERSION = 0x0108, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    DWORD				m_dwActFlag;					//	���� �ൿ.
    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;					//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    EMCROWACT_UP		m_emActionUP;					//	�ൿ ���� HP 50%�̻�.
    EMCROWACT_DN		m_emActionDN;					//	�ൿ ���� HP 50%����.

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)
    float				m_fActionDNRate;				// �ൿ ���� ���� %

    BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
    SNATIVEID			m_sMobLinkID;					// ����� ������ ID
    float				m_fMobLinkDelay;				// ����� ���� ���� ������
    float				m_fMobLinkScale;				// ����� ���� ��ü ũ��

    SCROWPATTERN		m_Pattern;						
    std::vector< SCROWPATTERN >	m_vecPatternList;
};

struct SCROWACTION
{
    // Version ���泻�� : SCROWPATTERN::RANDOMPATTERNNUM -> 10���� ����
    enum { VERSION = 0x0110, SALENUM = 3, PATTERNNUM = 10, ATTACKSETNUM = 4 };	

    DWORD				m_dwActFlag;					//	���� �ൿ. EMCROW_NPCACT

    EMMOVETYPE			m_emMoveType;					//	�̵� Ÿ��.
    float				m_fDriftHeight;					//	�ξ� ����.

    float				m_fWalkVelo;					//	�̵� �ӵ�.
    BOOL				m_bRun;							//	�ٱ� ����.
    float				m_fRunVelo;						//	�ٱ� �ӵ�.

    WORD				m_wBodyRadius;					//	��ü���� �ݰ�.
    std::string			m_strSkinObj;					//	��Ų ����.
    std::string			m_strTalkFile;					//	��ȭ ����.

    BOOL				m_bAfterFall_NoBody;			//	�״� ������ ����ü �����. ( ȿ���� ��ä�ÿ� ���� )
    std::string			m_strBirthEffect;				//	ź���� ������ ȿ��.
    std::string			m_strFallingEffect;				//	�״� ������ ������ ȿ��.
    std::string			m_strBlowEffect;				//	Ÿ�ݽ� ����Ʈ, CROW �������� ������ �� �ְ�.

    std::string			m_strSaleFile[SALENUM];			//	Item �Ǹ� ���.

    float				m_fLiveTime;					//  Ȱ�������� �ð�(EMCROWACT_AUTODROP �÷����� ���)

    BOOL				m_bMobLink;						// �ش���Ͱ� ������ ����Ǿ��ִ� ���͸� ȣ������ ����.
    SNATIVEID			m_sMobLinkID;					// ����� ������ ID
    float				m_fMobLinkDelay;				// ����� ���� ���� ������
    float				m_fMobLinkScale;				// ����� ���� ��ü ũ��

    SCROWPATTERN		m_Pattern;												
    std::vector< SCROWPATTERN >	m_vecPatternList;


    SCROWACTION();
    SCROWACTION(const SCROWACTION& value);

    SCROWACTION& operator= ( const SCROWACTION_100 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_101 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_102 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_103 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_104 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_105 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_106 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_107 &OldData );
    SCROWACTION& operator= ( const SCROWACTION_108 &OldData );	//	������ ���� ���� ����

    SCROWACTION& operator= ( const SCROWACTION &value );

    bool LOAD103 ( sc::BaseStream &SFile );
    bool LOAD104 ( sc::BaseStream &SFile );
    bool LOAD105 ( sc::BaseStream &SFile );
    bool LOAD106 ( sc::BaseStream &SFile );
    bool LOAD107 ( sc::BaseStream &SFile );
    bool LOAD108 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
    bool LOAD109 ( sc::BaseStream &SFile );	//	SCROWPATTERN::RANDOMPATTERNNUM -> 10���� ����

    bool LOAD ( sc::BaseStream &SFile );
    bool SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead(std::fstream& SFile);
    VOID SaveCsv(std::fstream& SFile);
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);

    //! ���� �ൿ
    inline DWORD GetActFlag() const { return m_dwActFlag; }
};

#endif // _CROW_ACTION_H_