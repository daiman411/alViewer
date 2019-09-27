
// alViewerView.cpp : implementation of the CalViewerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "alViewer.h"
#endif

#include "alViewerDoc.h"
#include "alViewerView.h"
#include "MainFrm.h"

#include "CalBitmapData.h"
#include "CDrawCanvas.h"
#include <math.h>  

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CalViewerView

IMPLEMENT_DYNCREATE(CalViewerView, CView)

BEGIN_MESSAGE_MAP(CalViewerView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_KILLFOCUS()
ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

// CalViewerView construction/destruction

CalViewerView::CalViewerView()
{
	// TODO: add construction code here
	m_DrawMode = 2;
	m_Scale = 1.0;
	m_bMoving = false;
	m_bScaling = false;
	m_bSelecting = false;
	m_pTmpRect = nullptr;
	m_strText.Empty();
	m_nowPos.SetPoint(0, 0);
	m_OldPos.SetPoint(0, 0);
	m_Offset.SetPoint(0, 0);
	m_OffsetPre.SetPoint(0, 0);
	m_DrawRect.SetRect(0, 0, 0, 0);
	m_SelRect.SetRect(0, 0, 0, 0);
	m_ImgRect.SetRect(0, 0, 0, 0);
	SetScrollSizes(MM_TEXT, CSize(0, 0));
}

CalViewerView::~CalViewerView()
{
}

BOOL CalViewerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CalViewerView drawing

void CalViewerView::OnDraw(CDC* pDC)
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CRect rcClient;
	this->GetClientRect(&rcClient);
	COLORREF crPen = RGB(255,0,0);

	// Clear first
	if (m_pDrawCanvas)
	{
		Rect rcCanvas = CRect2Rect(rcClient);
		m_pDrawCanvas->ClearDraw(rcCanvas);

		// Draw to buffer
		m_pDrawCanvas->DrawImage(pDC->GetSafeHdc(), rcCanvas, CRect2Rect(m_DrawRect), pDoc->ImgBuffer(), pDoc->ImgBitmapInfo(), pDoc->ClearBuffer);
		pDoc->ClearBuffer = false;

		// Draw select area
		if (!m_SelRect.IsRectEmpty())
			m_pDrawCanvas->DrawFocusRect(CRect2Rect(m_SelRect), crPen);

		// Draw output text
		if (!m_strText.IsEmpty())
			m_pDrawCanvas->DrawStringText(m_strText.GetBuffer(), Rect(0, 0, 200, 100), crPen);

		// render it~
		m_pDrawCanvas->RePaint(pDC->GetSafeHdc(), rcCanvas);
	}

	pDoc->UpdateStatusRatio(m_Scale);
}


// CalViewerView printing

BOOL CalViewerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CalViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CalViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CalViewerView diagnostics

#ifdef _DEBUG
void CalViewerView::AssertValid() const
{
	CView::AssertValid();
}

void CalViewerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CalViewerDoc* CalViewerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CalViewerDoc)));
	return (CalViewerDoc*)m_pDocument;
}
#endif //_DEBUG


// CalViewerView message handlers
void CalViewerView::OnInitialUpdate()
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	m_DrawMode = pDoc->DocMode == 2 ? 0 : 2;
	m_pDrawCanvas = std::unique_ptr<CDrawCanvas>(new CDrawCanvas(this->GetSafeHwnd(), 3));


	// TODO: add draw code for native data here
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);

	m_DrawRect = CalcDrawROI(pDoc, rcClient);

	ResyncScrollSizes();
	CScrollView::OnInitialUpdate();
}

void CalViewerView::ResyncScrollSizes()
{
	CClientDC dc(NULL);
	OnPrepareDC(&dc);
	CSize sizeDoc = GetDocument()->GetDocSize();
	dc.LPtoDP(&sizeDoc);
	SetScrollSizes(MM_TEXT, sizeDoc);
}

