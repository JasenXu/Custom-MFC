/********************************************************************
	Copyright 	(C) 2014 All Right Reserved
	Created:	2014/11/12 18:26
	FileName: 	PaintTabCtrl.h
	Version:	1.0.0.0
	Author:		Jasen.Xu	
	Purpose:	自定义TAB控件的实现
*********************************************************************/
#pragma once
#include "afxwin.h"
#include "afxtempl.h"


// 标签页切换消息
#ifndef WM_SELCHANGE
#define WM_SELCHANGE	(WM_USER+1)
#endif


/************************************************************************/
/* 标签基本类                                                             */
/************************************************************************/
class CPaintTabItem : public CButton
{
public:
	// 标签状态
	enum StatusFlags
	{
		statusNormal,		// 常态
		statusMove,			// 鼠标经过
		statusSelect,		// 选中
	};

	CPaintTabItem(void)
		: m_nItemWidth(78)
		, m_nItemHeight(30)
		, m_nStatusFlag(0)
		, m_pFont(NULL)
		, m_hIcon(NULL)
		, m_nIndex(0)
	{
	}

	~CPaintTabItem(void){}

protected:
	COLORREF		m_clrBk;			// 标签底色
	COLORREF		m_clrTxt;			// 标签文本色
	LPCTSTR			m_lpszTabName;		// 标签名
	int				m_nItemWidth;		// 标签宽度
	int				m_nItemHeight;		// 标签高度
	UINT			m_nStatusFlag;		// 标签状态
	CFont			*m_pFont;			// 标签字体
	HICON			m_hIcon;			// 图标句柄
	int				m_nIndex;			// 标签在TAB控件中的序号
	CToolTipCtrl	m_toolTip;			// 提示信息		

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
	//************************************************************************
	// Method:    SetFont
	// Purpose:   设置标签字体
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CFont * pFont
	//************************************************************************
	void SetFont(CFont * pFont)
	{
		m_pFont = pFont;
	}

	//************************************************************************
	// Method:    Create
	// Purpose:   创建标签
	// Access:    public 
	// Returns:   BOOL	成功为TRUE，失败为FALSE
	// Qualifier:
	// Parameter: const RECT & rect		标签尺寸
	// Parameter: LPCTSTR lpszTabName	标签名
	// Parameter: CWnd * pParentWnd		父窗口
	// Parameter: HICON hIcon			图标句柄
	// Parameter: CFont * pFont			标签字体
	//************************************************************************
	BOOL Create(int nIndex, const SIZE& size, LPCTSTR lpszTabName, CWnd * pParentWnd, HICON hIcon = NULL, CFont * pFont = NULL)
	{
		m_lpszTabName = lpszTabName;
		m_hIcon = hIcon;
		m_pFont = pFont;
		m_nItemWidth = size.cx;
		m_nItemHeight = size.cy;
		m_nIndex = nIndex;

		return CButton::Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(0, 0, size.cx, size.cy), pParentWnd, NULL);
	}

	//************************************************************************
	// Method:    SetSize
	// Purpose:   设置标签尺寸
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: int nWidth	标签宽度		-1表示保持原数值
	// Parameter: int nHeight	标签高度		-1表示保持原数值
	//************************************************************************
	void SetSize(int nWidth, int nHeight)
	{
		if (nWidth > 0)
		{
			m_nItemWidth = nWidth;
		}
		
		if (nHeight > 0)
		{
			m_nItemHeight = nHeight;
		}	
	}

	//************************************************************************
	// Method:    SetTabStatus
	// Purpose:   设置标签状态
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: UINT nStatusFlag
	//************************************************************************
	void SetTabStatus(UINT nStatusFlag)
	{
		m_nStatusFlag = nStatusFlag;

		// 刷新显示标签
		Invalidate(FALSE);
	}
	

	//************************************************************************
	// Method:    SetToolTipText
	// Brief:	  更新工具提示信息
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: CString strTip	新的提示信息
	//************************************************************************
	void SetToolTipText(CString strTip)
	{
		if (m_toolTip.GetSafeHwnd() == NULL)
		{
			// 启用工具提示控件
			EnableToolTips(TRUE);

			m_toolTip.Create(this);

			// 注册工具提示控件
			m_toolTip.AddTool(this);

			// 激活工具提示控件
			m_toolTip.Activate(TRUE);
		}
		
		m_toolTip.UpdateTipText(strTip, this);
	}
};


/************************************************************************/
/* 自定义控件实现类									                    */
/************************************************************************/
class CPaintTabCtrl : public CWnd
{
private:
	// 标签管理
	struct TabMgr
	{
		CPaintTabItem	tab;		// 标签
		CDialogEx		*pDlg;		// 标签页对话框
	};

public:
	// StyleFlags
	enum TabStyleFlags
	{
		styleLeft		= 0x00000001,		// 标签靠左排列
		styleCenter		= 0x00000002,		// 标签居中排列
		styleHorizontal	= 0x00000004,		// 标签横向排列
		styleVertical	= 0x00000008,		// 标签纵向排列
		styleSplit		= 0x00000010,		// 标签均匀排列
	};

	CPaintTabCtrl(void);
	virtual ~CPaintTabCtrl(void);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnSelchange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNcHitTest(CPoint point);

protected:
	int					m_nCurSel;				// 当前显示的标签页序号
	CArray<TabMgr *>	m_arTabMgr;				// 标签页管理组
	CFont				m_font;					// 控件默认字体
	CFont				*m_pFont;				// 控件自定义字体
	int					m_nHighLineHeight;		// 标签栏底部高亮线的高度
	int					m_nTabItemWidth;		// 标签宽度
	int					m_nTabItemHeight;		// 标签高度
	UINT				m_nTabStyle;			// 标签栏样式

public:
	BOOL Create(const RECT& rect, CWnd * pParent, UINT nID = NULL);	

	LONG AddItem(CDialogEx * pDlg, UINT nIDDialog, const SIZE& size, LPCTSTR lpszTabName, UINT nIDIcon = NULL, CString strToolTip = NULL);

	void SetItemSize(int nWidth, int nHeight);
	void SetFont(CFont * pFont);

	void GetClientRect(LPRECT lpRect) const;
	void SetTabStyle(UINT nStyleFlags);
	UINT GetTabStyle(void) const;
};

