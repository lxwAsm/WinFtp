#pragma  once
#include <Windows.h>
#include <WinInet.h>
#include <string>
#include <vector>
#include <fstream>
#pragma comment(lib, "WinInet.lib")

class BinaryData
{
public:
	BinaryData();
	explicit BinaryData(int size);
	//BinaryData(const BinaryData &);
	~BinaryData();
	void append(byte n);
	void append(byte *buffer, int size);
	void append(std::string n);
	int  find(const char *s);
	std::string substr(int start, int end);
	void erase(int start, int end);
	const byte* raw_buffer();
	int	 size();
	std::string	to_string();
	std::string user_data;
private:
	std::vector<byte>	data;
};

class Connect{
public:
	virtual bool		Open(std::string server, std::string username, std::string password, unsigned int port) = 0;
	virtual BinaryData	Download(std::string url) = 0;
	virtual bool		Close()=0;
};

class WinFTP :public Connect{
public:
	WinFTP();
	bool		Open(std::string server, std::string username, std::string password, unsigned int port);
	BinaryData	Download(std::string path);
	bool		Upload(std::string path, BYTE *pUploadData, DWORD dwUploadDataSize);
	bool		Upload(std::string path,BinaryData &bd);
	bool		Upload(std::string path,std::ifstream &file);
	std::string		FtpCommand(std::string cmd,bool fExpectResponse);
	bool		CreateDirectory(std::string dirname);
	bool		RemoveDirectory(std::string dirname);
	bool		DeleteFile(std::string path);
	bool		Close();
private:
	HINTERNET	hInternet;
	HINTERNET	hConnect;
	std::wstring wserver;
	std::wstring wusername;
	std::wstring wpassword;
};


//---------------------------------------------------------------------------------
std::wstring s2ws(const std::string& str)
{
	std::wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}//

//--------------binarydata----------------
BinaryData::BinaryData()
{

}

BinaryData::BinaryData(int maxsize)
{
	data.reserve(maxsize);
}


void BinaryData::append(byte n){
	data.push_back(n);
}

int	 BinaryData::size(){
	return data.size();
}
void BinaryData::append(const std::string n){
	data.insert(data.end(), n.begin(), n.end());

}

int  BinaryData::find(const char *s){
	return std::string(data.begin(), data.end()).find(s);
}

std::string BinaryData::substr(int start, int end){
	return std::string(data.begin() + start, data.begin() + end);
}

