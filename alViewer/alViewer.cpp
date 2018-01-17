
// alViewer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "alViewer.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "alViewerDoc.h"
#include "alViewerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CalViewerApp

BEGIN_MESSAGE_MAP(CalViewerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CalViewerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CalViewerApp construction

CalViewerApp::CalViewerApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("alViewer.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CalViewerApp object

CalViewerApp theApp;


// CalViewerApp initialization

BOOL CalViewerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_alViewerTYPE,
		RUNTIME_CLASS(CalViewerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CalViewerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	LoadSetting();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CalViewerApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	RestoreSetting();
	AfxOleTerm(FALSE);

	return CWinApp::ExitInstance();
}

BOOL CalViewerApp::LoadSetting()
{
	HKEY hkey;
	TCHAR *path = _T("SOFTWARE\\altek\\alViewer\\RawFILE");
	long lRetCode = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &hkey);

	if (lRetCode != ERROR_SUCCESS)
	{
		HKEY hKey;
		LONG result = 0;
		lRetCode = RegCreateKeyEx(HKEY_CURRENT_USER, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hKey, NULL);

		if (lRetCode == ERROR_SUCCESS)
		{
			DWORD W = (DWORD)Global::RAW_IMAGE_WIDTH;
			DWORD H = (DWORD)Global::RAW_IMAGE_HEIGHT;
			DWORD FORMAT = (DWORD)Global::RAW_IMAGE_FORMAT;
			RegSetValueEx(hKey, _T("width"), NULL, REG_DWORD, (LPBYTE)&W, sizeof(DWORD));
			RegSetValueEx(hKey, _T("height"), NULL, REG_DWORD, (LPBYTE)&H, sizeof(DWORD));
			RegSetValueEx(hKey, _T("format"), NULL, REG_DWORD, (LPBYTE)&FORMAT, sizeof(DWORD));
		}
	}
	else
	{
		DWORD dataW, dataH, dataForamt;		
		DWORD size = sizeof(dataW);
		DWORD type = REG_DWORD;
		LONG nError = ERROR_SUCCESS;

		nError = RegQueryValueEx(hkey, _T("width"), NULL, &type, (LPBYTE)&dataW, &size);
		nError = RegQueryValueEx(hkey, _T("height"), NULL, &type, (LPBYTE)&dataH, &size);
		nError = RegQueryValueEx(hkey, _T("format"), NULL, &type, (LPBYTE)&dataForamt, &size);

		Global::RAW_IMAGE_WIDTH = dataW;
		Global::RAW_IMAGE_HEIGHT = dataH;
		Global::RAW_IMAGE_FORMAT = dataForamt;	
	}

	RegCloseKey(hkey);

	return TRUE;
}

BOOL CalViewerApp::RestoreSetting()
{
	HKEY hkey;
	TCHAR *path = _T("SOFTWARE\\altek\\alViewer\\RawFILE");
	long lRetCode = RegOpenKeyEx(HKEY_CURRENT_USER, path, 0, KEY_ALL_ACCESS, &hkey);

	if (lRetCode != ERROR_SUCCESS)
	{
		lRetCode = RegCreateKeyEx(HKEY_CURRENT_USER, path, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS | KEY_WRITE, NULL, &hkey, NULL);
	}

	if(lRetCode == ERROR_SUCCESS)
	{
		DWORD W = (DWORD)Global::RAW_IMAGE_WIDTH;
		DWORD H = (DWORD)Global::RAW_IMAGE_HEIGHT;
		DWORD FORMAT = (DWORD)Global::RAW_IMAGE_FORMAT;
		RegSetValueEx(hkey, _T("width"), NULL, REG_DWORD, (LPBYTE)&W, sizeof(DWORD));
		RegSetValueEx(hkey, _T("height"), NULL, REG_DWORD, (LPBYTE)&H, sizeof(DWORD));
		RegSetValueEx(hkey, _T("format"), NULL, REG_DWORD, (LPBYTE)&FORMAT, sizeof(DWORD));
	}

	RegCloseKey(hkey);

	return TRUE;
}


// CalViewerApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CalViewerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CalViewerApp message handlers

