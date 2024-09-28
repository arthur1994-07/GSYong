#ifndef CCustomDrawTreeCtrl_H
#define CCustomDrawTreeCtrl_H

/**
	사용자 트리 컨트롤 클래스
	트리 컨트롤에 선택 시 배경을 그라를 먹혀 그릴 수 있다.

	참조 : http://www.codeproject.com/Articles/140828/A-Multi-selectable-Tree-Control-Supports-Transpare
	
 */
class CCustomDrawTreeCtrl
	: public CTreeCtrl
{
		DECLARE_DYNCREATE(CCustomDrawTreeCtrl)

public:
		CCustomDrawTreeCtrl();
virtual ~CCustomDrawTreeCtrl();

		enum eTreeCtrlStyle
		{
			TCST_FULLROW,		//@ 전체 라인 선택 스타일 
			TCST_RIGHTROW,		//@ 텍스트 이하 모든 것(즉 오른쪽면 전체) 선택 스타일
			TCST_TEXT,			//@ 텍스트만 선택 가능 스타일
		};


		// 생성 관련 함수 


		/**
			트리 컨트롤을 생성 한다.
			외부에서 트리 컨트롤을 Create 로 생성 할 수 도 있고
			서브 클래싱으로 (DDX_Control을 호출할때)으로 할 수도 있으므로
			OnCreate 와 PreSubclassWindow의 함수가 있다.

		 */
		int						OnCreate(LPCREATESTRUCT lpCreateStruct);
		void					PreSubclassWindow();

		/**
			트리 컨트롤을 초기화 한다.

		 */
		void					OnInitTreeCtrl();

		/**
			풀라인 실렉션이 가능한지 살핀다.
			TVS_HASLINES 과 TVS_FULLROWSELECT 은 겹치면 안 되므로 이를 살핀다.
			TVS_FULLROWSELECT 는 전체 라인을 배경으로 하는 방법이고
			TVS_HASLINES 는 말 그대로 트리 아이템에 라인을 그려주는 방법이다.

			이 2가지가 겹칠 수 없다. 당연하겠지만 전체 라인을 배경으로 그려주는데
			라인을 같이 그려버리면 이상해 질 것이다.

		 */
		bool					HasValidFullRowSelectStyle();

        /**
        	선택된 아이템을 새로 그린다.
            사이즈가 변경 될때(OnSize 호출시) 그려주는게 이상해서
            다시 호출해주는 용도로 사용 한다. 이상한 이유는 ;;; 잘 모름

        	\param bErase 배경을 다시 그릴지 선택
         */
        void                    RedrawSelectedItem(BOOL bErase = FALSE);


        /**
            입력되는 데이터를 가지는 아이템을 찾는다.

         */
        HTREEITEM               FindItemFromData(DWORD dwData);
        HTREEITEM               RecursiveFindItemFromData(HTREEITEM hNode, DWORD dwData);

protected:


		// 마우스 이벤트 함수 


		/**
			마우스 이동에 대한 처리를 한다.
			마우스 이동시 핫아이템을 그려주어야 한다. 이를 처리한다.
			핫 아이템은 클릭되지 않았지만 현재 마우스가 있는 영역의 아이템이다.

		 */
		void					OnMouseMove(UINT nFlags, CPoint point);

		/**
			마우스가 트리 컨트롤 영역을 벗어나면 호출 된다.
			현재 핫 아이템을 해제 하고 화면을 갱신하는 역할을 한다.
			
		 */
		LRESULT					OnMouseLeave(WPARAM wParam, LPARAM lParam);

		/**
			왼쪽 마우스가 업 될 때 Select 한다.
			다운 일 때 하면 사용자가 다운 후에 이동을 할 수 있으므로
			반드시 업일 때 한다.

			\param nFlags 
			\param point 마우스 업 위치
		 */
		void					OnLButtonUp(UINT nFlags, CPoint point);


		// 영역(Rect) 관련 함수


		/**
			아이템의 사각 영역을 계산 한다.
			우리는 eTreeCtrlStyle 에서 정의한 3가지 타입을 가진다. 
			이 3가지에 맞게 계산 한다.

			\param hItem 아이템 핸들
			\return 아이템 영역
		 */
		CRect					GetFillRect(HTREEITEM hItem);


		// Draw 관련 함수


		void					OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
		void					OnDrawTreeItem(CDC* pDC, HTREEITEM hItem, UINT nItemState);		
		CSize					OnDrawText(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState, bool bCalcOnly = false);
		CSize					DrawItemText(CDC* pDC, CRect rect, HTREEITEM hItem, LPCTSTR lpcszText, COLORREF clrTxtColor, bool bCalcOnly = false);		
		void					OnFillBackground(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState);


        /**
			아이템을 무효화(Invalidate) 시켜 새로 그리도록 한다.

			\param hItem 무효화할 아이템 핸들
			\param bErase 배경을 지울지 설정 한다.
		 */
		void					InvalidateItem(HTREEITEM hItem, BOOL bErase = TRUE);	

		/**
			전체 트리 컨트롤을 새로 그린다.
			처음 시작시나 타 윈도우에 가려졌거나 Min, Max 되었을 때 등에 호출 된다.

		 */
		void					OnPaint();

		/**
			배경을 지울 때 호출 된다.
			우리는 커스텀으로 배경도 모두 그려주므로 배경을 처리하지 않게 해야 한다.
			아니면 fliker(깜박임)만 생긴다.

			\return TRUE 일 경우 배경처리를 했으므로 할 필요 없다고 알린다.
				    FALSE 일 경우 배경처리를 못했으므로 해라고 알린다.
		 */
		BOOL					OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()

private:
		// 트리 컨트롤 스타일, 굳이 스타일이 왜 변수로 있느냐 하면
		// FULLROW 와 그렇지 않은 경우 2가지 밖에 없는데 이것 외에 우리는
		// TCST_RIGHTROW 이 모드가 있으므로 필요
		eTreeCtrlStyle			m_eTreeCtrlStyle;

		// 핫 아이템 관련 변수, 핫 아이템은 현재 마우스 위치의 아이템
		HTREEITEM				m_hHotTreeItem;
		// 마우스가 트리컨트롤을 벗어날 경우의 처리를 위해 트랙킹 한다.
		bool					m_bMouseEventsTracked;
};

#endif