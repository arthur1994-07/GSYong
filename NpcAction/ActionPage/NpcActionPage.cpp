// NpcActionPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"
#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/SingleobjectTextControl.h"

#include "../Edit/SheetWithTab.h"
#include "../NpcAction.h"
#include "./NpcActionPage.h"
//#include <strstream>

// CNpcActionPage 대화 상자입니다.

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CNpcActionPage, CPropertyPage)
CNpcActionPage::CNpcActionPage( LOGFONT logfont )
	: CPropertyPage(CNpcActionPage::IDD)
	, m_pFont ( NULL )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CNpcActionPage::~CNpcActionPage()
{
	SAFE_DELETE ( m_pFont );
}

void CNpcActionPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_POSTIVE_ANSWER2, m_listctrlCase);
	DDX_Control(pDX, IDC_LIST_CASE_ANSWER, m_listctrlAnswer);
}

BEGIN_MESSAGE_MAP(CNpcActionPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_CASE_EDIT, OnBnClickedButtonCaseEdit)
	ON_BN_CLICKED(IDC_BUTTON_CASE_DEL, OnBnClickedButtonCaseDel)
	ON_BN_CLICKED(IDC_BUTTON_CASE_NEW, OnBnClickedButtonCaseNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_POSTIVE_ANSWER2, OnLvnItemchangedListPostiveAnswer2)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_POSTIVE_ANSWER2, OnNMDblclkListPostiveAnswer2)
	ON_BN_CLICKED(IDC_BUTTON_CASE_COPY, OnBnClickedButtonCaseCopy)
END_MESSAGE_MAP()


// CNpcActionPage 메시지 처리기입니다.

