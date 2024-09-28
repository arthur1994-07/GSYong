#ifndef EditState_H
#define EditState_H

#include "Param.h"
#include <boost/smart_ptr/shared_ptr.hpp>

struct	DxSkinPiece;
struct	DxCharPart;
class	DxSkinChar;
class	DxSkinAniControl;
class   DxAttBone;
class   DxVehicle;
class   DxAttBoneLink;
class   CAssetDlg;
class   CCharEditView;

/**
	캐릭터 에디터 기본 상태 처리기
	캐릭터 에디터에서 기본적으로 처리해야 하는 상태를 처리 한다.

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

		 */
virtual	void			                SetFocus(CParam& kParam) = 0;

		/**
			메인파일을 재로딩 해야 할 경우 호출 된다.

		 */
virtual void			                Reload(CParam& kParam) = 0;

		/**
			파일 갱신기를 모두 제거 해야 할 경우 호출 된다.

		 */
virtual void			                Clear(CParam& kParam);

		/**
			파일 갱신기를 삽입 해야 할 경우 호출 된다.

		 */
virtual void			                Insert(CParam& kParam) = 0;

		/**
			어셋들을 갱신 한다.

		 */
virtual void                            Asset(CParam& kParam) = 0;

        /**
            현재 사용되어 지는 파일들을 구조적으로 한곳으로 집결 시킨다.
            3ds 맥스의 Archive 와 유사하지만 zip 을 사용하지 않는다.

         */
virtual void                            Archive(CParam& kParam) = 0;

		/**
			파일명이 변경되었을 경우 호출 된다.
		 */
virtual void			                Change(CParam& kParam);

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
			파일 갱신기를 삽입 한다.

		 */
		void			                InsertTexture(DxSkinPiece* pSkinPiece, DxCharPart* pPart);
		void			                InsertMesh(DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice);
		void			                InsertMesh(DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DxSkinAniControl* pSkinAni, CCharEditView* pView);
		void			                InsertSkeleton(DxSkinPiece* pSkinPiece, LPDIRECT3DDEVICEQ pd3dDevice);
		void			                InsertSkeleton(DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DxSkinAniControl* pSkinAni, CCharEditView* pView);
		void			                InsertSkinAni(DxSkinAniControl* pSkinAniCtrl);
        void			                InsertLODData(DxSkinChar* pkSkinChar, DxSkinPiece* pSkinPiece = 0);
        void			                InsertLODData(DxAttBone* pkAttBone, DxSkinPiece* pSkinPiece = 0);
        void			                InsertLODData(DxVehicle* pkVehicle, DxSkinPiece* pSkinPiece = 0);        

		/**
			캐쉬로 부터 완전히 리소스를 삭제 한다.

		 */
		void			                RemovePiece(DxSkinChar* pSkinChar);
		void			                RemovePiece(DxSkinChar* pSkinChar, const TCHAR* pCpsFile);
		void			                RemovePiece(DxSkinPiece* pSkinPiece);
		void			                RemoveAni(DxSkinAniControl* pAniCtrl);
		void			                RemovePart(DxSkinPiece* pSkinPiece);
        void			                RemoveChar(DxSkinChar* pSkinChar);

        /**
            현재 뷰의 어셋들을 리스팅 한다.

         */
        void                            SkinPieceAsset(DxSkinPiece* pSkinPiece, CAssetDlg* pDlg, HTREEITEM hRoot, bool bBone = false);
        void                            SkinCharAsset(DxSkinChar* pSkinChar, CAssetDlg* pDlg, HTREEITEM hRoot);
        void                            AnimationAsset(DxSkinAniControl* pAniCtrl, CAssetDlg* pDlg, HTREEITEM hRoot);
        void                            AttBoneAsset(DxAttBone* pAttPiece, CAssetDlg* pDlg, HTREEITEM hRoot);
        void                            AttBoneLinkAsset(DxAttBoneLink* pAttLink, CAssetDlg* pDlg, HTREEITEM hRoot);
        void                            SkeletonAsset(const TCHAR* pSkeletonFile, CAssetDlg* pDlg, HTREEITEM hRoot);
        void                            VehicleAsset(DxVehicle* pVehicle, CAssetDlg* pDlg, HTREEITEM hRoot);

protected:
        std::tr1::shared_ptr<CEditState>	m_spNext;
};

#endif