void CalViewerView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CalViewerDoc* pDoc = GetDocument();
	CScrollView::OnPrepareDC(pDC, pInfo);

	//pDC->SetMapMode(MM_ANISOTROPIC);
	//CSize sizeDoc = pDoc->GetDocSize();
	//sizeDoc.cy = -sizeDoc.cy;
	//pDC->SetWindowExt(sizeDoc);

	//CSize sizeNum, sizeDenom;
	//pDoc->GetZoomFactor(&sizeNum, &sizeDenom);

	//int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	//int yLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSY);

	//long xExt = (long)sizeDoc.cx * xLogPixPerInch * sizeNum.cx;
	//xExt /= 100 * (long)sizeDenom.cx;
	//long yExt = (long)sizeDoc.cy * yLogPixPerInch * sizeNum.cy;
	//yExt /= 100 * (long)sizeDenom.cy;
	//pDC->SetViewportExt((int)xExt, (int)-yExt);
}


CPoint CalViewerView::CalcDrawOffset(CRect &rcImgROI, CPoint center_pt)
{
	CPoint offset;
	CRect rcClient;
	this->GetClientRect(&rcClient);
	//this->GetWindowRect(&rcClient);

	offset.x = center_pt.x - rcImgROI.Width() / 2;
	offset.y = center_pt.y - rcImgROI.Height() / 2;
	return offset;
}

double CalViewerView::GetBestFitScale(CRect &client, CRect &image)
{
	double scale_x = client.Width() / (double)(image.Width());
	double scale_y = client.Height() / (double)(image.Height());

	return ((scale_x > scale_y) ? scale_y : scale_x);
}

CRect CalViewerView::CalcDrawRect(CRect &client_rect, CPoint &offset, double scale, int w, int h)
{
	CRect show_rect;
	show_rect.left = offset.x;
	show_rect.top = offset.y;
	show_rect.bottom = show_rect.top + (LONG)(h * scale + 0.5);
	show_rect.right = show_rect.left + (LONG)(w * scale + 0.5);

	offset = CalcDrawOffset(show_rect, CPoint(client_rect.Width() / 2, client_rect.Height() / 2));
	show_rect.OffsetRect(offset);

	return show_rect;
}

CRect CalViewerView::CalcScaleRect(CRect draw_rect, double scale, int w, int h)
{
	CRect show_rect;

	int cX = draw_rect.CenterPoint().x;
	int cY = draw_rect.CenterPoint().y;
	int nW = (int)round(w * scale);
	int nH = (int)round(h * scale);

	show_rect.left = cX - nW / 2;
	show_rect.top = cY - nH /2 ;
	show_rect.right = show_rect.left + nW;
	show_rect.bottom = show_rect.top + nH;

	//m_Offset.x = show_rect.left;
	//m_Offset.y = show_rect.top;

	return show_rect;
}

CRect CalViewerView::CalcDrawROI(CalViewerDoc* pDoc, CRect &rcClient)
{
	m_Offset.SetPoint(0, 0);

	CRect rcDraw;
	int imgW = abs(pDoc->ImgWidth());
	int imgH = abs(pDoc->ImgHeight());
	
	m_Offset.SetPoint(0, 0);
	CRect imageRect(0, 0, imgW, imgH);
	m_Scale = GetBestFitScale(rcClient, imageRect);
	rcDraw = CalcDrawRect(rcClient, m_Offset, m_Scale, imgW, imgH);
	rcDraw.NormalizeRect();

	return rcDraw;
}

#define FLOAT_MANTISSA_BITS 23
#define FLOAT_MANTISSA_MASK ((1<<(FLOAT_MANTISSA_BITS))-1)

/* Non-negative float values */
#define Float2Int(_flt_, _int_) \
    (*((float *)(&(_int_)))=(_flt_)+((float)(1<<(FLOAT_MANTISSA_BITS))), \
     (_int_)&=FLOAT_MANTISSA_MASK)

CPoint CalViewerView::ImagePtFromPB(CPoint &pt, CRect &rcDraw, int w , int h)
{
	CPoint retPt(0, 0);
	int posX(0), posY(0);
    Float2Int(((pt.x - rcDraw.TopLeft().x) / (float)rcDraw.Width() * w)  , posX);
	Float2Int(((pt.y - rcDraw.TopLeft().y) / (float)rcDraw.Height() * h) , posY);

    retPt.x = posX;
	retPt.y = posY;

	return retPt;
}


