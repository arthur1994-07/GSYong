#include "../pch.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "./NpcDialogueCase.h"
#include "./NpcTalkControl.h"
#include "./NpcTalkCondition.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CNpcDialogueCase::CNpcDialogueCase()
{
}

CNpcDialogueCase::~CNpcDialogueCase()
{
	Reset();
}

void CNpcDialogueCase::Reset()
{
	ResetCondition();
	ResetTalkControl();
	SetBasicTalk("");
}

CNpcDialogueCase& CNpcDialogueCase::operator= (const CNpcDialogueCase& rNpcDialogCase)
{
    Reset();

    if (rNpcDialogCase.GetCondition())
    {
        SetCondition(*rNpcDialogCase.GetCondition());
    }

    SetBasicTalk(rNpcDialogCase.GetBasicTalk());

    if (rNpcDialogCase.GetTalkControl())
    {
        SetTalkControl(*(rNpcDialogCase.GetTalkControl()));
    }
    return *this;
}

void CNpcDialogueCase::SetBasicTalk(const std::string& strTalk)
{
	m_strBasicTalk = strTalk;
}

const std::string CNpcDialogueCase::GetBasicTalk () const
{
// 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
// 	{
// 		CString strTalkKey;
// 		strTalkKey.Format( "%s_%d_%d"
// 			, m_spNpcDialogueSet->m_TalkName.c_str()
// 			, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
// 			, m_dwItemIndex );
// 	}
// 	else
		return m_strBasicTalk;
}

BOOL CNpcDialogueCase::SetTalkControl(const CNpcTalkControl& sTalkControl)
{
    std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
	m_spTalkControl = spControl;
	*m_spTalkControl.get() = sTalkControl;
	return TRUE;
}

BOOL CNpcDialogueCase::SetTalkControl(std::tr1::shared_ptr<CNpcTalkControl> spTalkControl)
{    
    m_spTalkControl = spTalkControl;
    return TRUE;
}

void CNpcDialogueCase::ResetTalkControl()
{
	m_spTalkControl.reset();
}

std::tr1::shared_ptr<CNpcTalkControl> CNpcDialogueCase::GetTalkControl() const
{
	return m_spTalkControl;
}

BOOL CNpcDialogueCase::SetCondition(const SNpcTalkCondition& sCondition)
{
	ResetCondition();
	//m_spCondition.reset();
    std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
    m_spCondition = spCondition;
	*m_spCondition.get() = sCondition;
	return TRUE;
}

BOOL CNpcDialogueCase::SetCondition(std::tr1::shared_ptr<SNpcTalkCondition> spCondition)
{
    ResetCondition();
    m_spCondition = spCondition;
    return TRUE;
}

void CNpcDialogueCase::ResetCondition()
{
	//SAFE_DELETE(m_spCondition);
    m_spCondition.reset();
}

std::tr1::shared_ptr<SNpcTalkCondition> CNpcDialogueCase::GetCondition() const
{	
	return m_spCondition;
}

DWORD CNpcDialogueCase::GetTalkNum() const
{
	std::tr1::shared_ptr<CNpcTalkControl> spTalkCtrl = GetTalkControl();
	if (spTalkCtrl)
        return spTalkCtrl->GetTalkNum();
    else
        return 0;
}

BOOL CNpcDialogueCase::SaveFile(sc::SerialFile& SFile, CString& strAnswerBuffer, const DWORD dwDlgID, const DWORD dwCnt )
{
	if (!SFile.IsOpen())
	{
		GASSERT(0 && "파일이 열려있지 않습니다.");
		return FALSE;
	}

	SFile << (WORD) VERSION;

	SFile << m_strBasicTalk;

	SFile << BOOL(m_spCondition.get() != NULL);
	if (m_spCondition)
        m_spCondition->SAVE(SFile);

	SFile << BOOL(m_spTalkControl.get() !=NULL);
	if (m_spTalkControl)
        m_spTalkControl->SaveFile(SFile, strAnswerBuffer, dwDlgID, dwCnt);

	return TRUE;
}
/*
void CNpcDialogueCase::SaveCSVFile( std::string &strLine )
{

}
*/

BOOL CNpcDialogueCase::LoadFile ( sc::BaseStream& SFile )
{
    if ( !SFile.IsOpen () )
    {
        GASSERT ( 0 && "파일이 열려있지 않습니다." );
        return FALSE;
    }

    WORD wVer(0);
    BOOL bExist(FALSE);

    SFile >> wVer;

    switch ( wVer )
    {
    case 0x0001:	LOAD_0001(SFile);	break;
    case 0x0002:	LOAD_0002(SFile);	break;
    case 0x0003:	LOAD_0003(SFile);	break;
    };

    return TRUE;
}

BOOL CNpcDialogueCase::LOAD_0003(sc::BaseStream& SFile)
{
    BOOL bExist(FALSE);

    SFile >> m_strBasicTalk;

    SFile >> bExist;
    if (bExist)
    {
        std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
        m_spCondition = spCondition;
        m_spCondition->LOAD(SFile);
    }

    SFile >> bExist;
    if (bExist)
    {
        std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
        m_spTalkControl = spControl;
        m_spTalkControl->LoadFile(SFile);
    }

    return TRUE;
}

BOOL CNpcDialogueCase::LOAD_0001 ( sc::BaseStream& SFile )
{
	BOOL bExist(FALSE);

	SFile >> m_strBasicTalk;

	SFile >> bExist;
	if (bExist)
	{
		std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
        m_spTalkControl = spControl;
		m_spTalkControl->LoadFile(SFile);
	}

	return TRUE;
}

BOOL CNpcDialogueCase::LOAD_0002 ( sc::BaseStream& SFile )
{
	BOOL bExist(FALSE);

	SFile >> bExist;
	if ( bExist )
	{
		std::tr1::shared_ptr<SNpcTalkCondition> spCondition(new SNpcTalkCondition);
        m_spCondition = spCondition;
		m_spCondition->LOAD(SFile);
	}

	SFile >> m_strBasicTalk;

	SFile >> bExist;
	if ( bExist )
	{
		std::tr1::shared_ptr<CNpcTalkControl> spControl(new CNpcTalkControl);
        m_spTalkControl = spControl;
		m_spTalkControl->LoadFile(SFile);
	}

	return TRUE;
}