#pragma once

//////////////////////////////////////////////////////////////////////////
//
// 2013.01.25~02.13 chjeon 
// CProductResetWindow Ŭ���� �߰�
// ���� �ʱ�ȭ ������ UI
//
//////////////////////////////////////////////////////////////////////////
#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"
#include "../../../RanLogic/MSG/ProductMsg.h"

//�׸��ʱ�ȭ �ִ� ����
const int MAX_PRODUCT_RESET = 4;

class CBasicLineBoxSmart;
class CBasicTextBox;
class RnButton;
class CCheckBox;

class CProductResetWindow : public CUIWindowEx
{
private:
	/**
		�����ʱ�ȭ ������ ������Ʈ��  ID 
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
		������
	*/
	CProductResetWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	/**
		�����ʱ�ȭ �⺻ �����츦 ����� �ش�, Ÿ��Ʋ
	*/
	virtual void			CreateBaseWindow();

	/**
		�����ʱ�ȭ ������ ������Ʈ���� ����� �ش�.(��ư,üũ�ڽ�,Ÿ��Ʋ,�ؽ�Ʈ ��)
		������Ʈ���� �θ� ��Ʈ���� �� Ŭ������ �������ش�.
	*/
	virtual void			CreateSubControl();

	/**
		�ؽ�ƮUI ������ ������ ����
	*/
	CBasicTextBox*			CreateResetWindowText( const std::string& key, CD3DFontPar* pFont, const std::string& text );
	CBasicTextBox*			CreateResetWindowText( const std::string& key, CD3DFontPar* pFont );

	/**
		�����ʱ�ȭ ������ ���� ��� �� ���� �� �ʱ�ȭ������ ����
	*/
	void					Open(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	bool					Init(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	bool					SetData(WORD wResetAbleNum, WORD wPosX, WORD wPosY);
	void					OpenResetWindow(WORD wResetAbleNum, WORD wPosX, WORD wPosY);

	/**
		�����ʱ�ȭ �������� ����Ѵٸ� �����ʱ�ȭ �����쿡 �� �����׸���
		üũ�ڽ��ؽ�Ʈ�� ����, ������ üũ�ڽ��� �����.
	*/
	bool					SetSubControlData();

	/**
		�����ʱ�ȭ ������ ���� ������ ��ġ ���� �Լ�
	*/
	bool					SetWindowPos();

	/**
		�����ʱ�ȭ ��û�ϱ�
	*/
	void					ReqProductTypeDel();

	/**
		�����ʱ�ȭ ������ ���� ��� �� �ʱ�ȭ �� �ʱ�ȭ������ �ݱ�
	*/
	void					Close();
	void					Clear();
	void					CloseResetWindow();

	/**
		���� �����ʱ�ȭ �����쿡�� ������ üũ�� üũ�׸��� ���Ϳ� �־��ش�.
	*/
	void					SetVecCheckedProduct( );

	/**
		�ʱ�ȭ�� ������ ������ ���� ������ üũ�� ������ ���ؼ� üũ�� ���� �ʱ� ���� �Լ�
		��ǻ� ������ ������ ������ �ʱ�ȭ���� �������� ���ٸ� ���� ���� �����ش�.
	*/
	void					SetCheckOff( UIGUID ControlID );

	/**
		���� �����ʱ�ȭ �����쿡�� ������ üũ�� üũ������ ��������� �����Ѵ�.
	*/
	void					SetCheckedNum();

	/**
		������ üũ���� ����������� �޾ƿ´�.
	*/
	const WORD&				GetCheckedNum();

	/**
		�ʱ�ȭ �����찡 ������ �κ��丮 ������ Lock
		�ʱ�ȭ �����찡 ������ �κ��丮 ������ UnLock
	*/
	bool					SetInventoryLock();
	bool					SetInventoryUnLock();

	/**
		�����ʱ�ȭ �������� ��� �� '��' ��ư�� ���� ��� ���ɿ��θ� �˻��Ѵ�.
	*/
	bool					IsPossibleReset();

	/**
		�н��� �׸��� �ִ��� �˻��Ѵ�.
	*/
	bool					IsStudyProduct();

	virtual	void			SetVisibleSingle ( BOOL bVisible );

protected:	
	virtual	void			TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	CBasicTextBox*			m_pTextBoxTitle;							// �����ʱ�ȭ ������ Ÿ��Ʋ
	CBasicTextBox*			m_pTextBoxMessageTop;						// �����ʱ�ȭ ������ ��� �ؽ�Ʈ
	CBasicTextBox*			m_pTextBoxMessageBottom;					// �����ʱ�ȭ ������ �ϴ� �ؽ�Ʈ

	CBasicTextBox*			m_pTextBoxProductList[MAX_PRODUCT_RESET];	// ������ ������ �����׸� �ؽ�Ʈ �迭
	int						m_nProductEnum[MAX_PRODUCT_RESET];			// ������ ������ �����׸� enum �迭

	CCheckBox*				m_pCheckBoxProductList[MAX_PRODUCT_RESET];  // ������ ������ �����׸� üũ�ڽ� �迭

	RnButton*				m_pYes;										// ��		��ư��Ʈ�� ������
	RnButton*				m_pNo;										// �ƴϿ�	��ư��Ʈ�� ������

	WORD					m_wResetAble;								// �ʱ�ȭ�������� �ʱ�ȭ ������ ����
	WORD					m_wUserCheckedNum;							// �ʱ�ȭ������ ���� ������ �׸��� üũ�� ����

	WORD					m_wPosX;									// �ʱ�ȭ������ �κ��丮 x��ġ 
	WORD					m_wPosY;									// �ʱ�ȭ������ �κ��丮 y��ġ

	std::vector<int>		m_vecStudyProduct;							// ������ ������ �����׸�
	std::vector<int>		m_vecCheckedProduct;						// ������ ������ �׸��� ������ ����

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