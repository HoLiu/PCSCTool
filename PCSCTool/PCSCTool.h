
// PCSCTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CPCSCToolApp: 
// �йش����ʵ�֣������ PCSCTool.cpp
//

class CPCSCToolApp : public CWinApp
{
public:
	CPCSCToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CPCSCToolApp theApp;