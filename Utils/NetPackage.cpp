// NetPackage.cpp : 实现文件
//

#include "stdafx.h"
#include "NetPackage.h"


// CNetPackage

CNetPackage::CNetPackage()
	: m_usDataType(0)
	, m_usDataChannel(0)
	, m_pBuf(nullptr)
	, m_nBufSize(32*1024*1024)
	, m_nLen(0)
	, m_oleLastRecvTime(COleDateTime::GetCurrentTime())
	, m_nWritePos(0)
	, m_nReadPos(0)
	, m_bFindHeader(FALSE)
	, m_nFrameLen(0)
	, m_hCLose(CreateEvent(NULL, TRUE, FALSE, NULL))
	, m_hPackageThread(nullptr)
	, m_bWorkFlag(FALSE)
	, m_nCallBack(0)
{
	memset(m_chFrameFlag, FRAME_FLAG, FRAME_FLAG_LEN);
}

CNetPackage::~CNetPackage()
{
	SetEvent(m_hCLose);
	if (m_hPackageThread)
	{
		if (::WaitForSingleObject(m_hPackageThread->m_hThread, WAIT_THREAD_EXIT) == WAIT_TIMEOUT)
			::TerminateThread(m_hPackageThread->m_hThread, 1);
		m_hPackageThread = nullptr;
	}

	if (m_pBuf)
	{
		delete[] m_pBuf;
		m_pBuf = nullptr;
	}

	if (m_pNetBuf)
	{
		delete m_pNetBuf;
		m_pNetBuf = nullptr;
	}
}


// CNetPackage 消息处理程序



// =========================================================================================
// 初始化任务
// =========================================================================================
void CNetPackage::Init(void)
{
	m_bWorkFlag = TRUE;

	m_pBuf = new BYTE[m_nBufSize]();
	m_pNetBuf = new CNetBuffer;
	m_pNetBuf->Init(16*1024*1024, 5);

	m_hPackageThread = AfxBeginThread((AFX_THREADPROC)PackageProc, (LPVOID)this);
}

// =========================================================================================
// 复制数据到组包缓冲区
// =========================================================================================
int CNetPackage::CopyData(char* pBuf, UINT nLen)
{
	LockWrite();
	if (m_nLen >= m_nBufSize)
	{
		UnLockWrite();
		return;
	}

	int i(0);
	for (; i < nLen; ++i)
	{
		m_pBuf[m_nWritePos] = *(pBuf+i);
		m_nWritePos = ++m_nWritePos % m_nBufSize;
		if (++m_nLen >= m_nBufSize)
		{
			break;
		}
	}
	UnLockWrite();

	return i;
}

// =========================================================================================
// 设置数据类别
// =========================================================================================
void CNetPackage::SetDataType(USHORT usType)
{
	LockWrite();
	m_usDataType = usType;
	UnLockWrite();
}


USHORT CNetPackage::GetDataType(void)
{
	return USHORT(m_usDataType);
}

// =========================================================================================
// 设置数据通道
// =========================================================================================
void CNetPackage::SetDataChannel(USHORT usChannel)
{
	LockWrite();
	m_usDataChannel = usChannel;
	UnLockWrite();
}


USHORT CNetPackage::GetDataChannel(void)
{
	return USHORT(m_usDataChannel);
}

// =========================================================================================
// 查找帧头
// =========================================================================================
void CNetPackage::FindHeader(void)
{
	// 循环找头
	while (m_nLen >= sizeof(MULTICAST_HEADER))
	{
		// 查找头标志
		for (UINT i(0); m_pBuf[m_nReadPos] != FRAME_FLAG && m_nLen > 0; ++i)
		{
			m_nReadPos = (m_nReadPos+1) % m_nBufSize;
			m_nLen--;

			// 每过500个字节未找到头，暂停1毫秒
			if (i > 500)
			{
				i = 0;
				::Sleep(1);
			}
		}

		if (IsHeader())
		{
			// 读取帧长度
			UINT nFrameLenPos = (m_nReadPos+sizeof(MULTICAST_HEADER)-sizeof(UINT)) % m_nBufSize;

			BYTE* p = (BYTE*)(&m_nFrameLen);
			for (UINT n(0); n < sizeof(m_nFrameLen); ++n)
			{
				*(p+n) = m_pBuf[nFrameLenPos];
				nFrameLenPos = (++nFrameLenPos) % m_nBufSize;
			}

			// 若帧长度大于缓冲区长度，重新找头
			if(m_nFrameLen > m_nBufSize)
			{
				m_nReadPos = nFrameLenPos;
				m_nLen -= sizeof(MULTICAST_HEADER)-sizeof(UINT);
				continue;
			}

			m_bFindHeader = TRUE;
			return;
		}
	}
}

