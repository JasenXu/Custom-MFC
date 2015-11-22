#include "StdAfx.h"
#include "IPConfig.h"
#pragma comment(lib,"Ws2_32.lib") 

CIPConfig::CIPConfig(void)
{
}


CIPConfig::~CIPConfig(void)
{
}


void CIPConfig::GetLanAdapterName(char * pszAdapterName)
{
	HKEY hKey, hSubKey, hNdiIntKey;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}"), 0, KEY_READ, &hKey ) != ERROR_SUCCESS)
	{
		return;
	}

	DWORD dwIndex(0), dwBufSize(MAX_PATH), dwDataType(0);
	TCHAR szSubKey[MAX_PATH] = {0};
	u_char szData[MAX_PATH] = {0};

	while (::RegEnumKeyEx(hKey, ++dwIndex, szSubKey, &dwBufSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
	{
		if (::RegOpenKeyEx(hKey, szSubKey, 0, KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{
			if (::RegOpenKeyEx(hSubKey, _T("Ndi\\Interfaces"), 0, KEY_READ, &hNdiIntKey) == ERROR_SUCCESS)
			{
				dwBufSize = MAX_PATH;
				if (::RegQueryValueEx(hNdiIntKey, _T("LowerRange"), 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
				{
					// 判断是不是以太网卡
					if (strcmp((char *)szData, "ethernet") == 0)
					{
						dwBufSize = MAX_PATH;
						if (::RegQueryValueEx(hSubKey, _T("DriverDesc"), 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
						{
							// szData中便是适配器的详细描述
							dwBufSize = MAX_PATH;
							if (::RegQueryValueEx(hSubKey, _T("NetCfgInstanceID"), 0, &dwDataType, szData, &dwBufSize) == ERROR_SUCCESS)
							{
								// szData中便是适配器名称
								strcpy_s(pszAdapterName, MAX_PATH, (const char *)szData);
								break;
							}
						}
					}
				}
				::RegCloseKey(hNdiIntKey);
			}
			::RegCloseKey(hSubKey);
		}
		dwBufSize = MAX_PATH;
	}

	::RegCloseKey(hKey);
}


BOOL CIPConfig::RegisterIPAddress(LPCSTR pszAdapterName, LPCSTR pszIPAddress, LPCSTR pszNetMask, LPCSTR pszGateway, LPCSTR pszDNSServer1, LPCSTR pszDNSServer2)
{
	HKEY hKey(NULL);
	CString strKeyName(_T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\"));
	strKeyName += pszAdapterName;

	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKeyName, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
	{
		return FALSE;
	}

	CString strDNSServer(pszDNSServer1);
	strDNSServer += _T(",");
	strDNSServer += pszDNSServer2;

	char szIPAddress[100] = {0};
	char szNetMask[100] = {0};
	char szGateway[100] = {0};
	char szDNSServer[100] = {0};

	strncpy_s(szIPAddress, 100, pszIPAddress, 98);
	strncpy_s(szNetMask, 100, pszNetMask, 98);
	strncpy_s(szGateway, 100, pszGateway, 98);
	WideCharToMultiByte(CP_ACP, 0, strDNSServer.GetBuffer(0), -1, szDNSServer, strDNSServer.GetLength(), NULL, NULL);
	strDNSServer.ReleaseBuffer();

	int nIPLen = strlen(szIPAddress);
	int nMaskLen = strlen(szNetMask);
	int nGatewayLen = strlen(szGateway); 
	int nDNSLen = strlen(szDNSServer);

	// REG_MULTI_SZ数据需要在后面再加个0 
	*(szIPAddress+nIPLen+1) = 0x00;
	nIPLen += 2;

	*(szNetMask+nMaskLen+1) = 0x00;
	nMaskLen += 2;

	*(szGateway+nMaskLen+1) = 0x00;
	nGatewayLen += 2;

	*(szDNSServer+nDNSLen+1) = 0x00;
	nDNSLen += 2;

	::RegSetValueEx(hKey, _T("IPAddress"), 0, REG_MULTI_SZ, (unsigned char*)szIPAddress, nIPLen);
	::RegSetValueEx(hKey, _T("SubnetMask"), 0, REG_MULTI_SZ, (unsigned char*)szNetMask, nMaskLen);
	::RegSetValueEx(hKey, _T("DefaultGateway"), 0, REG_MULTI_SZ, (unsigned char*)szGateway, nGatewayLen);
	::RegSetValueEx(hKey, _T("NameServer"), 0, REG_SZ, (unsigned char*)szDNSServer, nDNSLen);

	::RegCloseKey(hKey);

	return TRUE;
}


BOOL CIPConfig::NotifyIPChange(LPCSTR pszAdapterName, int nIndex, LPCSTR pszIPAddress, LPCSTR pszNetMask)
{
	BOOL bResult(FALSE);
	HINSTANCE hDhcpDll;
	DHCPNOTIFYPROC pDhcpNotifyProc;
	WCHAR wszAdapterName[MAX_PATH] = {0};

	::MultiByteToWideChar(CP_ACP, 0, pszAdapterName, -1, wszAdapterName, MAX_PATH);

	hDhcpDll = ::LoadLibrary(_T("dhcpcsvc.dll"));
	if (hDhcpDll == NULL)
	{
		return FALSE;
	}

	pDhcpNotifyProc = (DHCPNOTIFYPROC)::GetProcAddress(hDhcpDll, "DhcpNotifyConfigChange");
	if (pDhcpNotifyProc != NULL)
	{
		if ((pDhcpNotifyProc)(NULL, wszAdapterName, TRUE, nIndex, inet_addr(pszIPAddress), inet_addr(pszNetMask), 0) == ERROR_SUCCESS)
		{
			bResult = TRUE;
		}
	}

	::FreeLibrary(hDhcpDll);
	return bResult;
}
