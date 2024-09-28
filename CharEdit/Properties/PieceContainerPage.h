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
	조각을 담는 페이지를 정의 한다.
    현재 우리는 조각을 담는 페이지는 캐릭터, 어태치본, 비어클 이다.
    이러한 것들이 갖는 공통된 인터페이스를 정의 한다.

    몇몇 함수들은 기존 코드를 포팅하기 위해 억지로 만든 함수들이 있다.
    이를 고려해야 한다.
	
 */
class CPieceContainerPage
    : public CPropertyPage
{
public:
        CPieceContainerPage(UINT nIDTemplate);
virtual ~CPieceContainerPage();

        /**
        	Nc 가 파괴 될때 생성한 시트를 파괴 한다.

         */
virtual void				    PostNcDestroy();

        /**
        	활성화 되면 서브 시트들도 모두 활성화 시킨다.
	        
         */
virtual BOOL 				    OnSetActive();

        /**
        	조각을 생성 한다.

            \param nParts 파츠 인덱스
         */
virtual DxSkinPiece*            CreateSkinPiece(int nParts) = 0;

        /**
        	활성화할 조각의 파츠를 설정 한다.

	        \param nParts 파츠 인덱스
         */
virtual void                    SetSkinPiece(int nParts);

        /**
        	조각을 설정 한다.
            조각은 캐릭터, 어태치본 등 해당 컨테이너에 따라 처리가 다르므로
            이를 처리 하는 함수

	        \param pSkinPiece 설정할 조각
         */
virtual void                    SetSkinPiece(DxSkinPiece* pSkinPiece);

        /**
        	현재 활성화 되어 있는 스킨을 출력 한다.

            \param nParts 파츠 인덱스
         */
virtual DxSkinPiece*            GetSkinPiece(int nParts = -1);

        /**
        	조각의 타입을 설정한다.
            굳이 따로 함수를 두는 이유는 하나의 변수로 조각의 타입을 쓰는게 아니라
            따로 타입을 두기 때문에 부득이 하게 함수를 추가 했다.

	        \param pSkinPiece 타입을 설정할 조각
         */
virtual void                    SetSkinPieceType(DxSkinPiece* pSkinPiece);

        /**
        	설정된 조각을 제거 한다.
            제거함으로써 처리되어야 하는 로직들을 구현해 준다.

            \param nParts 제거할 조각, -1 일 경우 현재 활성화 된 조각
         */
virtual void                    SkinPieceClear(int nParts = -1);

        /**
        	현재 설정 된 조각의 개수를 출력 한다.

	        \return 설정 된 조각의 개수
        */
virtual int                     GetSkinPieceSize();

        /**
        	활성화된 파트를 출력 한다.

	        \return 활성화된 파트
         */
virtual DxCharPart*             GetCharPart();

        /**
        	현재 캐릭터와 동일한 본 파일을 사용하는지 체크 한다.

	        \param pName 본 파일명
	        \return 동일한지 유무
         */
virtual bool                    IsSameBoneFile(const TCHAR* pName);

        /**
        	현재 캐릭터에 파츠가 설정 되어 있는지 체크 한다.

	        \param pTempSkinPiece 체크할 임시 파츠
	        \return 이미 파츠가 있다면 true, 없다면 false
         */
virtual bool                    HasParts(DxSkinPiece* pTempSkinPiece);

        /**
            파츠를 새로 셋팅 한다.
            파츠를 새로 셋팅하는 이유는 피스의 이펙터, 메쉬 등이 변경되면
            이 정보를 파츠에 다시 셋팅해줘야 렌더링 등이 정상적으로 되기 떄문이다.
        	
         */
virtual void                    ReSetPart();

        /**
        	모든 파츠들을 초기화 한다.

         */
virtual void                    ReSetPages();

        /**
        	현재 파츠를 입력된 파츠 넘버(인덱스)로 교환 한다.

	        \param nParts 변경할 파츠 넘버 인덱스
         */
virtual void                    SwapSkinPiece(int nParts);

        /**
        	현재 파츠를 입력된 파츠 넘버(인덱스)로 교체 한다.
            교환과 교체를 잘 구분해야 한다. 교환은 A 를 B 와 서로 바꾸는 것이고
            교체는 A 를 B 로 바꾸는 것이다.  교체됨으로 해서 B는 사라진다.
            이를 주의 한다.

	        \param nParts 교체할 파츠 넘버(인덱스(
         */
virtual void                    ReplaceSkinPiece(int nParts);

        /**
        	몇몇 UI 를 숨긴다.
            만든 이유는 따른 이펙트와는 달리 EffVisualMaterialPage(비주얼 매터리얼)은
            페이지 전체를 사용하는데 이를 가능하게 하려면 몇몇 UI 는 숨겨줄 필요가 있다.

            \param bShow 보여줌 유무
         */
virtual void                    ShowWindow(bool bShow);

        /**
        	스핀컨트롤이 수정될 경우 호출 된다.
            현재 우리는 캐릭터의 반경(Radius) 와 높이(Height) 를 변경할 떄 사용 한다.
            또 오버레이 이펙터에서는 컬러높이값 변경에 사용 한다.

         */
virtual void                    SetSpinControl(int nRadius, int nHeight, int nColorHeight);

        /**
        	다이얼로그가 초기화 될때 호출 된다.
            하지만 우리는 현재 비주얼 메터리얼 파일이 외부에서 변경 되면
            호출되어 변경된 파일을 새로 적용 한다.

         */
virtual void                    ResetDialog();

        /**
        	키 처리를 수행 한다.
            프레임(FrameMove)이 처리될때 호출 되므로 이에 따라 처리해 줄 것을 처리 한다.

         */
virtual void                    UpdateKeyBoardMouse();

        /**
        	현재 컨테이너의 부모 시트를 입력 한다.
            이 함수는 조각을 없애는 포팅을 완료하면 삭제 할 것이다.

	        \param *pSheetTab 부모 시트
         */
        void					SetSheetTab( CsheetWithTabChar *pSheetTab )		{ m_pSheetTabParent = pSheetTab; }

        /**
        	피스컨테이너가 가지는 시트
            전부다 사용할수도 아니면 개별적으로 몇개만 사용할 수도 있다.
        	
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
