#include "StdAfx.h"
#include "ImageDisplay.h"


CImageDisplay::CImageDisplay(void)
	: m_pImageBuf(NULL)
	, m_rc(0, 0, 480, 320)
	, m_bValidData(FALSE)
	, m_lpbi(NULL)
	, m_lpbf(NULL)
	, m_hDrawDib(NULL)
{
	m_pImageBuf = new BYTE[BUF_SIZE]();
	m_hDrawDib = ::DrawDibOpen();

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
}


CImageDisplay::~CImageDisplay(void)
{
	if (m_hDrawDib)
	{
		::DrawDibClose(m_hDrawDib);
	}

	if (m_pImageBuf)
	{
		delete[] m_pImageBuf;
		m_pImageBuf = NULL;
	}
}


void CImageDisplay::DrawBitmap(HDC hDC, CRect rc)
{
	if (m_bValidData = FALSE)
	{
		return;
	}

	m_lpbf = (BITMAPFILEHEADER *)m_pImageBuf;
	m_lpbi = (BITMAPINFOHEADER *)(m_pImageBuf+sizeof(BITMAPFILEHEADER));

	if (m_hDrawDib != NULL)
	{
		::DrawDibDraw(m_hDrawDib, hDC, 
			rc.left, rc.top, rc.Width(), rc.Height(),
			m_lpbi, (LPVOID)(m_pImageBuf+m_lpbf->bfOffBits),
			0, 0, -1, -1,
			DDF_SAME_HDC);
	}
}


void CImageDisplay::CopyImageData(const LPVOID pBuf, UINT nCount)
{
	if (pBuf)
	{
		memcpy_s(m_pImageBuf, BUF_SIZE, (BYTE *)pBuf, nCount);
		m_bValidData = TRUE;
	}
	else
	{
		m_bValidData = FALSE;
	}
}


void CImageDisplay::DisplayImage(CDC * pDC)
{
	DrawBitmap(pDC->m_hDC, m_rc);
}


void CImageDisplay::SetDisplayRect(CRect rc)
{
	if (rc.IsRectEmpty() == 0)
	{
		m_rc.CopyRect(rc);
	}
	else
	{
		m_rc.SetRect(0, 0, 480, 320);
	}
}


void CImageDisplay::DisplayImage(CDC * pDC, const LPVOID lpBuf, UINT nCount)
{
	if (lpBuf == NULL || nCount == 0)
	{
		pDC->FillSolidRect(&m_rc, RGB(0, 0, 0));
		pDC->SetTextColor(RGB(240, 240, 240));
		pDC->SelectObject(&m_fontTag);
		pDC->DrawText(_T("NO  IMAGE"), &m_rc, DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	else
	{
		CopyImageData(lpBuf, nCount);
		DrawBitmap(pDC->m_hDC, m_rc);
	}	
}


void CImageDisplay::GetDisplayRect(LPRECT lpRect)
{
	lpRect->top = m_rc.top;
	lpRect->left = m_rc.left;
	lpRect->bottom = m_rc.bottom;
	lpRect->right = m_rc.right;
}
