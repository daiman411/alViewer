#pragma once
#include <vector>

namespace func
{
	typedef struct { double x, y; int group; } point_t, *point;

	typedef struct _LabelInfo
	{
		unsigned int Cnt;
		long Left;
		long Top;
		unsigned int Width;
		unsigned int Height;
		POINT Centroid;
		std::vector<POINT> mPoint;

		_LabelInfo() {
			Cnt = 0;
			Width = 0;
			Height = 0;
			Centroid = { 0, 0 };
		}

		~_LabelInfo() {
			std::vector<POINT>().swap(mPoint);
		}

	} LabelInfo;

	typedef struct _Label
	{
		unsigned int Cnt;
		std::vector<LabelInfo> blob;

		_Label() {
			Cnt = 0;
		}
	} Label_t;

}