// =========================================================================================
// 判断帧头标志、数据类别、数据通道
// =========================================================================================
BOOL CNetPackage::IsHeader(void)
{
	//判断数据是否为帧头
	int nPos(m_nReadPos);
	for(UINT i(0); i < FRAME_FLAG_LEN; ++i)
	{
		if(m_pBuf[nPos] != FRAME_FLAG)
		{
			//读位置向前偏移i，长度减少相应字节
			m_nReadPos = (m_nReadPos+i) % m_nBufSize;
			m_nLen -= i;
			return FALSE;
		}
		//读位置向前偏移1
		nPos = ++nPos % m_nBufSize;
	}

#if 1
	USHORT n(0);
	BYTE* p = (BYTE*)&n;

	// 判断数据类别
	*p = m_pBuf[nPos];
	*(p+1) = m_pBuf[++nPos % m_nBufSize];
	if (n != m_usDataType)
	{
		//读位置向前偏移2，长度减少相应字节
		m_nReadPos = (m_nReadPos+2) % m_nBufSize;
		m_nLen -= 2;
		return FALSE;
	}

	// 判断数据通道
	*p = m_pBuf[++nPos % m_nBufSize];
	*(p+1) = m_pBuf[++nPos % m_nBufSize];
	if (n != m_usDataChannel)
	{
		//读位置向前偏移2，长度减少相应字节
		m_nReadPos = (m_nReadPos+2) % m_nBufSize;
		m_nLen -= 2;
		return FALSE;
	}
#endif
	TRACE(_T("\nfind out! "));
	return TRUE;
}

// =========================================================================================
// 判断帧头标志
// =========================================================================================
BOOL CNetPackage::IsHeader(UINT nStartPos)
{
	//判断数据是否为帧头
	for(UINT i(0); i < FRAME_FLAG_LEN; ++i)
	{
		if(m_pBuf[nStartPos] != FRAME_FLAG)
		{
		//	TRACE(_T("is not header %d\n"), i);
			return FALSE;
		}
		//读位置偏移1
		nStartPos = ++nStartPos % m_nBufSize;
	}
	return TRUE;
}

// =========================================================================================
// 组包线程
// =========================================================================================
UINT CNetPackage::PackageProc(LPVOID lpParam)
{
	ASSERT(lpParam);
	CNetPackage* ptr = (CNetPackage*)lpParam;
	while (::WaitForSingleObject(ptr->m_hCLose, 1) == WAIT_TIMEOUT)
	{
//  		if (ptr->m_nLen <= sizeof(MULTICAST_HEADER))
// 			continue;

	//	ptr->LockRead();
		ptr->PackageLoop();
	//	ptr->UnLockRead();
	}

	return 0;
}

