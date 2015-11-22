#pragma once
#include "Vfw.h"
#include "atltypes.h"
#pragma comment(lib,"Vfw32.lib")

#ifndef BUF_SIZE
#define BUF_SIZE 8*1024*1024
#endif

class CImageDisplay
{
public:
	CImageDisplay(void);
	virtual ~CImageDisplay(void);
	void DrawBitmap(HDC hDC, CRect rc);

protected:
	HDRAWDIB			m_hDrawDib;
	CFont				m_fontTag;
	BITMAPINFOHEADER	*m_lpbi;
	BITMAPFILEHEADER	*m_lpbf;
	BYTE				*m_pImageBuf;
	CRect				m_rc;
	BOOL				m_bValidData;
	CCriticalSection	m_cs;

public:
	void CopyImageData(const LPVOID pBuf, UINT nCount);
	void DisplayImage(CDC * pDC);
	void SetDisplayRect(CRect rc);

	void DisplayImage(CDC * pDC, const LPVOID lpBuf, UINT nCount);
	void GetDisplayRect(LPRECT lpRect);
};

