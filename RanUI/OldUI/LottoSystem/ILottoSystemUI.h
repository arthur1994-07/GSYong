#pragma once

#include "../../../enginelib/GUInterface/UIWindow.h"

#include "../Util/MessageBox.h"

#include "./LottoUIDateType.h"

namespace LottoSystem
{
	class CLottoSystemClient;
}

class ILottoMainUI
{
public:
	virtual ~ILottoMainUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void InitializeUI( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void UpdateAccumulateMoneyTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void UpdateDrawingDateStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateBuyListStateProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void UpdateDrawingDateProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateProgressTimeProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void UpdateMachineTurn( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateMachine(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int drawingNum ) = 0;
	virtual void UpdateWinNum( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateBuyList(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int idx = -1 ) = 0;
	virtual void UpdateBuyListCount(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int count  ) = 0;
	virtual void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void UpdateMachineAnimation(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		float fElapsedTime ) = 0;

	virtual void OpenMessageBox(
		const CMessageBox::BOXTITLE eBoxTitle,
		const std::string& strMessage, 
		const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OK,
		unsigned int nIdentity = 0 ) = 0;

	virtual void InactiveConfirmButton() = 0;
	virtual void ActiveConfirmButton() = 0;
};

class ILottoBuyLottoUI
{
public:
	virtual ~ILottoBuyLottoUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void UpdateHaveMoney( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
};

class ILottoBuyListInfo
{
public:
	virtual ~ILottoBuyListInfo() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void ChangeMainTab() = 0;

	virtual void UpdateBuyList( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateChangeState( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;

	virtual void ConfirmRanking() = 0;
	virtual void ConfirmStateOff() = 0;
};

class ILottoWinManListInfo
{
public:
	virtual ~ILottoWinManListInfo() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void ChangeMainTab() = 0;

	virtual void UpdateWinNumProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateAccumulateMoneyProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateWinManList( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void UpdateTurnProc( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
};

class ILottoDisplayUI
{
public:
	virtual ~ILottoDisplayUI() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;

	virtual void UpdateAccumulateMoney( const LottoSystem::CLottoSystemClient* pLottoSystem ) = 0;
	virtual void ResetTextAnimation() = 0;
};

class ILottoNotifyButton
{
public:
	virtual ~ILottoNotifyButton() { };

public:
	virtual void CreateUIWindowAndRegisterOwnership() = 0;

	virtual bool IsVisible() = 0;
};