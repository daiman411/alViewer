
// alViewerDoc.cpp : implementation of the CalViewerDoc class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "alViewer.h"
#endif

#include "alViewerDoc.h"
#include "alViewerView.h"
#include "CalBitmapData.h"
#include "CStreamCap.h"
#include "DlgRawPaser.h"
#include "alTaskImgProc.h"
#include <propkey.h>
#include <assert.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CalViewerDoc

IMPLEMENT_DYNCREATE(CalViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CalViewerDoc, CDocument)
END_MESSAGE_MAP()


// CalViewerDoc construction/destruction

CalViewerDoc::CalViewerDoc()
{
	// TODO: add one-time construction code here
	m_alBitmapData = std::shared_ptr<CalBitmapData>(new CalBitmapData());
    m_taskImgProc = std::shared_ptr<CalTaskImgProc>(new CalTaskImgProc(this));

	m_fldImgs.clear();
	m_IdxOfFolderImage = 0;
	m_pathCurrentImage = _T("");
	DocMode = 0;	//0: common 1: raw 2: uvc
	m_nVideoMode = 0;
	pExitEvent = nullptr;
	m_pVideoShowThread = nullptr;
	m_pVideoRenderThread = nullptr;
	m_strInputMediaDataPath = _T("");
	m_ImgRawWidth = Global::RAW_IMAGE_WIDTH;
	m_ImgRawHeight = Global::RAW_IMAGE_HEIGHT;
	m_ImgRawFormat = Global::RAW_IMAGE_FORMAT;
}

CalViewerDoc::~CalViewerDoc()
{
	StopPreviewThread();

	m_strInputMediaDataPath.Empty();
	Global::RAW_IMAGE_WIDTH = ImgWidth();
	Global::RAW_IMAGE_HEIGHT = ImgHeight();
	Global::RAW_IMAGE_FORMAT = ImgFormat();

	//UpdateStatusToolbarStatus(false);
}

BOOL CalViewerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	m_sizeDoc = CSize(200, 200);

	// Check if this is shell command
	CString sCmdLine = AfxGetApp()->m_lpCmdLine;

	if (!sCmdLine.IsEmpty())
	{
		AfxMessageBox(sCmdLine);
		CWinApp* pApp = AfxGetApp();
		ASSERT(pApp != NULL);

		CString fileName = sCmdLine;
		OnOpenDocument(fileName.GetBuffer());
	}

	return TRUE;
}


// CalViewerDoc serialization

void CalViewerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void CalViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void CalViewerDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data. 
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CalViewerDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CalViewerDoc diagnostics

#ifdef _DEBUG
void CalViewerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CalViewerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CalViewerDoc commands
CView* CalViewerDoc::GetActiveMainView()
{
    CView*pView = NULL;
    POSITION pos = GetFirstViewPosition();

    if (pos != NULL)
    {
        pView = GetNextView(pos);

        if (pView != NULL)
        {
            return pView;
        }
    }

    return nullptr;
}


std::shared_ptr<CalBitmapData> CalViewerDoc::BmpData()
{
	return m_alBitmapData;
}

int CalViewerDoc::ImgWidth()
{
	return abs(m_alBitmapData->BmpInfo.bmiHeader.biWidth);
}

int CalViewerDoc::ImgHeight()
{
	return abs(m_alBitmapData->BmpInfo.bmiHeader.biHeight);
}

int CalViewerDoc::ImgFormat()
{
	return (int)m_alBitmapData->RawFormat;
}

void* CalViewerDoc::ImgBuffer()
{
	return m_alBitmapData->m_pImageBuffer;
}

BITMAPINFO* CalViewerDoc::ImgBitmapInfo()
{
	return &m_alBitmapData->BmpInfo;
}

BOOL CalViewerDoc::ClearBitmapImage()
{
	return m_alBitmapData->ClearBitmapData();
}

const TCHAR* CalViewerDoc::GetInputMediaPath()
{
	return m_strInputMediaDataPath.GetBuffer();
}

BOOL CalViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString strFileName = lpszPathName;
	CString strFolderPath = strFileName.Left(strFileName.ReverseFind(_T('\\')));
	FindImageFiles(strFolderPath, m_fldImgs);
	auto idx = std::find(std::begin(m_fldImgs),std::end(m_fldImgs), strFileName);
	m_IdxOfFolderImage = (int)std::distance(std::begin(m_fldImgs), idx);
	ClearBuffer = false;

	return OpenImageFile(strFileName);
}

