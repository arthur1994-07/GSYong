#pragma once

//////////////////////////////////////////////////////////////////////////
//
// 2013.01.25~02.13 chjeon 
// CProductResetWindow 클래스 추가
// 제작 초기화 윈도우 UI
//
//////////////////////////////////////////////////////////////////////////
#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/MSG/ProductMsg.h"

//항목초기화 최대 갯수
const int MAX_PRODUCT_RESET = 4;

class CBasicLineBoxSmart;
class CBasicTextBox;
class RnButton;
class CCheckBox;

class CProductResetWindow : public CUIWindowEx
{
private:
	/**
		제작초기화 윈도우 서브컨트롤  ID 
	*/
	enum
	{
		ID_BTN_YES = ET_CONTROL_NEXT + 1,
		ID_BTN_NO,
		ID_TEXT_BOX,
		ID_TEXT_TOP,
		ID_TEXT_BOTTOM,

		ID_CHECKBOX_PRODUCT_0,
		ID_CHECKBOX_PRODUCT_1,
		ID_CHECKBOX_PRODUCT_2,
		ID_CHECKBOX_PRODUCT_3,

		ID_CHECKBOX_MAX,
	};

public:
	/**
		생성자
	*/
	CProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	/**
		제작초기화 기본 윈도우를 만들어 준다, 타이틀
	*/
	virtual void			CreateBaseWindow();

	/**
		제작초기화 윈도우 서브컨트롤을 만들어 준다.(버튼,체크박스,타이틀,텍스트 등)
		서브컨트롤의 부모 컨트롤을 이 클래스로 지정해준다.
	*/
	virtual void			CreateSubControl();

	/**
		텍스트UI 만든후 포인터 리턴
	*/
	CBasicTextBox*			CreateResetWindowText( const std::string& key, CD3DFontPar* pFont, const std::string& text );
	CBasicTextBox*			CreateResetWindowText( const std::string& key, CD3DFontPar* pFont );

	/**
		제작초기화 아이템 사용시 모든 값 설정 후 초기화윈도우 열기
	*/
	void					Open(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	bool					Init(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	bool					SetData(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	void					OpenResetWindow(WORD wResetAbleNum, WORD wPosX, WORD wPosY);

	/**
		제작초기화 아이템을 사용한다면 제작초기화 윈도우에 들어갈 제작항목을
		체크박스텍스트에 설정, 나머지 체크박스는 숨긴다.
	*/
	bool					SetSubControlData();

	/**
		제작초기화 아이템 사용시 윈도우 위치 설정 함수
	*/
	bool					SetWindowPos();

	/**
		제작초기화 요청하기
	*/
	void					ReqProductTypeDel();

	/**
		제작초기화 아이템 사용시 모든 값 초기화 후 초기화윈도우 닫기
	*/
	void					Close();
	void					Clear();
	void					CloseResetWindow();

	/**
		현재 제작초기화 윈도우에서 유저가 체크한 체크항목을 벡터에 넣어준다.
	*/
	void					SetVecCheckedProduct( );

	/**
		초기화가 가능한 개수와 현재 유저가 체크한 개수를 비교해서 체크가 되지 않기 위한 함수
		사실상 유저가 선택한 개수가 초기화가능 개수보다 많다면 선택 해제 시켜준다.
	*/
	void					SetCheckOff( UIGUID ControlID );

	/**
		현재 제작초기화 윈도우에서 유저가 체크한 체크개수를 멤버변수에 대입한다.
	*/
	void					SetCheckedNum();

	/**
		대입한 체크개수 멤버변수를를 받아온다.
	*/
	const WORD&				GetCheckedNum();

	/**
		초기화 윈도우가 열리면 인벤토리 윈도우 Lock
		초기화 윈도우가 닫히면 인벤토리 윈도우 UnLock
	*/
	bool					SetInventoryLock();
	bool					SetInventoryUnLock();

	/**
		제작초기화 아이템을 사용 시 '예' 버튼을 누를 경우 가능여부를 검사한다.
	*/
	bool					IsPossibleReset();

	/**
		학습한 항목이 있는지 검사한다.
	*/
	bool					IsStudyProduct();

	virtual	void			SetVisibleSingle ( BOOL bVisible );

protected:	
	virtual	void			TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	CBasicTextBox*			m_pTextBoxTitle;							// 제작초기화 윈도우 타이틀
	CBasicTextBox*			m_pTextBoxMessageTop;						// 제작초기화 윈도우 상단 텍스트
	CBasicTextBox*			m_pTextBoxMessageBottom;					// 제작초기화 윈도우 하단 텍스트

	CBasicTextBox*			m_pTextBoxProductList[MAX_PRODUCT_RESET];	// 유저가 습득한 제작항목 텍스트 배열
	int						m_nProductEnum[MAX_PRODUCT_RESET];			// 유저가 습득한 제작항목 enum 배열

	CCheckBox*				m_pCheckBoxProductList[MAX_PRODUCT_RESET];  // 유저가 습득한 제작항목 체크박스 배열

	RnButton*				m_pYes;										// 예		버튼컨트롤 포인터
	RnButton*				m_pNo;										// 아니오	버튼컨트롤 포인터

	WORD					m_wResetAble;								// 초기화아이템의 초기화 가능한 개수
	WORD					m_wUserCheckedNum;							// 초기화아이템 사용시 유저가 항목을 체크한 개수

	WORD					m_wPosX;									// 초기화아이템 인벤토리 x위치 
	WORD					m_wPosY;									// 초기화아이템 인벤토리 y위치

	std::vector<int>		m_vecStudyProduct;							// 유저가 습득한 제작항목
	std::vector<int>		m_vecCheckedProduct;						// 유저가 선택한 항목을 저장할 벡터

	CInnerInterface*		m_pInterface;
	GLGaeaClient*			m_pGaeaClient;
};

class MyProductResetWindow : public IProductResetWindow, private CProductResetWindow
{
public:
	MyProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual void	CreateUIWindowAndRegisterOwnership();
	virtual void	Open(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
};