CPoint CalViewerView::ImageUPtFromPB(CPoint &pt, CRect &rcDraw, int w, int h)
{
    CPoint retPt(0, 0);
    int posX(0), posY(0);
    Float2Int(floor((pt.x - rcDraw.TopLeft().x) / (float)rcDraw.Width() * w), posX);
    Float2Int(floor((pt.y - rcDraw.TopLeft().y) / (float)rcDraw.Height() * h), posY);

    retPt.x = posX;
    retPt.y = posY;

    return retPt;
}

CPoint CalViewerView::PBPtFromImage(CPoint &pt, CRect &rcDraw, int w, int h)
{
	CPoint retPt(0, 0);
	int posX(0), posY(0);

	posX = (int)round( pt.x * m_Scale + (float)rcDraw.TopLeft().x);
	posY = (int)round( pt.y * m_Scale + (float)rcDraw.TopLeft().y);

	retPt.x = posX;
	retPt.y = posY;

	return retPt;
}

CRect CalViewerView::ImageRectFromPB(CRect &rc, CRect &rcDraw, int w, int h)
{
	CRect tmpRect;

	CPoint lt = ImagePtFromPB(rc.TopLeft(), rcDraw, w, h);
	CPoint rb = ImagePtFromPB(rc.BottomRight(), rcDraw, w, h);
	tmpRect.SetRect(lt, rb);

	return tmpRect;
}

CRect CalViewerView::PBRectFromImage(CRect &rc, CRect &rcDraw, int w, int h)
{
	CRect tmpRect;

	CPoint lt = PBPtFromImage(rc.TopLeft(), rcDraw, w, h);
	CPoint rb = PBPtFromImage(rc.BottomRight(), rcDraw, w, h);
	tmpRect.SetRect(lt, rb);

	return tmpRect;
}

BOOL CalViewerView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	//return CView::OnEraseBkgnd(pDC);
	return true;
}


void CalViewerView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_nowPos = point;
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	CalViewerDoc* pDoc = GetDocument();
	int width = pDoc->ImgWidth();
	int height = pDoc->ImgHeight();

	if (m_bMoving)
	{
		int dx = point.x - m_OffsetPre.x;
		int dy = point.y - m_OffsetPre.y;
		m_OffsetPre = point;
		m_Offset.x += dx;
		m_Offset.y += dy;
		m_DrawRect.OffsetRect(dx, dy);
		m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);
		Invalidate(FALSE);
	}
	else if (m_bSelecting)
	{
		m_pTmpRect->SetRect(m_OldPos.x, m_OldPos.y, point.x, point.y);
		m_pTmpRect->NormalizeRect();
		m_ImgRect = ImageRectFromPB(*m_pTmpRect, m_DrawRect, pDoc->ImgWidth(), pDoc->ImgHeight());
		Invalidate(FALSE);
	}

	if (m_DrawRect.PtInRect(point))
	{
		int width = pDoc->ImgWidth();
		int height = pDoc->ImgHeight();

		if (width && height)
		{
		    CPoint pt = ImageUPtFromPB(point, m_DrawRect, width, height);
		    PIXEL_DATA RGB = pDoc->BmpData()->GetPixel(pt.x, pt.y);
		    BYTE R = RGB.R;
		    BYTE G = RGB.G;
		    BYTE B = RGB.B;

		    pDoc->UpdateStatusPos(pt.x, pt.y);
		    pDoc->UpdateStatusRGB(R, G, B);
		    pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
		}
	}


	CView::OnMouseMove(nFlags, point);
}


void CalViewerView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_Scale = 1.0;

	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	int width = pDoc->ImgWidth();
	int height = pDoc->ImgHeight();

	//pDoc->ClearBitmapImage();
	// Reset draw image
	m_DrawRect = CalcDrawROI(pDoc, rcClient);
	m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);

	Invalidate(FALSE);

	pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
	CView::OnMButtonDown(nFlags, point);
}

