// Note : ItemImage 확장판
//		  수량 표시 추가

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
	void			SetTurnNum ( const WORD wTurnNum ); //겹침 아이템이 아닌경우 값을넣어도 숫자가 표시되지 않음

private:
	WORD			m_wTurnNum;
	
private:
	CBasicTextBox*	m_pNumberBox;
};