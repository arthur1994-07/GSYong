//	BasicScrollBarEx Ŭ����
//
//	���� �ۼ��� : ���⿱
//	���� ������ :
//	�α�
//		[2003.12.1]
//			@ ��������

#pragma	once

#include "../EngineLib/GUInterface/BasicTree.h"

class	CBasicTreeEx : public CBasicTree
{
public:
	CBasicTreeEx (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicTreeEx ();

public:
 //   void	CreateBaseTree ( char* szBaseTree );

public:
	CBasicTreeNode*	InsertNode ( CBasicTreeNode* pParent, const CString& strNodeName );

private:
	UIGUID	m_NodeWndID;
};