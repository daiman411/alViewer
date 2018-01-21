// DlgRawPaser.cpp : implementation file
//

#include "stdafx.h"
#include "alViewer.h"
#include "alViewerDoc.h"
#include "DlgRawPaser.h"
#include "afxdialogex.h"
#include "CalBitmapData.h"
#include "CDRawCanvas.h"

// CDlgRawPaser dialog

IMPLEMENT_DYNAMIC(CDlgRawPaser, CDialogEx)

CDlgRawPaser::CDlgRawPaser(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_RAWPASER, pParent)
{
	m_width = 720;
	m_height = 480;
	m_header = 0;
	m_stride = -1;
	m_format = 0;
	m_colorOrder = 0;
	m_bInitDone = false;
	m_bReloadFile = false;
	m_ImgPath.Empty();
}

CDlgRawPaser::CDlgRawPaser(CWnd* pParent, CDocument* pDoc)
	: CDialogEx(IDD_DLG_RAWPASER, pParent)
{
	m_width = 720;
	m_height = 480;
	m_header = 0;
	m_stride = -1;
	m_format = 0;
	m_colorOrder = 0;
	m_ImgPath.Empty();
	m_bInitDone = false;
	m_bReloadFile = false;
	m_pMainDoc = (CalViewerDoc*)(pDoc);
}


CDlgRawPaser::~CDlgRawPaser()
{
}

void CDlgRawPaser::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RAW_W, m_tbWidth);
	DDX_Control(pDX, IDC_EDIT_RAW_H, m_tbHeight);
	DDX_Control(pDX, IDC_EDIT_HEADER_SIZE, m_tbHeader);
	DDX_Control(pDX, IDC_CK_RGGB, m_ck_RGGB);
	DDX_Control(pDX, IDC_CK_GRBG, m_ck_GRBG);
	DDX_Control(pDX, IDC_CK_GBRG, m_ck_GBRG);
	DDX_Control(pDX, IDC_CK_BGGR, m_ck_BGGR);
	DDX_Control(pDX, IDC_CMB_IMG_FORMAT, m_cb_ImageFormat);
	DDX_Control(pDX, IDC_EDIT_STRIDE, m_tbStride);
}


BEGIN_MESSAGE_MAP(CDlgRawPaser, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgRawPaser::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRawPaser::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_CMB_IMG_FORMAT, &CDlgRawPaser::OnCbnSelchangeCmbImgFormat)
	ON_BN_CLICKED(IDC_CK_RGGB, &CDlgRawPaser::OnBnClickedCkRggb)
	ON_BN_CLICKED(IDC_CK_GRBG, &CDlgRawPaser::OnBnClickedCkGrbg)
	ON_BN_CLICKED(IDC_CK_GBRG, &CDlgRawPaser::OnBnClickedCkGbrg)
	ON_BN_CLICKED(IDC_CK_BGGR, &CDlgRawPaser::OnBnClickedCkBggr)
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_EDIT_RAW_W, &CDlgRawPaser::OnEnChangeEditRawW)
	ON_EN_CHANGE(IDC_EDIT_RAW_H, &CDlgRawPaser::OnEnChangeEditRawH)
	ON_EN_CHANGE(IDC_EDIT_HEADER_SIZE, &CDlgRawPaser::OnEnChangeEditHeaderSize)
	ON_EN_CHANGE(IDC_EDIT_STRIDE, &CDlgRawPaser::OnEnChangeEditStride)
END_MESSAGE_MAP()


