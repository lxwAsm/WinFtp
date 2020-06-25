#include "win_ftp.hpp"
#include <iostream>

using	namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	WinFTP	ftp;
	if (!ftp.Open("127.0.0.1", "admin", "test123",21)){
		cout << "Login FTP Failed" << endl;
		getchar();
		return -1;
	}
	cout << "---------GetData--------" << endl;
	BinaryData	data;
	data = ftp.Download("/key.txt");
	cout << data.to_string() << endl;
	cout << "------------------------" << endl;
	string updata = "user upload data,for test";
	ifstream	f(R"(C:\Users\jack\Desktop\key.txt)", ios::in | ios::binary);
	if (ftp.CreateDirectory("/newfolder")){
		cout << "Make Success" << endl;
		if (!ftp.Upload("/newfolder/w.txt", f)){
			cout << "Upload Failed" << endl;
			getchar();
			return -1;
		}
		else{
			cout << "-----Upload Data Success------" << endl;
		}
	}
	cout << ftp.FtpCommand("LIST\r\n") << endl; //Expect Responce
  ftp.FtpCommand("MKD newfolder\r\n",FALSE); //no responce
	getchar();
	return 0;
}

