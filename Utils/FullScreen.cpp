/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/11 17:09
	FileName: 	FullScreen.cpp
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	
*********************************************************************/
#include "StdAfx.h"
#include "FullScreen.h"


CFullScreen::CFullScreen(CWnd * pParentWnd)
	: m_pParentWnd(pParentWnd)
	, m_bFullScreen(FALSE)
{
}


CFullScreen::~CFullScreen(void)
{
}


//************************************************************************
// Method:    EnterFullScreen
// Purpose:   全屏显示
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFullScreen::EnterFullScreen(void)
{
	m_pParentWnd->GetWindowPlacement(&m_OldWndPlacement);

	CRect rcWindow;
	m_pParentWnd->GetWindowRect(&rcWindow);

	CRect rcClient;
	m_pParentWnd->RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, m_pParentWnd->reposQuery, &rcClient);
	m_pParentWnd->ClientToScreen(&rcClient);

	// 获取屏幕的分辨率
	int nFullWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nFullHeight = ::GetSystemMetrics(SM_CYSCREEN);

	// 将除控制条外的客户区全屏显示从（0, 0)到(nFullWidth, nFullHeight)区域
	// 将(0, 0)和(nFullWidth, nFullHeight)两个点外扩充原窗口和除控制条之外的客户区位置间的差值, 就得到全屏显示的窗口位置
	m_rcFullScreen.left	= rcWindow.left - rcClient.left;
	m_rcFullScreen.top = rcWindow.top - rcClient.top;
	m_rcFullScreen.right = rcWindow.right - rcClient.right + nFullWidth;
	m_rcFullScreen.bottom = rcWindow.bottom - rcClient.bottom + nFullHeight;

	// 设置全屏显示标志
	m_bFullScreen = TRUE;

	// 进入全屏显示状态
	WINDOWPLACEMENT wndpl = {0};
	wndpl.length = sizeof(WINDOWPLACEMENT);
	wndpl.flags = 0;
	wndpl.showCmd = SW_SHOWNORMAL;
	wndpl.rcNormalPosition = m_rcFullScreen;
	m_pParentWnd->SetWindowPlacement(&wndpl);
}


//************************************************************************
// Method:    EndFullScreen
// Purpose:	  退出全屏显示
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
//************************************************************************
void CFullScreen::EndFullScreen(void)
{
	if (m_bFullScreen)
	{
		m_pParentWnd->ShowWindow(SW_HIDE);
		m_pParentWnd->SetWindowPlacement(&m_OldWndPlacement);
		m_bFullScreen = FALSE;
	}
}


//************************************************************************
// Method:    IsFullScreen
// Purpose:	  判断是否处于全屏状态, 全屏时TRUE, 非全屏时FALSE
// Access:    public 
// Returns:   BOOL
// Qualifier:
// Parameter: void
//************************************************************************
BOOL CFullScreen::IsFullScreen(void)
{
	return m_bFullScreen;
}