BOOL CalViewerView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bSelecting)
		return CView::OnMouseWheel(nFlags, zDelta, pt);

	ZoomView(zDelta, true);

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CalViewerView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: Add your message handler code here
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	int width = pDoc->ImgWidth();
	int height = pDoc->ImgHeight();

	m_DrawRect = CalcDrawROI(pDoc, rcClient);
	m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);
    if (m_pDrawCanvas) { m_pDrawCanvas->UpdateWindowSize(); }
}


void CalViewerView::OnSizing(UINT fwSide, LPRECT pRect)
{
	CView::OnSizing(fwSide, pRect);

	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	int width = pDoc->ImgWidth();
	int height = pDoc->ImgHeight();

	// TODO: Add your message handler code here
	m_DrawRect = CalcDrawROI(pDoc, CRect(pRect));
	m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);
    if (m_pDrawCanvas) { m_pDrawCanvas->UpdateWindowSize(); }
}


void CalViewerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CMainFrame* pMainFrm = static_cast<CMainFrame*>(AfxGetMainWnd());
	CalViewerDoc* pDoc = GetDocument();

	if (pMainFrm->IsSelectMode())
	{
		if (m_DrawRect.PtInRect(point))
		{
			m_bSelecting = true;
			m_ImgRect.SetRectEmpty();

			m_OldPos = point;
			m_SelRect.SetRect(m_OldPos.x, m_OldPos.y, m_OldPos.x + 1, m_OldPos.y + 1);
			m_SelRect.NormalizeRect();
			m_pTmpRect = &m_SelRect;

			m_ImgRect = ImageRectFromPB(*m_pTmpRect, m_DrawRect, pDoc->ImgWidth(), pDoc->ImgHeight());
			pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
		}
	}

	CScrollView::OnLButtonDown(nFlags, point);
}


void CalViewerView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMainFrame* pMainFrm = static_cast<CMainFrame*>(AfxGetMainWnd());
	CalViewerDoc* pDoc = GetDocument();

	if (pMainFrm->IsSelectMode())
	{
		if (m_bSelecting)
		{
			if (m_pTmpRect)
			{
				m_pTmpRect->SetRect(m_OldPos.x, m_OldPos.y, point.x, point.y);
				m_pTmpRect->NormalizeRect();

				if (m_pTmpRect->Width() < 2 && m_pTmpRect->Height() < 2)
				{
					m_pTmpRect->SetRectEmpty();
					m_ImgRect.SetRectEmpty();
					m_strText.Empty();
				}
				else
				{
					m_ImgRect = ImageRectFromPB(*m_pTmpRect, m_DrawRect, pDoc->ImgWidth(), pDoc->ImgHeight());

					// Calculate mean value
					PIXEL_STAT stat_mean = pDoc->BmpData()->GetSTAT(m_ImgRect);
					m_strText.Format(_T("R mean: %.3f\r\nGr mean: %.3f\r\nGb mean: %.3f\r\nB mean: %.3f\r\n"),
						stat_mean.R_Mean, stat_mean.Gr_Mean, stat_mean.Gb_Mean, stat_mean.B_Mean);

                    CString strMax;
                    strMax.Format(_T("\r\nR max: %.3f\r\nGr max: %.3f\r\nGb max: %.3f\r\nB max: %.3f\r\n"),
                        stat_mean.R_Max, stat_mean.Gr_Max, stat_mean.Gb_Max, stat_mean.B_Max);

                    CString strMin;
                    strMin.Format(_T("\r\nR min: %.3f\r\nGr min: %.3f\r\nGb min: %.3f\r\nB min: %.3f\r\n"),
                        stat_mean.R_Min, stat_mean.Gr_Min, stat_mean.Gb_Min, stat_mean.B_Min);

                    m_strText.Append(strMax);
                    m_strText.Append(strMin);
				}

				m_pTmpRect = nullptr;
				Invalidate(FALSE);
				pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
			}
		}
	}

	m_bSelecting = false;
	CScrollView::OnLButtonUp(nFlags, point);
}

void CalViewerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bMoving = true;
	m_OffsetPre = point;

	CScrollView::OnRButtonDown(nFlags, point);
}


void CalViewerView::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_bMoving = false;

	CScrollView::OnRButtonUp(nFlags, point);
}