BOOL CalViewerDoc::OpenImageFile(CString a_filePath)
{
	CString strExtendName = _T("");
	CString strFileName = a_filePath;
    m_pathCurrentImage = a_filePath;

	if (strFileName.Find(_T("None")) > 0)
	{
		if (!m_strInputMediaDataPath.IsEmpty())
		{
			strFileName = m_strInputMediaDataPath;
			const int extLen = strFileName.GetLength() - strFileName.ReverseFind('.') - 1;
			CString strExtName = strFileName.Right(extLen);
			strExtendName = strExtName;
		}
	}
	else
	{
		m_strInputMediaDataPath = strFileName;
		const int extLen = strFileName.GetLength() - strFileName.ReverseFind('.') - 1;
		CString strExtName = strFileName.Right(extLen);
		strExtendName = strExtName;
	}

	if (a_filePath.CompareNoCase(_T("UVC")) == 0)
	{
		DocMode = 2;
		CreateNewVideoStream();
	}
	else if (strExtendName.CompareNoCase(_T("mp4")) == 0)
	{
		DocMode = 2;
		m_nVideoMode = 1;	//Direct show
		CreateNewVideoRender();
	}
	else
	{
		if (strExtendName.CompareNoCase(_T("bmp")) == 0)
		{
			m_alBitmapData->LoadCommonFile(a_filePath.GetBuffer());
		}
		else if (strExtendName.CompareNoCase(_T("jpg")) == 0 ||
			strExtendName.CompareNoCase(_T("jpeg")) == 0)
		{
			m_alBitmapData->LoadCommonFile(a_filePath.GetBuffer());
		}
		else if (strExtendName.CompareNoCase(_T("jpg")) == 0)
		{
			m_alBitmapData->LoadCommonFile(a_filePath.GetBuffer());
		}
		else if (strExtendName.CompareNoCase(_T("png")) == 0)
		{
			m_alBitmapData->LoadCommonFile(a_filePath.GetBuffer());
			UpdateStatusSize(ImgWidth(), ImgHeight());
		}
		else if (strExtendName.CompareNoCase(_T("tif")) == 0 ||
			strExtendName.CompareNoCase(_T("tiff")) == 0)
		{
			m_alBitmapData->LoadCommonFile(a_filePath.GetBuffer());
		}
		else if (strExtendName.CompareNoCase(_T("raw")) == 0)
		{
			DocMode = 1;

			// Load Raw image format
			CDlgRawPaser dlgRawPaser(NULL, this);
			dlgRawPaser.SetRawPath(a_filePath.GetBuffer());
			dlgRawPaser.SetWidth(m_ImgRawWidth);
			dlgRawPaser.SetHeight(m_ImgRawHeight);
			dlgRawPaser.SetFormat(m_ImgRawFormat);

			if (dlgRawPaser.DoModal() == IDOK)
			{
				//RAW_FORMAT_PK format_pk;
				m_ImgRawWidth = dlgRawPaser.GetWidth();
				m_ImgRawHeight = dlgRawPaser.GetHeight();
				m_ImgRawFormat = dlgRawPaser.GetFormat();
			}
			else
			{
				m_ImgRawWidth = 720;
				m_ImgRawHeight = 480;
				m_ImgRawFormat = 0;

				return FALSE;
			}
		}

		// Update infomation
		UpdateStatusSize(ImgWidth(), ImgHeight());
	}

	UpdateStatusToolbarStatus(true);
	this->SetPathName(a_filePath.GetBuffer(), false);

	return TRUE;
}

BOOL CalViewerDoc::SaveImageFile(CString a_filePath)
{
	try
	{
		m_alBitmapData->SaveCommonFile(a_filePath.GetBuffer());
	}
	catch (...)
	{
		AfxMessageBox(_T("Save image file failed!"));
	}

	return TRUE;
}

