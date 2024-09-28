#pragma	once
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Interface/UIEditBoxLink.h"
#include "../Interface/DynamicBasicTextBoxExLink.h"

class	CInnerInterface;
class	CBasicTextBox;
class	GLGaeaClient;
class   RnButton;
class   CBasicLineBoxSmart;
class	CSwapImage;


const int nMaxMember = 8;
class	RnCDMClubInfo: public  CUIWindowEx

{
public:
	enum
	{
		RNCDM_CLUB_INFO_CLOSE_BUTTON = ET_CONTROL_NEXT,
	};

public:
	RnCDMClubInfo (GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~RnCDMClubInfo ();

public:
	void	CreateSubControl ();	



public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );
	void			SetMemberImage( int nIndex, EMCHARINDEX emCharIndex );
	void			SetMemberName( int nIndex, const CString strMemberName );
	void			SetTitleClubNameMemberNum( const CString strClubName, int nMembernum );
	void			SetRnCDMClubInfoPos(int nPosX, int nPosY);


private:
	CBasicTextBox*		m_pTitle;
	CBasicLineBoxSmart* m_pBGLine;
	CSwapImage*			m_pMemberImage[nMaxMember];
	CBasicTextBox*		m_pMemberName[nMaxMember];
	RnButton*			m_pCloseButton;
	int					m_nPosX;
	int					m_nPosY;

protected:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface*    m_pInterface;

};

class MyRnCDMClubInfo  : public IRnCDMClubInfo, private RnCDMClubInfo
{
public:
	MyRnCDMClubInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyRnCDMClubInfo() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	void	SetMemberImage( int nIndex, EMCHARINDEX emCharIndex );
	void	SetMemberName( int nIndex, const CString strMemberName );
	void	SetTitleClubNameMemberNum( const CString strClubName, int nMembernum );
	void	SetRnCDMClubInfoPos(int nPosX, int nPosY);

};
