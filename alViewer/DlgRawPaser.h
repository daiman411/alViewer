#pragma once
#include "afxwin.h"
#include <memory>

// CDlgRawPaser dialog

class CDrawCanvas;
class CalViewerDoc;
class CDlgRawPaser : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRawPaser)

public:
	CDlgRawPaser(CWnd* pParent = NULL);   // standard constructor
	CDlgRawPaser(CWnd* pParent, CDocument* pDoc);
	virtual ~CDlgRawPaser();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_RAWPASER };
#endif


public:
	int GetWidth();
	int GetHeight();
	int GetHeader();
	int GetStride();
	int GetFormat();
	int GetColorOrder();

	void SetWidth(int w);
	void SetHeight(int h);
	void SetFormat(int format);
	void SetRawPath(CString path);

private:
	int m_width;
	int m_height;
	int m_header;
	int m_stride;
    int m_blvClamp;
	int m_format;
	int m_colorOrder;
	CString m_ImgPath;
	bool m_bInitDone;
	bool m_bReloadFile;

private:
	CalViewerDoc* m_pMainDoc;
	std::unique_ptr<CDrawCanvas> m_pDrawCanvas;

private:
	void ReloadRawFileData(bool bReload, bool bUpdateStride=true);
	int ConvertRawFormat(int index);
	ULONGLONG QueryFileSize(CString rawPath);
	CRect CalcDrawROI(CalViewerDoc* pDoc, CRect &rcClient);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_tbWidth;
	CEdit m_tbHeight;
	CEdit m_tbHeader;
	CEdit m_tbPitch;
	CEdit m_tbStride;
    CEdit m_tbBLVClamp;
    CButton m_ck_RGGB;
	CButton m_ck_GRBG;
	CButton m_ck_GBRG;
	CButton m_ck_BGGR;
	CComboBox m_cb_ImageFormat;

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeCmbImgFormat();
	afx_msg void OnBnClickedCkRggb();
	afx_msg void OnBnClickedCkGrbg();
	afx_msg void OnBnClickedCkGbrg();
	afx_msg void OnBnClickedCkBggr();
	afx_msg void OnPaint();
	afx_msg void OnEnChangeEditRawW();
	afx_msg void OnEnChangeEditRawH();
	afx_msg void OnEnChangeEditHeaderSize();
	afx_msg void OnEnChangeEditStride();
    afx_msg void OnEnChangeEditBlacklv();
    
};
