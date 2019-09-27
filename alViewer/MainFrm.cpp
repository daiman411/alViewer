
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "alViewer.h"
#include "MainFrm.h"
#include "alViewerView.h"
#include "alViewerDoc.h"
#include "DlgHistogram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_USER + 100, OnUpdateStatusSize)
	ON_MESSAGE(WM_USER + 101, OnUpdateStatusPOS)
	ON_MESSAGE(WM_USER + 102, OnUpdateStatusRGB)
	ON_MESSAGE(WM_USER + 103, OnUpdateStatusRect)
	ON_MESSAGE(WM_USER + 104, OnUpdateStatusRatio)
	ON_MESSAGE(WM_USER + 105, OnUpdateDrawViewRect)
	ON_MESSAGE(WM_USER + 200, OnEnableToolbarStatus)
	ON_MESSAGE(WM_USER + 201, OnDisableToolbarStatus)
	ON_MESSAGE(WM_USER + 301, OnStopPreviewVideo)
	ON_COMMAND(ID_FILE_OPEN, &CMainFrame::OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_ARROW, &CMainFrame::OnUpdateViewerArrow)
	ON_COMMAND(ID_VIEWER_ARROW, &CMainFrame::OnViewerArrow)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_SELECT, &CMainFrame::OnUpdateViewerSelect)
	ON_COMMAND(ID_VIEWER_SELECT, &CMainFrame::OnViewerSelect)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_ZOOMIN, &CMainFrame::OnUpdateViewerZoomin)
	ON_COMMAND(ID_VIEWER_ZOOMIN, &CMainFrame::OnViewerZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_ZOOMOUT, &CMainFrame::OnUpdateViewerZoomout)
	ON_COMMAND(ID_VIEWER_ZOOMOUT, &CMainFrame::OnViewerZoomout)
	ON_COMMAND(ID_VIEWER_ZOOMFIT, &CMainFrame::OnViewerZoomfit)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_ZOOMFIT, &CMainFrame::OnUpdateViewerZoomfit)
	ON_COMMAND(ID_VIEWER_PREONE, &CMainFrame::OnViewerPreOne)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_PREONE, &CMainFrame::OnUpdateViewerPreOne)
	ON_COMMAND(ID_VIEWER_NEXTONE, &CMainFrame::OnViewerNextOne)
	ON_UPDATE_COMMAND_UI(ID_VIEWER_NEXTONE, &CMainFrame::OnUpdateViewerNextOne)

	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_PREVIEWE_START, &CMainFrame::OnUpdatePrevieweStart)
	ON_COMMAND(ID_PREVIEWE_START, &CMainFrame::OnPrevieweStart)
	ON_UPDATE_COMMAND_UI(ID_PREVIEWE_STOP, &CMainFrame::OnUpdatePrevieweStop)
	ON_COMMAND(ID_PREVIEWE_STOP, &CMainFrame::OnPrevieweStop)
	ON_COMMAND(ID_VIEW_SHOW_HIST, &CMainFrame::OnViewShowHist)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_HIST, &CMainFrame::OnUpdateViewShowHist)
    ON_COMMAND_EX(ID_WINDOW_ARRANGE, &CMainFrame::OnWindowCommand)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,				// status line indicator
	ID_INDICATOR_STATUS_SIZE,
	ID_INDICATOR_STATUS_POS,
	ID_INDICATOR_STATUS_RGB,
	ID_INDICATOR_STATUS_RECT,
	ID_INDICATOR_STATUS_PROG,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_bEnableToolbar = false;
	m_bEnableVideobar = true;
	m_ToolBarFunc = TOOL_FUNC_NONE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP /*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndVideoBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP /*| CBRS_GRIPPER*/ | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndVideoBar.LoadToolBar(IDR_VIDEOTOOLBAR))
	{
		TRACE0("Failed to create videobar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CRect rect;
	GetClientRect(rect);

	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, rect.Width() * 1 / 6);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STATUS_SIZE, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_STATUS_POS, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_STATUS_RGB, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_STATUS_RECT, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_STATUS_PROG, SBPS_NORMAL, (int)(rect.Width() * 2.8 / 6));

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndVideoBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBarLeftOf(&m_wndVideoBar, &m_wndToolBar);

	return 0;
}

void CMainFrame::DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf)
{
	CRect rect;
	DWORD dw;
	UINT n;

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	RecalcLayout(TRUE);

	LeftOf->GetWindowRect(&rect);
	rect.OffsetRect(1, 0);
	dw = LeftOf->GetBarStyle();
	n = 0;
	n = (dw&CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw&CBRS_ALIGN_BOTTOM && n == 0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw&CBRS_ALIGN_LEFT && n == 0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw&CBRS_ALIGN_RIGHT && n == 0) ? AFX_IDW_DOCKBAR_RIGHT : n;

	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. By calculating a rectangle, we
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(Bar, n, &rect);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers
void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWnd::OnDropFiles(hDropInfo);
}


