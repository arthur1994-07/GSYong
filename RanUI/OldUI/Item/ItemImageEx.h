// Note : ItemImage Ȯ����
//		  ���� ǥ�� �߰�

#pragma	once

#include "../Item/ItemImage.h"

#include "../../../EngineLib/G-Logic/GLDefine.h"

class CBasicTextBox;

class CItemImageEx : public CItemImage
{
public:
	CItemImageEx(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CItemImageEx ();

public:
	virtual void CreateSubControl (const char* szControlKeywordImageSize = "ITEM_IMAGE");

public:
	virtual void	ResetItem ();
	virtual void	SetItemProcess( SNATIVEID sNativeID );
	virtual void	ReSetItemProcess();

public:
	void			SetTurnNum ( const WORD wTurnNum ); //��ħ �������� �ƴѰ�� �����־ ���ڰ� ǥ�õ��� ����

private:
	WORD			m_wTurnNum;
	
private:
	CBasicTextBox*	m_pNumberBox;
};