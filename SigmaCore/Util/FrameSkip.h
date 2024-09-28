#ifndef _SC_FRAME_SKIP_H_
#define _SC_FRAME_SKIP_H_

#pragma once

namespace sc
{
    //! http://kldp.net/snippet/detail.php?type=snippet&id=18
    //! http://www.gamza.net/ez2000/ezboard.exe?db=Programming&action=read&dbf=6&page=0&depth=1
    //! http://www.gpgstudy.com/gpgiki/%EC%97%B4%EB%A6%B0%20%EA%B2%8C%EC%9E%84%20%EC%BD%94%EB%93%9C
    //! copyright �輺�� [2002�� 08�� 24��]
    class FrameSkip
    {
    public:
        //! ��������� ����/�Ҹ���. 
        FrameSkip();
        virtual ~FrameSkip();

    protected: 
        float m_Timer; 
        float m_SecPerFrame; 

    public: 
        inline void clear() 
        { 
            setFramePerSec( 60.0f ); 
            m_Timer = 0.0f; 
        }

        inline void setFramePerSec( float fps )
        { 
            m_SecPerFrame = 1.0f/fps;
        } 

        //! ���ϴ� �����Ӻ��� �ʹ� ������, 
        //! false�� ��ȯ�ؼ� �ڵ带 ���۽�Ű�� �ʵ��� �Ѵ�. 
        //! dt�� '��'���� (�и��� �ƴ�!!!) 
        inline bool update( float dt ) 
        { 
            m_Timer += dt;       
            if( m_Timer<0 ) return false;       
            // �������ӿ� �ش��ϴ� �ð��� ����. 
            m_Timer -= m_SecPerFrame; 
            return true; 
        }

        //! Update�Ŀ� ȣ���ؼ� frame skip�� �����ؾ� �ϴ��� �˻��Ѵ�. 
        //! frame skip�� �Ͼ�� �Ѵٸ� true�� ��ȯ�Ѵ�. 
        inline bool isFrameSkip() 
        { 
            return m_Timer >= 0;
        }
    };

}; // End of namespace CORE_COMMON
#endif // _CORE_COMMON_FRAME_SKIP_H_