#pragma once


enum RAW_FORMAT
{
	Y_RAW_8BITS = 0,
	Y_RAW_10BITS = 1,
	Y_RAW_12BITS = 2,
	Y_RAW_14BITS = 3,
	Y_RAW_16BITS = 4,

	RAW_8BITS	= 5,
    RAW_10BITS = 6,
    RAW_12BITS = 7,
    RAW_14BITS = 8,
    RAW_16BITS = 9,
	RAW_16BITS_A	= 10,

    RAW_8BITS_NoCFAi = 11,
    RAW_16BITS_NoCFAi = 12,

	RAW_MIPI_8BITS	= 21,
	RAW_MIPI_10BITS = 22,
	RAW_MIPI_12BITS = 23,

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

    double B_Max;
    double Gr_Max;
    double Gb_Max;
    double R_Max;

    double B_Min;
    double Gr_Min;
    double Gb_Min;
    double R_Min;
} PIXEL_STAT;

typedef struct _RGBA_BUFF
{
	int nSize;
	BYTE *B;
	BYTE *G;
	BYTE *R;
	BYTE *A;
	BYTE *Y;

	_RGBA_BUFF()
	{
		B = G = R = A = Y = nullptr;
	}

	~_RGBA_BUFF()
	{
		if (B != nullptr)
		{
			delete[]B;
			B = nullptr;
		}

		if (G != nullptr)
		{
			delete[]G;
			G = nullptr;
		}

		if (R != nullptr)
		{
			delete[]R;
			R = nullptr;
		}

		if (A != nullptr)
		{
			delete[]A;
			A = nullptr;
		}

		if (Y != nullptr)
		{
			delete[]Y;
			Y = nullptr;
		}
	}

} RGBA_BUFF;

typedef struct _RAW_FOTMAT_PACKAGE
{
	int RAW_WIDTH;
	int RAW_HEIGHT;
	int RAW_HEADER;
	int RAW_STRIDE;
	int RAW_FORMAT;
	int RAW_CR_ORDER;
	int RAW_PITCHOUT;
    int RAW_BLACKLV;
} RAW_FORMAT_PK;

class CalBitmapData
{
public:
	CalBitmapData();
	~CalBitmapData();

public:
	UINT RawFormat;
	UINT RawColorOrd;
    UINT RawClamp;
    UINT RawPxCnt; // Chn * depth
	LPBYTE m_pImageBuffer;
    LPBYTE m_pImageBackup;
	BITMAPINFO BmpInfo;
	BITMAP  bmp;

public: //operator
	BOOL LoadCommonFile(LPCTSTR path);
	BOOL SaveCommonFile(LPCTSTR path);
	BOOL LoadRAWFile(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL CreateBitmapInfo(long width, long height, int depth, int chn);
	BOOL FlipRawDataImage(long width, long height);
    BOOL DemosaicNormal();
    BOOL DemosaicAdvance();
    BOOL ClearBitmapData();

public: //Getter
	CalBitmapData* Clone();
	CalBitmapData* CreateCropImage(RECT &region);
    CalBitmapData* CreateSrcImage();
	Gdiplus::Bitmap* CreateBitmapFromHBITMAP(HBITMAP hBitmap);
	BOOL GetBuff(RECT &region, RGBA_BUFF *buff);
	PIXEL_DATA GetPixel(int x, int y);
	PIXEL_STAT GetSTAT(RECT &region);

private: // Sub function
	// Load image
	BOOL LoadBmpFile(LPCTSTR path);
	BOOL LoadJpgFile(LPCTSTR path);
	BOOL LoadResourceBitmap(UINT uID);
	BOOL LoadCommonFile_GDI(LPCTSTR path);
	BOOL LoadCommonFile_WIC(LPCTSTR path);
	BOOL LoadYRAWFile_8bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadYRAWFile_16bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_8bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_16bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_MIPI_10bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_MIPI_12bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_QCMM_10bits(LPCTSTR path, RAW_FORMAT_PK &format);
	BOOL LoadRAWFile_QCMM_12bits(LPCTSTR path, RAW_FORMAT_PK &format);

	// Save Image
	BOOL SaveCommonFile_WIC(LPCTSTR path);

private:
	int Bpp(int depth, int chn);
	void FillBitmapInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	BOOL GetBitmapInfo(long width, long height, int depth, int chn, BITMAPINFO* bmpInfo);
	BOOL GetBitmapInfo(Gdiplus::Bitmap *bmp, BITMAPINFO* bmpInfo);
};