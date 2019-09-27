#include "stdafx.h"
#include "CalBitmapData.h"
#include "alTaskImgProc.h"
#include "alViewerDoc.h"
#include "ITask_ImgProc.h"

CalTaskImgProc::CalTaskImgProc()
{
	m_bProcDone = true;
}

CalTaskImgProc::CalTaskImgProc(CDocument* pDoc)
{
	m_bProcDone = true;
	m_pDoc = pDoc;
}


CalTaskImgProc::~CalTaskImgProc()
{

}


std::shared_ptr<ITask_ImgAlgoProcBase> CalTaskImgProc::CreateTaskInstace(ImgProcTaskEnum enTask)
{
	switch (enTask)
	{
	case Task_Mirro:
		return std::shared_ptr<ITask_ImgAlgoProcBase>(new ITask_ImgProc_Mirror());
		break;

	case Task_Flip:
		return std::shared_ptr<ITask_ImgAlgoProcBase>(new ITask_ImgProc_Flip());
		break;

	case Task_RotateCW90:
		return std::shared_ptr<ITask_ImgAlgoProcBase>(new ITask_ImgProc_RotateP90());
		break;

	case Task_RotateCCW90:
		return std::shared_ptr<ITask_ImgAlgoProcBase>(new ITask_ImgProc_RotateN90());
		break;
	}

	return nullptr;
}

void CalTaskImgProc::MainloopImgProc()
{
	CalViewerDoc* pDoc = dynamic_cast<CalViewerDoc*>(m_pDoc);
	std::shared_ptr<ITask_ImgAlgoProcBase> obj = CreateTaskInstace(m_enTask);
	m_bProcDone = FALSE;
	m_msgProc.Empty();

	if (obj != nullptr)
	{		
		obj->Execute(pDoc->BmpData().get());
	}

	m_msgProc = obj->GetFinalMessage();
	m_bProcDone = TRUE;
}

void CalTaskImgProc::ExecuteImgProcTask(UINT mode)
{
	if (m_bProcDone)
	{
		m_enTask = (ImgProcTaskEnum)mode;
		m_thrImgProcWorker = new_MemFunThread(this, &CalTaskImgProc::MainloopImgProc);
		m_thrImgProcWorker->Execute();
		m_bProcDone = false;
	}
}

BOOL CalTaskImgProc::IsTaskProcDone()
{
	return m_bProcDone;
}

CString CalTaskImgProc::ProcMessage()
{
	return m_msgProc;
}


