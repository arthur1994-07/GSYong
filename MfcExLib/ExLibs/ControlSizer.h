#ifndef CControlSizer_H
#define CControlSizer_H

/**
	���̾�α� � ���� ��Ʈ�ѵ��� ũ�⸦
	�������� �����ϴ� ����� �ϴ� Ŭ����
	
	���� : http://www.codeguru.com/cpp/w-d/dislog/resizabledialogs/article.php/c1917/FlickerFree-Resizing-Dialog.htm

 */
class CControlSizer
{
public:
		CControlSizer();

		/**
			���� ũ��, ���� ��ġ ���� �ؾ� ��

		 */
		enum eSizeType
		{
			ST_NONE,		//@ ���� ũ��, ���� ��ġ
			ST_RESIZE,		//@ ���� ũ��, ���� ��ġ
			ST_REPOS,		//@ ���� ũ��, ���� ��ġ
			ST_RELATIVE,	//@ ���� ������ ������ ���� ��ġ �̵�, ���� ũ��
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