LRESULT CMainFrame::OnUpdateStatusSize(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	CString msgText(str);
	m_wndStatusBar.SetPaneText(1, msgText, 1);
	m_wndStatusBar.Invalidate();

	return 0;
}

LRESULT CMainFrame::OnUpdateStatusPOS(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	CString msgText(str);
	m_wndStatusBar.SetPaneText(2, msgText, 1);
	m_wndStatusBar.Invalidate();

	return 0;
}

LRESULT CMainFrame::OnUpdateStatusRGB(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	CString msgText(str);
	m_wndStatusBar.SetPaneText(3, msgText, 1);
	m_wndStatusBar.Invalidate();

	return 0;
}

LRESULT CMainFrame::OnUpdateStatusRect(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	CString msgText(str);
	m_wndStatusBar.SetPaneText(4, msgText, 1);
	m_wndStatusBar.Invalidate();

	return 0;
}

LRESULT CMainFrame::OnUpdateStatusRatio(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR str = (LPCTSTR)lParam;
	CString msgText(str);
	m_wndStatusBar.SetPaneText(5, msgText, 1);
	m_wndStatusBar.Invalidate();

	return 0;
}

LRESULT CMainFrame::OnUpdateDrawViewRect(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->UpdateDrawView();
	}

	return 0;
}

LRESULT CMainFrame::OnStopPreviewVideo(WPARAM wParam, LPARAM lParam)
{
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerDoc* pMainDoc = static_cast<CalViewerDoc*>(pChildWnd->GetActiveView()->GetDocument());
		pMainDoc->StopPreviewThread();
	}

	return 0;
}

LRESULT CMainFrame::OnEnableToolbarStatus(WPARAM wParam, LPARAM lParam)
{
	m_bEnableToolbar = true;
	OnViewerSelect();
	return 0;
}

LRESULT CMainFrame::OnDisableToolbarStatus(WPARAM wParam, LPARAM lParam)
{
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd == nullptr)
		m_bEnableToolbar = false;

	return 0;
}

void CMainFrame::OnUpdateViewerArrow(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableToolbar);
}


void CMainFrame::OnViewerArrow()
{
	// TODO: Add your command handler code here
	m_ToolBarFunc = TOOL_FUNC_ARROW;
	//m_wndToolBar.GetToolBarCtrl().PressButton(ID_VIEWER_ARROW);
	//m_wndToolBar.GetToolBarCtrl().PressButton(ID_VIEWER_SELECT, false);
}


void CMainFrame::OnUpdateViewerSelect(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableToolbar);
}


void CMainFrame::OnViewerSelect()
{
	// TODO: Add your command handler code here
	m_ToolBarFunc = TOOL_FUNC_SELECT;
	//m_wndToolBar.GetToolBarCtrl().PressButton(ID_VIEWER_SELECT);
	//m_wndToolBar.GetToolBarCtrl().PressButton(ID_VIEWER_ARROW, false);
}

void CMainFrame::OnFileOpen()
{
	TCHAR szFilters[] = _T("Jpg Files (*.jpg)|*.jpg|Bmp Files (*.bmp)|*.bmp|Raw Files (*.raw)|*.raw|All Files (*.*)|*.*||");

	// Create an Open dialog; the default file name extension is ".my".
	CFileDialog fileDlg(TRUE, _T("jpg"), _T("*.jpg"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CWinApp* pApp = AfxGetApp();
		ASSERT(pApp != NULL);

		CString fileName = fileDlg.GetPathName();
		pApp->OpenDocumentFile(fileName.GetBuffer());
	}
}

void CMainFrame::OnUpdateViewerZoomin(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableToolbar);
}


void CMainFrame::OnViewerZoomin()
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->ZoomView_In();
	}
}


void CMainFrame::OnUpdateViewerZoomout(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableToolbar);
}


void CMainFrame::OnViewerZoomout()
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->ZoomView_Out();
	}
}


void CMainFrame::OnViewerZoomfit()
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->ZoomView_Fit();
	}
}

void CMainFrame::OnViewerPreOne()
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->MovePrvousOne();
	}
}

void CMainFrame::OnViewerNextOne()
{
	// TODO: Add your command handler code here
	CMDIChildWnd* pChildWnd = MDIGetActive();

	if (pChildWnd)
	{
		CalViewerView* pMainView = static_cast<CalViewerView*>(pChildWnd->GetActiveView());
		pMainView->MoveNextOne();
	}
}

void CMainFrame::OnUpdateViewerZoomfit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableToolbar);
}

