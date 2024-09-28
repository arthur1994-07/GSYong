#pragma once

#include "../../enginelib/G-Logic/TypeDefine.h"

class GenItemData
{
public:

public:

private:
	int nSpecID; // GenItem �׷� ID;

	SNATIVEID skillID; // �������� ��� �Ǳ� ���� �ʿ��� ��ų(�ɷ� �ִ� ��ų);

	SNATIVEID itemID; // ������ ID;
	std::string stringItemName; // ������ �̸�;	

	SNATIVEID crowID; // �� �������� ����ϴ� Crow�� ID;
	std::string stringCrowName; // Crow�� �̸�;

	std::string stringGenItemFileName; // GenItem �׷��� ���� ���� �̸�;

	int sizeGenNum; // GenItem �׷� ������ ��� �� ����;

	float rateSpec; // GenItem �׷� ������ nSpecID�� ���� �������� ��� �� Ȯ��;
	float rateGen; // GenItem���� ���� �������� �߻��� Ȯ��;
	float rateTotal; // �� �� Ȯ���� ��(nSpecID�� ���� �� ���� Ȯ��);

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