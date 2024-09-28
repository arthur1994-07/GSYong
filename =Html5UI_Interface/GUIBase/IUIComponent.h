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
	UI�� Component Ŭ����;
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
	IUIComponent( UIID uiID				// Component�� ID;
		, const std::string& uiStrID	// Component�� StrID;
		, UIRECT rcParentGlobalPos		// �θ� ������ ��ġ;
		, UIRECT rcLocalPos				// Component�� Local Pos;
		, IUIComponent* pParent );		// �θ�;
	virtual ~IUIComponent();

public:
	/// Component Ư���� �´� �߰� ���� �κ�;
	virtual void Initialize() { }
	virtual void Destroy() { }
	virtual bool FrameMove( float timeDelta ) { return true; }
	virtual bool Render( float timeDelta );

public:
	/// �⺻������ �̷������ �ϴ� �κ�;
	void UIInitialize();
	void UIDestroy();
	bool UIFrameMove( float timeDelta );
	bool UIRender( float timeDelta );

public:
	/// Event�� �޾������� �⺻ó��;
	virtual bool OnKeyEvent( std::tr1::shared_ptr<CUIKeyEvent> keyEvent );
	virtual bool OnMouseEvent( std::tr1::shared_ptr<CUIMouseEvent> mouseEvent );
	virtual bool OnWheelEvent( std::tr1::shared_ptr<CUIWheelEvent> wheelEvent );
	virtual bool OnInvokeEvent( std::tr1::shared_ptr<CUIInvokeEvent> invokeEvent );

public:
	/// Virtual �Լ�;
	virtual void ShowComponent();
	virtual void HideComponent();

public:
	/// Member �Լ�;
	// ��ġ �̵�;
	void MoveComponentPoint( UIPOINT<int> point );

public:
	/// �ڽ� Component ���� �Լ�;
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
	/// Inline �Լ�;
	// ��ġ ���� ����;
	inline COMPONENT_POSITION GetComponentPosition() { return m_position; }
	virtual UIRECT GetPosition() { return m_position.rcLocalPos; }
	inline UIRECT GetGlobalPosition() { return m_position.rcGlobalPos; }
	
	// ID ����;
	inline const std::string& GetTextureID() const { return m_textureID; }
	inline void SetTextureID( const std::string& textureID ) { m_textureID = textureID; }
	inline const std::string& GetStrID() const { return m_uiStrID; }
	inline void SetStrID( const std::string& strID ) { m_uiStrID = strID; }
	inline UIID GetUIID() const { return m_uiID; }
	inline void SetUIID( UIID uiID ) { m_uiID = uiID; }

	// Show State
	inline bool ShowState() { return m_bShow; }

	// �θ� ������Ʈ;
	inline IUIComponent* GetUIParent() const { return m_pMyParent; }
	inline void SetUIParent( IUIComponent* pParent ) { m_pMyParent = pParent; }

protected:
	/// Component
	/*------------------------------------------------------------------- 
		# Component���� ComponentManager�� ���� ���� ����;
		Component�� �ڽ� Component���� ID��;
		�� �θ� Component�ȿ����� Unique�ϸ� �ȴ�.;
		�ݸ� Window�� ID�� ��𿡼��� Unique�ؾ� �Ѵ�.;
		�׷��� Window�� WindowManager�� ���� ����������;
		Component�� Component �����ΰ� �ڽĵ��� �����Ѵ�;
	-------------------------------------------------------------------*/
	IUIComponent* m_pMyParent;			// �θ� Component;

	// �ڽ� Components;
	MAP			m_map;
	LIST		m_list;
	int			m_autoMakeID;
	IUIFactory* m_pUIFactory;

	/// ID
	UIID m_uiID;						// Component ID
	std::string m_uiStrID;				// Component StrID
	std::string m_textureID;			// Component Texture ID

	/// ��ġ;
	COMPONENT_POSITION m_position;		// UI Position
	UIRECT m_parentGlobalPos;			// Parent Global Position

	/// bool
	bool m_bShow;						// Component�� Show ����;
};

}