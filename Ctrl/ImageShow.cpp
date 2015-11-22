#include "StdAfx.h"
#include "ImageShow.h"


CImageShow::CImageShow()
	: m_pImageBuf(NULL)
	, m_lpbi(NULL)
	, m_nShowMode(FULL_WINDOW)
	, m_hDrawDib(NULL)
	, m_dwScale(1.0)
	, m_nZoom(100)
	, m_nImageSize(0)
{
	m_clrBkgr = RGB(0, 0, 0);
	m_pImageBuf = new BYTE[IMAGE_SHOW_BUF_SIZE]();

	m_fontTag.CreateFont(
		40,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,           // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Times New Roman"));    // lpszFacename

	m_hDrawDib = ::DrawDibOpen();
}


CImageShow::~CImageShow()
{
	if (m_hDrawDib)
	{
		::DrawDibClose(m_hDrawDib);
	}

	SecureDeleteBuffer(m_pImageBuf);
}


BEGIN_MESSAGE_MAP(CImageShow, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(0x5901, OnFullWindow)
	ON_COMMAND(0x5910, OnScale50)
	ON_COMMAND(0x5914, OnScale100)
	ON_COMMAND(0x5918, OnScale200)
END_MESSAGE_MAP()


void CImageShow::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	CRect rc;
	GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);

	CBitmap memBmp;
	memBmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* pOldBmp(memDC.SelectObject(&memBmp));

	if (m_pImageBuf == NULL || m_nImageSize == 0)
	{
		memDC.FillSolidRect(&rc, RGB(0, 0, 0));
		memDC.SetTextColor(RGB(240, 240, 240));
		memDC.SelectObject(&m_fontTag);
		memDC.DrawText(_T("NO  IMAGE"), &rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	else
	{
		memDC.FillSolidRect(&rc, m_clrBkgr);
		DrawBitmap(memDC.m_hDC);
	}
	
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBmp);
	memBmp.DeleteObject();
	memDC.DeleteDC();
}


BOOL CImageShow::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}


void CImageShow::OnRButtonDown(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	CMenu menu;
	VERIFY(menu.CreatePopupMenu());

	if (m_nShowMode == FULL_WINDOW)
	{
		menu.AppendMenu(MF_STRING | MF_CHECKED, 0x5901, _T("适应窗口大小"));
	}
	else
	{
		menu.AppendMenu(MF_STRING, 0x5901, _T("适应窗口大小"));
	}

	if (m_nShowMode == SCALE_50)
	{
		menu.AppendMenu(MF_STRING | MF_CHECKED, 0x5910, _T("一半大小"));
	}
	else
	{
		menu.AppendMenu(MF_STRING, 0x5910, _T("一半大小"));
	}

	if (m_nShowMode == SCALE_100)
	{
		menu.AppendMenu(MF_STRING | MF_CHECKED, 0x5914, _T("正常大小"));
	}
	else
	{
		menu.AppendMenu(MF_STRING, 0x5914, _T("正常大小"));
	}

	if (m_nShowMode == SCALE_200)
	{
		menu.AppendMenu(MF_STRING | MF_CHECKED, 0x5918, _T("2倍大小"));
	}
	else
	{
		menu.AppendMenu(MF_STRING, 0x5918, _T("2倍大小"));
	}

	::TrackPopupMenu(menu.m_hMenu, 0, point.x,
		point.y, 0, m_hWnd, NULL);

	CWnd::OnRButtonDown(nFlags, point);
}


void CImageShow::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDblClk(nFlags, point);
}


BOOL CImageShow::PreCreateWindow(CREATESTRUCT& cs)
{
	static LPTSTR szClassName(NULL);

	if (!CWnd::PreCreateWindow(cs))
	{
		return FALSE;
	}

	if (szClassName == NULL)
	{
		WNDCLASS wndClass;
		::GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndClass);
		wndClass.lpszClassName = _T("Custom Image Show Class");
		wndClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = ::DefWindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = AfxGetInstanceHandle();
		wndClass.hIcon = NULL;
		wndClass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wndClass.lpszMenuName = NULL;
		if (!AfxRegisterClass(&wndClass))
			AfxThrowResourceException();

		szClassName = _T("Custom Image Show Class");
	}

	cs.lpszClass = szClassName;

	return TRUE;
}


BOOL CImageShow::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}


