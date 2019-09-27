
// alViewerView.h : interface of the CalViewerView class
//

#pragma once
#include <memory>

class CDrawCanvas;
class CalViewerDoc;
class CalViewerView : public CScrollView
{
protected: // create from serialization only
	CalViewerView();
	DECLARE_DYNCREATE(CalViewerView)

// Attributes
public:
	CalViewerDoc* GetDocument() const;
	void ResyncScrollSizes();

// Operations
public:
	void ZoomView_In();
	void ZoomView_Out();
	void ZoomView_Fit();
	void MoveNextOne();
	void MovePrvousOne();
	void UpdateDrawView();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
public:
	virtual ~CalViewerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool m_bMoving;
	bool m_bScaling;
	bool m_bSelecting;
	int m_DrawMode;
	double m_Scale;
	CPoint m_nowPos;
	CPoint m_OldPos;
	CPoint m_Offset;
	CPoint m_OffsetPre;
	CRect m_DrawRect;
	CRect m_SelRect;
	CRect m_ImgRect;
	CRect *m_pTmpRect;
	CString m_strText;
	std::unique_ptr<CDrawCanvas> m_pDrawCanvas;

private:
	CPoint ImagePtFromPB(CPoint &pt, CRect &rcDraw, int w, int h);
    CPoint ImageUPtFromPB(CPoint &pt, CRect &rcDraw, int w, int h);
    CPoint PBPtFromImage(CPoint &pt, CRect &rcDraw, int w, int h);
	CRect ImageRectFromPB(CRect &rc, CRect &rcDraw, int w, int h);
	CRect PBRectFromImage(CRect &rc, CRect &rcDraw, int w, int h);
	CRect CalcDrawROI(CalViewerDoc* pDoc, CRect &rcClient);
	CRect CalcDrawRect(CRect &client_rect, CPoint &offset, double scale, int w, int h);
	CRect CalcScaleRect(CRect draw_rect, double scale, int w, int h);
	CPoint CalcDrawOffset(CRect &rcImgROI, CPoint center_pt);
	Rect CRect2Rect(CRect &rc);
	double GetBestFitScale(CRect &client, CRect &image);
	void ZoomView(short zDelta, bool cursor = false);

// Generated message map functions
protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseLeave();
};

#ifndef _DEBUG  // debug version in alViewerView.cpp
inline CalViewerDoc* CalViewerView::GetDocument() const
   { return reinterpret_cast<CalViewerDoc*>(m_pDocument); }
#endif

