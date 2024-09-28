#ifndef CCustomDrawTreeCtrl_H
#define CCustomDrawTreeCtrl_H

/**
	����� Ʈ�� ��Ʈ�� Ŭ����
	Ʈ�� ��Ʈ�ѿ� ���� �� ����� �׶� ���� �׸� �� �ִ�.

	���� : http://www.codeproject.com/Articles/140828/A-Multi-selectable-Tree-Control-Supports-Transpare
	
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
			TCST_FULLROW,		//@ ��ü ���� ���� ��Ÿ�� 
			TCST_RIGHTROW,		//@ �ؽ�Ʈ ���� ��� ��(�� �����ʸ� ��ü) ���� ��Ÿ��
			TCST_TEXT,			//@ �ؽ�Ʈ�� ���� ���� ��Ÿ��
		};


		// ���� ���� �Լ� 


		/**
			Ʈ�� ��Ʈ���� ���� �Ѵ�.
			�ܺο��� Ʈ�� ��Ʈ���� Create �� ���� �� �� �� �ְ�
			���� Ŭ�������� (DDX_Control�� ȣ���Ҷ�)���� �� ���� �����Ƿ�
			OnCreate �� PreSubclassWindow�� �Լ��� �ִ�.

		 */
		int						OnCreate(LPCREATESTRUCT lpCreateStruct);
		void					PreSubclassWindow();

		/**
			Ʈ�� ��Ʈ���� �ʱ�ȭ �Ѵ�.

		 */
		void					OnInitTreeCtrl();

		/**
			Ǯ���� �Ƿ����� �������� ���ɴ�.
			TVS_HASLINES �� TVS_FULLROWSELECT �� ��ġ�� �� �ǹǷ� �̸� ���ɴ�.
			TVS_FULLROWSELECT �� ��ü ������ ������� �ϴ� ����̰�
			TVS_HASLINES �� �� �״�� Ʈ�� �����ۿ� ������ �׷��ִ� ����̴�.

			�� 2������ ��ĥ �� ����. �翬�ϰ����� ��ü ������ ������� �׷��ִµ�
			������ ���� �׷������� �̻��� �� ���̴�.

		 */
		bool					HasValidFullRowSelectStyle();

        /**
        	���õ� �������� ���� �׸���.
            ����� ���� �ɶ�(OnSize ȣ���) �׷��ִ°� �̻��ؼ�
            �ٽ� ȣ�����ִ� �뵵�� ��� �Ѵ�. �̻��� ������ ;;; �� ��

        	\param bErase ����� �ٽ� �׸��� ����
         */
        void                    RedrawSelectedItem(BOOL bErase = FALSE);


        /**
            �ԷµǴ� �����͸� ������ �������� ã�´�.

         */
        HTREEITEM               FindItemFromData(DWORD dwData);
        HTREEITEM               RecursiveFindItemFromData(HTREEITEM hNode, DWORD dwData);

protected:


		// ���콺 �̺�Ʈ �Լ� 


		/**
			���콺 �̵��� ���� ó���� �Ѵ�.
			���콺 �̵��� �־������� �׷��־�� �Ѵ�. �̸� ó���Ѵ�.
			�� �������� Ŭ������ �ʾ����� ���� ���콺�� �ִ� ������ �������̴�.

		 */
		void					OnMouseMove(UINT nFlags, CPoint point);

		/**
			���콺�� Ʈ�� ��Ʈ�� ������ ����� ȣ�� �ȴ�.
			���� �� �������� ���� �ϰ� ȭ���� �����ϴ� ������ �Ѵ�.
			
		 */
		LRESULT					OnMouseLeave(WPARAM wParam, LPARAM lParam);

		/**
			���� ���콺�� �� �� �� Select �Ѵ�.
			�ٿ� �� �� �ϸ� ����ڰ� �ٿ� �Ŀ� �̵��� �� �� �����Ƿ�
			�ݵ�� ���� �� �Ѵ�.

			\param nFlags 
			\param point ���콺 �� ��ġ
		 */
		void					OnLButtonUp(UINT nFlags, CPoint point);


		// ����(Rect) ���� �Լ�


		/**
			�������� �簢 ������ ��� �Ѵ�.
			�츮�� eTreeCtrlStyle ���� ������ 3���� Ÿ���� ������. 
			�� 3������ �°� ��� �Ѵ�.

			\param hItem ������ �ڵ�
			\return ������ ����
		 */
		CRect					GetFillRect(HTREEITEM hItem);


		// Draw ���� �Լ�


		void					OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
		void					OnDrawTreeItem(CDC* pDC, HTREEITEM hItem, UINT nItemState);		
		CSize					OnDrawText(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState, bool bCalcOnly = false);
		CSize					DrawItemText(CDC* pDC, CRect rect, HTREEITEM hItem, LPCTSTR lpcszText, COLORREF clrTxtColor, bool bCalcOnly = false);		
		void					OnFillBackground(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState);


        /**
			�������� ��ȿȭ(Invalidate) ���� ���� �׸����� �Ѵ�.

			\param hItem ��ȿȭ�� ������ �ڵ�
			\param bErase ����� ������ ���� �Ѵ�.
		 */
		void					InvalidateItem(HTREEITEM hItem, BOOL bErase = TRUE);	

		/**
			��ü Ʈ�� ��Ʈ���� ���� �׸���.
			ó�� ���۽ó� Ÿ �����쿡 �������ų� Min, Max �Ǿ��� �� � ȣ�� �ȴ�.

		 */
		void					OnPaint();

		/**
			����� ���� �� ȣ�� �ȴ�.
			�츮�� Ŀ�������� ��浵 ��� �׷��ֹǷ� ����� ó������ �ʰ� �ؾ� �Ѵ�.
			�ƴϸ� fliker(������)�� �����.

			\return TRUE �� ��� ���ó���� �����Ƿ� �� �ʿ� ���ٰ� �˸���.
				    FALSE �� ��� ���ó���� �������Ƿ� �ض�� �˸���.
		 */
		BOOL					OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()

private:
		// Ʈ�� ��Ʈ�� ��Ÿ��, ���� ��Ÿ���� �� ������ �ִ��� �ϸ�
		// FULLROW �� �׷��� ���� ��� 2���� �ۿ� ���µ� �̰� �ܿ� �츮��
		// TCST_RIGHTROW �� ��尡 �����Ƿ� �ʿ�
		eTreeCtrlStyle			m_eTreeCtrlStyle;

		// �� ������ ���� ����, �� �������� ���� ���콺 ��ġ�� ������
		HTREEITEM				m_hHotTreeItem;
		// ���콺�� Ʈ����Ʈ���� ��� ����� ó���� ���� Ʈ��ŷ �Ѵ�.
		bool					m_bMouseEventsTracked;
};

#endif