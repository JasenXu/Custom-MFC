#include "StdAfx.h"
#include "PaintTabCtrl.h"

/************************************************************************/
/* CPaintTabItem 消息响应函数                                             */
/************************************************************************/
BEGIN_MESSAGE_MAP(CPaintTabItem, CButton)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



void CPaintTabItem::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 获取标签尺寸
	CRect rc;
	GetWindowRect(&rc);
	ScreenToClient(&rc);

	// 根据不同状态，绘制不同颜色的标签
	switch (m_nStatusFlag)
	{
#ifdef DARK_BACKGR
	case statusMove:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(28, 151, 234);
		break;
	case statusSelect:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(0, 122, 204);
		break;
	case statusNormal:
	default:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(45, 45, 48);
		break;
#else
	case statusMove:
		m_clrTxt = RGB(240, 240, 240);
		m_clrBk = RGB(91, 113, 153);
		break;
	case statusSelect:
		m_clrTxt = RGB(0, 0, 0);
		m_clrBk = RGB(255, 242, 157);
		break;
	case statusNormal:
	default:
		m_clrTxt = RGB(255, 255, 255);
		m_clrBk = RGB(54, 78, 111);
		break;
#endif
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	if (m_pFont)
	{
		memDC.SelectObject(m_pFont);
	}

	// 填涂标签底色
	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);
	memDC.FillSolidRect(&rc, m_clrBk);

	// 显示标签名
	memDC.SetTextColor(m_clrTxt);
	if (m_hIcon)
	{
		ICONINFO info = {0};
		GetIconInfo(m_hIcon, &info);

		memDC.DrawIcon(6, m_nItemHeight/2-info.yHotspot, m_hIcon);
		rc.left = rc.left + 2*info.xHotspot + 6 + 7;

		memDC.DrawText(m_lpszTabName, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}
	else
	{
		memDC.DrawText(m_lpszTabName, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
	}

	dc.BitBlt(0, 0, m_nItemWidth, m_nItemHeight, &memDC, 0, 0, SRCCOPY);

	// 释放资源
	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CPaintTabItem::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CButton::OnEraseBkgnd(pDC);
}


//************************************************************************
// Method:    OnMouseMove
// Purpose:   更改标签状态为鼠标经过
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: UINT nFlags
// Parameter: CPoint point
//************************************************************************
void CPaintTabItem::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_nStatusFlag != statusSelect)
	{
		m_nStatusFlag = statusMove;
	}
	
	CButton::OnMouseMove(nFlags, point);
}


//************************************************************************
// Method:    OnMouseLeave
// Purpose:   恢复标签状态为常态
// Access:    protected 
// Returns:   void
// Qualifier:
//************************************************************************
void CPaintTabItem::OnMouseLeave()
{
	if (m_nStatusFlag != statusSelect)
	{
		m_nStatusFlag = statusNormal;
	}
	
	CButton::OnMouseLeave();
}


//************************************************************************
// Method:    OnLButtonDown
// Purpose:   更改标签状态为选中
// Access:    protected 
// Returns:   void
// Qualifier:
// Parameter: UINT nFlags
// Parameter: CPoint point
//************************************************************************
void CPaintTabItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_nStatusFlag = statusSelect;

	// 通知CPaintTabCtrl管理类切换标签页
	::PostMessage(GetParent()->GetSafeHwnd(), WM_SELCHANGE, (WPARAM)m_nIndex, NULL);

	CButton::OnLButtonDown(nFlags, point);
}


BOOL CPaintTabItem::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}

	return CButton::PreTranslateMessage(pMsg);
}


/************************************************************************/
/* CPaintTabCtrl 消息响应函数                                             */
/************************************************************************/
CPaintTabCtrl::CPaintTabCtrl(void)
	: m_nCurSel(0)
	, m_pFont(NULL)
	, m_nHighLineHeight(2)
	, m_nTabItemWidth(78)
	, m_nTabItemHeight(30)
	, m_nTabStyle(0x00000001)
{
	m_font.CreateFont(
		20,							// nHeight
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
		_T("Arial"));					 // lpszFacename
}