void CalViewerView::ZoomView(short zDelta, bool cursor)
{
	CalViewerDoc* pDoc = GetDocument();
	CPoint cenPoint;

	if (m_DrawRect.PtInRect(m_nowPos) && cursor)
	{
		cenPoint = m_nowPos;
	}
	else
	{
		cenPoint = m_DrawRect.CenterPoint();
	}

	if (zDelta > 0)
	{
		int width = pDoc->ImgWidth();
		int height = pDoc->ImgHeight();

		CPoint BfImgPt = ImagePtFromPB(cenPoint, m_DrawRect, width, height);

		int Odx = m_DrawRect.CenterPoint().x - cenPoint.x;
		int Ody = m_DrawRect.CenterPoint().y - cenPoint.y;

		m_Scale += m_Scale * 0.2;
		m_DrawRect = CalcScaleRect(m_DrawRect, m_Scale, width, height);

		CPoint BfPBPt = PBPtFromImage(BfImgPt, m_DrawRect, width, height);

		int Ndx = m_DrawRect.CenterPoint().x - BfPBPt.x;
		int Ndy = m_DrawRect.CenterPoint().y - BfPBPt.y;

		int ddx = Ndx - Odx;
		int ddy = Ndy - Ody;

		int deltaX = int((ddx));
		int deltaY = int((ddy));
		m_DrawRect.OffsetRect(deltaX, deltaY);
		m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);
	}
	else if (zDelta < 0)
	{
		int width = pDoc->ImgWidth();
		int height = pDoc->ImgHeight();

		CPoint BfImgPt = ImagePtFromPB(cenPoint, m_DrawRect, width, height);

		int Odx = m_DrawRect.CenterPoint().x - cenPoint.x;
		int Ody = m_DrawRect.CenterPoint().y - cenPoint.y;

		m_Scale -= m_Scale * 0.2;
		m_DrawRect = CalcScaleRect(m_DrawRect, m_Scale, width, height);

		CPoint BfPBPt = PBPtFromImage(BfImgPt, m_DrawRect, width, height);

		int Ndx = m_DrawRect.CenterPoint().x - BfPBPt.x;
		int Ndy = m_DrawRect.CenterPoint().y - BfPBPt.y;

		int ddx = Ndx - Odx;
		int ddy = Ndy - Ody;

		int deltaX = int((ddx));
		int deltaY = int((ddy));
		m_DrawRect.OffsetRect(deltaX, deltaY);
		m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);
	}

	Invalidate(FALSE);
}

void CalViewerView::ZoomView_In()
{
	ZoomView(1);
}

void CalViewerView::ZoomView_Out()
{
	ZoomView(-1);
}

void CalViewerView::ZoomView_Fit()
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	int width = pDoc->ImgWidth();
	int height = pDoc->ImgHeight();

	// Reset draw image
	m_Scale = 1.0;
	m_DrawRect = CalcDrawROI(pDoc, rcClient);
	m_SelRect = PBRectFromImage(m_ImgRect, m_DrawRect, width, height);

	Invalidate(FALSE);

	pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
}

void CalViewerView::MoveNextOne()
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->MoveToNextImage();
	Invalidate(FALSE);
}

void CalViewerView::MovePrvousOne()
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->MoveToPreImage();
	Invalidate(FALSE);
}

void CalViewerView::UpdateDrawView()
{
	CalViewerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect rcClient(0, 0, 0, 0);
	GetClientRect(rcClient);
	m_DrawRect = CalcDrawROI(pDoc, rcClient);
}

void CalViewerView::OnKillFocus(CWnd* pNewWnd)
{
	CScrollView::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	m_bMoving = false;
	m_bSelecting = false;
}


void CalViewerView::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnMouseLeave();

	m_bMoving = false;
	m_bSelecting = false;
}

Rect CalViewerView::CRect2Rect(CRect &rc)
{
	return Rect(rc.left, rc.top, rc.Width(), rc.Height());
}


