// ImageButton.cpp : 实现文件
//

#include "stdafx.h"
#include "ImageButton.h"


// CImageButton

IMPLEMENT_DYNAMIC(CImageButton, CButton)

CImageButton::CImageButton()
	: m_pFont(NULL)
	, m_bMouseOver(FALSE)
	, m_bMouseDown(FALSE)
	, m_clrBkByNormal(0x00f2d272)
	, m_clrEdgeByNormal(RGB(160, 200, 255))
	, m_clrBkByOver(0x00f2e292)
	, m_clrEdgeByOver(RGB(160, 200, 255))
	, m_clrBkByDown(0x00f2d272)
	, m_clrEdgeByDown(0x00ffeeca)
	, m_clrGround(::GetSysColor(COLOR_BTNFACE))
	, m_nBitmapID(0)
	, m_bDisabled(FALSE)
	, m_nWidth(0)
	, m_nHeight(0)
{
	
}

CImageButton::~CImageButton()
{
}


BEGIN_MESSAGE_MAP(CImageButton, CButton)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CImageButton 消息处理程序



void CImageButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// 设置文本背景透明
	dc.SetBkMode(TRANSPARENT);

	// 设置文本字体
	if (m_pFont)
	{
		dc.SelectObject(m_pFont);
	}

	// 获取按钮尺寸
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);

	CBitmap bmp;
//	BITMAP bitmap;
	bmp.LoadBitmap(m_nBitmapID);
//	bmp.GetBitmap(&bitmap);

	int xSrc(0);
	int ySrc(0);

	// 按钮形态判断
	if (m_bDisabled == TRUE)
	{
		// 按钮失效
		xSrc = 90;
		ySrc = 0;
	}
	else if (m_bMouseOver == TRUE)
	{
		if (m_bMouseDown == FALSE)
		{
			// 鼠标经过
			xSrc = m_nWidth;
			ySrc = 0;
			
		} 
		else
		{
			// 鼠标按下
			xSrc = m_nWidth*2;
			ySrc = 0;
		}
	}
	else
	{
		// 正常形态
		xSrc = 0;
		ySrc = 0;
	}

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBmp(memDC.SelectObject(&memBmp));

	memDC.SelectObject(bmp);
	dc.BitBlt(0, 0, m_nWidth, m_nHeight, &memDC, xSrc, ySrc, SRCCOPY);

	// 释放资源
	ReleaseDC(&dc);
}

/*---------------------------------------------------------*\
|	鼠标进入													|
\*---------------------------------------------------------*/
void CImageButton::OnMouseMove(UINT nFlags, CPoint point)
{
	m_bMouseOver = TRUE;

	CButton::OnMouseMove(nFlags, point);
}

/*---------------------------------------------------------*\
|	鼠标离开													|
\*---------------------------------------------------------*/
void CImageButton::OnMouseLeave()
{
	m_bMouseOver = FALSE;

	CButton::OnMouseLeave();
}

/*---------------------------------------------------------*\
|	鼠标左键按下												|
\*---------------------------------------------------------*/
void CImageButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = TRUE;

	CButton::OnLButtonDown(nFlags, point);
}

/*---------------------------------------------------------*\
|	鼠标左键释放												|
\*---------------------------------------------------------*/
void CImageButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMouseDown = FALSE;

	CButton::OnLButtonUp(nFlags, point);
}

/*---------------------------------------------------------*\
|	清除背景													|
\*---------------------------------------------------------*/
BOOL CImageButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

	return CButton::OnEraseBkgnd(pDC);
}

/*---------------------------------------------------------*\
|	设置正常背景色											|
\*---------------------------------------------------------*/
void CImageButton::SetBackColorByNormal(COLORREF clrBk)
{
	m_clrBkByNormal = clrBk;
}

/*---------------------------------------------------------*\
|	设置正常边界色											|
\*---------------------------------------------------------*/
void CImageButton::SetEdgeColorByNormal(COLORREF clrEdge)
{
	m_clrEdgeByNormal = clrEdge;
}

/*---------------------------------------------------------*\
|	设置鼠标经过背景色										|
\*---------------------------------------------------------*/
void CImageButton::SetBackColorByOver(COLORREF clrBk)
{
	m_clrBkByOver = clrBk;
}