void BinaryData::erase(int start, int end){
	data.erase(data.begin() + start, data.begin() + end);
}
const byte* BinaryData::raw_buffer(){
	return data.data();
}
void BinaryData::append(byte *buffer, int size){
	data.insert(data.end(), buffer, buffer + size);
}
std::string BinaryData::to_string(){
	return std::string(data.begin(), data.end());
}
BinaryData::~BinaryData()
{

}
//-----------------------------------------
WinFTP::WinFTP(){
	hInternet = NULL;
	hConnect = NULL;
}
bool	WinFTP::Open(std::string server, std::string username, std::string password,unsigned int port=21){
	// 建立会话
	this->wserver = s2ws(server);
	this->wusername = s2ws(username);
	this->wpassword = s2ws(password);
	hInternet = ::InternetOpen(L"WinInet Ftp Download V1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (NULL == hInternet)
	{
		return FALSE;
	}
	// 建立连接
	hConnect = ::InternetConnect(hInternet, wserver.c_str(), port, wusername.c_str(), wpassword.c_str() , INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (NULL == hConnect)
	{
		return FALSE;
	}
	return TRUE;
}

BinaryData	WinFTP::Download(std::string path){
	std::wstring wpath = s2ws(path);
	DWORD dwBufferSize = 4096;
	BinaryData	bd;
	DWORD dwBytesReturn = 0;
	DWORD dwOffset = 0;
	BOOL bRet = FALSE;
	auto hFTPFile = ::FtpOpenFile(hConnect, wpath.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (NULL == hFTPFile)
	{
		throw "FtpOpenFile Failed";
	}
	// 获取文件大小
	DWORD dwDownloadDataSize = ::FtpGetFileSize(hFTPFile, NULL);
	auto pBuf = new BYTE[dwBufferSize];
	if (!pBuf){
		InternetCloseHandle(hFTPFile);
		throw "NewMemory Failed";
	}
	::RtlZeroMemory(pBuf, dwBufferSize);
	do
	{
		bRet = ::InternetReadFile(hFTPFile, pBuf, dwBufferSize, &dwBytesReturn);
		if (FALSE == bRet)
		{
			break;
		}
		//::RtlCopyMemory((pDownloadData + dwOffset), pBuf, dwBytesReturn);
		bd.append(pBuf, dwBytesReturn);
		dwOffset = dwOffset + dwBytesReturn;

	} while (dwDownloadDataSize > dwOffset);
	InternetCloseHandle(hFTPFile);
	return bd;
}

bool	WinFTP::Upload(std::string path, BYTE *pUploadData, DWORD dwUploadDataSize){
	std::wstring wpath = s2ws(path);
	DWORD dwBytesReturn = 0;
	BOOL bRet = FALSE;
	auto hFTPFile = ::FtpOpenFile(hConnect, wpath.c_str(), GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (NULL == hFTPFile)
	{
		return FALSE;
	}
	// 上传数据
	bRet = ::InternetWriteFile(hFTPFile, pUploadData, dwUploadDataSize, &dwBytesReturn);
	InternetCloseHandle(hFTPFile);
	return bRet;
}

bool	WinFTP::Upload(std::string path,BinaryData &bd){
	std::wstring wpath = s2ws(path);
	DWORD dwBytesReturn = 0;
	BOOL bRet = FALSE;
	auto hFTPFile = ::FtpOpenFile(hConnect, wpath.c_str(), GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (NULL == hFTPFile)
	{
		return FALSE;
	}
	// 上传数据
	bRet = ::InternetWriteFile(hFTPFile, bd.raw_buffer(), bd.size(), &dwBytesReturn);
	return bRet;
}

bool	WinFTP::Upload(std::string path,std::ifstream &file){
	std::wstring wpath = s2ws(path);
	DWORD dwBytesReturn = 0;
	DWORD dwBufferSize = (1<<10)*10;//10k
	BOOL bRet = FALSE;
	auto hFTPFile = ::FtpOpenFile(hConnect, wpath.c_str(), GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (NULL == hFTPFile)
	{
		return FALSE;
	}
	if (!file.is_open()){
		return  FALSE;
	}
	// 上传数据
	auto pBuf = new BYTE[dwBufferSize];
	if (!pBuf){
		return FALSE;
	}
	memset(pBuf, 0, dwBufferSize);
	do 
	{
		file.read((char*)pBuf, dwBufferSize);
		auto rSize = file.gcount();
		bRet = ::InternetWriteFile(hFTPFile, pBuf, rSize, &dwBytesReturn);
		if (rSize != dwBufferSize){
			break;
		}
	} while (TRUE);
	delete[] pBuf;
	InternetCloseHandle(hFTPFile);
	return bRet;
}

std::string	WinFTP::FtpCommand(std::string cmd,bool fExpectResponse=TRUE){
	HINTERNET	hFtpCommand=NULL;
	DWORD dwBufferSize = 4096;
	BinaryData	bd;
	DWORD dwBytesReturn = 0;
	DWORD dwOffset = 0;
	BOOL bRet = FALSE;
	if (!FtpCommandA(hConnect, fExpectResponse, FTP_TRANSFER_TYPE_BINARY, cmd.c_str(), 0xfa, &hFtpCommand)){
		throw "FtpCommand Failed";
	}
	if (fExpectResponse == FALSE){
		InternetCloseHandle(hFtpCommand);
		return "";
	}
	auto pBuf = new BYTE[dwBufferSize];
	if (!pBuf){
		InternetCloseHandle(hFtpCommand);
		throw "NewMemory Failed";
	}
	::RtlZeroMemory(pBuf, dwBufferSize);
	do
	{
		bRet = ::InternetReadFile(hFtpCommand, pBuf, dwBufferSize, &dwBytesReturn);
		if (FALSE == bRet)
		{
			break;
		}
		bd.append(pBuf, dwBytesReturn);
	} while (dwBufferSize==dwBytesReturn);

	delete[] pBuf;
	InternetCloseHandle(hFtpCommand);
	return bd.to_string();

}

bool	WinFTP::CreateDirectory(std::string dirname){
	return FtpCreateDirectoryA(hConnect, dirname.c_str());
}

bool	WinFTP::RemoveDirectory(std::string dirname){
	return FtpRemoveDirectoryA(hConnect, dirname.c_str());

}
bool	WinFTP::DeleteFile(std::string path){
	return FtpDeleteFileA(hConnect, path.c_str());
}

bool	WinFTP::Close(){
	if (NULL != hConnect)
	{
		::InternetCloseHandle(hConnect);
	}
	if (NULL != hInternet)
	{
		::InternetCloseHandle(hInternet);
	}
	return TRUE;
}
