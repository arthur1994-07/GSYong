
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_CHECKBOX_PUSH = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CCheckBox : public CUIGroup
{
protected:
	enum
	{
		CHECKBOX_NONE = NO_ID,
	};

public:
	CCheckBox(EngineDeviceMan* pEngineDevice);
	virtual	~CCheckBox ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void CreateSubControl ( const char* szCheckOnKeyword, const char* szCheckOffKeyword );

public:
	virtual void Reset	     ();
	virtual void SetCheckOn  ();
	virtual void SetCheckOff ();
	virtual void SetCheck    ( const BOOL bCheck );

public:
	void		 Toggle    () { if ( IsChecked() ) SetCheck( FALSE ); else SetCheck ( TRUE ); }
	BOOL		 IsChecked () { return m_bChecked; }

protected:
	BOOL		 m_bChecked;

protected:
	UIRECT		 m_rcTexPosOn;
	UIRECT		 m_rcTexPosOff;
	CString		 m_strTextureNameOn;
	CString		 m_strTextureNameOff;

};