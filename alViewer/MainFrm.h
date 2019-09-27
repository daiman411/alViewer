
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar          m_wndToolBar;
	CToolBar          m_wndVideoBar;
	CStatusBar        m_wndStatusBar;

	bool			  m_bEnableToolbar;
	bool			  m_bEnableVideobar;

public:
	TOOL_FUNC		  m_ToolBarFunc;

public:
	bool IsSelectMode() { return m_ToolBarFunc == TOOL_FUNC_SELECT; }
	void DockControlBarLeftOf(CToolBar* Bar, CToolBar* LeftOf);
	CDocument* GetDocument();
    BOOL OnWindowCommand(UINT nID);

public:
	LRESULT OnUpdateStatusSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatusPOS(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatusRGB(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatusRect(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateStatusRatio(WPARAM wParam, LPARAM lParam);
	LRESULT OnEnableToolbarStatus(WPARAM wParam, LPARAM lParam);
	LRESULT OnDisableToolbarStatus(WPARAM wParam, LPARAM lParam);
	LRESULT OnUpdateDrawViewRect(WPARAM wParam, LPARAM lParam);
	LRESULT OnStopPreviewVideo(WPARAM wParam, LPARAM lParam);

// Generated message map functions
protected:
	afx_msg void OnFileOpen();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnUpdateViewerArrow(CCmdUI *pCmdUI);
	afx_msg void OnViewerArrow();
	afx_msg void OnUpdateViewerSelect(CCmdUI *pCmdUI);
	afx_msg void OnViewerSelect();
	afx_msg void OnUpdateViewerZoomin(CCmdUI *pCmdUI);
	afx_msg void OnViewerZoomin();
	afx_msg void OnUpdateViewerZoomout(CCmdUI *pCmdUI);
	afx_msg void OnViewerZoomout();
	afx_msg void OnViewerZoomfit();
	afx_msg void OnViewerPreOne();
	afx_msg void OnViewerNextOne();
	afx_msg void OnUpdateViewerZoomfit(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewerPreOne(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewerNextOne(CCmdUI *pCmdUI);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdatePrevieweStart(CCmdUI *pCmdUI);
	afx_msg void OnPrevieweStart();
	afx_msg void OnUpdatePrevieweStop(CCmdUI *pCmdUI);
	afx_msg void OnPrevieweStop();
	afx_msg void OnViewShowHist();
	afx_msg void OnUpdateViewShowHist(CCmdUI *pCmdUI);

};


