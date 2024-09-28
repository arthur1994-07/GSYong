#pragma once

#include "./ComponentDefine.h"
#include "../GLogicEx.h"

class GLActor;

/************************************************************************/
/* Component System														*/
/* Game Object(GLActor)�� Componet�� ���������� �߰��ϱ� ���� �ý���;*/
/************************************************************************/
namespace ComponentSystem
{
	// Component�� Basic Interface;
	class IComponentBase
	{
	public:
		IComponentBase ( GLActor* pOwner ) : m_pOwner( pOwner ) { }
		virtual ~IComponentBase () { }

	public:
		// Component�� ���� ID;
		virtual const EMCOMP_ID GetComponentID() const = 0;

		// Component�� ������ �����ϱ� ���� ID;
		// ���� ������ Component��� ���� ID�� �����Ͽ�;
		// ���� �ߺ����� �߰����� ���ϰ� �Ѵ�;
		virtual const EMFAMILY_ID GetFamilyID() const = 0;

		virtual void Initialize() { }
		virtual void Destroy() { }
		virtual void Reset() { }

		// ������ ���� Update�� �����Ѵ�;
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
		// ������ Component�� ����ϴ� ����( Actor )�� �˰��ִ�;
		GLActor* m_pOwner;
	};
}