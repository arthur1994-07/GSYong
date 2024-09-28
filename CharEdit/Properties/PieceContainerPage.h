#pragma once

#include "afxwin.h"
#include <boost/tr1/memory.hpp>
#include "../EngineLib/DxMeshs/SkinMesh/DxPieceDefine.h"
#include "../resource.h"

struct DxSkinPiece;
struct DxCharPart;
class CsheetWithTabChar;
class CsheetWithTabFrame;
class CsheetWithTabAnim;
class CsheetWithTabBone;
class CsheetWithTabPiece;
class CsheetWithTabBoardEdit;

typedef std::tr1::shared_ptr<DxSkinPiece> SPSkinPiece;

/**
	������ ��� �������� ���� �Ѵ�.
    ���� �츮�� ������ ��� �������� ĳ����, ����ġ��, ���Ŭ �̴�.
    �̷��� �͵��� ���� ����� �������̽��� ���� �Ѵ�.

    ��� �Լ����� ���� �ڵ带 �����ϱ� ���� ������ ���� �Լ����� �ִ�.
    �̸� ����ؾ� �Ѵ�.
	
 */
class CPieceContainerPage
    : public CPropertyPage
{
public:
        CPieceContainerPage(UINT nIDTemplate);
virtual ~CPieceContainerPage();

        /**
        	Nc �� �ı� �ɶ� ������ ��Ʈ�� �ı� �Ѵ�.

         */
virtual void				    PostNcDestroy();

        /**
        	Ȱ��ȭ �Ǹ� ���� ��Ʈ�鵵 ��� Ȱ��ȭ ��Ų��.
	        
         */
virtual BOOL 				    OnSetActive();

        /**
        	������ ���� �Ѵ�.

            \param nParts ���� �ε���
         */
virtual DxSkinPiece*            CreateSkinPiece(int nParts) = 0;

        /**
        	Ȱ��ȭ�� ������ ������ ���� �Ѵ�.

	        \param nParts ���� �ε���
         */
virtual void                    SetSkinPiece(int nParts);

        /**
        	������ ���� �Ѵ�.
            ������ ĳ����, ����ġ�� �� �ش� �����̳ʿ� ���� ó���� �ٸ��Ƿ�
            �̸� ó�� �ϴ� �Լ�

	        \param pSkinPiece ������ ����
         */
virtual void                    SetSkinPiece(DxSkinPiece* pSkinPiece);

        /**
        	���� Ȱ��ȭ �Ǿ� �ִ� ��Ų�� ��� �Ѵ�.

            \param nParts ���� �ε���
         */
virtual DxSkinPiece*            GetSkinPiece(int nParts = -1);

        /**
        	������ Ÿ���� �����Ѵ�.
            ���� ���� �Լ��� �δ� ������ �ϳ��� ������ ������ Ÿ���� ���°� �ƴ϶�
            ���� Ÿ���� �α� ������ �ε��� �ϰ� �Լ��� �߰� �ߴ�.

	        \param pSkinPiece Ÿ���� ������ ����
         */
virtual void                    SetSkinPieceType(DxSkinPiece* pSkinPiece);

        /**
        	������ ������ ���� �Ѵ�.
            ���������ν� ó���Ǿ�� �ϴ� �������� ������ �ش�.

            \param nParts ������ ����, -1 �� ��� ���� Ȱ��ȭ �� ����
         */
virtual void                    SkinPieceClear(int nParts = -1);

        /**
        	���� ���� �� ������ ������ ��� �Ѵ�.

	        \return ���� �� ������ ����
        */
virtual int                     GetSkinPieceSize();

        /**
        	Ȱ��ȭ�� ��Ʈ�� ��� �Ѵ�.

	        \return Ȱ��ȭ�� ��Ʈ
         */
virtual DxCharPart*             GetCharPart();

        /**
        	���� ĳ���Ϳ� ������ �� ������ ����ϴ��� üũ �Ѵ�.

	        \param pName �� ���ϸ�
	        \return �������� ����
         */
virtual bool                    IsSameBoneFile(const TCHAR* pName);

        /**
        	���� ĳ���Ϳ� ������ ���� �Ǿ� �ִ��� üũ �Ѵ�.

	        \param pTempSkinPiece üũ�� �ӽ� ����
	        \return �̹� ������ �ִٸ� true, ���ٸ� false
         */
virtual bool                    HasParts(DxSkinPiece* pTempSkinPiece);

        /**
            ������ ���� ���� �Ѵ�.
            ������ ���� �����ϴ� ������ �ǽ��� ������, �޽� ���� ����Ǹ�
            �� ������ ������ �ٽ� ��������� ������ ���� ���������� �Ǳ� �����̴�.
        	
         */
virtual void                    ReSetPart();

        /**
        	��� �������� �ʱ�ȭ �Ѵ�.

         */
virtual void                    ReSetPages();

        /**
        	���� ������ �Էµ� ���� �ѹ�(�ε���)�� ��ȯ �Ѵ�.

	        \param nParts ������ ���� �ѹ� �ε���
         */
virtual void                    SwapSkinPiece(int nParts);

        /**
        	���� ������ �Էµ� ���� �ѹ�(�ε���)�� ��ü �Ѵ�.
            ��ȯ�� ��ü�� �� �����ؾ� �Ѵ�. ��ȯ�� A �� B �� ���� �ٲٴ� ���̰�
            ��ü�� A �� B �� �ٲٴ� ���̴�.  ��ü������ �ؼ� B�� �������.
            �̸� ���� �Ѵ�.

	        \param nParts ��ü�� ���� �ѹ�(�ε���(
         */
virtual void                    ReplaceSkinPiece(int nParts);

        /**
        	��� UI �� �����.
            ���� ������ ���� ����Ʈ�ʹ� �޸� EffVisualMaterialPage(���־� ���͸���)��
            ������ ��ü�� ����ϴµ� �̸� �����ϰ� �Ϸ��� ��� UI �� ������ �ʿ䰡 �ִ�.

            \param bShow ������ ����
         */
virtual void                    ShowWindow(bool bShow);

        /**
        	������Ʈ���� ������ ��� ȣ�� �ȴ�.
            ���� �츮�� ĳ������ �ݰ�(Radius) �� ����(Height) �� ������ �� ��� �Ѵ�.
            �� �������� �����Ϳ����� �÷����̰� ���濡 ��� �Ѵ�.

         */
virtual void                    SetSpinControl(int nRadius, int nHeight, int nColorHeight);

        /**
        	���̾�αװ� �ʱ�ȭ �ɶ� ȣ�� �ȴ�.
            ������ �츮�� ���� ���־� ���͸��� ������ �ܺο��� ���� �Ǹ�
            ȣ��Ǿ� ����� ������ ���� ���� �Ѵ�.

         */
virtual void                    ResetDialog();

        /**
        	Ű ó���� ���� �Ѵ�.
            ������(FrameMove)�� ó���ɶ� ȣ�� �ǹǷ� �̿� ���� ó���� �� ���� ó�� �Ѵ�.

         */
virtual void                    UpdateKeyBoardMouse();

        /**
        	���� �����̳��� �θ� ��Ʈ�� �Է� �Ѵ�.
            �� �Լ��� ������ ���ִ� ������ �Ϸ��ϸ� ���� �� ���̴�.

	        \param *pSheetTab �θ� ��Ʈ
         */
        void					SetSheetTab( CsheetWithTabChar *pSheetTab )		{ m_pSheetTabParent = pSheetTab; }

        /**
        	�ǽ������̳ʰ� ������ ��Ʈ
            ���δ� ����Ҽ��� �ƴϸ� ���������� ��� ����� ���� �ִ�.
        	
         */
        CsheetWithTabBone*	    GetBoneTab ()									{ return m_psheetWithTabBone; }
        CsheetWithTabFrame*	    GetFrameTab ()									{ return m_psheetWithTabFrame; }
        CsheetWithTabAnim*	    GetAnimationTab ()								{ return m_psheetWithTabAnim; }
        CsheetWithTabPiece*	    GetPieceTab ()								    { return m_psheetWithTabPiece; }
        CsheetWithTabBoardEdit*	GetBoardEdit ()									{ return m_psheetWithTabBoardEdit; }

protected:

        int                     m_nParts;
        DxSkinPiece**           m_ppSkinPieces;
        CsheetWithTabChar*		m_pSheetTabParent;
        CsheetWithTabFrame*     m_psheetWithTabFrame;
        CsheetWithTabAnim*      m_psheetWithTabAnim;
        CsheetWithTabBone*      m_psheetWithTabBone;
        CsheetWithTabPiece*     m_psheetWithTabPiece;
        CsheetWithTabBoardEdit* m_psheetWithTabBoardEdit;
};
