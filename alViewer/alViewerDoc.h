
// alViewerDoc.h : interface of the CalViewerDoc class
//


#pragma once
#include <memory>

class CalBitmapData;
class CalViewerDoc : public CDocument
{
protected: // create from serialization only
	CalViewerDoc();
	DECLARE_DYNCREATE(CalViewerDoc)

// Attributes
public:
	int ImgWidth();
	int ImgHeight();
	int ImgFormat();
	UINT DocMode;
	void* ImgBuffer();
	BITMAPINFO* ImgBitmapInfo();

// Operations
public:
	std::shared_ptr<CalBitmapData> BmpData();
	void StopPreviewThread();
	void UpdateStatusSize(int w, int h);
	void UpdateStatusPos(int x, int y);
	void UpdateStatusRGB(UINT r, UINT g, UINT b);
	void UpdateStatusRect(int x, int y, int w, int h);
	void UpdateStatusRatio(double scale);
	void UpdateStatusToolbarStatus(bool enable);
	void UpdateDrawViewRect();
	void UpdateViewFrame();
	void ApplicationDoEvent();
	CSize GetDocSize() { return m_sizeDoc; }

private:
	int m_ImgRawWidth;
	int m_ImgRawHeight;
	int m_ImgRawFormat;
	std::shared_ptr<CalBitmapData> m_alBitmapData;

// Capture thread
private:
	CEvent* pExitEvent;
	CWinThread*	m_pVideoShowThread;
	static UINT CaptureVideoFrameThread(LPVOID lpParam);
	void CreateNewVideoStream();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Implementation
public:
	virtual ~CalViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CSize           m_sizeDoc;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