BOOL CalViewerDoc::SaveRawDataFile(CString a_filePath)
{
    try
    {
        FILE *pf = nullptr;

        errno_t err;
        err = _tfopen_s(&pf, a_filePath.GetBuffer(), _T("wb"));
        if (err != 0) {
            _tprintf(_T("can not create file %s.\n"), a_filePath.GetBuffer());
            return FALSE;
        }

        //RGBA_BUFF buff;
        //m_alBitmapData->GetBuff(CRect(0,0, m_ImgRawWidth, m_ImgRawHeight), &buff);

        unsigned long size = m_ImgRawWidth*m_ImgRawHeight;
        fwrite(m_alBitmapData->m_pImageBackup, sizeof(BYTE), size, pf);
        //fwrite(buff.Y, sizeof(unsigned char), size, pf);
        fclose(pf);
    }
    catch (...)
    {
        AfxMessageBox(_T("Save image file failed!"));
    }

    return TRUE;
}

void CalViewerDoc::StartPreviewThread()
{
	if (!m_strInputMediaDataPath.IsEmpty())
	{
		const int extLen = m_strInputMediaDataPath.GetLength() - m_strInputMediaDataPath.ReverseFind('.') - 1;
		CString strExtName = m_strInputMediaDataPath.Right(extLen);

		if (strExtName.CompareNoCase(_T("UVC")) == 0)
		{
			DocMode = 2;
			m_nVideoMode = 0;
			m_pVideoShowThread = nullptr;
			CreateNewVideoStream();
		}
		else if (strExtName.CompareNoCase(_T("mp4")) == 0)
		{
			DocMode = 2;
			m_nVideoMode = 1;
			m_pVideoRenderThread = nullptr;
			CreateNewVideoRender();
		}
	}
}

void CalViewerDoc::StopPreviewThread()
{
	if (pExitEvent)
	{
		pExitEvent->SetEvent();

		if (m_pVideoShowThread)
		{
		while (::WaitForSingleObject(m_pVideoShowThread->m_hThread, 1) != WAIT_OBJECT_0)
			ApplicationDoEvent();
		}

		if (m_pVideoRenderThread)
		{
			while (::WaitForSingleObject(m_pVideoRenderThread->m_hThread, 1) != WAIT_OBJECT_0)
				ApplicationDoEvent();
		}

		delete pExitEvent;
		pExitEvent = nullptr;
	}

	if (m_pVideoShowThread)
		delete m_pVideoShowThread;
	m_pVideoShowThread = nullptr;

	if (m_pVideoRenderThread)
		delete m_pVideoRenderThread;
	m_pVideoRenderThread = nullptr;
}

void CalViewerDoc::UpdateStatusSize(int w, int h)
{
	CString tmp;
	tmp.Format(_T("W: %d, H: %d"), w, h);
	AfxGetMainWnd()->SendMessage(WM_USER + 100, 0, (LPARAM)tmp.GetBuffer());
}

void CalViewerDoc::UpdateStatusPos(int x, int y)
{
	CString tmp;
	tmp.Format(_T("X: %d, Y: %d"), x, y);
	AfxGetMainWnd()->SendMessage(WM_USER + 101, 0, (LPARAM)tmp.GetBuffer());
}

void CalViewerDoc::UpdateStatusRGB(UINT r, UINT g, UINT b)
{
	CString tmp;
	tmp.Format(_T("R: %d, G: %d, B: %d"), r, g, b);
	AfxGetMainWnd()->SendMessage(WM_USER + 102, 0, (LPARAM)tmp.GetBuffer());
}

void CalViewerDoc::UpdateStatusRect(int x, int y, int w, int h)
{
	CString tmp;
	tmp.Format(_T("%d, %d, %d, %d"), x, y, w, h);
	AfxGetMainWnd()->SendMessage(WM_USER + 103, 0, (LPARAM)tmp.GetBuffer());
}

void CalViewerDoc::UpdateStatusRatio(double scale)
{
	CString tmp;
	tmp.Format(_T("%d"), int(scale*100.0));
	tmp += _T("%");
	AfxGetMainWnd()->SendMessage(WM_USER + 104, 0, (LPARAM)tmp.GetBuffer());
}

void CalViewerDoc::UpdateDrawViewRect()
{
	AfxGetMainWnd()->SendMessage(WM_USER + 105, 0, 0);
}

void CalViewerDoc::UpdateStatusToolbarStatus(bool bEnable)
{
	if(bEnable)
		AfxGetMainWnd()->SendMessage(WM_USER + 200, 0, 0);
	else
		AfxGetMainWnd()->SendMessage(WM_USER + 201, 0, 0);
}

