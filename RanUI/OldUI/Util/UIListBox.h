#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/UIControlMessage.h"

class CUIGroup;
class EngineDeviceMan;
class CBasicScrollBarEx;
class CBasicLineBoxSmart;

/**
	기본 컨트롤(CUIControl) 기반의 리스트 박스를 정의 한다.

    기본 컨트롤로 표시가능한 부분은 모두 리스트 박스화 할수 있다.
    예를들어 이미지(텍스처) 기반 리스트 박스를 만들 수 있다.

    리스트 박스에서 리스트를 선택 하게 되면 부모에 메시지가 전달 된다.

 */
class CUIListBox : public CUIGroup
{
public:
		CUIListBox(EngineDeviceMan* pEngineDevice);
virtual ~CUIListBox();

		typedef std::vector<CUIControl*>	ControlContainer;
		typedef ControlContainer::iterator	ControlContainerIterator;

        // 마우스에 의한 선택 변경 시 부모 통지 메시지
        static const DWORD  LB_SELCHANGED = 0x01000000;

		/**
			리스트 박스를 생성 한다.
			문자열들은 컨트롤의 형태를 정의하는 XML의 Control ID 이다.

			\param ctrl 리스트 박스 컨트롤 정의 XML
			\param outline 선택 영역 텍스처 정의 XML, 0 시 사용 안함
			\param scroll 스크롤 정의 XML, 0 시 사용 안함
			\param fLine 줄 여백
			\param fFront 앞 여백
			\param fBack  뒤 여백
			\return 생성 성공 유무
		 */
virtual bool				Create(const TCHAR* ctrl, const TCHAR* outline = 0, const TCHAR* scroll = 0, const float fLine = 0.0f, const float fFront = 0.0f, const float fBack = 0.0f);

		/**
			매 프레임 호출 되어 처리될 사항을 처리 한다.
			우리는 스크롤에서 이벤트가 발생하면 TranlateUIMessage 로 전달되지 않으므로
			Update 에서 스크롤 변경을 살피면서 변경시 이에 맞게 변경 사항을 처리 한다.

		 */
virtual void				Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

		/**
			리스트 아이템 관련 함수

		 */
virtual void				AddItem(const TCHAR* item, ...);
virtual void				SetItem(int nIndex, const TCHAR* item, ...);
virtual void				DelItem(int nIndex);
virtual void				ClearItem();

		/**
			선택 관련 함수
			현재 싱글 실렉션만 지원하지만 향후 멀티 실렉션도 지원하면 좋을 것이다.

		 */
virtual int					GetCurSel() const { return m_nSelectedLine; }
virtual int                 GetTotalLine() const { return m_nTotalLine; }
virtual void				SetCurSel(int nSelect);

		/**
			각 컨트롤 영역을 비주얼 적으로 보고 싶을때 true 를 설정 한다.
			디버깅용이나 픽쉘 맞추기 용으로 활용 한다.

		 */
		void				SetDebugLineBox(bool bDebug) { m_bDebug = bDebug; }
        
		/**
			해당 인덱스에 컨트롤의 visible 을 설정 한다.

		 */
		void 		        SetVisible(int nIndex, bool bVisible);
        bool                GetVisible(int nIndex);

protected:
		/**
			컨트롤을 만든다.
			중복 코드를 방지 한다.

			\param pControl 컨트롤 XML
			\return 성공시 컨트롤 포인트, 실패시 NULL
		 */
		CUIControl*			CreateControl(const TCHAR* pControl);

		/**
			컨트롤의 위치를 설정 한다.
			
		 */
		void				SetPosControl(CUIControl* pControl);

		/**
			인덱스에 해당하는 컨트롤을 얻는다.

		 */
		CUIControl* 		GetControl(int nIndex);

		/**
			컨트롤로 부터 영역을 추출 한다.
			월드 기반임에 주의 한다.

		 */
		const UIRECT		GetRectLineFromControl(int nLine);

		/**
			라인의 영역을 구한다.
			영역은 월드 기반 좌표임에 주의 한다.

		 */
		const UIRECT		GetRectLine(int nLine);
		const UIRECT		GetRectLine(int nLine, CUIControl* pControl);