void CNpcActionPage::OnBnClickedButtonOk()
{
	InverseUpdateItems ();

	if ( m_spDialogue )
	{
		// 기존 스트링 테이블 삭제;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CNpcDialogue::DLGCASE_ITER dlgCaseIter = m_spDialogue->m_vecCase.begin()
				, dlgCaseEnd = m_spDialogue->m_vecCase.end();
			for ( INT iCount = 0; dlgCaseIter!=dlgCaseEnd; ++dlgCaseIter, ++iCount )
			{
				std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseIter;
				CString strTalkKey;
				strTalkKey.Format("%s_%d_%d"
					, m_spNpcDialogueSet->m_TalkName.c_str()
					, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
					, iCount);
				CGameTextMan::GetInstance().DeleteString(strTalkKey.GetString(), CGameTextMan::EM_NPCTALK_TEXT);

				if ( spDlgCase->GetTalkControl() )
				{
					CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
					CNpcTalkControl::NPCMAP_IT itrBegin, itrEnd = talkMapConst.end();
					for ( itrBegin=talkMapConst.begin(); itrBegin!=itrEnd; ++itrBegin )
					{
						std::tr1::shared_ptr<SNpcTalk> spNpcTalk = itrBegin->second;

						CString strAnswerKey;
						strAnswerKey.Format("%s_%d_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), iCount, spNpcTalk->m_dwNID );
						CGameTextMan::GetInstance().DeleteString(strAnswerKey.GetString(), CGameTextMan::EM_NPCTALK_TEXT);		
					}
				}
			}
		}
// 		
// 		// 추가되는 데이터로 갱신;
// 		dlgCaseIter = m_DummyDialogue.m_vecCase.begin();
// 		dlgCaseEnd = m_DummyDialogue.m_vecCase.end();
// 		for ( INT iCount = 0; dlgCaseIter!=dlgCaseEnd; ++dlgCaseIter, ++iCount )
// 		{
// 			std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseIter;
// 			CString strTalkKey;
// 			strTalkKey.Format("%s_%d_%d"
// 				, m_spNpcDialogueSet->m_TalkName.c_str()
// 				, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
// 				, iCount);
// 
// 			char szTalk[1024];
// 			m_listctrlCase.GetItemText(iCount, 1, szTalk, 1024);
// 			CGameTextMan::GetInstance().InsertString(strTalkKey.GetString(), szTalk, CGameTextMan::EM_NPCTALK_TEXT);
// 
// 			if ( spDlgCase->GetTalkControl() )
// 			{
// 				CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
// 				CNpcTalkControl::NPCMAP_IT itrBegin, itrEnd = talkMapConst.end();
// 				for ( itrBegin=talkMapConst.begin(); itrBegin!=itrEnd; ++itrBegin )
// 				{
// 					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = itrBegin->second;
// 
// 					CString strAnswerKey;
// 					strAnswerKey.Format("%s_%d_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), iCount, spNpcTalk->m_dwNID );
// 				}
// 			}
// 		}

		*m_spDialogue.get() = m_DummyDialogue;
	}
	else
	{
		
		DWORD dwIndex = m_DummyDialogue.FindNonCondition();
		if ( dwIndex==UINT_MAX )	dwIndex = 0;	
		if ( !m_DummyDialogue.GetDlgCase(dwIndex) )
		{
			m_pSheetTab->ActiveActionTable ();
			return;
		}

		spNpcDialogSet spDialogueSet = m_pSheetTab->GetDialogueSet ();

        std::tr1::shared_ptr<CNpcDialogue> spNewDialogue(new CNpcDialogue);
		*spNewDialogue.get() = m_DummyDialogue;

		if ( !spDialogueSet->AddDialogue ( spNewDialogue ) )
		{
			MessageBox ( "Registered the same ID already." );
			//SAFE_DELETE ( spNewDialogue );
			return;
		}
	}

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		// 추가되는 데이터로 갱신;
		CNpcDialogue::DLGCASE_ITER dlgCaseIter = m_DummyDialogue.m_vecCase.begin()
			, dlgCaseEnd = m_DummyDialogue.m_vecCase.end();
		for ( INT iCount = 0; dlgCaseIter!=dlgCaseEnd; ++dlgCaseIter, ++iCount )
		{
			std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseIter;
			CString strTalkKey;
			strTalkKey.Format("%s_%d_%d"
				, m_spNpcDialogueSet->m_TalkName.c_str()
				, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID()
				, iCount);

			CString strTalkDesc = CSingleobjectTextControl::GetInstance().GetText(strTalkKey.GetString()).GetString();
			CGameTextMan::GetInstance().InsertString(strTalkKey.GetString(), strTalkDesc.GetString(), CGameTextMan::EM_NPCTALK_TEXT);

			if ( spDlgCase->GetTalkControl() )
			{
				CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
				CNpcTalkControl::NPCMAP_IT itrBegin, itrEnd = talkMapConst.end();
				for ( itrBegin=talkMapConst.begin(); itrBegin!=itrEnd; ++itrBegin )
				{
					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = itrBegin->second;

					CString strAnswerKey;
					strAnswerKey.Format("%s_%d_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), iCount, spNpcTalk->m_dwNID );
					CString strAnswerDesc = CSingleobjectTextControl::GetInstance().GetText(strAnswerKey.GetString()).GetString();
					CGameTextMan::GetInstance().InsertString(strAnswerKey.GetString(), strAnswerDesc.GetString(), CGameTextMan::EM_NPCTALK_TEXT);
				}
			}
		}
	}

	m_pSheetTab->ActiveActionTable ();
}

void CNpcActionPage::OnBnClickedButtonCancel()
{
	m_pSheetTab->ActiveActionTable ();
}

void CNpcActionPage::SetDialogue ( spNpcDialogSet spNpcDialogueSet, std::tr1::shared_ptr<CNpcDialogue> spDialogue )
{	
	m_DummyDialogue.Reset ();

	m_spNpcDialogueSet = spNpcDialogueSet;
	m_spDialogue = spDialogue;
    if ( m_spDialogue )
	{
		m_DummyDialogue = *m_spDialogue.get();		
	}
	else
	{
		DWORD NewNativeID = m_pSheetTab->GetDialogueSet()->MakeDlgNID();
		m_DummyDialogue.SetDlgNID ( NewNativeID );
	}

	// 수정 완료 이전까지 변경된 문자열를 기록하기 위한 문자열 동기화;
	NpcTalkStringtableSynchronization();
}

