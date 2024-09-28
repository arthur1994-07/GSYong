#ifndef _SC_FRAME_SKIP_H_
#define _SC_FRAME_SKIP_H_

#pragma once

namespace sc
{
    //! http://kldp.net/snippet/detail.php?type=snippet&id=18
    //! http://www.gamza.net/ez2000/ezboard.exe?db=Programming&action=read&dbf=6&page=0&depth=1
    //! http://www.gpgstudy.com/gpgiki/%EC%97%B4%EB%A6%B0%20%EA%B2%8C%EC%9E%84%20%EC%BD%94%EB%93%9C
    //! copyright 김성수 [2002년 08월 24일]
    class FrameSkip
    {
    public:
        //! 멤버변수와 생성/소멸자. 
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

        //! 원하는 프레임보다 너무 빠르면, 
        //! false를 반환해서 코드를 동작시키지 않도록 한다. 
        //! dt는 '초'단위 (밀리초 아님!!!) 
        inline bool update( float dt ) 
        { 
            m_Timer += dt;       
            if( m_Timer<0 ) return false;       
            // 한프레임에 해당하는 시간을 뺀다. 
            m_Timer -= m_SecPerFrame; 
            return true; 
        }

        //! Update후에 호출해서 frame skip을 수행해야 하는지 검사한다. 
        //! frame skip이 일어나야 한다면 true를 반환한다. 
        inline bool isFrameSkip() 
        { 
            return m_Timer >= 0;
        }
    };

}; // End of namespace CORE_COMMON
#endif // _CORE_COMMON_FRAME_SKIP_H_