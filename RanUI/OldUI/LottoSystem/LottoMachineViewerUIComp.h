#pragma once

#include "../Util/UIGroupHelper.h"

#include "../../RanLogicClient/LottoSystem/GLLottoDataTypeClient.h"

class CInnerInterface;
class EngineDeviceMan;

class CViewFrameMeshComponent;
class CBasicLineBoxSmart;

namespace LottoSystem
{
	class CLottoSystemClient;
}

// 3D Lotto Machine Viewer Component;
class CLottoMachineViewerUIComp : public CUIGroupHelper
{
protected:
	enum
	{
		ELottoMachine = NO_ID + 1
	};

public:
	CLottoMachineViewerUIComp( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CLottoMachineViewerUIComp();

public:
	virtual void CreateSubControl();

public:
	void UpdateTurn( const LottoSystem::CLottoSystemClient* pLottoSystem );
	void UpdateMachine(
		const LottoSystem::CLottoSystemClient* pLottoSystem,
		int drawingNum );
	void AnimationTrigger();

	void UpdateAnimation( float fElapsedTime );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void SetVisibleSingle( BOOL bVisible );

private:
	CInnerInterface* m_pInterface;

	CViewFrameMeshComponent* m_pViewer;
	CBasicLineBoxSmart* m_pDontUseMachineBox;

	LottoSystem::LOTTO_SYSTEM_ID m_id;
};