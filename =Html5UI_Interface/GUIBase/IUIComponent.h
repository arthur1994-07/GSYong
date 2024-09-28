#pragma once

#include "UIDataType.h"
#include "UIPosData.h"
#include "IUIEventable.h"
#include "IView.h"

#include <hash_map>
#include <list>

namespace GUIBase
{

class IUIFactory;
struct WINDOW_CFG;

/*------------------------------
	UI의 Component 클래스;
------------------------------*/
class IUIComponent : public IUIEventable, public IView
{
public:
	typedef std::list< IUIComponent* >				LIST;
	typedef	LIST::iterator							LIST_ITER;
	typedef	LIST::const_iterator					LIST_CITER;
	typedef	LIST::reverse_iterator					LIST_RITER;

private:
	typedef stdext::hash_map< UIID, LIST_ITER >		MAP;	
	typedef MAP::iterator							MAP_ITER;
	typedef MAP::const_iterator						MAP_CITER;

public:
	IUIComponent( UIID uiID				// Component의 ID;
		, const std::string& uiStrID	// Component의 StrID;
		, UIRECT rcParentGlobalPos		// 부모 윈도우 위치;
		, UIRECT rcLocalPos				// Component의 Local Pos;
		, IUIComponent* pParent );		// 부모;
	virtual ~IUIComponent();

public:
	/// Component 특성에 맞는 추가 구현 부분;
	virtual void Initialize() { }
	virtual void Destroy() { }
	virtual bool FrameMove( float timeDelta ) { return true; }
	virtual bool Render( float timeDelta );

public:
	/// 기본적으로 이루어져야 하는 부분;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Event를 받았을때의 기본처리;
	virtual bool OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent );
	virtual bool OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent );
	virtual bool OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent );
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent );

public:
	/// Virtual 함수;
	virtual void ShowComponent();
	virtual void HideComponent();

public:
	/// Member 함수;
	// 위치 이동;
	void MoveComponentPoint( UIPOINT<int> point );

public:
	/// 자식 Component 관리 함수;
	bool IsEmpty();
	unsigned int Size();
	void RemoveAll( bool bDelete = true );
	bool Insert( const std::string& strID
		, WINDOW_CFG& windowCFG
		, UIRECT rcGlobalPos = UIRECT( 0, 0, 0, 0 ) );
	bool Erase( UIID uiID, bool bDelete = true );
	IUIComponent* At( UIID uiID );
	UIID LoadUIIDUsingStrID( const std::string& strID );
	void ChangeUIFactory( IUIFactory* pUIFactory );

public:
	/// Inline 함수;
	// 위치 정보 관리;
	inline COMPONENT_POSITION GetComponentPosition() { return m_position; }
	virtual UIRECT GetPosition() { return m_position.rcLocalPos; }
	inline UIRECT GetGlobalPosition() { return m_position.rcGlobalPos; }
	
	// ID 관리;
	inline const std::string& GetTextureID() const { return m_textureID; }
	inline void SetTextureID( const std::string& textureID ) { m_textureID = textureID; }
	inline const std::string& GetStrID() const { return m_uiStrID; }
	inline void SetStrID( const std::string& strID ) { m_uiStrID = strID; }
	inline UIID GetUIID() const { return m_uiID; }
	inline void SetUIID( UIID uiID ) { m_uiID = uiID; }

	// Show State
	inline bool ShowState() { return m_bShow; }

	// 부모 컴포넌트;
	inline IUIComponent* GetUIParent() const { return m_pMyParent; }
	inline void SetUIParent( IUIComponent* pParent ) { m_pMyParent = pParent; }

protected:
	/// Component
	/*------------------------------------------------------------------- 
		# Component에는 ComponentManager가 따로 없는 이유;
		Component의 자식 Component들의 ID는;
		그 부모 Component안에서만 Unique하면 된다.;
		반면 Window의 ID는 어디에서든 Unique해야 한다.;
		그래서 Window는 WindowManager가 따로 관리하지만;
		Component는 Component 스스로가 자식들을 관리한다;
	-------------------------------------------------------------------*/
	IUIComponent* m_pMyParent;			// 부모 Component;

	// 자식 Components;
	MAP			m_map;
	LIST		m_list;
	int			m_autoMakeID;
	IUIFactory* m_pUIFactory;

	/// ID
	UIID m_uiID;						// Component ID
	std::string m_uiStrID;				// Component StrID
	std::string m_textureID;			// Component Texture ID

	/// 위치;
	COMPONENT_POSITION m_position;		// UI Position
	UIRECT m_parentGlobalPos;			// Parent Global Position

	/// bool
	bool m_bShow;						// Component의 Show 여부;
};

}