CPaintTabCtrl::~CPaintTabCtrl(void)
{
	while (m_arTabMgr.GetSize())
	{
		TabMgr *p = m_arTabMgr.GetAt(0);
		if (p)
		{
			delete p;
			p = NULL;
		}
		m_arTabMgr.RemoveAt(0);
	}
}


BEGIN_MESSAGE_MAP(CPaintTabCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_MESSAGE(WM_SELCHANGE, &CPaintTabCtrl::OnSelchange)
	ON_WM_ERASEBKGND()
//	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


void CPaintTabCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rc;
	CWnd::GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap *pOldBmp = memDC.SelectObject(&memBmp);

	// 绘制控件标签栏底色
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, 0, rc.Width(), m_nTabItemHeight, RGB(45, 45, 48));
#else
	memDC.FillSolidRect(0, 0, rc.Width(), m_nTabItemHeight, RGB(41, 57, 86));
#endif

	// 绘制高亮线
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, m_nTabItemHeight, rc.Width(), m_nHighLineHeight, RGB(0, 122, 204));
//	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight, rc.Width(),
//		1, RGB(0, 122, 204));
#else
	memDC.FillSolidRect(0, m_nTabItemHeight, rc.Width(), m_nHighLineHeight, RGB(255, 242, 157));
	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight, rc.Width(),
		1, RGB(133, 145, 162));
#endif

	// 绘制控件底色
#ifdef DARK_BACKGR
	memDC.FillSolidRect(0, m_nTabItemHeight+m_nHighLineHeight+1, rc.Width(), 
		rc.Height()-(m_nTabItemHeight+m_nHighLineHeight+1), RGB(45, 45, 48));
#else
	memDC.FillSolidRect(0, m_nTabItemHeight + m_nHighLineHeight+1, rc.Width(),
		rc.Height() - (m_nTabItemHeight + m_nHighLineHeight+1), RGB(255, 255, 255));
#endif

	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CPaintTabCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}


void CPaintTabCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (m_arTabMgr.GetSize() <= 0)
	{
		return;
	}

	// 调整所有标签的位置
	for (int i(0); i < m_arTabMgr.GetSize(); ++i)
	{
		TabMgr *p = m_arTabMgr.GetAt(i);
		if (i == m_arTabMgr.GetSize()-1 && p->pDlg == NULL)
		{
			p->tab.MoveWindow(max(i*(m_nTabItemWidth), cx-m_nTabItemWidth), 0, m_nTabItemWidth, m_nTabItemHeight);
		}
		else
		{
			if (m_nTabStyle&styleLeft)
			{
				p->tab.MoveWindow(i*(m_nTabItemWidth), 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			else if (m_nTabStyle&styleCenter && m_nTabStyle&styleSplit)
			{
				int nSplit = (cx-2*m_nTabItemWidth-m_arTabMgr.GetSize()*m_nTabItemWidth) / m_arTabMgr.GetSize();
				p->tab.MoveWindow(m_nTabItemWidth+i*(m_nTabItemWidth+nSplit), 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			else if (m_nTabStyle&styleCenter)
			{
				int nIndent = (cx - m_arTabMgr.GetSize()*m_nTabItemWidth) / 2;
				p->tab.MoveWindow(nIndent + i*m_nTabItemWidth, 0, m_nTabItemWidth, m_nTabItemHeight);
			}
			p->pDlg->MoveWindow(0, m_nTabItemHeight + m_nHighLineHeight + 1, cx, cy - (m_nTabItemHeight + m_nHighLineHeight + 1));
		}	
	}

	// 调整当前标签页的位置
/*	TabMgr *p = m_arTabMgr.GetAt(m_nCurSel);
	if (p->pDlg)
	{
		p->pDlg->MoveWindow(0, m_nTabItemHeight+m_nHighLineHeight+1, cx, cy-(m_nTabItemHeight+m_nHighLineHeight+1));
	}	
*/
}


//************************************************************************
// Method:    Create
// Purpose:   创建TAB控件
// Access:    public 
// Returns:   BOOL		成功返回TRUE，失败返回FLASE
// Qualifier:
// Parameter: const RECT & rect		控件尺寸
// Parameter: CWnd * pParent		父窗口
// Parameter: UINT nID				控件资源ID
//************************************************************************
BOOL CPaintTabCtrl::Create(const RECT& rect, CWnd * pParent, UINT nID)
{
	return CWnd::Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rect, pParent, nID);
}


//************************************************************************
// Method:    AddItem
// Purpose:   添加新的标签
// Access:    public 
// Returns:   LONG		新标签的序号，失败返回-1
// Qualifier:
// Parameter: CDialogEx * pDlg		标签页对话框
// Parameter: UINT nIDDialog		对话框资源ID
// Parameter: const RECT & rect		标签尺寸
// Parameter: LPCTSTR lpszTabName	标签名
// Parameter: HICON hIcon			图标句柄
//************************************************************************
LONG CPaintTabCtrl::AddItem(CDialogEx * pDlg, UINT nIDDialog, const SIZE& size, LPCTSTR lpszTabName, UINT nIDIcon, CString strToolTip)
{
	// 采用最大的尺寸
	if (m_nTabItemWidth < size.cx)
	{
		m_nTabItemWidth = size.cx;
		SetItemSize(m_nTabItemWidth, -1);
	}
	
	if (m_nTabItemHeight < size.cy)
	{
		m_nTabItemHeight = size.cy;
		SetItemSize(-1, m_nTabItemHeight);
	}

	// 获取图标句柄
	HICON hIcon(NULL);
	if (nIDIcon != NULL)
	{
		hIcon = AfxGetApp()->LoadIconW(nIDIcon);
		if (hIcon == NULL)
		{
			// 如果获取失败，则返回-1
			return -1;
		}

		// 根据图标大小调整标签栏高度
		ICONINFO info = {0};
		GetIconInfo(hIcon, &info);

		if (2*(info.yHotspot+4) > (DWORD)m_nTabItemHeight)
		{
			m_nTabItemHeight = 2*(info.yHotspot+4);
		}
	}

	// 创建标签
	BOOL bPass(FALSE);
	TabMgr *p = new TabMgr;
	if (m_pFont)
	{
		bPass = p->tab.Create(m_arTabMgr.GetSize(), CSize(m_nTabItemWidth, m_nTabItemHeight), lpszTabName, this, hIcon, m_pFont);
	} 
	else
	{
		bPass = p->tab.Create(m_arTabMgr.GetSize(), CSize(m_nTabItemWidth, m_nTabItemHeight), lpszTabName, this, hIcon, &m_font);
	}
	
	if (bPass == FALSE)
	{
		return -1;
	}

	p->tab.SetToolTipText(strToolTip);

	// 创建标签页
	p->pDlg = pDlg;
	if (pDlg)
	{
		bPass = pDlg->Create(nIDDialog, this);
	}
	

	if (bPass == FALSE)
	{
		return -1;
	}

	// 添加到标签页管理组
	m_arTabMgr.Add(p);
	if (m_arTabMgr.GetSize() == 1)
	{
		// 如果是第一个标签，则显示它
		p->tab.SetTabStatus(CPaintTabItem::statusSelect);
		if (pDlg)
		{
			pDlg->ShowWindow(SW_SHOW);
		}		
		m_nCurSel = 0;	
	}
	else
	{
		if (pDlg)
		{
			pDlg->ShowWindow(SW_HIDE);
		}	
	}
	
	return LONG(m_arTabMgr.GetSize()-1);
}


//************************************************************************
// Method:    SetItemSize
// Purpose:   设置标签尺寸
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: int nWidth		标签宽度		-1表示保持原数值
// Parameter: int nHeight		标签高度		-1表示保持原数值
//************************************************************************
void CPaintTabCtrl::SetItemSize(int nWidth, int nHeight)
{
	for (int i(0); i < m_arTabMgr.GetSize(); ++i)
	{
		TabMgr *p = m_arTabMgr.GetAt(i);
		p->tab.SetSize(nWidth, nHeight);
	}
}


//************************************************************************
// Method:    SetFont
// Purpose:   设置标签字体
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: CFont * pFont
//************************************************************************
void CPaintTabCtrl::SetFont(CFont * pFont)
{
	m_pFont = pFont;

	if (pFont)
	{
		for (int i(0); i < m_arTabMgr.GetSize(); ++i)
		{
			TabMgr *p = m_arTabMgr.GetAt(i);
			p->tab.SetFont(pFont);
		}
	}
}


//************************************************************************
// Method:    OnSelchange
// Purpose:   切换标签页
// Access:    protected 
// Returns:   LRESULT
// Qualifier:
// Parameter: WPARAM wParam		要显示的标签页序号
// Parameter: LPARAM lParam
//************************************************************************
afx_msg LRESULT CPaintTabCtrl::OnSelchange(WPARAM wParam, LPARAM lParam)
{
	// 如果出现标签序号大于管理组成员数的情况，则退出程序
	if ((int)wParam >= m_arTabMgr.GetSize())
	{
		::PostQuitMessage(0);
		return 0;
	}

	// 如果重复点击当前显示的标签页，则直接返回
	if (m_nCurSel == (int)wParam)
	{
		return 0;
	}

	// 隐藏先前显示的标签页
	TabMgr *p = m_arTabMgr.GetAt(m_nCurSel);
	if (p->pDlg)
	{
		p->pDlg->ShowWindow(SW_HIDE);
	}	

	// 重置之前的标签状态为常态
	p->tab.SetTabStatus(CPaintTabItem::statusNormal);

	// 获取当前显示的标签页序号
	m_nCurSel = (int)wParam;

	// 获取当前标签页的尺寸
	p = m_arTabMgr.GetAt(m_nCurSel);
	CRect rcShow;
	if (p->pDlg == NULL)
	{
		::PostQuitMessage(0);
		return 0;
		
	}

	p->pDlg->GetWindowRect(&rcShow);
	ScreenToClient(&rcShow);

	// 获取TAB控件客户区尺寸
	CRect rcTabCtrl;
	GetClientRect(&rcTabCtrl);

	// 如果要显示的标签页未铺满控件客户区，则调整标签页尺寸
	if (rcShow != rcTabCtrl)
	{
		p->pDlg->MoveWindow(&rcTabCtrl);
	}

	// 显示当前标签页
	p->pDlg->ShowWindow(SW_SHOW);
	return 0;
}


//************************************************************************
// Method:    GetClientRect
// Purpose:   获取TAB控件客户区的尺寸
// Access:    public 
// Returns:   void
// Qualifier: const
// Parameter: LPRECT lpRect
//************************************************************************
void CPaintTabCtrl::GetClientRect(LPRECT lpRect) const
{
	CRect rc;
	CWnd::GetClientRect(&rc);

	lpRect->left = rc.left;
	lpRect->top = rc.top + m_nTabItemHeight + m_nHighLineHeight + 1;
	lpRect->right = lpRect->left + rc.Width();
	lpRect->bottom = rc.bottom;	
}


//************************************************************************
// Method:    SetTabStyle
// Purpose:   设置标签栏样式
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: UINT nStyleFlags
//************************************************************************
void CPaintTabCtrl::SetTabStyle(UINT nStyleFlags)
{
	m_nTabStyle = nStyleFlags;
}


//************************************************************************
// Method:    GetTabStyle
// Purpose:   获取标签栏样式
// Access:    public 
// Returns:   UINT
// Qualifier: const
// Parameter: void
//************************************************************************
UINT CPaintTabCtrl::GetTabStyle(void) const
{
	return m_nTabStyle;
}


//************************************************************************
// Method:    OnNcHitTest
// Purpose:   拖动控件
// Access:    protected 
// Returns:   LRESULT
// Qualifier:
// Parameter: CPoint point
//************************************************************************
LRESULT CPaintTabCtrl::OnNcHitTest(CPoint point)
{
	CRect rc;
	GetClientRect(&rc);
	CRect rcBar(CPoint(0, 0), CSize(rc.Width(), m_nTabItemHeight+m_nHighLineHeight));

	if (rcBar.PtInRect(point))
	{
		//return HTCAPTION;
		//::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_NCHITTEST, NULL, NULL);
	}

	return CWnd::OnNcHitTest(point);
}