void CImageShow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO sih;
	GetScrollInfo(SB_HORZ, &sih, SIF_ALL);
	switch (nSBCode)
	{
		case SB_LINEUP:
			if (sih.nPos > sih.nMin)
				sih.nPos--;
			break;
		case SB_LINEDOWN:
			if (sih.nPos < (sih.nMax - (int)sih.nPage))
				sih.nPos++;
			break;
		case SB_PAGEUP:
			sih.nPos -= sih.nPage;
			if (sih.nPos<sih.nMin)
				sih.nPos = sih.nMin;
			break;
		case SB_PAGEDOWN:
			sih.nPos += sih.nPage;
			if (sih.nPos>sih.nMax)
				sih.nPos = sih.nMax;
		case SB_TOP:
			sih.nPos = sih.nMin;
			break;
		case SB_BOTTOM:
			sih.nPos = sih.nMax;
			break;
		case SB_THUMBTRACK:
		case SB_THUMBPOSITION:
			sih.nPos = sih.nTrackPos;
			break;
	}
	SetScrollInfo(SB_HORZ, &sih, TRUE);
	Invalidate();

//	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CImageShow::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, SIF_ALL);

	switch (nSBCode)
	{
		case SB_LINEUP:
			if (si.nPos > si.nMin)
				si.nPos--;
			break;

		case SB_LINEDOWN:
			if (si.nPos <= si.nMax - (int)si.nPage)
				si.nPos++;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			if (si.nPos < si.nMin)
				si.nPos = si.nMin;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			if (si.nPos > si.nMax)
				si.nPos = si.nMax;
			break;

		case SB_TOP:
			si.nPos = si.nMin;
			break;

		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
	}

	if (si.nPos == si.nMin)
	{
		if (si.nMax - si.nMin + 1 <= GetImageHeight() / VERT_SCROLL_HEIGHT)
		{
			SetAllScroll();
		}
	}

	SetScrollPos(SB_VERT, si.nPos);

	Invalidate();

//	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CImageShow::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	int nPos = zDelta / 120;
	if (nPos > 0)
	{
		PostMessage(WM_VSCROLL, (WPARAM)SB_PAGEUP);
	}
	else
	{
		PostMessage(WM_VSCROLL, (WPARAM)SB_PAGEDOWN);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CImageShow::DrawBitmap(HDC hDC)
{
	CRect rc;
	GetClientRect(rc);

	m_lpbi = (BITMAPINFOHEADER *)(m_pImageBuf + sizeof(BITMAPFILEHEADER));
	BYTE *lpData = m_pImageBuf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	if (m_hDrawDib == NULL)
	{
		return;
	}
		
	//绘制图像
	int nHorz = GetScrollPos(SB_HORZ);
	int nVert = GetScrollPos(SB_VERT);
	if (m_nShowMode == FULL_WINDOW)
	{
		DrawDibDraw(m_hDrawDib, hDC,
			0,
			0,
			rc.Width(),
			rc.Height(),
			m_lpbi,
			(void *)lpData,
			0, 0,
			m_lpbi->biWidth,
			m_lpbi->biHeight,
			DDF_SAME_HDC);
	}
	else
	{
		DrawDibDraw(m_hDrawDib, hDC,
			-nHorz,
			-nVert*VERT_SCROLL_HEIGHT,
			GetImageWidth(),
			GetImageHeight(),
			m_lpbi,
			(void *)lpData,
			0, 0,
			m_lpbi->biWidth,
			m_lpbi->biHeight,
			DDF_SAME_HDC);
	}
}


int CImageShow::GetImageWidth()
{
	if (m_lpbi)
	{
		return (int)(m_lpbi->biWidth*m_dwScale);
	}
	return 0;
}


int CImageShow::GetImageHeight()
{
	if (m_lpbi)
	{
		return (int)(m_lpbi->biHeight*m_dwScale);
	}
	return 0;
}


void CImageShow::InputImage(BYTE * lpImageBuf, int nImageSize)
{
	m_nImageSize = nImageSize;
	memcpy_s(m_pImageBuf, IMAGE_SHOW_BUF_SIZE, lpImageBuf, nImageSize);

	SetAllScroll();
	Invalidate(FALSE);
}


void CImageShow::SetAllScroll()
{
	// set all scroll bars
	CRect rcPreClient;
	GetWindowRect(&rcPreClient);
	rcPreClient.DeflateRect(1, 1);
	//get the scroll info
	int cx = ::GetSystemMetrics(SM_CXVSCROLL);
	int cy = ::GetSystemMetrics(SM_CYHSCROLL);

	SCROLLINFO siv;
	SCROLLINFO sih;
	GetScrollInfo(SB_HORZ, &sih, SIF_POS | SIF_RANGE);
	GetScrollInfo(SB_VERT, &siv, SIF_POS | SIF_RANGE);
	//decide need scroll bar or not
	BOOL bVertScroll = FALSE;
	BOOL bHorzScroll = FALSE;

	//if the full window mode disable the scroll bar
	if (m_nShowMode != FULL_WINDOW)
	{
		if (rcPreClient.Width() < GetImageWidth())
		{
			bHorzScroll = TRUE;
			if ((rcPreClient.Height() - cy) < GetImageHeight())
				bVertScroll = TRUE;
		}

		else if (rcPreClient.Height() < GetImageHeight())
		{
			bVertScroll = TRUE;
			if ((rcPreClient.Width() - cx) < GetImageWidth())
				bHorzScroll = TRUE;
		}

		//Set Scroll
		if (bHorzScroll)
		{
			if (rcPreClient.Height() > cy)
				rcPreClient.bottom -= cy;
			else
				rcPreClient.bottom = rcPreClient.top;
		}
		if (bVertScroll)
		{
			if (rcPreClient.Width() > cx)
				rcPreClient.right -= cx;
			else
				rcPreClient.right = rcPreClient.left;
		}
	}
	else
	{
		bHorzScroll = FALSE;
		bVertScroll = FALSE;
	}

	SetHorzScroll(&rcPreClient, bHorzScroll);
	SetVertScroll(&rcPreClient, bVertScroll);
}


BOOL CImageShow::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= WS_CHILD | WS_VISIBLE;

	return CWnd::Create(_T("Custom Image Show Class"), NULL, dwStyle, rect, pParentWnd, nID);
}


