#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"

#include "UIColorPickerSlide.h"

//#include "../../InnerInterface.h"
//#include "../../ModalCallerID.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIColorPickerSlide::CUIColorPickerSlide(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pImage_Slide(NULL)
    , m_pImage_Arrow(NULL)
    , m_nPosY(0)
    , m_nColorIndex(0)
    , m_dwColor(0)
    , m_bButtonDown(FALSE)
{
}

CUIColorPickerSlide::~CUIColorPickerSlide ()
{
}

void CUIColorPickerSlide::CreateSubControl ()
{
    // 슬라이드
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_SLIDE_BACKGROUND", UI_FLAG_DEFAULT, COLORPICKERSLIDE_SLIDE );
        RegisterControl ( pControl );
        m_pImage_Slide = pControl;
    }

    // 화살표
    {
        CUIControl* pControl = new CUIControl(m_pEngineDevice);
        pControl->CreateSub ( this, "COLORPICKER_SLIDE_ARROW", UI_FLAG_DEFAULT, COLORPICKERSLIDE_ARROW );
        RegisterControl ( pControl );
        m_pImage_Arrow = pControl;
    }

    InitColorArray ();

    UpdateSelectColor   ();
    UpdateArrowPosition ();
}

void CUIColorPickerSlide::InitColorArray ()
{
    LONG lAmount = 0;

    m_nBaseColorIndexArray.push_back(0*COLOR_SECTION);
    m_nBaseColorIndexArray.push_back(1*COLOR_SECTION);
    m_nBaseColorIndexArray.push_back(2*COLOR_SECTION);
    m_nBaseColorIndexArray.push_back(3*COLOR_SECTION);
    m_nBaseColorIndexArray.push_back(4*COLOR_SECTION);
    m_nBaseColorIndexArray.push_back(5*COLOR_SECTION);

    // B 증가. RBG
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 증가
        lAmount = (LONG)ceil( ((float)i / (float)COLOR_SECTION) * 255.0f );
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0xFF, 0, lAmount));
    }

    // R 감소 BRG
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 감소
        lAmount = (LONG)ceil( ((float)(COLOR_SECTION - i) / (float)COLOR_SECTION) * 255.0f ); 
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, lAmount, 0, 0xFF));
    }

    // G 증가 BGR
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 증가
        lAmount = (LONG)ceil( ((float)i / (float)COLOR_SECTION) * 255.0f );
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0, lAmount, 0xFF));
    }

    // B 감소 GBR
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 감소
        lAmount = (LONG)ceil( ((float)(COLOR_SECTION - i) / (float)COLOR_SECTION) * 255.0f); 
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0, 0xFF, lAmount));
    }        

    // R 증가 GRB
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 증가
        lAmount = (LONG)ceil( ((float)i / (float)COLOR_SECTION) * 255.0f);
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, lAmount, 0xFF, 0));
    }        

    // G 감소 RGB
    for (int i = 0; i < COLOR_SECTION; i++)
    {
        // 감소
        lAmount = (LONG)ceil( ((float)(COLOR_SECTION - i) / (float)COLOR_SECTION) * 255.0f);
        m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0xFF, lAmount, 0));
    }

    // Red 두번 더 넣어줌
    m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0xFF, 0, 0));
    m_dwColorArray.push_back(D3DCOLOR_ARGB(0xFF, 0xFF, 0, 0));
}

void CUIColorPickerSlide::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

    if ( m_bButtonDown )
    {
        const UIRECT& rcSlideGlobalPos = m_pImage_Slide->GetGlobalPos();

        float fSlideLocalY = (float)y - rcSlideGlobalPos.top;

        if ( fSlideLocalY < 0.0f )
        {
            fSlideLocalY = 0.0f;
        }
        else if ( rcSlideGlobalPos.sizeY < fSlideLocalY )
        {
            fSlideLocalY = rcSlideGlobalPos.sizeY;
        }

        if ( m_nPosY == fSlideLocalY )
        {
            return;
        }

        m_nPosY = (int)fSlideLocalY;

        UpdateSelectColor   ();
        UpdateArrowPosition ();

        AddMessageEx ( UIMSG_SLIDE_SELECT_UPDATE );
    }
}