void CNpcActionPage::NpcTalkStringtableSynchronization()
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CSingleobjectTextControl::GetInstance().Clear();

		std::string strDlgSetName(m_spNpcDialogueSet->GetDlgSetName().c_str());
		CNpcDialogueSet::DIALOGMAP_ITER dlgSetItr = m_spNpcDialogueSet->GetDialogueMap()->begin()
			, dlgSetEnd = m_spNpcDialogueSet->GetDialogueMap()->end();

		for (; dlgSetItr != dlgSetEnd; ++dlgSetItr)
		{
			std::tr1::shared_ptr<CNpcDialogue> spDialogue = dlgSetItr->second;

			CNpcDialogue::DLGCASE_ITER dlgCaseItr = spDialogue->m_vecCase.begin()
				, dlgCaseEnd = spDialogue->m_vecCase.end();
			for ( INT iCount = 0; dlgCaseItr != dlgCaseEnd; ++dlgCaseItr, ++iCount )
			{
				// talk;
				CString strTalkName;
				strTalkName.Format("%s_%d_%d", strDlgSetName.c_str(), spDialogue->GetDlgNID(), iCount);
				CString strTalkDesc = CGameTextMan::GetInstance().GetText(
					strTalkName.GetString()
					, 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();

				CSingleobjectTextControl::GetInstance().InsertString(strTalkName.GetString(), strTalkDesc.GetString());

				std::tr1::shared_ptr<CNpcDialogueCase> spDlgCase = *dlgCaseItr;
				if ( spDlgCase->GetTalkControl() == NULL )
					continue;

				CNpcTalkControl::NPCMAP talkMapConst = spDlgCase->GetTalkControl()->GetTalkMapConst();
				CNpcTalkControl::NPCMAP_IT npcitr = talkMapConst.begin()
					, npcEnd = talkMapConst.end();
				for (; npcitr!=npcEnd; ++npcitr )
				{
					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = npcitr->second;

					// answer;
					CString strAnswerName;
					strAnswerName.Format("%s_%d_%d_%d", strDlgSetName.c_str(), spDialogue->GetDlgNID(), iCount, spNpcTalk->m_dwNID );

					CString strAnswerDesc = CGameTextMan::GetInstance().GetText(
						strAnswerName.GetString()
						, 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();

					CSingleobjectTextControl::GetInstance().InsertString(strAnswerName.GetString(), strAnswerDesc.GetString());
				}
			}
		}
	}
}

void CNpcActionPage::UpdateItems ()
{
	CString strTemp;

	//	Note : Dlg ID를 적용한다.
	//
	SetWin_Num_int ( this, IDC_EDIT_DIALOGUE_ID, m_DummyDialogue.GetDlgNID() );

	//	Note : 분기문 리스트를 갱신합니다.
	//
	m_listctrlCase.DeleteAllItems ();

	TCHAR szTemp[32] = {0};

	const CNpcDialogue::DLGCASE& sVecDlgCase = m_DummyDialogue.GetDlgCaseVec();
	CNpcDialogue::DLGCASE_CITER iter = sVecDlgCase.begin();
	CNpcDialogue::DLGCASE_CITER iter_end = sVecDlgCase.end();
	for ( DWORD dwIndex=0; iter!=iter_end; ++iter, ++dwIndex )
	{
		std::tr1::shared_ptr<CNpcDialogueCase> spCase = (*iter);
		std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spCase->GetCondition();
		std::string strCondition;
		if (spCondition)
            spCondition->GetConditionText(strCondition);

		//std::strstream strStream;
		//strStream << nIndex << std::ends;
		
		_snprintf( szTemp, 32, "%u", dwIndex );

		int nItem = m_listctrlCase.InsertItem ( dwIndex, szTemp );
		m_listctrlCase.SetItemData ( nItem, dwIndex );
		
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strKey;
			strKey.Format("%s_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), dwIndex);
			// const char* szTalk = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT);
			const char* szTalk = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString());
			m_listctrlCase.SetItemText ( dwIndex, 1, szTalk );
		}
		else
		{
			m_listctrlCase.SetItemText ( dwIndex, 1, spCase->GetBasicTalk().c_str() );
		}
		m_listctrlCase.SetItemText ( dwIndex, 2, strCondition.c_str() );

		//std::strstream strStream2;
		//strStream2 << pCase->GetTalkNum() << std::ends;

		_snprintf( szTemp, 32, "%u", spCase->GetTalkNum() );

		m_listctrlCase.SetItemText ( dwIndex, 3, szTemp );

		//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
		//strStream2.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	}

	//	Note : 현제 선택된 분기문를 미리보기.
	//
	UpdateCase();
}

