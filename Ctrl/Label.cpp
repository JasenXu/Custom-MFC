/********************************************************************
 * Copyright (C) 2014 All Right Reserved
 * \date 	 2014/11/21 16:20
 * \file 	 Label.cpp
 * \version  1.0.0.0
 * \author 	 JasenXu
 * \brief 	 自定义标签的实现
*********************************************************************/
#include "StdAfx.h"
#include "Label.h"


CLabel::CLabel()
	: m_strLabel(_T("示例文本"))
	, m_pFont(NULL)
	, m_bNoBorder(FALSE)
	, m_strPrefix(_T(""))
	, m_nFormat(DT_LEFT | DT_VCENTER | DT_SINGLELINE)
{
	m_clrText = GetSysColor(COLOR_WINDOWTEXT);
	m_clrBkgr = GetSysColor(COLOR_WINDOW);
	m_clrBorder = RGB(133, 145, 162);

	m_font.CreateFont(
		13,							// nHeight
		0,							// nWidth
		0,							// nEscapement
		0,							// nOrientation
		FW_NORMAL,					// nWeight
		FALSE,						 // bItalic
		FALSE,						 // bUnderline
		0,							 // cStrikeOut
		DEFAULT_CHARSET,			 // nCharSet
		OUT_DEFAULT_PRECIS,			 // nOutPrecision
		CLIP_DEFAULT_PRECIS,			// nClipPrecision
		DEFAULT_QUALITY,				// nQuality
		DEFAULT_PITCH | FF_SWISS,		// nPitchAndFamily
		_T("宋体"));					 // lpszFacename
}


CLabel::~CLabel()
{
}


BEGIN_MESSAGE_MAP(CLabel, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	// 设置字体
	if (m_pFont)
	{
		memDC.SelectObject(m_pFont);
	}
	else
	{
		memDC.SelectObject(&m_font);
	}
	
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	memDC.SelectObject(&memBmp);

	// 绘制底色
	memDC.FillSolidRect(&rc, m_clrBkgr);

	// 设置文本色
	memDC.SetTextColor(m_clrText);

	CRect rcBorder;
	CRect rcText;
	if (m_strPrefix.IsEmpty() == FALSE)
	{
		CSize size = memDC.GetTextExtent(m_strPrefix);
		memDC.DrawText(m_strPrefix, &CRect(rc.left + 2, rc.top, rc.left + 2 + size.cx, rc.bottom),
			DT_LEFT | DT_VCENTER | DT_SINGLELINE);

		rcBorder = rc;
		rcBorder.left += 4;
		rcBorder.left = rcBorder.left + size.cx + 2;

		rcText = rcBorder;
	}
	else
	{
		rcBorder = rc;
		rcText = rc;
	}

	// 绘制边框
	if (m_bNoBorder == FALSE)
	{
		memDC.FrameRect(&rcBorder, &CBrush(m_clrBorder));
	}
	
	// 绘制文本
	rcText.left += 4;
	rcText.right -= 2;
	if ((m_nFormat & DT_SINGLELINE )== 0)
	{
		rcText.top += 10;
	}
	memDC.DrawText(m_strLabel, &rcText, m_nFormat);

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CLabel::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}


//************************************************************************
// Method:    SetLabelText
// Brief:	  设置标签文本内容
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString strLabel		文本内容
//************************************************************************
void CLabel::SetLabelText(CString strLabel)
{
	m_strLabel = strLabel;

	// 更新提示信息
	m_toolTip.UpdateTipText(m_strLabel, this);

	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetLabelInt
// Brief:	  设置标签数值内容
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nLabel			数值内容
//************************************************************************
void CLabel::SetLabelInt(int nLabel)
{
	m_strLabel.Format(_T("%d"), nLabel);

	// 更新提示信息
	m_toolTip.UpdateTipText(m_strLabel, this);

	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetTextColor
// Brief:	  设置文本色
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrText
//************************************************************************
void CLabel::SetTextColor(COLORREF clrText)
{
	m_clrText = clrText;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetBkColor
// Brief:	  设置背景色
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrBkgr
//************************************************************************
void CLabel::SetBkColor(COLORREF clrBkgr)
{
	m_clrBkgr = clrBkgr;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    Create
// Brief:	  创建标签
// Access:    public 
// Returns:   BOOL			成功为TRUE，否则为FALSE
// Qualifier:
// Parameter: const RECT & rect		标签尺寸
// Parameter: CWnd * pParentWnd		父窗口
//************************************************************************
BOOL CLabel::Create(const RECT& rect, CWnd * pParentWnd)
{
	if (rect.right-rect.left <= 0 || rect.bottom-rect.top <= 0)
	{
		return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE,
			CRect(0, 0, 200, 22), pParentWnd, NULL);
	}

	return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rect, pParentWnd, NULL);
}


//************************************************************************
// Method:    SetNoBorder
// Brief:	  设置有无边框
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: BOOL bNoBorder		TRUE表示无边框，FALSE表示有边框
//************************************************************************
void CLabel::SetNoBorder(BOOL bNoBorder)
{
	m_bNoBorder = bNoBorder;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetBorderColor
// Brief:	  设置边框颜色
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: COLORREF clrBorder
//************************************************************************
void CLabel::SetBorderColor(COLORREF clrBorder)
{
	m_clrBorder = clrBorder;
	Invalidate(FALSE);
}


//************************************************************************
// Method:    SetFont
// Brief:	  设置自定义字体
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CFont * pFont
//************************************************************************
void CLabel::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}


//************************************************************************
// Method:    SetPrefixString
// Brief:	  设置前缀信息
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CString strPrefix
//************************************************************************
void CLabel::SetPrefixString(CString strPrefix)
{
	m_strPrefix = strPrefix;
}


BOOL CLabel::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}

	return CWnd::PreTranslateMessage(pMsg);
}


//************************************************************************
// Method:    OnCreate
// Brief:	  在窗口创建后，添加ToolTip
// Access:    protected 
// Returns:   int
// Qualifier:
// Parameter: LPCREATESTRUCT lpCreateStruct
//************************************************************************
int CLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 启用工具提示控件
	EnableToolTips(TRUE);

	m_toolTip.Create(this);
	
	// 注册工具提示控件
	m_toolTip.AddTool(this);

	// 激活工具提示控件
	m_toolTip.Activate(TRUE);

	return 0;
}


//************************************************************************
// Method:    SetTextFormat
// Brief:	  设置文本对齐方式
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: UINT nFormat
//************************************************************************
void CLabel::SetTextFormat(UINT nFormat)
{
	m_nFormat = nFormat;
}