void CUIColorPickerSlide::UpdateSelectColor ()
{
    if ( !m_pImage_Slide )
    {
        return;
    }

    if ( m_dwColorArray.empty() )
    {
        return;
    }

    const UIRECT& rcSlideGlobalPos = m_pImage_Slide->GetGlobalPos();

    if ( rcSlideGlobalPos.sizeY == 0.0f )
    {
        GASSERT(!"rcSlideGlobalPos.sizeY 사이즈가 0입니다");
    }

    m_nColorIndex = (int) ( ((float)m_nPosY / rcSlideGlobalPos.sizeY) * (float)(m_dwColorArray.size() - 1) + 0.5f /*반올림*/ );

    m_dwColor = m_dwColorArray[m_nColorIndex];
}

void CUIColorPickerSlide::UpdateArrowPosition ()
{
    if ( !m_pImage_Arrow || !m_pImage_Slide )
    {
        return;
    }

    if ( m_dwColorArray.empty() )
    {
        return;
    }

    const UIRECT& rcSlideLocalPos = m_pImage_Slide->GetLocalPos();
    FLOAT         fArrowPosY      = 0.0f;

    if ( m_dwColorArray.size() == 1 )
    {
        fArrowPosY = 0.0f;
    }
    else
    {
        fArrowPosY = rcSlideLocalPos.sizeY * ((float)m_nColorIndex/(float)(m_dwColorArray.size() - 1));
    }

    const UIRECT& rcArrowLocalPos   = m_pImage_Arrow->GetLocalPos();
    const UIRECT& rcParentGlobalPos = GetGlobalPos();

    m_pImage_Arrow->SetLocalPos  ( D3DXVECTOR2(rcArrowLocalPos.left, fArrowPosY) );
    m_pImage_Arrow->SetGlobalPos ( D3DXVECTOR2(rcParentGlobalPos.left + rcArrowLocalPos.left, rcParentGlobalPos.top + fArrowPosY) );
}

void CUIColorPickerSlide::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
    switch ( ControlID )
    {
    case COLORPICKERSLIDE_SLIDE:
        {            
            if ( CHECK_MOUSE_IN ( dwMsg ) )
            {
                if ( dwMsg&(UIMSG_LB_DOWN) )
                {
                    m_bButtonDown = TRUE;
                }
            }

            if( !(dwMsg&(UIMSG_LB_DOWN|UIMSG_LB_PRESEED|UIMSG_LB_DRAG)) )
            {
                m_bButtonDown = FALSE;
            }
        }
        break;
    };

    CUIGroup::TranslateUIMessage ( ControlID, dwMsg );
}