void CNpcActionPage::UpdateCase ()
{
	//	Note : 현제 선택된 분기문를 미리보기.
	//
	m_listctrlAnswer.DeleteAllItems ();

	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if ( pos != NULL)
	{
		int nItem = m_listctrlCase.GetNextSelectedItem(pos);
		DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

		//	Note : 분기문 미리보기.
		//
		std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwData );
		if ( spCase )
		{
			std::string strTalk;
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strKey;
				strKey.Format("%s_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), nItem);
				// strTalk = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
				strTalk = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString();
			}
			else
			{
				strTalk = spCase->GetBasicTalk();
			}
			
			std::string strCondition;
			std::tr1::shared_ptr<SNpcTalkCondition> spCondition = spCase->GetCondition();
			if (spCondition)
                spCondition->GetConditionText(strCondition);

			SetWin_Text(this, IDC_EDIT_DIALOGUE_CASE, strTalk.c_str());
			SetWin_Text(this, IDC_EDIT_DIALOGUE_CASE_CONDITION, strCondition.c_str());

			std::tr1::shared_ptr<CNpcTalkControl> spTalkCtrl = spCase->GetTalkControl();
			if (spTalkCtrl)
			{
				TCHAR szTemp[32] = {0};

				const CNpcTalkControl::NPCMAP& mapTALK = *spTalkCtrl->GetTalkMap();
				CNpcTalkControl::NPCMAP_CIT iter = mapTALK.begin();
				CNpcTalkControl::NPCMAP_CIT iter_end = mapTALK.end();
				for ( int nIndex=0; iter!=iter_end; ++iter, ++nIndex )
				{
					std::tr1::shared_ptr<SNpcTalk> spTalk = iter->second;
					spTalk->GetTalk();

					//std::strstream strNID;
					//strNID << pTalk->m_dwNID << std::ends;
                    
                    DWORD TalkQuestID = spTalk->QuestProgressID();
                    DWORD TalkQuestStep = spTalk->QuestProgressStepID();
                    GLQUEST* pQuest = NULL;
                    GLQUEST_STEP* pQuestStep = NULL;
                    if (TalkQuestID != UINT_MAX)
                        pQuest = GLQuestMan::GetInstance().Find(TalkQuestID);

                    if (pQuest && TalkQuestStep != UINT_MAX)
                        pQuestStep = pQuest->GetSTEP(TalkQuestStep);

                    std::string strTalkQuest;
                    std::string strTalkQuestStep;

					_snprintf( szTemp, 32, "%u", spTalk->m_dwNID );

					int nItem = m_listctrlAnswer.InsertItem ( nIndex, szTemp );	

					if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
					{
						CString strKey;
						strKey.Format("%s_%d_%d_%d"
							, m_spNpcDialogueSet->m_TalkName.c_str()
							, m_DummyDialogue.GetDlgNID()
							, dwData, spTalk->m_dwNID);
						// const char* szTalkText = CGameTextMan::GetInstance().GetText(strKey.GetString(), 0, CGameTextMan::EM_NPCTALK_TEXT).GetString();
						const char* szTalkText = CSingleobjectTextControl::GetInstance().GetText(strKey.GetString()).GetString();

						m_listctrlAnswer.SetItemText ( nIndex, 1, szTalkText );
					}
					else
					{
						m_listctrlAnswer.SetItemText ( nIndex, 1, spTalk->GetTalk() );	//	ID
					}
					m_listctrlAnswer.SetItemText ( nIndex, 2, SNpcTalk::strACTIONTYPE[spTalk->m_nACTION].c_str() );	//	ID
					
					std::string strACT_NO;
					switch ( spTalk->m_nACTION )
					{
					case SNpcTalk::EM_PAGE_MOVE:
						m_spNpcDialogueSet->GetDlgText ( spTalk->m_dwACTION_NO, strACT_NO );
						break;

					case SNpcTalk::EM_BASIC:
						strACT_NO = SNpcTalk::szBASICDESC[spTalk->m_dwACTION_NO].c_str();
						break;

					case SNpcTalk::EM_QUEST_START:
                        {
						    m_spNpcDialogueSet->GetDlgText(spTalk->m_dwACTION_NO, strACT_NO);
                            for (size_t i=0; i<MAX_QUEST_START; ++i)
                            {
                                DWORD StartQuest = spTalk->QuestStartID(i);
                                if (StartQuest != UINT_MAX)
                                {
                                    GLQUEST* pStartQuest = GLQuestMan::GetInstance().Find(StartQuest);
									if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
									{
										CString strKey;
										strKey.Format("QN_%03d_%03d", pStartQuest->m_sNID.wMainID, pStartQuest->m_sNID.wSubID);
										if (pStartQuest)
										{
											strTalkQuest += sc::string::format("%1% %2% ", StartQuest, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
										}
										else
											strTalkQuest += sc::string::format("Unknown Quest %1% ", StartQuest);
									}
									else
									{
										if (pStartQuest)
											strTalkQuest += sc::string::format("%1% %2% ", StartQuest, pStartQuest->GetTITLE());
										else
											strTalkQuest += sc::string::format("Unknown Quest %1% ", StartQuest);
									}
                                }
                            }                            
                        }
						break;

					case SNpcTalk::EM_QUEST_STEP:
						{
							if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
							{
								CString strKey;
								strKey.Format("QN_%03d_%03d_P_%d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID, TalkQuestStep);
								CString strStepStr = CGameTextMan::GetInstance().GetQuestText(strKey.GetString());
								strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
								CString strTitleStr = CGameTextMan::GetInstance().GetQuestText(strKey.GetString());

								m_spNpcDialogueSet->GetDlgText(spTalk->m_dwACTION_NO, strACT_NO);
								if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
								{
									CString strKey;
									strKey.Format("QN_%03d_%03d", pQuest->m_sNID.wMainID, pQuest->m_sNID.wSubID);
									if (pQuest)
										strTalkQuest = sc::string::format("%1% %2%", TalkQuestID, CGameTextMan::GetInstance().GetQuestText(strKey.GetString()).GetString());
									else
										strTalkQuest = sc::string::format("Unknown Quest %1% ", TalkQuestID);
								}
								else
								{
									if (pQuest)
										strTalkQuest = sc::string::format("%1% %2%", TalkQuestID, strTitleStr);
									else
										strTalkQuest = sc::string::format("Unknown Quest %1%", TalkQuestID);
								}

								if (pQuestStep)
									strTalkQuestStep = sc::string::format("%1% %2%", TalkQuestStep, strStepStr);
								else
									strTalkQuestStep = sc::string::format("Unknown Quest Step %1%", TalkQuestStep);
							}
							else
							{
								m_spNpcDialogueSet->GetDlgText(spTalk->m_dwACTION_NO, strACT_NO);
								if (pQuest)
									strTalkQuest = sc::string::format("%1% %2%", TalkQuestID, pQuest->GetTITLE());
								else
									strTalkQuest = sc::string::format("Unknown Quest %1%", TalkQuestID);

								if (pQuestStep)
									strTalkQuestStep = sc::string::format("%1% %2%", TalkQuestStep, pQuestStep->GetTITLE());
								else
									strTalkQuestStep = sc::string::format("Unknown Quest Step %1%", TalkQuestStep);
							}
						}
						break;

					default:
						//std::strstream strStream;
						//strStream << pTalk->m_dwACTION_NO << std::ends;
						//strACT_NO = strStream.str();
						//strStream.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.

						_snprintf( szTemp, 32, "%u", spTalk->m_dwACTION_NO );
						strACT_NO = szTemp;
						break;
					};

					m_listctrlAnswer.SetItemText(nIndex, 3, strACT_NO.c_str());
                    m_listctrlAnswer.SetItemText(nIndex, 4, strTalkQuest.c_str());
                    m_listctrlAnswer.SetItemText(nIndex, 5, strTalkQuestStep.c_str());
					m_listctrlAnswer.SetItemData(nItem, (DWORD_PTR) spTalk->m_dwNID);

					//strNID.freeze( false );	// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				}
			}
		}
	}
	else
	{
		SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE, "" );
		SetWin_Text ( this, IDC_EDIT_DIALOGUE_CASE_CONDITION, "" );
	}
}

void CNpcActionPage::InverseUpdateItems ()
{
	CString strTemp;
	CEdit* pEdit = NULL;

	pEdit = (CEdit*) GetDlgItem ( IDC_EDIT_DIALOGUE_ID );
	pEdit->GetWindowText ( strTemp );

	DWORD NativeID = (DWORD) atoi ( strTemp.GetString () );
	m_DummyDialogue.SetDlgNID ( NativeID );
}

BOOL CNpcActionPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_listctrlCase.SetExtendedStyle ( m_listctrlCase.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_listctrlCase.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 4;	
	char* szColumnName1[nColumnCount] = { "ID", "Text Of Conversation", "Condition", "Number Of Answer" };
	int nColumnWidthPercent1[nColumnCount] = { 10, 50, 20, 20 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_listctrlCase.InsertColumn (i, &lvColumn );
	}

    const int AnswerColumnCount = 6;
	char* szColumnName2[AnswerColumnCount] = { "ID", "Answer", "Action Type", "Data", "Quest Num", "Quest Step" };
	int nColumnWidthPercent2[AnswerColumnCount] = { 10, 35, 10, 20, 10, 10 };

	m_listctrlAnswer.SetExtendedStyle ( m_listctrlAnswer.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_listctrlAnswer.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	for ( int i = 0; i < AnswerColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.pszText = szColumnName2[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent2[i] ) / 100;
		m_listctrlAnswer.InsertColumn (i, &lvColumn );
	}

	SetReadMode();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CNpcActionPage::OnLvnItemchangedListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	//	Note : 현제 선택된 분기문를 미리보기.
	//
	UpdateCase();

	*pResult = 0;
}

void CNpcActionPage::OnBnClickedButtonCaseNew()
{
	CNpcDialogueCase sCase;
	DWORD dwSize = m_DummyDialogue.AddCase ( sCase );
	std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwSize-1 );

 	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 	{
 		CString strInsertKey;
 		strInsertKey.Format("%s_%d_%d"
 			, m_spNpcDialogueSet->m_TalkName.c_str()
 			, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID(), dwSize-1);
 		// CGameTextMan::GetInstance().InsertString(strInsertKey.GetString(), "skip", CGameTextMan::EM_NPCTALK_TEXT);
		CSingleobjectTextControl::GetInstance().InsertString(strInsertKey.GetString(), "skip");
 	}

	m_pSheetTab->ActiveActionAnswer ( m_spNpcDialogueSet, spCase, dwSize-1 );
}

void CNpcActionPage::OnBnClickedButtonCaseEdit()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to edit.", "Information", MB_OK );
		return;
	}
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

	//	Note : Case 편집 수행.
	//
	std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwData );
	m_pSheetTab->ActiveActionAnswer ( m_spNpcDialogueSet, spCase, dwData );
}