// CDlgRawPaser message handlers
BOOL CDlgRawPaser::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_pDrawCanvas = std::unique_ptr<CDrawCanvas>(new CDrawCanvas(this->GetSafeHwnd()));

	// Check file size 
	ULONGLONG fileSize = QueryFileSize(m_ImgPath);

	if (m_width*m_height > fileSize)
	{
		m_width = 720;
		m_height = 480;
	}

	CString tmp;
	tmp.Format(_T("%d"), m_width);
	m_tbWidth.SetWindowText(tmp);

	tmp.Format(_T("%d"), m_height);
	m_tbHeight.SetWindowText(tmp);

	tmp.Format(_T("%d"), m_stride);
	m_tbStride.SetWindowText(tmp);

	m_tbHeader.SetWindowText(_T("0"));
	m_cb_ImageFormat.SetCurSel(m_format);
	m_ck_RGGB.SetCheck(BST_CHECKED);
	m_ck_GRBG.SetCheck(BST_UNCHECKED);
	m_ck_GBRG.SetCheck(BST_UNCHECKED);
	m_ck_BGGR.SetCheck(BST_UNCHECKED);

	RAW_FORMAT_PK format_pk;
	format_pk.RAW_WIDTH = m_width;
	format_pk.RAW_HEIGHT = m_height;
	format_pk.RAW_HEADER = m_header;
	format_pk.RAW_FORMAT = ConvertRawFormat(m_format);;
	format_pk.RAW_STRIDE = m_stride;
	format_pk.RAW_CR_ORDER = m_colorOrder;

	if (!m_pMainDoc->BmpData()->LoadRAWFile(m_ImgPath.GetBuffer(), format_pk))
	{
		AfxMessageBox(_T("Can't paser raw data"));
	}

	// Update stride
	m_bReloadFile = false;
	m_stride = format_pk.RAW_PITCHOUT;
	tmp.Format(_T("%d"), m_stride);
	m_tbStride.SetWindowText(tmp);

	m_bInitDone = true;

	return TRUE;
}

void CDlgRawPaser::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CDlgRawPaser::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}


void CDlgRawPaser::OnCbnSelchangeCmbImgFormat()
{
	// TODO: Add your control notification handler code here
	m_stride = -1;
	m_format = m_cb_ImageFormat.GetCurSel();
	
	ReloadRawFileData(true);
	Invalidate();
}


void CDlgRawPaser::OnBnClickedCkRggb()
{
	// TODO: Add your control notification handler code here
	m_ck_RGGB.SetCheck(BST_CHECKED);
	m_ck_GRBG.SetCheck(BST_UNCHECKED);
	m_ck_GBRG.SetCheck(BST_UNCHECKED);
	m_ck_BGGR.SetCheck(BST_UNCHECKED);

	m_colorOrder = 0;

	ReloadRawFileData(true);
	Invalidate();
}


void CDlgRawPaser::OnBnClickedCkGrbg()
{
	// TODO: Add your control notification handler code here
	m_ck_RGGB.SetCheck(BST_UNCHECKED);
	m_ck_GRBG.SetCheck(BST_CHECKED);
	m_ck_GBRG.SetCheck(BST_UNCHECKED);
	m_ck_BGGR.SetCheck(BST_UNCHECKED);

	m_colorOrder = 1;

	ReloadRawFileData(true);
	Invalidate();
}


void CDlgRawPaser::OnBnClickedCkGbrg()
{
	// TODO: Add your control notification handler code here
	m_ck_RGGB.SetCheck(BST_UNCHECKED);
	m_ck_GRBG.SetCheck(BST_UNCHECKED);
	m_ck_GBRG.SetCheck(BST_CHECKED);
	m_ck_BGGR.SetCheck(BST_UNCHECKED);

	m_colorOrder = 2;

	ReloadRawFileData(true);
	Invalidate();
}


void CDlgRawPaser::OnBnClickedCkBggr()
{
	// TODO: Add your control notification handler code here
	m_ck_RGGB.SetCheck(BST_UNCHECKED);
	m_ck_GRBG.SetCheck(BST_UNCHECKED);
	m_ck_GBRG.SetCheck(BST_UNCHECKED);
	m_ck_BGGR.SetCheck(BST_CHECKED);

	m_colorOrder = 3;

	ReloadRawFileData(true);
	Invalidate();
}


