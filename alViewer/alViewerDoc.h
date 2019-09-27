// alViewerDoc.h : interface of the CalViewerDoc class
//
#pragma once
#include <memory>
#include <vector>

class CalBitmapData;
class CalTaskImgProc;
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
	bool ClearBuffer;
	BITMAPINFO* ImgBitmapInfo();
	BOOL ClearBitmapImage();
	const TCHAR* GetInputMediaPath();

// Operations
public:
	std::shared_ptr<CalBitmapData> BmpData();
	void StartPreviewThread();
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
	void MoveToNextImage();
	void MoveToPreImage();
    void ExecuteImgProc(UINT mode);
    BOOL OpenImageFile(CString a_filePath);
	BOOL SaveImageFile(CString a_filePath);
    BOOL SaveRawDataFile(CString a_filePath);
	CSize GetDocSize() { return m_sizeDoc; }
	CString ImagePath() { return m_pathCurrentImage; }
	UINT GetVideoMode() { return m_nVideoMode;  }
	CView* GetViewFrame();

private:
	int m_ImgRawWidth;
	int m_ImgRawHeight;
	int m_ImgRawFormat;
	int m_nVideoMode;
	CString m_strInputMediaDataPath;
	std::shared_ptr<CalBitmapData> m_alBitmapData;
    std::shared_ptr<CalTaskImgProc> m_taskImgProc;

// Capture thread
private:
	CEvent* pExitEvent;
	CWinThread*	m_pVideoShowThread;
	CWinThread*	m_pVideoRenderThread;
	static UINT CaptureVideoFrameThread(LPVOID lpParam);
	static UINT RenderVideoFrameThread(LPVOID lpParam);
	void CreateNewVideoStream();
	void CreateNewVideoRender();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    CView* GetActiveMainView();
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

private: // Utility
	int m_IdxOfFolderImage;
	CString m_pathCurrentImage;
	std::vector<CString> m_fldImgs;
	bool FindImageFiles(CString folderPath, std::vector<CString> &a_OutFile);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
