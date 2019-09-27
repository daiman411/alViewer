#pragma once


// CDlgHistogram dialog

class CDlgHistogram : /*public CDialogEx,*/ public CDockablePane
{
	DECLARE_DYNAMIC(CDlgHistogram)

public:
	CDlgHistogram(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgHistogram();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HISTOGRAM };
#endif

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
