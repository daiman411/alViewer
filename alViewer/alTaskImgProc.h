#pragma once
#include <memory>
#include "ThreadModel.h"

enum ImgProcTaskEnum
{
	Task_None = 0,
	
	// Image prcess
	Task_RotateCW90 = 100,
	Task_RotateCCW90 = 101,
	Task_Mirro = 110,
	Task_Flip = 111,

	// IQ Process
	Task_MTF_SFR_RECT = 300,
	Task_MTF_SFR_SLANT = 301,
	Task_MTF_CTF = 310,
	Task_MTF_TVL = 320,

};

class ITask_ImgAlgoProcBase;
class CalTaskImgProc
{
public:
	CalTaskImgProc(CDocument* pDoc);
	~CalTaskImgProc();

private:
	CalTaskImgProc();

private:
	CDocument* m_pDoc;

public:
	BOOL IsTaskProcDone();
	CString ProcMessage();
	void ExecuteImgProcTask(UINT mode);

private:
	CString m_msgProc;
	bool m_bProcDone;
	void MainloopImgProc();
	ImgProcTaskEnum m_enTask;
	std::shared_ptr<ITask_ImgAlgoProcBase> CreateTaskInstace(ImgProcTaskEnum enTask);
	MemFunThread<CalTaskImgProc>* m_thrImgProcWorker;								// 主工作執行緒   
};