void CImageShow::OnFullWindow()
{
	m_dwScale = 1.0;
	m_nShowMode = FULL_WINDOW;
	SetAllScroll();
	Invalidate(FALSE);
}

void CImageShow::OnScale50()
{
	m_nShowMode = SCALE_50;
	m_dwScale = 0.5;
	SetAllScroll();
	Invalidate(FALSE);
}

void CImageShow::OnScale100()
{
	m_nShowMode = SCALE_100;
	m_dwScale = 1;
	SetAllScroll();
	Invalidate(FALSE);
}

void CImageShow::OnScale200()
{
	m_nShowMode = SCALE_200;
	m_dwScale = 2;
	SetAllScroll();
	Invalidate(FALSE);
}

void CImageShow::SetHorzScroll(CRect * pPreClient, BOOL bHorz)
{
	SCROLLINFO sih;
	GetScrollInfo(SB_HORZ, &sih, SIF_ALL);
	if (bHorz)
	{
		sih.cbSize = sizeof(SCROLLINFO);
		sih.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		sih.nMin = 0;
		sih.nMax = GetImageWidth() - 1;
		sih.nPage = pPreClient->Width();
		sih.nPos = sih.nPos;
		//the last page
		if (sih.nPos > (sih.nMax - (int)sih.nPage))
			sih.nPos = sih.nMax - sih.nPage;
	}
	else
	{
		sih.cbSize = sizeof(SCROLLINFO);
		sih.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		sih.nMin = 0;
		sih.nMax = 0;
		sih.nPage = 0;
		sih.nPos = 0;
	}
	SetScrollInfo(SB_HORZ, &sih, TRUE);
}


void CImageShow::SetVertScroll(CRect * pPreClient, BOOL bVert)
{
	SCROLLINFO siv;
	GetScrollInfo(SB_VERT, &siv, SIF_ALL);

	if (bVert)
	{
		int nShow = pPreClient->Height() / VERT_SCROLL_HEIGHT;
		siv.cbSize = sizeof(SCROLLINFO);
		siv.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		siv.nMin = 0;
		siv.nMax = GetImageHeight() / VERT_SCROLL_HEIGHT - 1;
		siv.nPage = max(1, nShow);
		if (siv.nPos > siv.nMax)
		{
			siv.nPos = siv.nMax;
		}
	}
	else
	{
		siv.cbSize = sizeof(SCROLLINFO);
		siv.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
		siv.nMin = 0;
		siv.nMax = 0;
		siv.nPage = 0;
		siv.nPos = 0;
	}
	SetScrollInfo(SB_VERT, &siv, TRUE);
}
