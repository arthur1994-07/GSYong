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
	월드 에디터 기본 상태 처리기
	월드 에디터에서 기본적으로 처리해야 하는 상태를 처리 한다.

 */
class CEditState
{
public:
		/**
			현재 상태는 다음 상태를 가진다.
			만약 다음 상태를 가지지 않는다면 0 으로 처리 한다.

		 */
		CEditState(CEditState* pNext) : m_spNext(pNext) {}
virtual ~CEditState();

		/**
			모든 상태를 파괴 한다.

		 */
virtual void			                Destroy();

		/**
			자신의 상태넘버를 출력 한다.
			이 상태 넘버를 통해 상태를 구분하므로 고유해야 한다.

		 */
virtual int				                GetState() = 0;

		/**
			자신이 포커싱이 되면 호출 된다.

			\param nOld 이전 포커싱 상태
		 */
virtual	void			                SetFocus(int nOld) = 0;

		/**
			메인파일을 재로딩 한다.

		 */
virtual void			                Reload(CParam& kParam) = 0;

		/**
			파일 갱신기를 삽입, 제거 한다.

		 */
virtual void			                Insert(CParam& kParam) = 0;
virtual void			                Clear(CParam& kParam);

		/**
			어셋들을 추가, 수정, 선택 한다.
            현재 Asset() 은 어셋들이 초기화 될 떄 사용 된다.

		 */
virtual void                            Asset(CParam& kParam) = 0;
virtual void                            EditAsset(CParam& kParam) = 0;
virtual void                            SelectAsset(CParam& kParam) = 0;


        /**
            현재 사용되어 지는 파일들을 구조적으로 한곳으로 집결 시킨다.
            3ds 맥스의 Archive 와 유사하지만 zip 을 사용하지 않는다.

         */
virtual void                            Archive(CParam& kParam) = 0;

		/**
			파일 갱신기를 업데이트 한다.

		 */
virtual void			                Update(CParam& kParam);

		/**
			상태 전이 리스트에 메시지를 전송 한다.

		 */
virtual void			                SendMessage(CParam& kParam);

protected:

		/**
			중첩된 갱신기가 있는지 체크 한다.

		 */
		bool			                IsDuplicatedNotifier(const TCHAR* pName);

 		/**
 			파일 갱신기를 삽입 한다.
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
			캐쉬된 리소스를 완전 제거 한다.

		 */
		void			                RemoveAllTexture();
		void			                RemoveAllPiece();

        /**
            어셋 뷰 처리
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
        	피스를 어셋창에 추가 한다.

            \param pPieceInfo 추가할 피스 정보
        	\param pAssetDlg 어셋창        
        	\param hParent 트리에 추가할 부모 노드
         */
        void                            InsertPiece(DxPieceManagerPSF::MATRIXPISNAME* pPieceInfo, CAssetDlg* pAssetDlg, HTREEITEM hParent);

protected:
        std::tr1::shared_ptr<CEditState>	m_spNext;
};

#endif