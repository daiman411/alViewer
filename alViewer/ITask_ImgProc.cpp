#include "stdafx.h"
#include "ITask_ImgProc.h"
#include "alImgProc.h"
//#define SAVE_RAW_DATA
using namespace func;
using namespace std;

bool ITask_ImgProc_Mirror::Execute(CalBitmapData* image)
{
	LPBYTE imageBuff = image->m_pImageBuffer;
	int w = abs(image->BmpInfo.bmiHeader.biWidth);
	int h = abs(image->BmpInfo.bmiHeader.biHeight);
	int chn = 4;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
	RGBA_BUFF buff;
	image->GetBuff(rect, &buff);

	// Start to execute
	CImgProc::ImageMirror(buff.B, w, h);
	CImgProc::ImageMirror(buff.G, w, h);
	CImgProc::ImageMirror(buff.R, w, h);
	//CImgProc::ImageMirror(buff.Y, w, h);

	for (int j = 0; j < int(h); j++)
	{
		for (int i = 0; i < int(w); i++)
		{
			imageBuff[j*w * chn + i * chn] = buff.B[j*w+i];
			imageBuff[j*w * chn + i * chn+1] = buff.G[j*w + i];
			imageBuff[j*w * chn + i * chn+2] = buff.R[j*w + i];
			imageBuff[j*w * chn + i * chn+3] = 1;
		}
	}

	// Update source image
	image->CreateBitmapInfo(w, h, 8, 4);

	return true;
}

bool ITask_ImgProc_Flip::Execute(CalBitmapData* image)
{
	LPBYTE imageBuff = image->m_pImageBuffer;
	int w = abs(image->BmpInfo.bmiHeader.biWidth);
	int h = abs(image->BmpInfo.bmiHeader.biHeight);
	int chn = 4;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
	RGBA_BUFF buff;
	image->GetBuff(rect, &buff);

	// Start to execute
	CImgProc::ImageFlip(buff.B, w, h);
	CImgProc::ImageFlip(buff.G, w, h);
	CImgProc::ImageFlip(buff.R, w, h);
	//CImgProc::ImageFlip(buff.A, w, h);

	for (int j = 0; j < int(h); j++)
	{
		for (int i = 0; i < int(w); i++)
		{
			imageBuff[j*w * chn + i * chn] = buff.B[j*w + i];
			imageBuff[j*w * chn + i * chn + 1] = buff.G[j*w + i];
			imageBuff[j*w * chn + i * chn + 2] = buff.R[j*w + i];
			imageBuff[j*w * chn + i * chn + 3] = 1;
		}
	}

	// Update source image
	image->CreateBitmapInfo(w, h, 8, 4);

	return true;
}

bool ITask_ImgProc_RotateP90::Execute(CalBitmapData* image)
{
	LPBYTE imageBuff = image->m_pImageBuffer;
	int w = abs(image->BmpInfo.bmiHeader.biWidth);
	int h = abs(image->BmpInfo.bmiHeader.biHeight);
	int chn = 4;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
	RGBA_BUFF buff;
	image->GetBuff(rect, &buff);

	// Start to execute
	CImgProc::ImageRotate(buff.B, w, h);
	CImgProc::ImageRotate(buff.G, w, h);
	CImgProc::ImageRotate(buff.R, w, h);
	CImgProc::ImageRotate(buff.A, w, h);

	for (int j = 0; j < int(h); j++)
	{
		for (int i = 0; i < int(w); i++)
		{
			imageBuff[j*w * chn + i * chn] = buff.B[j*w + i];
			imageBuff[j*w * chn + i * chn + 1] = buff.G[j*w + i];
			imageBuff[j*w * chn + i * chn + 2] = buff.R[j*w + i];
			imageBuff[j*w * chn + i * chn + 3] = buff.A[j*w + i];;
		}
	}

	// Update source image
	image->CreateBitmapInfo(h, w, 8, 4);

	return true;
}

bool ITask_ImgProc_RotateN90::Execute(CalBitmapData* image)
{
	LPBYTE imageBuff = image->m_pImageBuffer;
	int w = abs(image->BmpInfo.bmiHeader.biWidth);
	int h = abs(image->BmpInfo.bmiHeader.biHeight);
	int chn = 4;

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;
	RGBA_BUFF buff;
	image->GetBuff(rect, &buff);

	// Start to execute
	CImgProc::ImageRotate(buff.B, w, h, false);
	CImgProc::ImageRotate(buff.G, w, h, false);
	CImgProc::ImageRotate(buff.R, w, h, false);
	CImgProc::ImageRotate(buff.A, w, h, false);

	for (int j = 0; j < int(h); j++)
	{
		for (int i = 0; i < int(w); i++)
		{
			imageBuff[j*w * chn + i * chn] = buff.B[j*w + i];
			imageBuff[j*w * chn + i * chn + 1] = buff.G[j*w + i];
			imageBuff[j*w * chn + i * chn + 2] = buff.R[j*w + i];
			imageBuff[j*w * chn + i * chn + 3] = buff.A[j*w + i];;
		}
	}

	// Update source image
	image->CreateBitmapInfo(h, w, 8, 4);

	return true;
}


