#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLCompetition.h"

#include <vector>

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicLineBoxSmart;
class CBasicButton;
class CSwapImage;
class CSkillImage;

class CCTFHistory : public CUIGroup
{
private:
	struct SKILLIMAGE
	{
		CSkillImage*		pImage;
		CBasicLineBoxSmart* pLine;

		SKILLIMAGE()
			: pImage(NULL)
			, pLine(NULL)
		{
		}

		SKILLIMAGE(CSkillImage* _pImage, CBasicLineBoxSmart* _pLine)
			: pImage(_pImage)
			, pLine(_pLine)
		{
		}
	};

	typedef std::vector<SKILLIMAGE>	 VEC_SKILLIMAGE;
	typedef VEC_SKILLIMAGE::iterator VEC_SKILLIMAGE_ITER;

private:
	enum
	{
		CTF_HISTORY_NONE = NO_ID + 1,
		CTF_HISTORY_RANKING_BUTTON,
		CTF_HISTORY_BUFF_BEGIN,
	};

public:
	CCTFHistory(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCTFHistory ();

public:
	void CreateSubControl ( LPDIRECT3DDEVICEQ* ppd3dDevice );

public:
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

public:
	void            Update ();

public:
	void			SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

private:
	CBasicTextBox*  CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign );

private:
	CBasicTextBox*	 m_pText_Time;
	CBasicTextBox*	 m_pText_Buff;
	CBasicTextBox*	 m_pText_Ranking;

	CSwapImage*		 m_pImage_AuthSchool;

	CUIControl*		 m_pAuthSchoolControl;
	CUIControl*		 m_pBuffControl;
	CUIControl*		 m_pBuffGapControl;
	CUIControl*		 m_pBuffItemControl;

	CBasicButton*	 m_pButton_Ranking;

	VEC_SKILLIMAGE	 m_vecSkillImage;

private:
	PVP::ENUM_CTF_TYPE	m_emCTFType;

protected:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;

protected:
	 LPDIRECT3DDEVICEQ* m_ppd3dDevice;

};