void CDlgRawPaser::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages

					   // TODO: add draw code for native data here
	
	CRect rcPreview;
	GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rcPreview);
	CDC* pDC = GetDlgItem(IDC_PREVIEW)->GetDC();
	Rect rcView(rcPreview.left, rcPreview.top, rcPreview.Width(), rcPreview.Height());

	// Clear first
	m_pDrawCanvas->ClearDraw(rcView);

	// Draw to buffer 
	CRect rcDraw = CalcDrawROI(m_pMainDoc, rcPreview);
	Gdiplus::Rect rcCalDraw(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height());
	m_pDrawCanvas->DrawImage(pDC->GetSafeHdc(), rcView, rcCalDraw, m_pMainDoc->ImgBuffer(), m_pMainDoc->ImgBitmapInfo());

	// render it~
	m_pDrawCanvas->RePaint(pDC->GetSafeHdc(), rcView);
}

CRect CDlgRawPaser::CalcDrawROI(CalViewerDoc* pDoc, CRect &rcClient)
{
	CRect rcDraw;
	int imgW = abs(pDoc->ImgWidth());
	int imgH = abs(pDoc->ImgHeight());
	int leftPos = 0;
	int topPos = 0;

	float fRatioW = (float)rcClient.Width() / (float)(imgW);
	float fRatioH = (float)rcClient.Height() / (float)(imgH);

	float fRatio = min(fRatioW, fRatioH);

	if (fRatio > 1.0)
		fRatio = 1.0;

	int nDispW = (int)((float)imgW * fRatio);
	int nDispH = (int)((float)imgH * fRatio);

	if (rcClient.Width() > nDispW)
	{
		leftPos = (rcClient.Width() - nDispW) / 2;
	}

	if (rcClient.Height() > nDispH)
	{
		topPos = (rcClient.Height() - nDispH) / 2;
	}

	rcDraw.SetRect(leftPos, topPos, leftPos + nDispW, topPos + nDispH);
	rcDraw.NormalizeRect();

	return rcDraw;
}


void CDlgRawPaser::OnEnChangeEditRawW()
{
	if (m_bInitDone)
	{
		CString strTmp;
		m_tbWidth.GetWindowText(strTmp);
		m_width = _ttoi(strTmp.GetBuffer());

		m_width = max(m_width, 1);
		m_stride = -1;

		ReloadRawFileData(true);
		Invalidate();
	}
}


void CDlgRawPaser::OnEnChangeEditRawH()
{
	if (m_bInitDone)
	{
		CString strTmp;
		m_tbHeight.GetWindowText(strTmp);
		m_height = _ttoi(strTmp.GetBuffer());

		m_height = max(m_height, 1);
		m_stride = -1;

		ReloadRawFileData(true);
		Invalidate();
	}
}


void CDlgRawPaser::OnEnChangeEditHeaderSize()
{
	if (m_bInitDone)
	{
		CString strTmp;
		m_tbHeader.GetWindowText(strTmp);
		m_header = _ttoi(strTmp.GetBuffer());
		m_stride = -1;

		ReloadRawFileData(m_bReloadFile);
		Invalidate();
	}
}


void CDlgRawPaser::OnEnChangeEditStride()
{
	if (m_bInitDone)
	{
		CString strTmp;
		m_tbStride.GetWindowText(strTmp);
		m_stride = _ttoi(strTmp.GetBuffer());

		ReloadRawFileData(m_bReloadFile);
		Invalidate();
	}
}