BOOL CalViewerView::PreTranslateMessage(MSG* pMsg)
{
	//  TODO:  Add  your  specialized  code  here  and/or  call  the  base  class
	if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case _T('S'):
		{
			CTime ct;
			ct = CTime::GetCurrentTime();
			CString strDate = ct.Format(_T("_%Y%m%d-%H%M%S"));
			int nRawImageSize = 0;

			CalViewerDoc* doc = (CalViewerDoc*)this->GetDocument();
			CString loadPicName = doc->ImagePath();
			CString savePicPath = loadPicName.Mid(0, loadPicName.ReverseFind(_T('.')));
			savePicPath.Append(strDate);
			savePicPath += _T(".png");
			doc->SaveImageFile(savePicPath);
		}
		break;

		case _T('A'):
		{
			if (GetKeyState(VK_CONTROL) != 0)
			{
				UpdateDrawView();
				CalViewerDoc* pDoc = (CalViewerDoc*)this->GetDocument();

				CRect wholeImage = CRect(0, 0, pDoc->ImgWidth(), pDoc->ImgHeight());			   			
				m_SelRect = PBRectFromImage(wholeImage, m_DrawRect, pDoc->ImgWidth(), pDoc->ImgHeight());
				m_ImgRect = wholeImage;

				// Calculate mean value
				PIXEL_STAT stat_mean = pDoc->BmpData()->GetSTAT(wholeImage);
				m_strText.Format(_T("R mean: %.3f\r\nGr mean: %.3f\r\nGb mean: %.3f\r\nB mean: %.3f\r\n"),
					stat_mean.R_Mean, stat_mean.Gr_Mean, stat_mean.Gb_Mean, stat_mean.B_Mean);

                CString strMax;
                strMax.Format(_T("\r\nR max: %.3f\r\nGr max: %.3f\r\nGb max: %.3f\r\nB max: %.3f\r\n"),
                    stat_mean.R_Max, stat_mean.Gr_Max, stat_mean.Gb_Max, stat_mean.B_Max);

                CString strMin;
                strMin.Format(_T("\r\nR min: %.3f\r\nGr min: %.3f\r\nGb min: %.3f\r\nB min: %.3f\r\n"),
                    stat_mean.R_Min, stat_mean.Gr_Min, stat_mean.Gb_Min, stat_mean.B_Min);

                m_strText.Append(strMax);
                m_strText.Append(strMin);

				Invalidate(FALSE);
				pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
			}
		}
		break;

        case _T('X'):
        {
            if (GetKeyState(VK_CONTROL) != 0)
            {
                CTime ct;
                ct = CTime::GetCurrentTime();
                CString strDate = ct.Format(_T("_%Y%m%d-%H%M%S"));
                int nRawImageSize = 0;

                CalViewerDoc* doc = (CalViewerDoc*)this->GetDocument();
                CString loadPicName = doc->ImagePath();
                CString savePicPath = loadPicName.Mid(0, loadPicName.ReverseFind(_T('.')));
                savePicPath.Append(strDate);
                savePicPath += _T(".bin");
                doc->SaveRawDataFile(savePicPath);
            }
        }
        break;

        case _T('M'):
        {
            if (GetKeyState(VK_CONTROL) != 0)
            {
                UpdateDrawView();
                CalViewerDoc* pDoc = (CalViewerDoc*)this->GetDocument();
                pDoc->ExecuteImgProc(110);

                Invalidate(FALSE);
                pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
            }
        }
        break;

        case _T('F'):
        {
            if (GetKeyState(VK_CONTROL) != 0)
            {
                UpdateDrawView();
                CalViewerDoc* pDoc = (CalViewerDoc*)this->GetDocument();
                pDoc->ExecuteImgProc(111);

                Invalidate(FALSE);
                pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
            }
        }
        break;

        case _T('R'):
        {
            if (GetKeyState(VK_CONTROL) != 0)
            {
                UpdateDrawView();
                CalViewerDoc* pDoc = (CalViewerDoc*)this->GetDocument();
                pDoc->ExecuteImgProc(100);

                //Fit to view size
                ZoomView_Fit();
                Invalidate(FALSE);
                pDoc->UpdateStatusRect(m_ImgRect.left, m_ImgRect.top, m_ImgRect.Width(), m_ImgRect.Height());
            }
        }
        break;
		}
	}

	return  CView::PreTranslateMessage(pMsg);
}
