#pragma once
#include <vector>
#include "CalBitmapData.h"


class ITask_ImgAlgoProcBase
{
public:
	ITask_ImgAlgoProcBase() {}
	virtual ~ITask_ImgAlgoProcBase() {}

public:
	virtual bool Execute(CalBitmapData* image) { return false; }
	virtual TCHAR* GetFinalMessage(){ return (TCHAR*)m_strMsg.c_str(); }
	virtual void* GetFinalResultData() { return nullptr; }

protected:
	tstring m_strMsg;
};


//!===================================================================================
//! Brief: These classes are responsible for handling image process task
//!			 such as, Mirror, flip, rotate and so on
//!
//!===================================================================================

class ITask_ImgProc_Mirror : public ITask_ImgAlgoProcBase
{
public:
	ITask_ImgProc_Mirror() {};
	virtual ~ITask_ImgProc_Mirror() {};

public:
	virtual bool Execute(CalBitmapData* image);
};

class ITask_ImgProc_Flip : public ITask_ImgAlgoProcBase
{
public:
	ITask_ImgProc_Flip() {};
	virtual ~ITask_ImgProc_Flip() {};

public:
	virtual bool Execute(CalBitmapData* image);
};

class ITask_ImgProc_RotateP90 : public ITask_ImgAlgoProcBase
{
public:
	ITask_ImgProc_RotateP90() {};
	virtual ~ITask_ImgProc_RotateP90() {};

public:
	virtual bool Execute(CalBitmapData* image);
};

class ITask_ImgProc_RotateN90 : public ITask_ImgAlgoProcBase
{
public:
	ITask_ImgProc_RotateN90() {};
	virtual ~ITask_ImgProc_RotateN90() {};

public:
	virtual bool Execute(CalBitmapData* image);
};