		/**
			내부 컨트롤 생성 함수들

		 */
virtual bool				CreateCtrl(const TCHAR* ctrl);
virtual bool				CreateScroll(const TCHAR* scroll);
virtual bool				CreateLineBox(const TCHAR* outline);
		
		/**
			현재 마우스 위치의 선택된 아이템을 찾는다.

			\return 선택 영역 변경되었으므로 갱신해야 하는 유무
		 */
virtual bool				UpdateMousePos(int x, int y);

		/**
			아이템 삽입, 삭제 등 아이템 변경에 따른 스크롤썸을 변경 한다.

		 */
virtual void				UpdateScrollThumb();

		/**
			스크롤 되었는지 유무 판별

		 */
virtual bool				UpdateIsScrolled();

		/**
			스크롤 되었을 경우 처리 수행

		 */
virtual void				UpdateBeScrolled();

		/**
			선택 영역을 변경 한다.
			유저가 클릭에 의해 선택 라인이 변경되면 선택 영역을 변경 해 준다.

		 */
virtual void				UpdateSelected();

		/**
			아이템 변경(추가, 삭제)에 따른 보여지는 영역의 갱신을 처리 한다.

		 */
virtual void				UpdateVisibleLine();

		/**
			보여지는 첫번째 라인의 인덱스를 갱신 한다.

		 */
virtual void				UpdateVisibleLineFirstIdx();

		/**
			보여지는 라인들의 위치를 갱신 하고 모두 보여지게(visible) 처리 한다.

		 */
virtual void				UpdateVisibleLinePosWithVisible();

		/**
			현재 보이는 라인을 안보이게 or 보이게 한다.

		 */
virtual void				UpdateVisibleLineToVisible();
virtual void				UpdateVisibleLineToUnVisible();

		/**
			좌표 맞추는 작업의 편의를 위해 전체 영역, 텍스트 박스등의 영역을 표시 한다.
			이를 통해 좌표를 맞추거나 내부 오류를 수정시 사용 하도록 한다.

		 */
		void				CreateDebugLineBox(CUIControl* pControl);
#ifndef RELEASE
		bool				m_bDebug;
#endif

		/**
			컨트롤의 GlobalPos 위치를 갱신 한다.
			주의 할점은 GlobalPos 만 변경하게 되면 부모 기반 위치 갱신시에
			LocalPos 로 위치를 재계산하는데 이럴경우 실제 원하는 GlobalPos 가 안되므로
			부모 기반 LocalPos 도 같이 입력해 주어야 한다.

		 */
		void				SetControlGlobalPos(CUIControl* pCtrl, const D3DXVECTOR2& rtGlobalPos);
		void				SetControlGlobalRect(CUIControl* pCtrl, const UIRECT& rtGlobalRect);

		/**
			컨트롤

		 */
		ControlContainer	m_kControls;		//@ 컨트롤 포인터들
		TSTRING				m_strControl;		//@ 컨트롤 XML
		CBasicLineBoxSmart*	m_pSeleted;			//@ 선택 영역 컨트롤
		CBasicScrollBarEx*	m_pScrollBar;		//@ 스크롤 컨트롤


        /**
            컨트롤 visible 제어

         */
        typedef std::map<int, bool>	CtrlVisContainer;
        CtrlVisContainer    m_kVisible;          //@ 컨트롤들 visible 정보

		/**
			스크롤 관련

		 */	
		float 				m_fPercent;			//@ 스크롤 현재 퍼센티지

		/**
			선택 관련

		 */	
		int					m_nSelectedLine;	//@ 선택 된 라인 인덱스(없을시 -1)

		/**
			라인 관련

		 */
		int					m_nTotalLine;		//@ 모든 라인 수
		int					m_nVisibleLine;		//@ 보여지는 라인 수
		int					m_nVisibleFirstIdx;	//@ 보여지는 라인의 첫번째 라인 인덱스

		/**
			여백 정보

		 */
		float				m_fLineInterval;	//@ 선택 영역 라인 여백
		float				m_fFrontInterval;	//@ 선택 영역 앞 여백
		float				m_fBackInterval;	//@ 선택 영역 뒤 여백
};
