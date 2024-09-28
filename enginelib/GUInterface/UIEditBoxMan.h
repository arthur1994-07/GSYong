#pragma	once

#include "UIGroup.h"

#include <boost/function.hpp>
#include <vector>

#define getBool(n) ((n) != 0)

class	CUIEditBox;
class	CD3DFontPar;

class CUIEditBoxMan : public CUIGroup
{
private:
	typedef boost::function< bool ( UIGUID ) > FunctionV1;

static	const	int		nDEFAULT_TABINDEX;

public:
	CUIEditBoxMan (EngineDeviceMan* pEngineDevice);
	virtual	~CUIEditBoxMan ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void	Init ();

	void	BeginEdit ();	
	void	EndEdit ();

	void	ClearEdit ( UIGUID ControlID );

	bool	IsMODE_NATIVE ();
	void	DoMODE_TOGGLE ();

	void	GoNextTab ();
	void	GoThisTab ();

	CString	GetEditString ();
	int		GetEditLength ();

	CString	GetEditString ( UIGUID ControlID );
	int		GetEditLength ( UIGUID ControlID );

	void	SetEditString ( const CString& strInput );
	void	SetEditString ( UIGUID ControlID, const CString& strInput );

	CUIEditBox*	GetEditBox ( const UIGUID& cID );

	void	SetVisibleEdit ( const UIGUID& cID, const BOOL& bVisible );

	void	SetHide ( UIGUID ControlID, BOOL bHide );
	BOOL	IsHide ( UIGUID ControlID );
	
	void	CreateEditBox(
        UIGUID WndID,
        const char* szEditBox,
        const char* szCarrat,
        BOOL bCarratUseRender,
        DWORD dwDiffuse,
        CD3DFontPar* pFont,
        int nLIMITCHAR, 
		BOOL bUseOnlyNumber = FALSE,
		BOOL bUseOnlyDecimalNumber = FALSE);
	void	AddEditBox(
		CUIEditBox* pEditBox );

	UIGUID GetBeginEditBox(); // Note : 현재 에디터중인 에디터 박스를 얻는다.

	void DisableKeyInput();
	void UsableKeyInput();

    int	GetLimitInput( UIGUID ControlID );

public:
	// Manager가 관리하는 Function이 Focus를 잃었을 경우;
	// 확인해야할 내용을 처리하는 함수를 추가한다;
	void AddConfirmFunc( FunctionV1 fnConfirm );

private:
	BOOL	RegisterControl ( CUIControl* pNewControl );

	BOOL	FindTabIndexControl ();

	void	SetEditBox ( CUIEditBox* pEditBox )		{ m_pEditBox = pEditBox; }

private:
	int		m_nCurTabIndex;
	CUIEditBox*	m_pEditBox;

	std::vector< FunctionV1 > m_confirmFuncs;
};
