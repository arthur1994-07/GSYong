
#include "stdafx.h"
#include "mrcSig.h"


#include "mrcSigBase.h"

#include "../enginelib/DxResponseMan.h"
#include "../enginelib/GUInterface/UIMan.h"
#include "../enginelib/Common/SUBPATH.h"
#include "../enginelib/GUInterface/UIRenderQueue.h"
#include "../enginelib/TextTexture/TextUtil.h"

#include "mrcLayer.h"
#include "mrcLayerMgr.h"
#include "mrcRenderer.h"




namespace mrcSig
{
	
//	static CUIMan s_uiMan;
	static DxResponseMan* s_dxResponseMan;
	static SUBPATH path(SP_OFFICE_TEST);
	static LPDIRECT3DDEVICEQ s_device = NULL;
	
	enum 
	{
		WND_INVALID,
		WND_TEST_1,
		WND_TEST_2,
		WND_TEST_3,
		WND_TEST_4,
	};


	class NumberGenerator
	{
	public:
		NumberGenerator()
		{
			mNumber = 0;
		}

		uint32 getNumber() { mrcASSERT( mNumber < UINT_MAX ); return mNumber++; }

		static NumberGenerator& getInstance() { static NumberGenerator ss; return ss; }
	private:
		uint32 mNumber;
	};

	class UIRenderQueue : public CUIRenderQueue
	{
	public:
		UIRenderQueue()	{}

		virtual VOID InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) {}
		virtual VOID RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice ) {}
		virtual VOID InvalidateDeviceObjects() {}
		virtual VOID DeleteDeviceObjects() {}

		virtual BOOL Render( LPDIRECT3DTEXTUREQ pTexture, UIVERTEX* pVertex )
		{
			mrcASSERT(pTexture);
			mMaterial.mTexture = pTexture;
			IRenderer::getInstance().setMaterial(mMaterial);
			
			Vertex vertex[4];
			for (int32 i=0; i < 4; ++i)
			{
				vertex[i].pos.x  = pVertex[i].x;
				vertex[i].pos.y  = pVertex[i].y;
				vertex[i].pos.z  = pVertex[i].z;
				vertex[i].uv.x   = pVertex[i].tu1;
				vertex[i].uv.y   = pVertex[i].tv1;
			}

			IRenderer::getInstance().draw(PRIMITIVE_QUAD,Matrix4::IDENTITY,vertex,4);
			return TRUE;
		}

		virtual VOID RenderImmediately() {}

	private:
		Material mMaterial;
	};



	static SP_Layer gTestBasicLayer;
	static SP_Layer gNormalLayer;
	static SP_Layer gBasicButton;

	void Init(void* device,HWND hWnd)
	{
		s_device = LPDIRECT3DDEVICEQ(device);
		UIBase::setDevice(s_device);

		CUIRenderQueue::SetRenderQueue( new UIRenderQueue );
		CUIRenderQueue::Get()->InitDeviceObjects(s_device);
		CUIRenderQueue::Get()->RestoreDeviceObjects(s_device);

		delete s_dxResponseMan;
		s_dxResponseMan = new DxResponseMan(SP_OFFICE_TEST,&path);	
		s_dxResponseMan->InitTest(hWnd,s_device);

//		s_uiMan.InitDeviceObjects(s_device); 
		DxFontMan::GetInstance().InitDeviceObjects(s_device);
		DxFontMan::GetInstance().RestoreDeviceObjects(s_device);

		CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		CD3DFontPar* pD3dFont8 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 8, D3DFONT_SHADOW|D3DFONT_ASCII );

		BasicTextBox* pTextBox = new BasicTextBox(s_dxResponseMan);
		pTextBox->create(WND_TEST_1,200,50,pD3dFont9,_mrcT("play test, 團辦噢 六六"));
//		s_uiMan.ShowGroupTop(WND_TEST_1);
	
		gTestBasicLayer = SP_Layer(pTextBox);
		LayerMgr::getInstance().insertChild(gTestBasicLayer);

// 		UIGroup* pNormal = new UIGroup(s_dxResponseMan);
// 		pNormal->create(WND_TEST_2);
// 		pNormal->setTexture( _mrcT("SuperHero"));
// 		gNormalLayer = SP_Layer(pNormal);
// 		LayerMgr::getInstance().insertChild(gNormalLayer);

		BasicButton* pBasicButton = new BasicButton(s_dxResponseMan);
		pBasicButton->create(WND_TEST_3,200,50,pD3dFont9,_mrcT("�挫�"));
		pBasicButton->setTexture( _mrcT("DefaultButton") );
		gBasicButton = SP_Layer(pBasicButton);
		LayerMgr::getInstance().insertChild(gBasicButton);
		
	}

	void Update(real elapsedTime)
	{
//		s_uiMan.FrameMove(elapsedTime);
		static bool init = false;
		if( init )
			return ;

		init = true;

		vector<SP_Layer> layerVec;
		LayerMgr::getInstance().getChildAll(&layerVec);
		LayerMgr::getInstance().moveChild( layerVec[3].get() , gTestBasicLayer.get() );
	}

	void Render(real elapsedTime)
	{
	}

	void Release()
	{
		gTestBasicLayer.reset();
		gNormalLayer.reset();
		gBasicButton.reset();

//		s_uiMan.FinalCleanup();
		CUIRenderQueue::CleanUp();
		CTextUtil::CleanUp();

		DxFontMan::GetInstance().DeleteDeviceObjects();
		DxFontMan::GetInstance().DeleteDxFontAll();

		delete s_dxResponseMan;
		s_dxResponseMan = NULL;
	}
}
