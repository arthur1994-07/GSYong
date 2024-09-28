#pragma once

#include "./ComponentDefine.h"
#include "../GLogicEx.h"

class GLActor;

/************************************************************************/
/* Component System														*/
/* Game Object(GLActor)의 Componet를 유동적으로 추가하기 위한 시스템;*/
/************************************************************************/
namespace ComponentSystem
{
	// Component의 Basic Interface;
	class IComponentBase
	{
	public:
		IComponentBase ( GLActor* pOwner ) : m_pOwner( pOwner ) { }
		virtual ~IComponentBase () { }

	public:
		// Component의 고유 ID;
		virtual const EMCOMP_ID GetComponentID() const = 0;

		// Component의 종류를 구분하기 위한 ID;
		// 같은 종류의 Component라면 같은 ID를 공유하여;
		// 서로 중복으로 추가하지 못하게 한다;
		virtual const EMFAMILY_ID GetFamilyID() const = 0;

		virtual void Initialize() { }
		virtual void Destroy() { }
		virtual void Reset() { }

		// 프레임 마다 Update를 수행한다;
		void FrameMove( float fElapsedTime )
		{
			BeginUpdate();

			Update( fElapsedTime );

			EndUpdate();
		}

	protected:
		virtual void BeginUpdate() { }
		virtual void Update( float fElapsedTime ) { }
		virtual void EndUpdate() { }

	public:
		inline GLActor* GetOwner() const { return m_pOwner; }

	private:
		// 실제로 Component를 사용하는 주인( Actor )을 알고있다;
		GLActor* m_pOwner;
	};
}