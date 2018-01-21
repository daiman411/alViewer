#pragma once


enum RAW_FORMAT
{
	RAW_8BITS	= 0,
	RAW_16BITS	= 1,
	RAW_MIPI_8BITS	= 11,
	RAW_MIPI_10BITS = 12,
	RAW_MIPI_12BITS = 13,

	RAW_QCOMM_8BITS	= 51,
	RAW_QCOMM_10BITS = 52,
	RAW_QCOMM_12BITS = 53,

	RAW_COMMON_FORMAT = 100,
};

typedef struct _PIXEL_DATA
{
	BYTE B;
	BYTE G;
	BYTE R;
	BYTE A;
} PIXEL_DATA;

typedef struct _PIXEL_STAT
{
	double B_Mean;
	double Gr_Mean;
	double Gb_Mean;
	double R_Mean;
} PIXEL_STAT;

typedef struct _RAW_FOTMAT_PACKAGE
{
	int RAW_WIDTH;
	int RAW_HEIGHT;
	int RAW_HEADER;
	int RAW_STRIDE;
	int RAW_FORMAT;
	int RAW_CR_ORDER;
	int RAW_PITCHOUT;
} RAW_FORMAT_PK;

class CalBitmapData
{
public:
	CalBitmapData();
	~CalBitmapData();

public:
	UINT RawFormat;
	UINT RawColorOrd;
	LPBYTE m_pImageBuffer;
	LPWORD m_pImageBackup;
	BITMAPINFO BmpInfo;
	BITMAP  bmp;

public: //operator
	BOOL LoadCommonFile(LPCTSTR path);
	BOOL LoadRAWFile(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL CreateBitmapInfo(long width, long height, int depth, int chn);

public: //Getter
	Gdiplus::Bitmap* CreateBitmapFromHBITMAP(HBITMAP hBitmap);
	PIXEL_DATA GetPixel(int x, int y);
	PIXEL_STAT GetMean(RECT &region);

private: // Sub function
	BOOL LoadBmpFile(LPCTSTR path);
	BOOL LoadJpgFile(LPCTSTR path);
	BOOL LoadResourceBitmap(UINT uID);
	BOOL LoadCommonFile_GDI(LPCTSTR path);
	BOOL LoadCommonFile_WIC(LPCTSTR path);
	BOOL LoadRAWFile_8bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_16bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_MIPI_10bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_MIPI_12bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_QCMM_10bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_QCMM_12bits(LPCTSTR path, RAW_FORMAT_PK &format);

private:
	int Bpp(int depth, int chn);
	void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	BOOL GetBitmapInfo(long width, long height, int depth, int chn, BITMAPINFO* bmpInfo);
	BOOL GetBitmapInfo(Gdiplus::Bitmap *bmp, BITMAPINFO* bmpInfo);
};