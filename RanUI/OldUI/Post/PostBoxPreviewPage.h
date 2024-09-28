
#pragma	once

#include "PostBoxReadPage.h"

#include "../../../RanLogicClient/Post/GLPostClient.h"

class CInnerInterface;

// Note : CPostBoxReadPage를 상속해서 만듬
class CPostBoxPreviewPage : public CPostBoxReadPage
{

protected:
	enum
	{
		POSTBOX_PAGE_PREVIEW_NONE = POSTBOX_PAGE_READ_NEXT,
	};


public:
	CPostBoxPreviewPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxPreviewPage();

protected:
	void		  DisableSubControl ();

protected:
	virtual void  CreateSubControl ();
	virtual void  OpenSubControl   ();
	virtual void  CloseSubControl  ();

public:
	virtual void  UpdateVariableControl	();
	virtual void  UpdateVariableText	();

public:
	virtual void  SetPost ( SAPOSTCLIENT* pPost );

public:
	SAPOSTCLIENT  m_PreviewPost;

};