void CNpcActionPage::OnBnClickedButtonCaseDel()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to delete.", "Information", MB_OK );
		return;
	}
	
	if ( IDYES==MessageBox("Do you want to delete 'Branching Statement'?","Caution",MB_YESNO) )
	{
		int nItem = m_listctrlCase.GetNextSelectedItem(pos);
		DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

 		// 문자열 정리;
 		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
 		{
 			// talk delete
 			CString strTalkKey;
 			strTalkKey.Format("%s_%d_%d"
 				, m_spNpcDialogueSet->m_TalkName.c_str()
 				, m_pSheetTab->m_NpcActionPage.GetCurrentDialogueNID(), dwData);
 			// CGameTextMan::GetInstance().DeleteString(strTalkKey.GetString(), CGameTextMan::EM_NPCTALK_TEXT);
			CSingleobjectTextControl::GetInstance().DeleteString(strTalkKey.GetString());

 			// answer delete
 			std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwData );
 			if ( spCase->GetTalkControl() )
 			{
 				CNpcTalkControl::NPCMAP talkMapConst = spCase->GetTalkControl()->GetTalkMapConst();
 				CNpcTalkControl::NPCMAP_IT itrBegin, itrEnd = talkMapConst.end();
 				for ( itrBegin=talkMapConst.begin(); itrBegin!=itrEnd; ++itrBegin )
 				{
 					std::tr1::shared_ptr<SNpcTalk> spNpcTalk = itrBegin->second;
 
 					CString strAnswerKey;
 					strAnswerKey.Format("%s_%d_%d_%d", m_spNpcDialogueSet->m_TalkName.c_str(), m_DummyDialogue.GetDlgNID(), dwData, spNpcTalk->m_dwNID );
 					//CGameTextMan::GetInstance().DeleteString(strAnswerKey.GetString(), CGameTextMan::EM_NPCTALK_TEXT);		
					CSingleobjectTextControl::GetInstance().DeleteString(strAnswerKey.GetString());
 				}
 			}
 		}

		// 데이터 삭제;
		m_DummyDialogue.DelCase ( dwData );

		UpdateItems ();
	}
}


