#ifndef CControlSizer_H
#define CControlSizer_H

/**
	다이얼로그 등에 붙은 컨트롤들의 크기를
	동적으로 조정하는 기능을 하는 클래스
	
	참고 : http://www.codeguru.com/cpp/w-d/dislog/resizabledialogs/article.php/c1917/FlickerFree-Resizing-Dialog.htm

 */
class CControlSizer
{
public:
		CControlSizer();

		/**
			변동 크기, 변동 위치 구현 해야 함

		 */
		enum eSizeType
		{
			ST_NONE,		//@ 고정 크기, 고정 위치
			ST_RESIZE,		//@ 변동 크기, 고정 위치
			ST_REPOS,		//@ 고정 크기, 변동 위치
			ST_RELATIVE,	//@ 넓이 높이의 비율에 따라 위치 이동, 고정 크기
		};

		struct CCtrlInfo
		{
			CWnd*			pCtrl;
			eSizeType		eWidthType;
			eSizeType		eHeightType;
			CRect			sRect;
			double			fWidthRation;
			double			fHeightRation;
		};

		void				Init(CWnd* pParent);
		bool				Insert(CWnd* pCtrl, eSizeType eWidthType = ST_RESIZE, eSizeType eHeightType = ST_RESIZE);
		void				OnSize(UINT nType, int cx, int cy);

protected:

		void				Calc(CRect& sParent, CCtrlInfo& kInfo);
		
		CWnd*				m_pParent;
		CRect				m_sParentRect;
		CArray<CCtrlInfo>	m_kCtrlInfoContainer;		
};

#endif