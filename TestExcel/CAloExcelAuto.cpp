#include "stdafx.h"
#include "CAloExcelAuto.h" 
#include "stdio.h" 
#include <comdef.h> 

HRESULT AutoWrap(int autoType, VARIANT *pvResult, IDispatch *pDisp, LPOLESTR ptName, int cArgs...) 
{  
    // Begin variable-argument list...  
    va_list marker;  
    va_start(marker, cArgs);  
    if(!pDisp)  
    {  
        MessageBox(NULL, "NULL IDispatch passed to AutoWrap()", "Error", 0x10010);
        _exit(0);
    }  
    // Variables used...  
    DISPPARAMS dp = { NULL, NULL, 0, 0 };  
    DISPID dispidNamed = DISPID_PROPERTYPUT;  
    DISPID dispID;  
    HRESULT hr;  
    char buf[200];  
    char szName[200];  
    // Convert down to ANSI  
    WideCharToMultiByte(CP_ACP, 0, ptName, -1, szName, 256, NULL, NULL);  
    // Get DISPID for name passed...  
    hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1, LOCALE_USER_DEFAULT, &dispID);
    if(FAILED(hr))  
    {  
        sprintf(buf, "IDispatch::GetIDsOfNames(\"%s\") failed w/err 0x%08lx", szName, hr);
        MessageBox(NULL, buf, "AutoWrap()", 0x10010); 
        _exit(0);
        return hr;
    }
    // Allocate memory for arguments...  
    VARIANT *pArgs = new VARIANT[cArgs+1];
    // Extract arguments...  
    for(int i=0; i<cArgs; i++)  
    {  
        pArgs[i] = va_arg(marker, VARIANT);  
    }  
    // Build DISPPARAMS  
    dp.cArgs = cArgs;  
    dp.rgvarg = pArgs;  
    // Handle special-case for property-puts!  
    if(autoType & DISPATCH_PROPERTYPUT)  
    {  
        dp.cNamedArgs = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }  
    // Make the call!  
    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT, autoType, &dp, pvResult, NULL, NULL);  
    if(FAILED(hr))  
    {  
        sprintf(buf, "IDispatch::Invoke(\"%s\"=%08lx) failed w/err 0x%08lx",  szName, dispID, hr); 
        MessageBox(NULL, buf, "AutoWrap()", 0x10010);
        switch(hr)  
        {  
        case DISP_E_BADPARAMCOUNT:  
            MessageBox(NULL, "DISP_E_BADPARAMCOUNT", "Error:", 0x10010); 
            break;  
        case DISP_E_BADVARTYPE:
            MessageBox(NULL, "DISP_E_BADVARTYPE", "Error:", 0x10010);
            break;  
        case DISP_E_EXCEPTION:
            MessageBox(NULL, "DISP_E_EXCEPTION", "Error:", 0x10010);
            break;
        case DISP_E_MEMBERNOTFOUND:
            MessageBox(NULL, "DISP_E_MEMBERNOTFOUND", "Error:", 0x10010);
            break;
        case DISP_E_NONAMEDARGS:
            MessageBox(NULL, "DISP_E_NONAMEDARGS", "Error:", 0x10010);
            break;
        case DISP_E_OVERFLOW:
            MessageBox(NULL, "DISP_E_OVERFLOW", "Error:", 0x10010);
            break;
        case DISP_E_PARAMNOTFOUND:
            MessageBox(NULL, "DISP_E_PARAMNOTFOUND", "Error:", 0x10010); 
            break;
        case DISP_E_TYPEMISMATCH:
            MessageBox(NULL, "DISP_E_TYPEMISMATCH", "Error:", 0x10010);
            break;
        case DISP_E_UNKNOWNINTERFACE:
            MessageBox(NULL, "DISP_E_UNKNOWNINTERFACE", "Error:", 0x10010);
            break;
        case DISP_E_UNKNOWNLCID:
            MessageBox(NULL, "DISP_E_UNKNOWNLCID", "Error:", 0x10010);
            break;  
        case DISP_E_PARAMNOTOPTIONAL:  
            MessageBox(NULL, "DISP_E_PARAMNOTOPTIONAL", "Error:", 0x10010);  
            break;  
        }  
        // _exit(0);
        return hr;
    }  

    // End variable-argument section...  
    va_end(marker);  
    delete [] pArgs;  
    return hr; 
} 
/**  
* @brief constructor  
*/ 
CAloExcelAuto::CAloExcelAuto() 
{  
    // initialize COM for this thread...  
    if(CoInitialize(NULL)!=S_OK)
        m_bCOMInited = false;
    else
        m_bCOMInited = true; 
} 
/**  
* @brief destructor  
*/ 
CAloExcelAuto::~CAloExcelAuto() 
{  
    // Uninitialized COM for this thread...  
    if (m_bCOMInited)
        CoUninitialize(); 
} 
/**  
* @brief Open a new Excel Window and create a new worksheet  
*/ 
int CAloExcelAuto::Init() 
{  
    // Get CLSID for our server...  
    CLSID clsid;  
    HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);  
    if(FAILED(hr))
    {  
        ::MessageBox(NULL, "CLSIDFromProgID() failed", "Error", 0x10010);
        return -1;
    }  
    // Start server and get IDispatch...   
    hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void **)&m_instance.pXlApp);
    if(FAILED(hr))  
    {  
        ::MessageBox(NULL, "Excel not registered properly", "Error", 0x10010);
        return -2;
    }  
    // Get Workbooks collection  
    VARIANT result;  
    VariantInit(&result);  
    AutoWrap(DISPATCH_PROPERTYGET, &result, m_instance.pXlApp, L"Workbooks", 0);  
    m_instance.pXlBooks = result.pdispVal;  
    m_bInit = true;  
    return 1;
} 
/**  
* @brief make excel app visible or not  
*/ 
int CAloExcelAuto::SetVisible(bool bVisible) 
{  
    if( !CheckExcelInit() )
    {  
        ::MessageBox(NULL, "Excel is not initialzed", "Error", 0x10010);
        return -1;
    }  
    // Make it visible (i.e. app.visible = 1)   
    VARIANT x;  
    x.vt = VT_I4;  
    x.lVal = bVisible ? 1 : 0;  
    HRESULT hr = AutoWrap(DISPATCH_PROPERTYPUT, NULL, m_instance.pXlApp, L"Visible", 1, x);  
    if( FAILED(hr))
    {  
        return -1;  
    }  
    return 1; 
} 
/**  
* @brief opne excel file   
* @param[in] file (const char*) the full path of the excel file to open   
* @return int if success 1, otherwise negative number   
*/ 
int CAloExcelAuto::Open(const char* file) 
{  
    if( Init() < 0)  
    {  
        ::MessageBox(NULL, "Fail to init excel", "Error", 0x10010);
        return -1;
    }  
    if( !CheckExcelInit() )  
    {  
        ::MessageBox(NULL, "Excel is not initialzed", "Error", 0x10010);
        return -1;  
    }  
    if( file == NULL)
    {  
        ::MessageBox(NULL, "file name is null", "Error", 0x10010);
        return -1;  
    }  
    VARIANT result;  
    VariantInit(&result);  
    _bstr_t str = file;  
    VARIANT vaPath;  
    vaPath.vt = VT_BSTR;  
    vaPath.bstrVal = ::SysAllocString(str);  
    AutoWrap(DISPATCH_METHOD, &result, m_instance.pXlBooks, L"Open", 1, vaPath);
    m_instance.pXlBook = result.pdispVal;  
    SysFreeString(vaPath.bstrVal);  
    return 1; 
} 
/**  
* @brief set active sheet  
*/ 
int CAloExcelAuto::SetActiveSheet(int nSheet) 
{  
    // Get ActiveSheet object  
    VARIANT result;  
    VariantInit(&result);  
    VARIANT vaSheet;  
    vaSheet.vt = VT_I4;  
    vaSheet.lVal = nSheet;  
    AutoWrap(DISPATCH_PROPERTYGET, &result, m_instance.pXlApp, L"Worksheets", 1, vaSheet);  
    m_instance.pXlSheet = result.pdispVal;  
    return 1; 
} 
/**  
* @brief save workbook  
*/ 
void CAloExcelAuto::Save() 
{  
    // Set .Saved property of workbook to 'Saved'  
    //VARIANT x;  
    //x.vt = VT_I4;  
    //x.lVal = bAsk ? 1 : 0;  
    //AutoWrap(DISPATCH_PROPERTYPUT, NULL, m_instance.pXlBook, L"Saved", 1, x);   
    AutoWrap(DISPATCH_METHOD, NULL, m_instance.pXlApp, L"Save", 0); 
} 
/**  
* @brief close workbook  
*/ 
void CAloExcelAuto::Close() 
{  
    //Save();   
    // Tell Excel to quit (i.e. App.Quit)  
    AutoWrap(DISPATCH_METHOD, NULL, m_instance.pXlApp, L"Quit", 0);
    m_instance.pXlSheet->Release();
    m_instance.pXlBook->Release();
    m_instance.pXlBooks->Release();
    m_instance.pXlApp->Release();
    // Tell Excel to quit (i.e. App.Quit)  
    //AutoWrap(DISPATCH_METHOD, NULL, m_instance.pXlApp, L"Close", 0);  
} 
/**  
* @brief get cell data on current active sheet  
* @param[in] pPos (char*) cell position e.g. B4  
* @param[out] pData (char*) cell data. string  
*/ 
bool CAloExcelAuto::GetData(char* pPos, char* pData) 
{  
    if( pPos == NULL || pData == NULL)  
        return false;  
    // Get Range object for the Range A1:O15...  
    _bstr_t str;  str=pPos; 
    // BSTR  str+=":";  
    //str+=pPos;  
    IDispatch *pXlRange;  
    {  
        VARIANT parm;  
        parm.vt = VT_BSTR;  
        parm.bstrVal = ::SysAllocString(str);  
        VARIANT result;  
        VariantInit(&result);  
        AutoWrap(DISPATCH_PROPERTYGET, &result, m_instance.pXlSheet, L"Range", 1, parm);
        VariantClear(&parm);  
        pXlRange = result.pdispVal;  
    }  
    VARIANT tmp;  
    tmp.vt=VT_BSTR;  
    AutoWrap(DISPATCH_PROPERTYGET, &tmp, pXlRange, L"Value", 0, 0);  
    if (tmp.vt==VT_EMPTY)  
    { 
        pData[0]='\0';  
    }  
    else  
    {  
        VariantChangeType(&tmp, &tmp, VARIANT_NOUSEROVERRIDE, VT_BSTR);
        str=tmp.bstrVal;
        strcpy(pData, str);
    }  
    // Release references...  
    pXlRange->Release();
    return true; 
} 
/**  
* @brief set cell data on current active sheet  
* @param[in] pPos (char*) cell position e.g. B4  
* @param[in] pData (char*) cell data. string  
*/ 
bool CAloExcelAuto::SetData(char* pPos, char* pData) 
{  
    if( pPos == NULL || pData == NULL)  
        return false;  
    _bstr_t str=pData;  
    VARIANT tmp;  
    tmp.vt=VT_BSTR;  
    tmp.bstrVal=::SysAllocString(str);  
    // Get Range object for the Range   
    str=pPos;  
    str+=":";  
    str+=pPos;  
    IDispatch *pXlRange;  
    {  
        VARIANT parm;  
        parm.vt = VT_BSTR;  
        parm.bstrVal = ::SysAllocString(str);  
        VARIANT result;  
        VariantInit(&result);  
        AutoWrap(DISPATCH_PROPERTYGET, &result, m_instance.pXlSheet, L"Range", 1, parm);  
        VariantClear(&parm);  
        pXlRange = result.pdispVal;  
    }  
    // Set range with our safearray...  
    AutoWrap(DISPATCH_PROPERTYPUT, NULL, pXlRange, L"Value", 1, tmp);//   
    // Release references...  
    pXlRange->Release();  
    return true; 
} 