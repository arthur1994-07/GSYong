#ifndef CEditState_H
#define CEditState_H

#include "Param.h"
#include <boost/smart_ptr/shared_ptr.hpp>
#include "../enginelib/DxLand/DxLandDef.h"
#include "../enginelib/DxTools/TextureManager.h"
#include "../enginelib/DxLand/DxPiece/DxPieceManagerPSF.h"

struct	DxFrame;
struct  DxSkinPiece;
struct  SBGMDATA;
struct  FOG_PROPERTY;
struct  SKY_PROPERTY;
struct  DXLANDEFF;
struct  DXPIECEOBJ;
class	DxSetLandMan;
class	CsheetWithTab;
class	DxPieceManagerPSF;
class	DxPieceManagerWLD;
class	DxStaticMesh;
class   DxStaticAniFrame;
class   DxSkinChar;
class   DxSkinAniControl;
class   DxEffSingle;
class   DxEffectBase;
class   CStaticSoundMan;
class   CAssetDlg;
class   DxFrameMesh;
class   DxOctree;
class   DxLightMan;
class   DxWeatherMan;
class   DxCamAniMan;
class   CStaticSoundMan;
class   DxLandGateMan;
class   CCollisionMap;
class   WaterSurf;
class   DXLANDSKINOBJ;

/**
	���� ������ �⺻ ���� ó����
	���� �����Ϳ��� �⺻������ ó���ؾ� �ϴ� ���¸� ó�� �Ѵ�.

 */
class CEditState
{
public:
		/**
			���� ���´� ���� ���¸� ������.
			���� ���� ���¸� ������ �ʴ´ٸ� 0 ���� ó�� �Ѵ�.

		 */
		CEditState(CEditState* pNext) : m_spNext(pNext) {}
virtual ~CEditState();

		/**
			��� ���¸� �ı� �Ѵ�.

		 */
virtual void			                Destroy();

		/**
			�ڽ��� ���³ѹ��� ��� �Ѵ�.
			�� ���� �ѹ��� ���� ���¸� �����ϹǷ� �����ؾ� �Ѵ�.

		 */
virtual int				                GetState() = 0;

		/**
			�ڽ��� ��Ŀ���� �Ǹ� ȣ�� �ȴ�.

			\param nOld ���� ��Ŀ�� ����
		 */
virtual	void			                SetFocus(int nOld) = 0;

		/**
			���������� ��ε� �Ѵ�.

		 */
virtual void			                Reload(CParam& kParam) = 0;

		/**
			���� ���ű⸦ ����, ���� �Ѵ�.

		 */
virtual void			                Insert(CParam& kParam) = 0;
virtual void			                Clear(CParam& kParam);

		/**
			��µ��� �߰�, ����, ���� �Ѵ�.
            ���� Asset() �� ��µ��� �ʱ�ȭ �� �� ��� �ȴ�.

		 */
virtual void                            Asset(CParam& kParam) = 0;
virtual void                            EditAsset(CParam& kParam) = 0;
virtual void                            SelectAsset(CParam& kParam) = 0;


        /**
            ���� ���Ǿ� ���� ���ϵ��� ���������� �Ѱ����� ���� ��Ų��.
            3ds �ƽ��� Archive �� ���������� zip �� ������� �ʴ´�.

         */
virtual void                            Archive(CParam& kParam) = 0;

		/**
			���� ���ű⸦ ������Ʈ �Ѵ�.

		 */
virtual void			                Update(CParam& kParam);

		/**
			���� ���� ����Ʈ�� �޽����� ���� �Ѵ�.

		 */
virtual void			                SendMessage(CParam& kParam);

protected:

		/**
			��ø�� ���űⰡ �ִ��� üũ �Ѵ�.

		 */
		bool			                IsDuplicatedNotifier(const TCHAR* pName);

 		/**
 			���� ���ű⸦ ���� �Ѵ�.
 		 */
  		void			                InsertFrameMeshX(DxSetLandMan* pSetLandMan, CsheetWithTab* pSheetWithTab);
		void			                InsertMeshTexture(DxFrame* pFrameRoot);
		void			                InsertTexture(const TCHAR* pFile, TextureManager::Resource* pRes);
		void			                InsertPieceTexture(MAPSINGLETEXMESH& kMesh);
		void			                InsertPieceTexture(DxStaticAniFrame* pStaticAniFrame);
		void			                InsertReplacePiece(DxPieceManagerPSF* pPieceMng);
		void			                InsertPiece(DxPieceManagerPSF* pPieceMng);
		void			                InsertPiece(DxPieceManagerWLD* pPieceMng);

		/**
			ĳ���� ���ҽ��� ���� ���� �Ѵ�.

		 */
		void			                RemoveAllTexture();
		void			                RemoveAllPiece();

        /**
            ��� �� ó��
         */
        void                            InsertFrame(DxFrame* pFrame, CAssetDlg* pAssetDlg, HTREEITEM hParent);
        void                            InsertPieceTexture(MAPSINGLETEXMESH& kMesh, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InsertPieceTexture(DxStaticAniFrame* pStaticAniFrame, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InsertReplacePiece(DxPieceManagerPSF* pPieceMng, CAssetDlg* pAssetDlg, HTREEITEM hNode);
		void                            InsertPiece(DxPieceManagerPSF* pPieceMng, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InsertLight(DxLightMan* pLightMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserWeather(DxWeatherMan* pWeatherMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserCamera(DxCamAniMan* pCameraMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserEffect(DXLANDEFF* pEffectList, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserSound(CStaticSoundMan* pSoundMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserSkinObj(DXLANDSKINOBJ* pSkinObj, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserLandGate(DxLandGateMan* pLandGateMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserBGM(SBGMDATA* pBGMMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserFog(FOG_PROPERTY* pFogMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserSkyBox(SKY_PROPERTY* pSkyBoxMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserCollision(CCollisionMap* pCollMan, CAssetDlg* pAssetDlg, HTREEITEM hNode);
        void                            InserWater(WaterSurf* pWaterSurf, CAssetDlg* pAssetDlg, HTREEITEM hNode);

        /**
        	�ǽ��� ���â�� �߰� �Ѵ�.

            \param pPieceInfo �߰��� �ǽ� ����
        	\param pAssetDlg ���â        
        	\param hParent Ʈ���� �߰��� �θ� ���
         */
        void                            InsertPiece(DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo, CAssetDlg* pAssetDlg, HTREEITEM hParent);

protected:
        std::tr1::shared_ptr<CEditState>	m_spNext;
};

#endif