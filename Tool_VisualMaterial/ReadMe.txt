================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : Tool_VisualMaterial Project Overview
===============================================================================

The application wizard has created this Tool_VisualMaterial application for 
you.  This application not only demonstrates the basics of using the Microsoft 
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your Tool_VisualMaterial application.

Tool_VisualMaterial.vcproj
    This is the main project file for VC++ projects generated using an application wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    application wizard.

Tool_VisualMaterial.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CTool_VisualMaterialApp application class.

Tool_VisualMaterial.cpp
    This is the main application source file that contains the application
    class CTool_VisualMaterialApp.

Tool_VisualMaterial.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\Tool_VisualMaterial.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Tool_VisualMaterial.rc.

res\Tool_VisualMaterial.rc2
    This file contains resources that are not edited by Microsoft 
    Visual C++. You should place all resources not editable by
    the resource editor in this file.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in Tool_VisualMaterial.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

Tool_VisualMaterialDoc.h, Tool_VisualMaterialDoc.cpp - the document
    These files contain your CTool_VisualMaterialDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CTool_VisualMaterialDoc::Serialize).

Tool_VisualMaterialView.h, Tool_VisualMaterialView.cpp - the view of the document
    These files contain your CTool_VisualMaterialView class.
    CTool_VisualMaterialView objects are used to view CTool_VisualMaterialDoc objects.





/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

Printing and Print Preview support
    The application wizard has generated code to handle the print, print setup, and print preview
    commands by calling member functions in the CView class from the MFC library.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Tool_VisualMaterial.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

Tool_VisualMaterial.manifest
	Application manifest files are used by Windows XP to describe an applications 
	dependency on specific versions of Side-by-Side assemblies. The loader uses this 
	information to load the appropriate assembly from the assembly cache or private 
	from the application. The Application manifest  maybe included for redistribution 
	as an external .manifest file that is installed in the same folder as the application 
	executable or it may be included in the executable in the form of a resource. 
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is in a 
language other than the operating system's current language, you will need 
to copy the corresponding localized resources MFC80XXX.DLL from the Microsoft
Visual C++ CD-ROM under the Win\System directory to your computer's system or 
system32 directory, and rename it to be MFCLOC.DLL.  ("XXX" stands for the 
language abbreviation.  For example, MFC80DEU.DLL contains resources 
translated to German.)  If you don't do this, some of the UI elements of 
your application will remain in the language of the operating system.

/////////////////////////////////////////////////////////////////////////////