void CDlgRawPaser::ReloadRawFileData(bool bReload)
{
	if (bReload)
	{
		RAW_FORMAT_PK format_pk;
		format_pk.RAW_WIDTH = m_width;
		format_pk.RAW_HEIGHT = m_height;
		format_pk.RAW_HEADER = m_header;
		format_pk.RAW_FORMAT = ConvertRawFormat(m_format);
		format_pk.RAW_STRIDE = m_stride;
		format_pk.RAW_CR_ORDER = m_colorOrder;

		if (!m_pMainDoc->BmpData()->LoadRAWFile(m_ImgPath.GetBuffer(), format_pk))
		{
			AfxMessageBox(_T("Can't paser raw data"));
		}

		m_bReloadFile = false;
		m_stride = format_pk.RAW_PITCHOUT;

		CString tmp;
		tmp.Format(_T("%d"), m_stride);
		m_tbStride.SetWindowText(tmp);
		m_bReloadFile = true;
	}
}

int CDlgRawPaser::GetWidth()
{ 
	return m_width; 
}

int CDlgRawPaser::GetHeight() 
{ 
	return m_height; 
}

int CDlgRawPaser::GetHeader() 
{ 
	return m_header; 
}

int CDlgRawPaser::GetStride() 
{ 
	return m_stride; 
}

int CDlgRawPaser::GetFormat() 
{ 
	switch (m_format)
	{
	case 0:
		return RAW_8BITS;
		break;

	case 1:
		return RAW_16BITS;
		break;

	case 2:
		return RAW_MIPI_8BITS;
		break;

	case 3:
		return RAW_MIPI_10BITS;
		break;

	case 4:
		return RAW_MIPI_12BITS;
		break;

	case 5:
		return RAW_QCOMM_8BITS;
		break;

	case 6:
		return RAW_QCOMM_10BITS;
		break;

	case 7:
		return RAW_QCOMM_12BITS;
		break;

	default:
		return RAW_8BITS;
		break;
	}
}

int CDlgRawPaser::GetColorOrder() 
{ 
	return m_colorOrder; 
}

void CDlgRawPaser::SetRawPath(CString path)
{ 
	m_ImgPath = path; 
}

void CDlgRawPaser::SetWidth(int w) 
{ 
	m_width = w; 
	m_width = min(m_width, 8000);
}

void CDlgRawPaser::SetHeight(int h) 
{ 
	m_height = h; 
	m_height = min(m_height, 8000);
}

void CDlgRawPaser::SetFormat(int format) 
{ 

	switch (format)
	{
	case RAW_8BITS:
		m_format = 0;
		break;

	case RAW_16BITS:
		m_format = 1;
		break;

	case RAW_MIPI_8BITS:
		m_format = 2;
		break;

	case RAW_MIPI_10BITS:
		m_format = 3;
		break;

	case RAW_MIPI_12BITS:
		m_format = 4;
		break;

	case RAW_QCOMM_8BITS:
		m_format = 5;
		break;

	case RAW_QCOMM_10BITS:
		m_format = 6;
		break;

	case RAW_QCOMM_12BITS:
		m_format = 7;
		break;
	}

}

int CDlgRawPaser::ConvertRawFormat(int index)
{
	int raw_format = RAW_8BITS;

	switch (index)
	{
	case 0:
		raw_format = RAW_8BITS;
		break;

	case 1:
		raw_format = RAW_16BITS;
		break;

	case 2:
		raw_format = RAW_MIPI_8BITS;
		break;

	case 3:
		raw_format = RAW_MIPI_10BITS;
		break;

	case 4:
		raw_format = RAW_MIPI_12BITS;
		break;

	case 5:
		raw_format = RAW_QCOMM_8BITS;
		break;

	case 6:
		raw_format = RAW_QCOMM_10BITS;
		break;

	case 7:
		raw_format = RAW_QCOMM_12BITS;
		break;

	default:
		raw_format = RAW_8BITS;
		break;
	}

	return raw_format;
}

ULONGLONG CDlgRawPaser::QueryFileSize(CString rawPath)
{
	ULONGLONG rawFileSize = 0;

	// Load input data
	CFile File;
	if (!File.Open(rawPath.GetBuffer(), CFile::modeRead | CFile::typeBinary))
		return rawFileSize;

	rawFileSize = File.GetLength();
	File.Close();

	return rawFileSize;
}