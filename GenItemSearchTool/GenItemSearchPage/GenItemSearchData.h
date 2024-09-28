#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

class GenItemData
{
public:

public:

private:
	int nSpecID; // GenItem 그룹 ID;

	SNATIVEID skillID; // 아이템이 드랖 되기 위해 필요한 스킬(걸려 있는 스킬);

	SNATIVEID itemID; // 아이템 ID;
	std::string stringItemName; // 아이템 이름;	

	SNATIVEID crowID; // 이 아이템을 드랖하는 Crow의 ID;
	std::string stringCrowName; // Crow의 이름;

	std::string stringGenItemFileName; // GenItem 그룹을 가진 파일 이름;

	int sizeGenNum; // GenItem 그룹 내에서 드랖 될 수량;

	float rateSpec; // GenItem 그룹 내에서 nSpecID에 속한 아이템이 드랖 될 확률;
	float rateGen; // GenItem으로 인해 아이템이 발생할 확률;
	float rateTotal; // 위 두 확률의 곱(nSpecID가 선택 될 최종 확률);

};

class CGenItemData : public CDialog
{
	DECLARE_DYNAMIC(CGenItemData)
public:

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog(void) override;

protected:
};