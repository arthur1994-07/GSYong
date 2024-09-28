//	BasicScrollBarEx 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.12.1]
//			@ 최초제작

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