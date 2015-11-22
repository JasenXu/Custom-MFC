#pragma once
#include "afxwin.h"
#include "Vfw.h"
#include "atltypes.h"
#pragma comment(lib,"Vfw32.lib")

#define IMAGE_SHOW_BUF_SIZE		(8*1024*1024)

#ifndef SecureDeleteBuffer
#define SecureDeleteBuffer(x) {if(x) delete[] (x); (x) = NULL;}
#endif // !SecureDeleteBuffer

#ifndef SecureDeleteObject
#define SecureDeleteObject(x) {if(x) delete (x); (x) = NULL;}
#endif // !SecureDeleteObject

#ifndef VERT_SCROLL_HEIGHT
#define VERT_SCROLL_HEIGHT	30
#endif // !VERT_SCROLL_HEIGHT


class CImageShow : public CWnd
{
public:
	CImageShow();
	virtual ~CImageShow();

private:
	enum ShowMode
	{
		FULL_WINDOW,
		SCALE_50,
		SCALE_100,
		SCALE_200
	};

	BYTE				*m_pImageBuf;
	BITMAPINFOHEADER	*m_lpbi;
	HDRAWDIB			m_hDrawDib;
	COLORREF			m_clrBkgr;
	UINT				m_nShowMode;
	double				m_dwScale;
	UINT				m_nZoom;
	CFont				m_fontTag;
	int					m_nImageSize;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	void OnScale200();
	void OnScale100();
	void OnScale50();
	void OnFullWindow();

private:
	void DrawBitmap(HDC hDC);
	int GetImageWidth();
	int GetImageHeight();
	void SetAllScroll();

public:
	void InputImage(BYTE * lpImageBuf, int nImageSize);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = NULL);
	void SetHorzScroll(CRect * pPreClient, BOOL bHorz);
	void SetVertScroll(CRect * pPreClient, BOOL bVert);
};

