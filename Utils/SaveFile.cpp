#include "StdAfx.h"
#include "SaveFile.h"


CSaveFile::CSaveFile(void)
	: m_olePrevTime(0, 0)
	, m_str1stDirPath(_T(""))
	, m_str2ndDirPath(_T(""))
	, m_str3rdDirPath(_T(""))
	, m_strExtend(_T(""))
{
	// 获取应用程序所在目录
	TCHAR szFilePath[MAX_PATH+1] = {0};
	::GetModuleFileName(NULL, szFilePath, MAX_PATH);

	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	m_strAppDirectory = szFilePath;
}


CSaveFile::~CSaveFile(void)
{
	if (m_file.m_hFile != CFile::hFileNull)
	{
		m_file.Close();
	}
}


void CSaveFile::CreateFileDirectory(LPCTSTR lpPathName)
{
	if (!::PathFileExists(lpPathName))
	{
		::CreateDirectory(lpPathName, NULL);
	}
}


BOOL CSaveFile::OpenFile(LPCTSTR lpszFilePath)
{
	return m_file.Open(lpszFilePath, CFile::modeCreate | CFile::modeWrite);
}


void CSaveFile::WriteFile(LPVOID lpBuf, UINT nCount)
{
	CString strFilePath(_T(""));
	if (IsMinuteChanged())
	{
		strFilePath.Format(_T("%04d%02d%02d%02d"), m_olePrevTime.GetYear(), m_olePrevTime.GetMonth(), 
			m_olePrevTime.GetDay(), m_olePrevTime.GetHour());

		CreateLastDirectory(strFilePath);

		if (m_file.m_hFile != CFile::hFileNull)
		{
			m_file.Close();
		}

		CString strFileName(_T(""));
		strFileName.Format(_T("\\%02d%02d"), m_olePrevTime.GetHour(), m_olePrevTime.GetMinute());
		strFileName += m_strExtend;
		BOOL b = OpenFile(m_str3rdDirPath+strFileName);
	}
	

	if (m_file.m_hFile != CFile::hFileNull)
	{
		m_file.Write(lpBuf, nCount);
	}
}


BOOL CSaveFile::IsHourChanged(void)
{
	COleDateTime oleCur = COleDateTime::GetCurrentTime();

	if (oleCur.GetHour() != m_olePrevTime.GetHour())
	{
		m_olePrevTime = oleCur;
		return TRUE;
	} 

	return FALSE;
}


CString CSaveFile::GetAppDirectory(void)
{
	return CString(m_strAppDirectory);
}


void CSaveFile::Create1stDirectory(LPCTSTR lpDirectoryPath)
{
	m_str1stDirPath = lpDirectoryPath;
	if (!::PathFileExists(lpDirectoryPath))
	{
		::CreateDirectory(lpDirectoryPath, NULL);
	}
}


void CSaveFile::Create2ndDirectory(LPCTSTR lpDirectoryName)
{
	m_str2ndDirPath = m_str1stDirPath + _T("\\") + lpDirectoryName;
	if (!::PathFileExists(m_str2ndDirPath))
	{
		::CreateDirectory(m_str2ndDirPath, NULL);
	}
}


void CSaveFile::CreateLastDirectory(LPCTSTR lpDirectoryName)
{
	
	if (m_str2ndDirPath.IsEmpty())
		m_str2ndDirPath = m_str1stDirPath;
	m_str3rdDirPath = m_str2ndDirPath + _T("\\") + lpDirectoryName;

	if (!::PathFileExists(m_str3rdDirPath))
	{
		::CreateDirectory(m_str3rdDirPath, NULL);
	}
}


BOOL CSaveFile::IsMinuteChanged(void)
{
	COleDateTime oleCur = COleDateTime::GetCurrentTime();

	if (oleCur.GetMinute() != m_olePrevTime.GetMinute())
	{
		m_olePrevTime = oleCur;
		return TRUE;
	} 

	return FALSE;
}


void CSaveFile::SetExtend(LPCTSTR lpszExtend)
{
	m_strExtend = lpszExtend;
}
