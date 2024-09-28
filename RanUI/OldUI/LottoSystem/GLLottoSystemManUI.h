#pragma once

#include "../../../SigmaCore/Util/GLSingletone.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

#include "../Util/MessageBox.h"

#include "./ILottoSystemUI.h"

#include "../../../SigmaCore/Json/MinJson.h"

class CInnerInterface;

namespace LottoSystem
{
	class CLottoSystemClient;
}

class GLLottoSystemManUI : public CSingletone<GLLottoSystemManUI>
{
protected:
	friend CSingletone;

public:
	GLLottoSystemManUI();
	~GLLottoSystemManUI();

public:
	void Initialize();
	void Destroy();
	bool Update( float fElapsedTime );

	void EventProc( float fElapsedTime );

public:
	void ChangeTabIndex( unsigned int nTabIndex );
	void ChangeTurn( unsigned int nTabIndex );

	void ClearLottoNum();

public:
	// ����UI �޽��� �ڽ� ����;
	void OpenMessageBoxMainUI(
		const CMessageBox::BOXTITLE eBoxTitle,
		const std::string& strMessage,
		const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OK,
		unsigned int nIdentity = 0 );

	// ����UI ��÷ Ȯ�� ��ư ��Ȱ��ȭ;
	void InactiveConfirmButtonMainUI();
	void ActiveConfirmButtonMainUI();

public:
	bool IsChildWindowOpen();
	bool IsBuyLottoUIOpen();

private:
	void UIInitializeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateCurrentStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateChangeStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateBuyListClearProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateTurnAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateConfirmTurnBuyListProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateConfirmTurnWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateTurnWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateWinManListProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIUpdateHaveMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIViewRandomTypeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIViewInventTypeProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIViewIncorrectSetWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIViewLockBuyProc( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UIViewLockChangeTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem );

	void UIUpdateNumDSPProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIUpdateNumProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIViewPostMessageProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIViewNumDSPProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIUpdateDrawingNumProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIViewCorrectSetWinNumProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIUpdateBuyListProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );
	void UIUpdateBuyListCountProc(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		const json_spirit::mObject outputObj );

public:
	inline void SetInterface( CInnerInterface* pInterface ) { m_pInterface = pInterface; }
	inline unsigned int GetCurTabIndex() { return m_nCurTabIndex; }
	inline bool GetConfirmState() { return m_bConfirm; }
	inline void SetConfirmState( bool bConfirm ) { m_bConfirm = bConfirm; }

	inline void SetAccumulateMoneyLED( unsigned int state ) { m_nAccumulateMoneyLEDCheckTabIdx = state; }
	inline unsigned int GetAccumulateMoneyLED() { return m_nAccumulateMoneyLEDCheckTabIdx; }

	inline LottoSystem::LOTTO_SYSTEM_LOTTO_NUM& GetLottoNum() { return m_lottoNum; }
	inline LottoSystem::EMLOTTO_BUY_TYPE& GetBuyType() { return m_buyType; }
	inline void SetLottoNum( LottoSystem::LOTTO_SYSTEM_LOTTO_NUM lottoNum ) { m_lottoNum = lottoNum; }
	inline void SetLottoBuyType( LottoSystem::EMLOTTO_BUY_TYPE buyType ) { m_buyType = buyType; }

private:
	CInnerInterface* m_pInterface;

	// ���� Tab Index;
	unsigned int m_nCurTabIndex;

	// ��÷ Ȯ�� ����;
	bool m_bConfirm;

	// ���� ���� �ζǹ�ȣ �ӽ� ����;
	LottoSystem::LOTTO_SYSTEM_LOTTO_NUM m_lottoNum;
	LottoSystem::EMLOTTO_BUY_TYPE m_buyType;

	// ���� ������ üũ ����;
	unsigned int m_nAccumulateMoneyLEDCheckTabIdx;

private:
	ILottoMainUI* m_pMainUI;
	ILottoBuyLottoUI* m_pLottoBuyLottoUI;
	ILottoBuyListInfo* m_pLottoBuyListInfo;
	ILottoWinManListInfo* m_pLottoWinManListInfo;
	ILottoDisplayUI* m_pDisplayUI;
	ILottoNotifyButton* m_pNotifyButton;
};