void CalViewerDoc::ApplicationDoEvent()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return;

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return;
}

void CalViewerDoc::CreateNewVideoStream()
{
	if (m_pVideoShowThread == nullptr)
	{
		pExitEvent = new CEvent(FALSE, TRUE); // Manu is true to keep SetEvent status
		m_pVideoShowThread = ::AfxBeginThread(CaptureVideoFrameThread, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_pVideoShowThread->m_bAutoDelete = FALSE;
		m_pVideoShowThread->ResumeThread();
		//SetThreadIdealProcessor(m_pVideoShowThread->m_hThread, 1);
	}
}

void CalViewerDoc::CreateNewVideoRender()
{
	if (m_pVideoRenderThread == nullptr)
	{
		pExitEvent = new CEvent(FALSE, TRUE); // Manu is true to keep SetEvent status
		m_pVideoRenderThread = ::AfxBeginThread(RenderVideoFrameThread, (LPVOID)this, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED, NULL);
		m_pVideoRenderThread->m_bAutoDelete = TRUE;
		m_pVideoRenderThread->ResumeThread();
	}
}

void CalViewerDoc::UpdateViewFrame()
{
	CView*pView = NULL;
	POSITION pos = GetFirstViewPosition();

	if (pos != NULL)
	{
		pView = GetNextView(pos);

		if (pView != NULL)
		{
			pView->Invalidate(FALSE);
			pView->UpdateWindow();
		}
	}
}

CView* CalViewerDoc::GetViewFrame()
{
	CView*pView = NULL;
	POSITION pos = GetFirstViewPosition();

	if (pos != NULL)
	{
		pView = GetNextView(pos);

		if (pView != NULL)
		{
			return pView;
		}
	}

	return nullptr;
}

void CalViewerDoc::MoveToNextImage()
{
	if (++m_IdxOfFolderImage < (int)m_fldImgs.size())
	{
		ClearBuffer = true;
		m_alBitmapData->ClearBitmapData();
		CString strFilePath = m_fldImgs[m_IdxOfFolderImage];
		OpenImageFile(strFilePath);
	}
	else
	{
		ClearBuffer = true;
		m_IdxOfFolderImage = 0;
		m_alBitmapData->ClearBitmapData();
		CString strFilePath = m_fldImgs[m_IdxOfFolderImage];
		OpenImageFile(strFilePath);
	}
}

void CalViewerDoc::MoveToPreImage()
{
	if (--m_IdxOfFolderImage > 0)
	{
		ClearBuffer = true;
		m_alBitmapData->ClearBitmapData();
		CString strFilePath = m_fldImgs[m_IdxOfFolderImage];
		OpenImageFile(strFilePath);
	}
	else
	{
		ClearBuffer = true;
		m_alBitmapData->ClearBitmapData();
		m_IdxOfFolderImage = (int)m_fldImgs.size() - 1;
		CString strFilePath = m_fldImgs[m_IdxOfFolderImage];
		OpenImageFile(strFilePath);
	}
}

void CalViewerDoc::ExecuteImgProc(UINT mode)
{
    if (m_taskImgProc)
    {
        m_taskImgProc->ExecuteImgProcTask(mode);
    }

    while (!m_taskImgProc->IsTaskProcDone())
    {
        ApplicationDoEvent();
    }

    ClearBuffer = true;
    CalViewerView* pView = static_cast<CalViewerView*>(GetActiveMainView());
    //pView->UpdateMessageStr(&CString(m_taskImgProc->ProcMessage()));
    UpdateViewFrame();

    return;
}

UINT CalViewerDoc::CaptureVideoFrameThread(LPVOID lpParam)
{
	CalViewerDoc* doc = (CalViewerDoc*)lpParam;
	
	CStreamCap* videoDevice = new CStreamCap(AfxGetMainWnd()->GetSafeHwnd(), doc->GetVideoMode());

	if (!videoDevice->InitialCap())
		return 0;

	if (!videoDevice->FindDevices())
		return 0;

	if (!videoDevice->ChooseDevices(1, 0))
		return 0;

	if (!videoDevice->BuildPreviewGraph(AfxGetMainWnd()->GetSafeHwnd()))
		return 0;

	if (!videoDevice->StartPreview())
		return 0;

	const int width = videoDevice->VideoWidth();
	const int height = videoDevice->VideoHeight();
	const int bufSize = width * height;
	doc->BmpData()->m_pImageBuffer = new BYTE[bufSize*3];	
	doc->BmpData()->BmpInfo.bmiHeader.biWidth = width;
	doc->BmpData()->BmpInfo.bmiHeader.biHeight = height;
	doc->UpdateStatusSize(width, height);
	doc->UpdateDrawViewRect();

	// Capture loop
	while (::WaitForSingleObject(doc->pExitEvent->m_hObject, 1) != WAIT_OBJECT_0)
	{
		if (videoDevice->GrabFrame((unsigned char *)doc->BmpData()->m_pImageBuffer, false, true))
		{
			doc->BmpData()->CreateBitmapInfo(width, height, 8, 3);
		}

		doc->UpdateViewFrame();
	}

	// Something wrong!! Need to fix
	if (!videoDevice->StopPreview())
		return 0;

	if (!videoDevice->ReleaseCap())
		return 0;

	delete videoDevice; 
	videoDevice = nullptr;

	// Terminate the thread
	::AfxEndThread(0, FALSE);

	return 1;
}

bool CalViewerDoc::FindImageFiles(CString folderPath, std::vector<CString> &a_OutFile)
{
	bool IsFound = false;
	TCHAR fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hFind;
	_stprintf_s(fileFound, _T("%s\\*.*"), folderPath.GetBuffer());
	hFind = FindFirstFile(fileFound, &info);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		TRACE("Invalid File Handle. GetLastError reports %d\n", GetLastError());
		IsFound = false;
	}
	else
	{
		do
		{
			_stprintf_s(fileFound, _T("%s\\%s"), folderPath.GetBuffer(), info.cFileName);

			CString strFileName(info.cFileName);
			strFileName.MakeLower();

			if (strFileName.Find(_T("jpg")) > 0 || strFileName.Find(_T("img")) > 0 || 
				strFileName.Find(_T("jpeg")) > 0 || strFileName.Find(_T("bmp")) > 0 ||
				strFileName.Find(_T("png")) > 0 || strFileName.Find(_T("tiff")) > 0 )
			{
				a_OutFile.push_back(CString(fileFound));
				IsFound = true;
			}
		} while (FindNextFile(hFind, &info));
	}

	FindClose(hFind);

	return IsFound;
}
UINT CalViewerDoc::RenderVideoFrameThread(LPVOID lpParam)
{
	CalViewerDoc* doc = (CalViewerDoc*)lpParam;

	CStreamCap* videoDevice = new CStreamCap(doc->GetViewFrame()->GetSafeHwnd(), doc->GetVideoMode());

	if (!videoDevice->InitialCap(doc->GetInputMediaPath()))
		return 0;

	if (!videoDevice->FindDevices())
		return 0;

	if (!videoDevice->ChooseDevices(1, 0))
		return 0;

	if (!videoDevice->BuildPreviewGraph(AfxGetMainWnd()->GetSafeHwnd()))
		return 0;

	if (!videoDevice->StartPreview())
		return 0;

	const int width = videoDevice->VideoWidth();
	const int height = videoDevice->VideoHeight();
	doc->UpdateStatusSize(width, height);

	//const int bufSize = width * height;
	//doc->BmpData()->m_pImageBuffer = new BYTE[bufSize * 3];
	//doc->BmpData()->BmpInfo.bmiHeader.biWidth = width;
	//doc->BmpData()->BmpInfo.bmiHeader.biHeight = height;
	//doc->UpdateStatusSize(width, height);
	//doc->UpdateDrawViewRect();

	// Capture loop
	while (::WaitForSingleObject(doc->pExitEvent->m_hObject, 1) != WAIT_OBJECT_0)
	{
		if (videoDevice->GrabFrame((unsigned char *)doc->BmpData()->m_pImageBuffer, false, true))
		{
			continue;
		}
		else
		{
			break;
		}
	}

	// Something wrong!! Need to fix
	if (!videoDevice->StopPreview())
		return 0;

	if (!videoDevice->ReleaseCap())
		return 0;

	delete videoDevice;
	videoDevice = nullptr;

	// Terminate the thread
	::AfxEndThread(0, FALSE);

	return 1;
}
