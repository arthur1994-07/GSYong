#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

namespace clubmark
{
	struct DXDATA;
}

class CBasicLineBoxSmart;
class CSwapImage;
class CBasicTextBox;
class GLGaeaClient;
class CInnerInterface;
class CUIControl;

struct PROGRESSTIME
{
	int nHour;
	int nMin;
	float fSecond;
	PROGRESSTIME()
		:nHour(0)
		,nMin(0)
		,fSecond(0)
	{
	}
};

class RnCDMProgress : public CUIGroup
{


protected:

	enum	//	기본 윈도우 ID ( 외부로 공개되지 않음 )
	{
		ID_BG_LINEBOX = NO_ID + 1,
	};

	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

private:
	CBasicLineBoxSmart* m_pBGLineBox;
	CUIControl* m_pBGTheFirstRank;
	CUIControl* m_pBGMyRank;

	CBasicTextBox* m_pTheFirstRank;
	CUIControl* m_pTheFirstClubImage;
	CBasicTextBox* m_pTheFirstClubName;
	CBasicTextBox* m_pTheFirstClubScore;

	CBasicTextBox* m_pMyRank;
	//CSwapImage* m_pMyClubImage;
	CUIControl* m_pMyClubImage;
	CBasicTextBox* m_pMyClubName;
	CBasicTextBox* m_pMyClubScore;

	CBasicTextBox* m_pCDMStateTEXT;
	CBasicTextBox* m_pCDMProgessTime;

    CBasicTextBox* m_pTopChangeText;

protected:
	BOOL			m_bFirstGap;
	D3DXVECTOR2		m_vGap;

	int				m_PosX;
	int				m_PosY;
	D3DXVECTOR2		m_vPos;

	PROGRESSTIME m_sProgessTime;
	float			m_fTimer;
    DWORD           m_dwNumberOneClubDbNum;
    float           m_fTopChangeMessageTime;

public:
	RnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~RnCDMProgress();

	void	CreateSubControl ();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual void SetVisibleSingle( BOOL bVisible );
public:	
	void RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
							  DWORD	dwNumberOneClubDbNum,
							  char*	szNumberOneName,
							  int	nNumberOneScore,
							  int	nMyRank,
							  DWORD	dwMyScore,
							  int	nHour,
							  int	nMin,
							  int	nSecond );

	void SetTheFirstRank( const CString& strText );
	void SetTheFirstClubName( const CString& strText );
	void SetTheFirstClubScore( const DWORD dwScore );

	void SetMyRank( const int nRank );
	void SetMyClubName( const CString& strText );
	void SetMyClubScore( const DWORD dwScore );

	void SetCDMStateTEXT( const CString& strText );
	void SetCDMProgessTime( int nHour, int nMin, float fSecond );
	void UpdateTime(float fElapsedTime);
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );


};

class MyRnCDMProgress: public IRnCDMProgress, private RnCDMProgress
{
public:
	MyRnCDMProgress( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
		:RnCDMProgress( pGaeaClient, pInterface, pEngineDevice )
	{

	}
	virtual ~MyRnCDMProgress()   {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void RefreashCdmProgress( DWORD	dwNumberOneClubMarkVER,
									  DWORD	dwNumberOneClubDbNum,
									  char*	szNumberOneName,
									  int	nNumberOneScore,
									  int	nMyRank,
									  DWORD	dwMyScore,
									  int	nHour,
									  int	nMin,
									  int	nSecond );
};