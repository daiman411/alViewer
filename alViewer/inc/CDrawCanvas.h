#pragma once
#include <memory>

#ifndef _EXPORT
#define _EXT_CLASS_TYPE //  __declspec(dllimport): static library don't need this 
#else
#define _EXT_CLASS_TYPE //__declspec(dllexport): static library don't need this 
#endif



class CDrawCanvasBase;
class _EXT_CLASS_TYPE CDrawCanvas
{
public:
    CDrawCanvas(HWND hWnd, UINT uMode = 0);
    virtual ~CDrawCanvas();

public:
    virtual void ClearDraw(Rect &rcClient, COLORREF pen_color=RGB(0,0,0));
    virtual void RePaint(HDC hDc, Rect &rcClient);
    virtual void DrawRectangle(Rect &rcClient, COLORREF &pen_color);
    virtual void DrawFocusRect(Rect &rcClient, COLORREF &pen_color);
    virtual void DrawSolidRect(Rect &rcROI, COLORREF &pen_color, COLORREF &brush_color);
    virtual void DrawStringText(LPCTSTR text, Point &ptPOS, COLORREF &pen_color);
	virtual void DrawStringText(LPCTSTR text, Rect &rcPOS, COLORREF &pen_color);
	virtual void DrawDashCross(Point &ptCenter, COLORREF &pen_color, int length);
    virtual void DrawSolidEllipse(Rect &rcROI, COLORREF &pen_color, int width);
	virtual void DrawHistogram(PathData &points, Rect &rcClient, COLORREF &pen_color);
    virtual void DrawImage(HDC hDc, Rect &rcClient, Rect &rcDraw, void* lpData, BITMAPINFO* bmpInfo, bool bClear=false);
	virtual void DrawGridVer(double orgX, double orgY, double sizeH, double sizeY, double dn, COLORREF &pen_color, double penSize);
	virtual void DrawGridHor(double orgX, double orgY, double sizeH, double sizeY, double dn, COLORREF &pen_color, double penSize);
	virtual void DrawHorAxis(PointF orgX, PointF orgY, double sizeH, double dn, tstring* strAry, COLORREF &pen_color, double penSize);
	virtual void DrawVerAxis(PointF orgX, PointF orgY, double sizeV, double dn, tstring* strAry, COLORREF &pen_color, double penSize);
	virtual void DrawPathLine(PathData &points, COLORREF &pen_color, double penSize = 1.0f);
    virtual void UpdateWindowSize();
	virtual COLORREF GetPixel(int x, int y);
	virtual void SetFontSize(float size);

protected:    
	std::unique_ptr<CDrawCanvasBase> m_CanvasBase;
};