# WinFtp
a lightly Windows Ftp Library for C++11

# Useage  
#include "win_ftp.hpp"  
#include <iostream>  
using	namespace std;  
//------------------Download-----------------------  
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
//------------------Upload--------------------------  
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
//-------------------List Dir----------------------  
cout << ftp.FtpCommand("LIST\r\n") << endl;  

//-----------------FtpCommand without Response----  
ftp.FtpCommand("MKD newfolder\r\n",FALSE);  
valid [command](http://www.nsftools.com/tips/RawFTP.htm#LIST)  

ABOR - abort a file transfer  
CWD - change working directory  
DELE - delete a remote file  
LIST - list remote files  
MDTM - return the modification time of a file  
MKD - make a remote directory  
NLST - name list of remote directory  
PASS - send password  
PASV - enter passive mode  
PORT - open a data port  
PWD - print working directory  
QUIT - terminate the connection  
RETR - retrieve a remote file  
RMD - remove a remote directory  
RNFR - rename from  
RNTO - rename to  
SITE - site-specific commands  
SIZE - return the size of a file  
STOR - store a file on the remote host  
TYPE - set transfer type  
USER - send username   
