#include "StdAfx.h"
#include "DualListBuffer.h"


CDualListBuffer::CDualListBuffer(void)
	: m_bFlushing(FALSE)
{
	m_pMutex = new CMutex(FALSE, NULL);

	for (UINT i(0); i < BUF_COUNT; ++i)
	{
		m_listPool.AddTail(new DATA_TYPE);
	}

	m_hBufEnough = CreateEvent(NULL, TRUE, FALSE, NULL);
}


CDualListBuffer::~CDualListBuffer(void)
{
	while (m_listData.GetCount())
	{
		DATA_TYPE_PTR ptr = m_listData.GetAt(0);
		delete ptr;
		ptr = NULL;
		m_listData.RemoveAt(0);
	}

	while (m_listPool.GetCount())
	{
		DATA_TYPE_PTR ptr = m_listPool.GetAt(0);
		delete ptr;
		ptr = NULL;
		m_listPool.RemoveAt(0);
	}

	if (m_pMutex)
	{
		delete m_pMutex;
		m_pMutex = NULL;
	}

	if (m_hBufEnough)
	{
		CloseHandle(m_hBufEnough);
		m_hBufEnough = NULL;
	}
}


void CDualListBuffer::SetFlushing(BOOL bFlush)
{
	m_bFlushing = bFlush;
}


BOOL CDualListBuffer::IsFlushing(void)
{
	return m_bFlushing;
}


int CDualListBuffer::ResetList(void)
{
	CSingleLock lock(m_pMutex);
	lock.Lock(1000);
	if (lock.IsLocked())
	{
		while (m_listData.GetCount())
		{
			m_listPool.AddTail(m_listData.RemoveHead());
		}
		lock.Unlock();
	}

	return 1;
}


int CDualListBuffer::GetListSize(void)
{
	return (m_listData.GetCount() + m_listPool.GetCount());
}


DATA_TYPE_PTR CDualListBuffer::GetDataBuf(void)
{
	CSingleLock lock(m_pMutex);
	DATA_TYPE_PTR pRet(NULL);
	lock.Lock(1000);
	if (lock.IsLocked())
	{
		if (m_listData.GetCount())
		{
			pRet = m_listData.RemoveHead();
		}
		lock.Unlock();
	}

	return pRet;
}


int CDualListBuffer::ReleaseDataBuf(DATA_TYPE_PTR pDataBuf)
{
	CSingleLock lock(m_pMutex);
	int nRet(0);
	lock.Lock(1000);
	if (lock.IsLocked())
	{
		if (pDataBuf)
		{
			m_listPool.AddTail(pDataBuf);
			nRet = 1;
		}
		lock.Unlock();
	}

	return nRet;
}


DATA_TYPE_PTR CDualListBuffer::GetWriteBuf(void)
{
	CSingleLock lock(m_pMutex);
	DATA_TYPE_PTR pRet(NULL);
	lock.Lock(1000);
	if (lock.IsLocked())
	{
		if (m_listPool.GetCount())
		{
			pRet = m_listPool.RemoveHead();
		}
		lock.Unlock();
	}

	return pRet;
}


int CDualListBuffer::ReleaseWriteBuf(DATA_TYPE_PTR pDataBuf)
{
	CSingleLock lock(m_pMutex);
	int nRet(0);
	lock.Lock(1000);
	if (lock.IsLocked())
	{
		if (pDataBuf)
		{
			m_listData.AddTail(pDataBuf);
			nRet = 1;
		}
		lock.Unlock();
	}

	return nRet;
}


DATA_TYPE_PTR CDualListBuffer::PointToDataHead(void)
{
	if (m_listData.GetCount())
	{
		return m_listData.GetHead();
	}

	return NULL;
}


int CDualListBuffer::RollBackDataHead(DATA_TYPE_PTR pDataBuf)
{
	DATA_TYPE_PTR ptr = GetDataBuf();
	if (ptr)
	{
		ReleaseDataBuf(ptr);
	}
	return 1;
}


DWORD CDualListBuffer::WaitForNext(DWORD dwTimeOut)
{
	return ::WaitForSingleObject(m_hBufEnough, dwTimeOut);
}
