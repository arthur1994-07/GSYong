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
	ĳ���� ������ �⺻ ���� ó����
	ĳ���� �����Ϳ��� �⺻������ ó���ؾ� �ϴ� ���¸� ó�� �Ѵ�.

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

		 */
virtual	void			                SetFocus(CParam& kParam) = 0;

		/**
			���������� ��ε� �ؾ� �� ��� ȣ�� �ȴ�.

		 */
virtual void			                Reload(CParam& kParam) = 0;

		/**
			���� ���ű⸦ ��� ���� �ؾ� �� ��� ȣ�� �ȴ�.

		 */
virtual void			                Clear(CParam& kParam);

		/**
			���� ���ű⸦ ���� �ؾ� �� ��� ȣ�� �ȴ�.

		 */
virtual void			                Insert(CParam& kParam) = 0;

		/**
			��µ��� ���� �Ѵ�.

		 */
virtual void                            Asset(CParam& kParam) = 0;

        /**
            ���� ���Ǿ� ���� ���ϵ��� ���������� �Ѱ����� ���� ��Ų��.
            3ds �ƽ��� Archive �� ���������� zip �� ������� �ʴ´�.

         */
virtual void                            Archive(CParam& kParam) = 0;

		/**
			���ϸ��� ����Ǿ��� ��� ȣ�� �ȴ�.
		 */
virtual void			                Change(CParam& kParam);

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
			���� ���ű⸦ ���� �Ѵ�.

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
			ĳ���� ���� ������ ���ҽ��� ���� �Ѵ�.

		 */
		void			                RemovePiece(DxSkinChar* pSkinChar);
		void			                RemovePiece(DxSkinChar* pSkinChar, const TCHAR* pCpsFile);
		void			                RemovePiece(DxSkinPiece* pSkinPiece);
		void			                RemoveAni(DxSkinAniControl* pAniCtrl);
		void			                RemovePart(DxSkinPiece* pSkinPiece);
        void			                RemoveChar(DxSkinChar* pSkinChar);

        /**
            ���� ���� ��µ��� ������ �Ѵ�.

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