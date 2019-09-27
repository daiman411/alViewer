#pragma once
#include "alImgStructure.h"

namespace func
{
 
	class CImgProc
	{
	public: 
		// Image operation
		static void ImageMirror(LPBYTE pImage, UINT width, UINT height);
		static void ImageFlip(LPBYTE pImage, UINT width, UINT height, int depth = 1);
		static void ImageRotate(LPBYTE pImage, UINT width, UINT height, bool cw=true);
		static void ImageRotateP90(LPBYTE pImage, UINT width, UINT height);
		static void ImageRotateN90(LPBYTE pImage, UINT width, UINT height);


		static void HighPass(LPBYTE pImage, LPBYTE pImageMask, UINT nWidth, UINT nHeight, int radius);
		static double STD(LPBYTE pbf, int width, int height);
		static double MEAN(LPBYTE pbf, int width, int height);
		static void Mean_BGR(PBYTE pIn_img, long width, long height, double* a_pfR, double* a_pfG, double* a_pfB);
		static int Bin_OtsuThresholding(unsigned int *a_pdHistogram, int a_dHistSize, int a_nHeight, int a_nWidth);
		static int Bin_FindThresholdInImg(unsigned char *a_pucImg, int a_dImgW, int a_dImgH);
		static bool ImageHist(LPBYTE pImage, UINT width, UINT height, unsigned int* a_pdHistogram);


		static BOOL testAWB_FullRGB(PBYTE a_pbf, int a_dwWidth, int a_dwHeight, double* a_pfAWB);
		static void testAWBApplyToRGB(PBYTE a_pInImg, int a_dwWidth, int a_dwHeight, double* a_pfAWBRatio, PBYTE a_pOutImg);
        static double RgnSumY(unsigned char *a_pIn, unsigned short a_ImgW, unsigned short a_ImgH, CPoint a_RgnStartPnt, unsigned short a_CalW, unsigned short a_CalH);

        // OTSU
        static void CalculatorValueOTSU(unsigned char *pIn, int nSize, unsigned char &nOtsuTH);
        static void ApplyThreshold(unsigned char *pIn, unsigned char *pOut, int nSize, unsigned char nValueTH, int type);

        // Connect component
        static void LabelImage(unsigned char* pIn, int* pOut, unsigned short nWidth, unsigned short nHeight);
        static void LabelImage(unsigned char* pIn, Label_t& Label, unsigned short nWidth, unsigned short nHeight);
        static void LabelMaxRegion(int* pIn, unsigned char* pOut, unsigned short nWidth, unsigned short nHeight);

        // Edge
        static void SobelFilter(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);
        static void MorphologyEdge(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);

        // Gaussian Blur
        static void GaussianBlur(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight, int MaskSize, double sigma);

        // Morphology 3x3 4-neighbor
        static void BinaryDilate(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);
        static void BinaryErode(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);
        static void BinaryOpen(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);
        static void BinaryClose(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight);

        // Harris Corner Detector
        static void HarrisCorner(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight, int MaskSize, double sigma);

        // k-mean
        static void KMeans(unsigned char *pIn, unsigned char *pOut, int nWidth, int nHeight, int K, POINT* Mean);

    private:
        static void Threshold_G(unsigned char *pIn, unsigned char *pOut, int nSize, unsigned char nValueTH);
        static void Threshold_GE(unsigned char *pIn, unsigned char *pOut, int nSize, unsigned char nValueTH);
        static void Threshold_S(unsigned char *pIn, unsigned char *pOut, int nSize, unsigned char nValueTH);
        static void Threshold_SE(unsigned char *pIn, unsigned char *pOut, int nSize, unsigned char nValueTH);

        static void LabelComponent(unsigned char* pIn, int* pOut, unsigned short* pSTACK, LabelInfo & Blob, unsigned short nWidth, unsigned short nHeight, int nLabelNo, unsigned short x, unsigned short y);

        template<class T>static void CopyOpMem(T *pIn, int nSize, int width, int idx, T *op_mem, int MaskSize);
        template<class T>static int Convolution(T *pIn, int *Mask, int nSize);
        template<class T>static double Convolution(T *pIn, double *Mask, int nSize);
        template<class T>static void IterateConv(T *pIn, T *pOut, int nSize, int nWidth, int *Mask, int MaskSize);
        template<class T>static void IterateConv(T *pIn, T *pOut, int nSize, int nWidth, double *Mask, int MaskSize);

        static void GaussianKernel(double *pMask, int MaskSize, double sigma);
        static void GaussianBlur(int *pIn, int *pOut, int nWidth, int nHeight, int MaskSize, double sigma);

        inline static int nearest(point pt, point cent, int n_cluster, double *d2);
        inline static double dist2(point a, point b);
    
	public:
        template<class T> static T MaxPixelValue(T* pImage, UINT width, UINT height, CPoint &point)
        {
            T MaxValue = 0;

            for (int i = 0; i < (int)height; i++)
                for (int j = 0; j < (int)width; j++)
                {
                    if (pImage[i * width + j] > MaxValue)
                    {
                        MaxValue = pImage[i * width + j];
                        point.x = j;
                        point.y = i;
                    }
                }
            return MaxValue;
        }

    };
}