// =========================================================================================
// 组包函数
// =========================================================================================
void CNetPackage::PackageLoop(void)
{
	if (m_bFindHeader)
	{
		// 记录初始位置
		UINT nPreReadPos(m_nReadPos);
		UINT nPreLen(m_nLen);

		if (m_nLen >= 2*sizeof(MULTICAST_HEADER)+m_nFrameLen)
		{
			// ----------- 偏移帧长度的字节的数据，如果接下来是帧头，则认为是完整帧 ------------ //
			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)+m_nFrameLen) % m_nBufSize;
			m_nLen = m_nLen - (sizeof(MULTICAST_HEADER)+m_nFrameLen);

			if (IsHeader(m_nReadPos))
			{
				//写入帧缓冲
				UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
				int nRet = WriteBuffer(nDataPos, m_nFrameLen);
				if (nRet >= 0)
				{
					::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
				}
				else
					TRACE("I---write buffer fail\n");
				TRACE("I--Post message\n");
			} 
			else
			{
				m_nReadPos = (nPreReadPos+sizeof(MULTICAST_HEADER)-sizeof(UINT)) % m_nBufSize;
				m_nLen = nPreLen - (sizeof(MULTICAST_HEADER)-sizeof(UINT));
			}
			TRACE(_T("....I%d, len:%d\n"), m_nFrameLen, m_nLen);
			m_bFindHeader = FALSE;
			return;
		}
		else if (m_nLen >= sizeof(MULTICAST_HEADER)+m_nFrameLen)
		{
			// --------------- 如果发现数据中间没有数据头，也可以成帧 --------------------- //
			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
			m_nLen = m_nLen - sizeof(MULTICAST_HEADER);
			UINT nFrameLen(0);
			BOOL bFindHeader(FALSE);

			while (nFrameLen <= m_nFrameLen)
			{
				if (IsHeader(m_nReadPos))
				{
					bFindHeader = TRUE;
					break;
				}
				m_nReadPos = ++m_nReadPos % m_nBufSize;
				++nFrameLen;
			}
			m_nReadPos = nPreReadPos;
			m_nLen = nPreLen;

			if (!bFindHeader)
			{
				//写入帧缓冲
				UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
				int nRet = WriteBuffer(nDataPos, m_nFrameLen);
				if (nRet >= 0)
				{
					::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
				}
				else
					TRACE("II---write buffer fail\n");
				TRACE("II---Post message\n");
				m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)+m_nFrameLen) % m_nBufSize;
				m_nLen = m_nLen - (sizeof(MULTICAST_HEADER)+m_nFrameLen);
			}
			TRACE(_T("....II%d, len:%d\n"), m_nFrameLen, m_nLen);
			m_bFindHeader = FALSE;
			return;
		} 
		else if (m_nLen < m_nFrameLen+sizeof(MULTICAST_HEADER) && m_nLen > sizeof(MULTICAST_HEADER) && !m_bWorkFlag)
		{
			// ------------------------ 对于残帧，依然显示 -----------------------//
			if (m_usDataType == TYPE_RADAR)
			{
				m_bFindHeader = FALSE;
				m_nFrameLen = 0;
				return;
			}

			m_nReadPos = (m_nReadPos+sizeof(MULTICAST_HEADER)) % m_nBufSize;
			m_nLen -= sizeof(MULTICAST_HEADER);

			UINT nFrameLen(0);
			BOOL bFindHeader(FALSE);
			while(nFrameLen <= m_nLen)
			{
				if(IsHeader(m_nReadPos))
				{
					bFindHeader = TRUE;
					break;
				}
				m_nReadPos = ++m_nReadPos % m_nBufSize;
				++nFrameLen;
			}

			//写入帧缓冲
			UINT nDataPos = (nPreReadPos+sizeof(MULTICAST_HEADER)) %m_nBufSize;
			int nRet = WriteBuffer(nDataPos, nFrameLen-1);
		//	TRACE("III---Post message\n");
			if (nRet >= 0)
			{
				::PostMessage((HWND)m_nCallBack, WM_NET_RESOLVE, (WPARAM)m_pNetBuf, NULL);
			}

			bFindHeader = FALSE;
			m_nLen = 0;
			return;
		}
	}
	else
	{
		FindHeader();
	}
}

// =========================================================================================
// 写锁定
// =========================================================================================
void CNetPackage::LockWrite(void)
{
	m_ccs.Lock();
}

// =========================================================================================
// 写解锁
// =========================================================================================
void CNetPackage::UnLockWrite(void)
{
	m_ccs.Unlock();
}

// =========================================================================================
// 读锁定
// =========================================================================================
void CNetPackage::LockRead(void)
{
	m_ccs.Lock();
}

// =========================================================================================
// 读解锁
// =========================================================================================
void CNetPackage::UnLockRead(void)
{
	m_ccs.Unlock();
}

// =========================================================================================
// 写入数据到待解析缓冲区
// =========================================================================================
BOOL CNetPackage::WriteBuffer(UINT nStartPos, UINT nLen)
{
	UINT nIndex(0);
	if (nStartPos+nLen > m_nBufSize-1)
	{
		// 折行处理
		UINT nLeft(m_nBufSize - nStartPos);
		UINT nRight(nLen - nLeft);
		nIndex = m_pNetBuf->AddData(m_pBuf+nStartPos, nLeft);
		if (nIndex >= 0)
		{
			nIndex = m_pNetBuf->AddDataAt(m_pBuf, nRight, nIndex);
		}
	}
	else
	{
		nIndex = m_pNetBuf->AddData(m_pBuf+nStartPos, nLen);
	}

	return nIndex;
}

// =========================================================================================
// 设置组包工作状态标记
// =========================================================================================
void CNetPackage::SetWorkFlag(BOOL bWork)
{
	LockWrite();
	m_bWorkFlag = bWork;
	UnLockWrite();
}

// =========================================================================================
// 读取待解析缓冲区数据
// =========================================================================================
void CNetPackage::ReadBuffer(PUCHAR& pBuf, UINT& nLen)
{
	m_pNetBuf->GetData(pBuf, nLen);
}

// =========================================================================================
// 设置回调窗口句柄
// =========================================================================================
void CNetPackage::SetCallBack(int nCallBack)
{
	LockWrite();
	m_nCallBack = nCallBack;
	UnLockWrite();
}

// =========================================================================================
// 释放已读取的缓冲区数据
// =========================================================================================
void CNetPackage::ReleaseBuffer(void)
{
	m_pNetBuf->DeleteData();
}
