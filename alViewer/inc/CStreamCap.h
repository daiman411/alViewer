#pragma once
#include <memory>

#ifndef _EXPORT
#define _EXT_CLASS_TYPE //  __declspec(dllimport): static library don't need this 
#else
#define _EXT_CLASS_TYPE //__declspec(dllexport): static library don't need this 
#endif

class CVideoCapBase;
class _EXT_CLASS_TYPE CStreamCap
{
public:
	CStreamCap(HWND hWnd, UINT uMode = 0);
	virtual ~CStreamCap();

public:
	virtual HRESULT Create(HWND hWnd);
	virtual BOOL InitialCap(const TCHAR* name = nullptr);
	virtual BOOL ReleaseCap();
	virtual BOOL FindDevices();
	virtual BOOL ChooseDevices(UINT idxVideo, UINT idxAudio);
	virtual BOOL BuildPreviewGraph(HWND hWnd);
	virtual BOOL StartPreview();
	virtual BOOL StopPreview();
	virtual BOOL PausePreview();
	virtual BOOL GrabFrame(unsigned char * dstBuffer, bool flipRedAndBlue, bool flipImage);
	virtual void ResizeWindow(HWND hWnd);
	virtual LONG VideoInputCount();
	virtual UINT DeviceCount();
	virtual UINT VideoWidth(); 
	virtual UINT VideoHeight();


protected:
	std::unique_ptr<CVideoCapBase> m_AVCapBase;
};