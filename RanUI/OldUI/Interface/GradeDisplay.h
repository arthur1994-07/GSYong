//	'ESC' Ű �������� ������ �޴�
//
//	���� �ۼ��� : ���⿱
//	���� ������ :
//	�α�
//		[2003.12.4]
//			@ ���� �ۼ�

#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CGradeDisplay : public CUIGroup
{
public:
    CGradeDisplay(EngineDeviceMan* pEngineDevice);
	virtual	~CGradeDisplay ();

public:
	void	CreateSubControl ();	
};