void CMainFrame::OnUpdateViewerPreOne(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateViewerNextOne(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
	CMDIFrameWnd::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(rect);
	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, rect.Width() * 1 / 6);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STATUS_SIZE, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_STATUS_POS, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_STATUS_RGB, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_STATUS_RECT, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_STATUS_PROG, SBPS_NORMAL, (int)(rect.Width() * 2.8 / 6));
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CRect rect;
	GetClientRect(rect);
	m_wndStatusBar.SetPaneInfo(0, ID_SEPARATOR, SBPS_NORMAL, rect.Width() * 1 / 6);
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_STATUS_SIZE, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_STATUS_POS, SBPS_NORMAL, (int)(rect.Width() * 0.5 / 6));
	m_wndStatusBar.SetPaneInfo(3, ID_INDICATOR_STATUS_RGB, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(4, ID_INDICATOR_STATUS_RECT, SBPS_NORMAL, (int)(rect.Width() * 0.8 / 6));
	m_wndStatusBar.SetPaneInfo(5, ID_INDICATOR_STATUS_PROG, SBPS_NORMAL, (int)(rect.Width() * 2.8 / 6));

}


void CMainFrame::OnUpdatePrevieweStart(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableVideobar);
}

void CMainFrame::OnPrevieweStart()
{
	// TODO: Add your command handler code here
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	if (GetDocument() == nullptr)
		pApp->OpenDocumentFile(_T("C:\\Video.UVC"), FALSE);
	else
	{
		CalViewerDoc* pMainDoc = static_cast<CalViewerDoc*>(GetDocument());
		pMainDoc->StartPreviewThread();
	}
}

void CMainFrame::OnUpdatePrevieweStop(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bEnableVideobar);
}

void CMainFrame::OnPrevieweStop()
{
	// TODO: Add your command handler code here
	OnStopPreviewVideo(0, 0);

	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	POSITION pos = pApp->GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)(pApp->GetNextDocTemplate(pos));
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc != NULL)
		{
			CalViewerDoc* pDoc = (CalViewerDoc*)(pTemplate->GetNextDoc(posDoc));

			if (pDoc)
			{
				if (pDoc->DocMode == 2)
				{
					pDoc->OnCloseDocument();
				}
			}
		}
	}
}

void CMainFrame::OnViewShowHist()
{
	CDlgHistogram *dlgHist = nullptr;
	//dlgHist.DoModal();

	if (dlgHist == nullptr)
	{
		CString strPropertiesWnd;
		//BOOL bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
		//ASSERT(bNameValid);

		dlgHist = new CDlgHistogram();

		if (!dlgHist->Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), FALSE, IDD_DIALOG_HISTOGRAM, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
		{
			TRACE0("Failed to create Properties window\n");
			return; // failed to create
		}
		dlgHist->EnableDocking(CBRS_ALIGN_ANY);
//		DockPane(dlgHist);
		dlgHist->ShowPane(TRUE, FALSE, TRUE);
		//m_wndFocusAdjPanel->StartAdjustFocus();
		//m_bIsButtonPushingDown = TRUE;
	}
	else
	{
		if (dlgHist->IsPaneVisible())
		{
			//m_bIsButtonPushingDown = FALSE;
			//m_wndFocusAdjPanel->StopAdjustFocus();
			dlgHist->ShowPane(FALSE, FALSE, FALSE);
		}
		else
		{
			//m_bIsButtonPushingDown = TRUE;
			dlgHist->ShowPane(TRUE, FALSE, TRUE);
			//m_wndFocusAdjPanel->StartAdjustFocus();
		}
	}
}

CDocument* CMainFrame::GetDocument()
{
	CWinApp* pApp = AfxGetApp();
	ASSERT(pApp != NULL);

	POSITION pos = pApp->GetFirstDocTemplatePosition();
	while (pos != NULL)
	{
		CDocTemplate* pTemplate = (CDocTemplate*)(pApp->GetNextDocTemplate(pos));
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc != NULL)
		{
			CalViewerDoc* pDoc = (CalViewerDoc*)(pTemplate->GetNextDoc(posDoc));

			if (pDoc)
			{
				return pDoc;
			}
		}
	}

	return nullptr;
}


void CMainFrame::OnUpdateViewShowHist(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
}

BOOL CMainFrame::OnWindowCommand(UINT nID)
{
    switch (nID)
    {
    case ID_WINDOW_ARRANGE: // For Window\Arrange Icons menu item, arrange
        MDIIconArrange();    // all minimized document child windows.
        break;

    case ID_WINDOW_CASCADE: // For Window\Cascade menu item, arrange
        MDICascade();        // all the MDI child windows in a cascade format.
        break;

    case ID_WINDOW_TILE_HORZ:       // For Window\Tile Horizontal menu item,
        MDITile(MDITILE_HORIZONTAL); // tile MDI child windows so that
        break;                       // one window appears above another.

    case ID_WINDOW_TILE_VERT:     // For Window\Tile Vertical menu item,
        MDITile(MDITILE_VERTICAL); // tile MDI child windows so that
        break;                     // one window appears beside another.
    }

    return TRUE;
}