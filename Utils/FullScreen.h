/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/11 16:55
	FileName: 	FullScreen.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	对话框应用程序全屏显示
*********************************************************************/
#pragma once
#include "atltypes.h"


class CFullScreen
{
public:
	CFullScreen(CWnd * pParentWnd);
	virtual ~CFullScreen(void);

private:
	CWnd			*m_pParentWnd;

protected:
	WINDOWPLACEMENT m_OldWndPlacement;		// 用于保存原窗口位置
	BOOL			m_bFullScreen;			// 全屏显示标志
	CRect			m_rcFullScreen;			// 表示全屏显示时的窗口位置

public:
	void EnterFullScreen(void); 
	void EndFullScreen(void);

	BOOL IsFullScreen(void);
};