void CNpcActionPage::OnBnClickedButtonUp()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );
	
	DWORD dwNEW_ID = m_DummyDialogue.ToUp ( dwData );

	if ( dwNEW_ID!=dwData )
	{
		UpdateItems ();
		m_listctrlCase.SetItemState( --nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	}
}

void CNpcActionPage::OnBnClickedButtonDown()
{
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)	return;
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );
	
	DWORD dwNEW_ID = m_DummyDialogue.ToDown ( dwData );

	if ( dwNEW_ID!=dwData )
	{
		UpdateItems ();
		m_listctrlCase.SetItemState( ++nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	}
}

void CNpcActionPage::SetReadMode()
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM
	const int nSkipNum = 5;
	const int nSkipID[nSkipNum] = 
    { 
        IDC_BUTTON_OK, 
        IDC_BUTTON_CANCEL, 
        IDC_BUTTON_CASE_EDIT, 
        IDC_LIST_POSTIVE_ANSWER2,
        IDC_LIST_CASE_ANSWER
    };

	int nID = 0;
	bool bOK = false;

	CWnd* pChildWnd = GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		bOK = false;
		nID = pChildWnd->GetDlgCtrlID();	

		for ( int i = 0; i < nSkipNum; ++i )
		{
			if ( nID == nSkipID[i] )
			{
				bOK = true;	
				break;
			}
		}

		if ( !bOK )	pChildWnd->EnableWindow( FALSE );

		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}

#endif
}


void CNpcActionPage::OnNMDblclkListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	NMITEMACTIVATE* pNMIA = (NMITEMACTIVATE*) pNMHDR;

	if ( pNMIA->iItem == -1 )
	{
		return ;
	}

	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( pNMIA->iItem );

	//	Note : Case 편집 수행.
	//
	std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwData );
	m_pSheetTab->ActiveActionAnswer ( m_spNpcDialogueSet, spCase, dwData );
}

void CNpcActionPage::OnBnClickedButtonCaseCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_listctrlCase.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox ( "Select the branching statement to copy.", "Information", MB_OK );
		return;
	}
	
	int nItem = m_listctrlCase.GetNextSelectedItem(pos);
	DWORD dwData = (DWORD) m_listctrlCase.GetItemData ( nItem );

	//	Note : Case 편집 수행.
	//
	std::tr1::shared_ptr<CNpcDialogueCase> spCase = m_DummyDialogue.GetDlgCase ( dwData );
	
	CNpcDialogueCase sCase;
	
	sCase = *spCase.get();		
	m_DummyDialogue.AddCase ( sCase );
	
	UpdateItems ();

}
