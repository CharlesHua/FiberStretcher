
// FiberStretcher.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFiberStretcherApp:
// �йش����ʵ�֣������ FiberStretcher.cpp
//

class CFiberStretcherApp : public CWinApp
{
public:
	CFiberStretcherApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFiberStretcherApp theApp;