/*---------------------------------------------------------*\
|	设置鼠标经过边界色										|
\*---------------------------------------------------------*/
void CImageButton::SetEdgeColorByOver(COLORREF clrEdge)
{
	m_clrEdgeByOver = clrEdge;
}

/*---------------------------------------------------------*\
|	设置鼠标按下背景色										|
\*---------------------------------------------------------*/
void CImageButton::SetBackColorByDown(COLORREF clrBk)
{
	m_clrBkByDown = clrBk;
}

/*---------------------------------------------------------*\
|	设置鼠标按下边界色										|
\*---------------------------------------------------------*/
void CImageButton::SetEdgeColorByDown(COLORREF clrEdge)
{
	m_clrEdgeByDown = clrEdge;
}

/*---------------------------------------------------------*\
|	设置按钮底色												|
\*---------------------------------------------------------*/
void CImageButton::SetGroundColor(COLORREF clrGround)
{
	m_clrGround = clrGround;
}

/*---------------------------------------------------------*\
|	设置字体													|
\*---------------------------------------------------------*/
void CImageButton::SetFont(CFont * pFont)
{
	m_pFont = pFont;
}


BOOL CImageButton::SetImageList(BUTTON_STATUS state, UINT uBitmapID, const SIZE& buttonSize, DWORD numButtons, COLORREF mask)
{
	HRESULT hr(S_OK);
	HBITMAP hBitmap(NULL);
	HIMAGELIST hImageListTmp;
	int nImageList;

	hImageListTmp = ImageList_Create(buttonSize.cx, buttonSize.cy, ILC_COLOR24 | ILC_MASK, numButtons, 0);
	if (hImageListTmp == NULL)
	{
		hr = __HRESULT_FROM_WIN32(GetLastError());
	}

	if (SUCCEEDED(hr))
	{
		hBitmap = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(uBitmapID));
		if (hBitmap == NULL)
		{
			hr = __HRESULT_FROM_WIN32(GetLastError());
		}
	}

	if (SUCCEEDED(hr))
	{
		nImageList = ImageList_AddMasked(hImageListTmp, hBitmap, mask);
		if (nImageList == -1)
		{
			hr = __HRESULT_FROM_WIN32(GetLastError());
		}
	}

	if (SUCCEEDED(hr))
	{
		switch (state)
		{
		case BS_Normal:
			{
				m_hImageList = hImageListTmp;
				SendMessage(TB_SETIMAGELIST, 0, (LPARAM)hImageListTmp);
			}
			break;
		case BS_Hot:
			{
				m_hImageList = hImageListTmp;
				SendMessage(TB_SETHOTIMAGELIST, 0, (LPARAM)hImageListTmp);
			}
			break;
		case BS_Disabled:
			{
				m_hImageList = hImageListTmp;
				SendMessage(TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hImageListTmp);
			}
			break;
		default:
			hr = E_INVALIDARG;
			DeleteObject(hImageListTmp);
		}
	}

	if (hBitmap)
	{
		DeleteObject(hBitmap);
	}

	return SUCCEEDED(hr);
}

void CImageButton::SetBitmap(UINT nBitmapID)
{
	m_nBitmapID = nBitmapID;
}


BOOL CImageButton::Create(const RECT& rect, CWnd * pParentWnd, UINT nID)
{
	return CButton::Create(NULL, WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, rect, pParentWnd, nID);
}


BOOL CImageButton::EnableWindow(BOOL bEnable)
{
	m_bDisabled = !bEnable;

	Invalidate(FALSE);
	return TRUE;
}


BOOL CImageButton::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEMOVE)
	{
		m_toolTip.RelayEvent(pMsg);
	}

	if (m_bDisabled == TRUE)
	{
			if (pMsg->message == WM_LBUTTONDBLCLK)
			{
				return TRUE;
			}
	}

	return CButton::PreTranslateMessage(pMsg);
}


BOOL CImageButton::IsButtonEnable(void)
{
	return !m_bDisabled;
}


void CImageButton::SetButtonSize(CSize size)
{
	m_nWidth = size.cx;
	m_nHeight = size.cy;
}


void CImageButton::SetButtonSize(int cx, int cy)
{
	m_nWidth = cx;
	m_nHeight = cy;
}

int CImageButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
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


void CImageButton::SetToolTipText(CString strTip)
{
	m_toolTip.UpdateTipText(strTip, this);
}