// 색깔에 맞는 최적 인덱스를 찾는다 (현재는 사용하지 않는다)
void CUIColorPickerSlide::UpdateByColor ( const DWORD dwColor )
{
    // 테이블중에서 있는지 찾아본다
    std::vector<DWORD>::iterator iter   = m_dwColorArray.begin();
    int                          nIndex = 0;

    for ( ; iter!=m_dwColorArray.end(); ++iter, ++nIndex )
    {
        if ( (*iter) == dwColor )
        {
            m_nPosY = GetPositionByIndex ( nIndex );
            UpdateSelectColor ();
            UpdateArrowPosition ();
            return;
        }
    }

    // 최적의 인덱스 찾기
    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    // 무채색이면 현재 인덱스
    if ( CompareColorAmount(nR, nG) && CompareColorAmount(nR, nB) && CompareColorAmount(nG, nB) )
    {
        return;
    }

    //------------------------------
    // 2색 이상 같은 값을 가질 경우 : 경계값
    //------------------------------

    if (CompareColorAmount(nR, nG))
    {
        if (nB < nR)
        {
            //trace("* 3 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_GRB));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_GRB] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }
        else
        {
            //trace("* 4 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_BRG));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_BRG] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }

        return;
    }
    else if (CompareColorAmount(nG, nB))
    {
        if (nR < nG)
        {
            //trace("* 6 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_BGR));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_BGR] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }
        else
        {
            //trace("* 7 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_RGB));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_RGB] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }

        return;
    }
    else if (CompareColorAmount(nB, nR))
    {
        if (nG < nB)
        {
            //trace("* 9 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_RBG));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_RBG] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }
        else
        {
            //trace("* 10 : " + HueColor.getBaseColorGroupIndex(BASECOLORINDEX_GBR));
            m_nPosY = GetPositionByIndex ( m_nBaseColorIndexArray[BASECOLORINDEX_GBR] );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
        }

        return;
    }

    //------------------------------
    // Step 1. Red, Green, Blue 순서 배열하여 BaseGroup 찾기
    //------------------------------

    int nBaseColorIndex = BASECOLORINDEX_RBG;

    if (nR > nG)
    {
        if (nR > nB)
        {
            if (nB > nG)
            {
                nBaseColorIndex = BASECOLORINDEX_RBG;
            }
            else
            {
                nBaseColorIndex = BASECOLORINDEX_RGB;
            }
        }
        else
        {
            nBaseColorIndex = BASECOLORINDEX_BRG;
        }
    }
    else
    {
        if (nG > nB)
        {
            if (nR > nB)
            {
                nBaseColorIndex = BASECOLORINDEX_GRB;
            }
            else
            {
                nBaseColorIndex = BASECOLORINDEX_GBR;
            }
        }
        else
        {
            nBaseColorIndex = BASECOLORINDEX_BGR;
        }
    }

    //------------------------------
    // 1개가 0인경우
    //------------------------------

    {
        int nTableIndex = m_nBaseColorIndexArray[nBaseColorIndex];
        int nCopyR = (int)((dwColor&0xFF0000)>>16);
        int nCopyG = (int)((dwColor&0xFF00)>>8);
        int nCopyB = (int)(dwColor&0xFF);

        if (nR == 0)
        {
            float fMultiple = 255.0f / (float)max(nB, nG);
            nCopyG = (int)((float)nG * fMultiple);
            nCopyB = (int)((float)nB * fMultiple);

            int nIndex = 0;

            if (CompareColorAmount(nCopyG, 255))
            {
                nIndex = nTableIndex + (COLOR_SECTION - (int)ceil((float)nCopyB/255.0f* (float)COLOR_SECTION));
            }
            else if (CompareColorAmount(nCopyB, 255))
            {
                nIndex = nTableIndex + (int)(ceil((float)nCopyG/255.0f * (float)COLOR_SECTION));
            }
            else
            {
                return;
            }

            m_nPosY = GetPositionByIndex ( nIndex );
            UpdateSelectColor   ();
            UpdateArrowPosition ();

            return;
        }
        else if (nG == 0)
        {
            float fMultiple = 255.0f / (float)max(nB, nR);
            nCopyB = (int)((float)nB * fMultiple);
            nCopyR = (int)((float)nR * fMultiple);

            int nIndex = 0;

            if (CompareColorAmount(nCopyR, 255))
            {
                nIndex = nTableIndex + (int)(ceil((float)nCopyB/255.0f * (float)COLOR_SECTION));
            }
            else if (CompareColorAmount(nCopyB, 255))
            {
                nIndex = nTableIndex + (COLOR_SECTION - (int)(ceil((float)nCopyR/255.0f * (float)COLOR_SECTION)));
            }
            else
            {
                return;
            }

            m_nPosY = GetPositionByIndex ( nIndex );
            UpdateSelectColor   ();
            UpdateArrowPosition ();

            return;
        }
        else if (nB == 0)
        {
            float fMultiple = 255.0f / (float)max(nR, nG);
            nCopyR = (int)((float)nR * fMultiple);
            nCopyG = (int)((float)nG * fMultiple);

            int nIndex = 0;

            if (CompareColorAmount(nCopyR, 255))
            {
                nIndex = nTableIndex + (COLOR_SECTION - (int)(ceil((float)nCopyG/255.0f * (float)COLOR_SECTION)));
            }
            else if (CompareColorAmount(nCopyG, 255))
            {
                nIndex = nTableIndex + (int)(ceil((float)nCopyR/255.0f * (float)COLOR_SECTION));
            }

            m_nPosY = GetPositionByIndex ( nIndex );
            UpdateSelectColor   ();
            UpdateArrowPosition ();

            return;
        }
    }

    switch (nBaseColorIndex)
    {
    case BASECOLORINDEX_RGB:
    case BASECOLORINDEX_GRB:
    case BASECOLORINDEX_GBR:
    case BASECOLORINDEX_BGR:
    case BASECOLORINDEX_BRG:
    case BASECOLORINDEX_RBG:
        {
            int nMin      = INT_MAX;
            int nMinIndex = 0;

            int i = 0;
            int n = COLOR_SECTION;

            int nMaxArray = (int)m_dwColorArray.size();

            for (i = 0; i < n; i++)
            {
                int    nTempIndex  = m_nBaseColorIndexArray[nBaseColorIndex] + i;
                DWORD  dwTempColor = m_dwColorArray[nTempIndex];

                int nCopyR = (int)((dwTempColor&0xFF0000)>>16);
                int nCopyG = (int)((dwTempColor&0xFF00)>>8);
                int nCopyB = (int)(dwTempColor&0xFF);

                int j = 0;
                int m = (int)m_dwColorArray.size();

                for (j = 0; j <= m; j++)
                {
                    int nTempR = nCopyR + (255 - nCopyR) * (nMaxArray - j) / nMaxArray;
                    int nTempG = nCopyG + (255 - nCopyG) * (nMaxArray - j) / nMaxArray;
                    int nTempB = nCopyB + (255 - nCopyB) * (nMaxArray - j) / nMaxArray;

                    float fRatioRed   = (float)nTempR / nR;
                    float fRatioGreen = (float)nTempG / nG;
                    float fRatioBlue  = (float)nTempB / nB;

                    float fRatioAverage = (fRatioRed + fRatioGreen + fRatioBlue) / 3.0f;

                    int nFar = (int)sqrt(
                        pow(fRatioRed - fRatioAverage, 2.0f) + 
                        pow(fRatioGreen - fRatioAverage, 2.0f) + 
                        pow(fRatioBlue - fRatioAverage, 2.0f));

                    if (nFar < nMin)
                    {
                        nMin = nFar;
                        nMinIndex = nTempIndex;
                    }
                }
            }

            //trace("* 14 : " + minIndex);

            m_nPosY = GetPositionByIndex ( nMinIndex );
            UpdateSelectColor   ();
            UpdateArrowPosition ();
            return;
        }
    }

    //trace("* 15 : " + 0);
}

// 색깔에 맞는 최적 인덱스를 찾는다
void CUIColorPickerSlide::UpdateByColor_2 ( const DWORD dwColor )
{
    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    float fR = (float)nR/255.0f;
    float fG = (float)nG/255.0f;
    float fB = (float)nB/255.0f;

    float fMax = max(max(fR, fB), fG);
    float fMin = min(min(fR, fB), fG);
    float fHue = 0.0f;

    if (fMax == fMin)
    {
        fHue = 0.0f;
    }
    else if (fMax == fR)
    {
        fHue = fmod( (60.0f * (fB - fG) / (fMax - fMin) + 360.0f), 360.0f);
    }
    else if (fMax == fB)
    {
        fHue = (60.0f * (fG - fR) / (fMax - fMin) + 120.0f);
    }
    else
    {
        fHue = (60.0f * (fR - fB) / (fMax - fMin) + 240.0f);
    }

    int newIndex = (int)(127.0f * fHue / 360.0f + 0.5f);

    m_nPosY = GetPositionByIndex ( newIndex );
    UpdateSelectColor   ();
    UpdateArrowPosition ();
}

const DWORD CUIColorPickerSlide::GetBestColor ( const DWORD dwColor )
{
    int nR = (int)((dwColor&0xFF0000)>>16);
    int nG = (int)((dwColor&0xFF00)>>8);
    int nB = (int)(dwColor&0xFF);

    float fR = (float)nR/255.0f;
    float fG = (float)nG/255.0f;
    float fB = (float)nB/255.0f;

    float fMax = max(max(fR, fB), fG);
    float fMin = min(min(fR, fB), fG);
    float fHue = 0.0f;

    if (fMax == fMin)
    {
        fHue = 0.0f;
    }
    else if (fMax == fR)
    {
        fHue = fmod( (60.0f * (fB - fG) / (fMax - fMin) + 360.0f), 360.0f);
    }
    else if (fMax == fB)
    {
        fHue = (60.0f * (fG - fR) / (fMax - fMin) + 120.0f);
    }
    else
    {
        fHue = (60.0f * (fR - fB) / (fMax - fMin) + 240.0f);
    }

    int newIndex = (int)(127.0f * fHue / 360.0f + 0.5f);
    return m_dwColorArray[newIndex];
}

// 컬러 수치가 (비교적)같은 값인지 비교한다.
const BOOL CUIColorPickerSlide::CompareColorAmount ( const int nColor1, const int nColor2 )
{
    if ( abs( nColor1 - nColor2 ) <= 1 )
    {
        return TRUE;
    }

    return FALSE;
}

// 인덱스에 따른 위치를 가져온다.
const int CUIColorPickerSlide::GetPositionByIndex ( const int nIndex )
{
    if ( !m_pImage_Slide )
    {
        return 0;
    }

    int nTempIndex = nIndex;

    if ( (int)m_dwColorArray.size() < nTempIndex )
    {
        nTempIndex = (int)m_dwColorArray.size() - 1;
    }

    if ( nTempIndex < 0 )
    {
        nTempIndex = 0;
    }

    const UIRECT& rcSlideGlobalPos = m_pImage_Slide->GetGlobalPos();

    int nPosY = (int)(((float)nTempIndex / (float)(m_dwColorArray.size() - 1)) * rcSlideGlobalPos.sizeY);

    return nPosY;
}
