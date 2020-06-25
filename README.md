# WinFtp
a lightly Windows Ftp Library for C++11

# Useage  
#include "win_ftp.hpp"  
#include <iostream>  
using	namespace std;  
  
## ------------------Download-----------------------  
WinFTP	ftp;  
if (!ftp.Open("127.0.0.1", "admin", "test123",21)){  
&nbsp;&nbsp;&nbsp;&nbsp;cout << "Login FTP Failed" << endl;  
&nbsp;&nbsp;&nbsp;&nbsp;getchar();  
&nbsp;&nbsp;&nbsp;&nbsp;return -1;  
}
cout << "---------GetData--------" << endl;  
BinaryData	data;  
data = ftp.Download("/key.txt");  
cout << data.to_string() << endl;  
  
## ------------------Upload--------------------------  
ifstream	f(R"(C:\Users\jack\Desktop\key.txt)", ios::in | ios::binary);  
if (ftp.CreateDirectory("/newfolder")){  
&nbsp;&nbsp;&nbsp;&nbsp;cout << "Make Success" << endl;  
&nbsp;&nbsp;&nbsp;&nbsp;if (!ftp.Upload("/newfolder/w.txt", f)){  
&nbsp;&nbsp;&nbsp;&nbsp;cout << "Upload Failed" << endl;  
&nbsp;&nbsp;&nbsp;&nbsp;getchar();  
&nbsp;&nbsp;&nbsp;&nbsp;return -1;  
&nbsp;&nbsp;&nbsp;&nbsp;}  
&nbsp;&nbsp;&nbsp;&nbsp;else{  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;cout << "-----Upload Data Success------" << endl;  
&nbsp;&nbsp;&nbsp;&nbsp;}  
}  
  
## -------------------List Dir----------------------  
cout << ftp.FtpCommand("LIST\r\n") << endl;  

## -----------------FtpCommand without Response-----  
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
