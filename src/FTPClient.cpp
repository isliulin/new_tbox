#include "FTPClient.h"

//FTPClient::FTPClient(){}

/*****************************************************************************
* Function Name : ~FTPClient
* Description   : 析构函数
* Input			: None
* Output        : None
* Return        : None
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
FTPClient::~FTPClient()
{
	close(socketfd);
}

/*****************************************************************************
* Function Name : FTPClient
* Description   : 构造函数
* Input			: char *ip
*                 int port
* Output        : None
* Return        : None
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
FTPClient::FTPClient(char *ip, int port)
{
	socketfd = -1;
	ftpServerIP = ip;
	ftpServerPort = port;
	isConnected = false;
	isReceivedData = false;

#if 0
	connectToFtpServer();

	while(!isReceivedData)
	{
		if(isConnected == false)
			connectToFtpServer();

		sleep(5);
	}
#endif
	for(int i = 0; i < 3; i++)
	{
		if(isReceivedData == false || isConnected == false)
		{
			connectToFtpServer();
		}
		else
		{
			break;
		}
		usleep(1000*100);
	}
}

/*****************************************************************************
* Function Name : connectToFtpServer
* Description   : 连接到FTP服务器
* Input			: None
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::connectToFtpServer()
{
	int ftpCode;
	uint8_t buff[MAX_BUFF_LEN] = {0};
	memset(&socketaddr, 0, sizeof(socketaddr));
	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(ftpServerPort);
	socketaddr.sin_addr.s_addr = inet_addr(ftpServerIP);

	if((socketfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
	{
		FTPLOG("socketfd:%d\n",socketfd);
		if (connect(socketfd, (struct sockaddr*) &socketaddr, sizeof(socketaddr)) != -1)
		{
			FTPLOG("Connect to server successfully,server IP:%s,PORT:%d\n",ftpServerIP,ftpServerPort);
			isConnected = true;

			if(recv(socketfd, buff, MAX_BUFF_LEN, 0) > 0)
			{
				FTPLOG("%s",buff);
				isReceivedData = true;

				sscanf((char *)buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 220)
				{
					FTPLOG("220 connect is error!");
					return -1;
				}
			}
		}
		else
		{
			close(socketfd);
			shutdown(socketfd, SHUT_RDWR);
			socketfd = -1;
			isConnected = false;
			FTPLOG("connect failed close socketfd!");
			return -1;
		}
	}

	return 0;
}

/*****************************************************************************
* Function Name : loginFTPServer
* Description   : 登录到FTP服务器
* Input			: None
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::loginFTPServer(char *userName, char *password)
{
	if(sendUserName(userName) < 0)
	{
		FTPLOG("send user name error!\n");
		return -1;
	}
	
	if(sendPassword(password) < 0)
	{
		FTPLOG("send pass word error!\n");
		return -1;
	}

	return 0;
}

/*****************************************************************************
* Function Name : sendUserName
* Description   : 发送用户名到FTP服务器
* Input			: char *userName
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::sendUserName(char *userName)
{
	int length;
	int ftpCode;
	char buff[MAX_BUFF_LEN] = {0};
	char *ptr = NULL;
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff, "USER %s\r\n", userName);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending sendUserName command to server.");
		return -1;
	}
	FTPLOG("Send sendUserName OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* recv server's code and info. eg:331 User name okay, need password.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 331)
				{
					FTPLOG("The User does not have enough permissions!");
					return -1;
				}
			}
	}

	return 0;
}

/*****************************************************************************
* Function Name : sendUserName
* Description   : 发送用户密码到FTP服务器
* Input			: char *password
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::sendPassword(char *password)
{
	int length;
	int ftpCode;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff, "PASS %s\r\n", password);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending sendPassword command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* recv server's code and info. eg:230 User logged in, proceed.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 230)
				{
					FTPLOG("password error!");
					return -1;
				}
			}
	}

	return 0;
}

/*****************************************************************************
* Function Name : setFTPServerToPasvMode
* Description   : 设置FTP服务器为被动模式
* Input			: char *PASV_IP
*                 int *PASV_Port
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::setFTPServerToPasvMode(char *PASV_IP, int *PASV_Port)
{
	int length;
	int ftpCode;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff, "PASV\r\n");

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending setFTPServerToPasvMode command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 227)
				{
					FTPLOG("Set passive mode failed!");
					return -1;
				}
				else
				{
					getPasvModeIpAndPort(buff, length, PASV_IP, PASV_Port);
				}
			}
	}

	return 0;
}

/*****************************************************************************
* Function Name : setFTPServerToPasvMode
* Description   : 以被动模式连接FTP
* Input			: char *ip
*                 int port
*                 int *pasvSockfd
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::connectPasvServer(char *ip, int port, int *pasvSockfd,int binany)
{
	int sockfd;
	struct sockaddr_in serveraddr;
	uint8_t buff[MAX_BUFF_LEN] = {0};
	memset(&serveraddr, 0, sizeof(serveraddr));

	FTPLOG("ip:%s, port:%d\n", ip, port);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	serveraddr.sin_addr.s_addr = inet_addr(ip);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
	{
		*pasvSockfd = sockfd;
		FTPLOG("sockfd:%d, *pasvSockfd: %d\n",sockfd, *pasvSockfd);
		
		if (connect(sockfd, (struct sockaddr*) &serveraddr, sizeof(sockaddr_in)) != -1)
		{
			FTPLOG("Connect to pasv server,IP:%s,PORT:%d\n",ip,port);
			if(binany) setFTPToBinaryTransferMode();
		}
		else
		{
			close(sockfd);
			shutdown(sockfd, SHUT_RDWR);
			FTPLOG("connect failed close socketfd!");
			return -1;
		}
	}

	return 0;
}

/*****************************************************************************
* Function Name : getPasvModeIpAndPort
* Description   : 获得被动模式IP和端口
* Input			: char *pData
*                 int len
*                 char *PasvIp
*                 int *PasvPort
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::getPasvModeIpAndPort(char *pData, int len, char *PasvIp, int *PasvPort)
{ 
    int i;
	int count = 0;
	char *pos = pData;
	char tmp1[4] = {0};
	char tmp2[4] = {0};
	int port1;
	int port2;
	
	//example: char str[] = "227 Entering Passive Mode (210,74,1,30,118,26)";Entering Passive Mode (111,200,239,211,43,35).

	printf("pData == %s\n", pData);

	while(1)
	{
		if(*pos == '(')
		{
			break;
		}
		pos++;
		len--;
	}
	pos = pos+1;
	len = len -1;
	int tempLen = len;
	
	printf("tempLen == %d\n", tempLen);
	printf("pos == %s\n", pos);
	for(i = 0; i<tempLen; i++)
	{
		if(*pos == ',') 
		{ 
			count++;
			if(count == 4)
			{
				break;
			}
			PasvIp[i] = '.';
		}
		else
		{
			PasvIp[i] = *pos;
			printf("PasvIp = %d, count == %d\n", PasvIp[i], count);
		}
		pos++;
		len--;
	}
	
	pos += 1;
	len = len - 1;
	tempLen = len;
	printf("tempLen == %d\n", tempLen);
	for(i = 0; i<tempLen; i++)
	{
		if(*pos == ',') 
		{
			port1 = atoi(tmp1);
			break;
		}
		tmp1[i] = *pos;
		pos++;
		len--;
	}
	
   	pos += 1;
	len = len -1;
	tempLen = len;
	for(i = 0; i<tempLen; i++)
	{
		if(*pos == ')') 
		{
			port2 = atoi(tmp2);
			break;
		}
		tmp2[i] = *pos;
		pos++;
	}

	*PasvPort = port1*256+port2;
	FTPLOG("port1:%d, port2:%d\n", port1, port2);
	FTPLOG("ip:%s, port:%d\n", PasvIp, *PasvPort);
	
    return 0; 
}

/*****************************************************************************
* Function Name : setFTPToBinaryTransferMode
* Description   : 设置FTP为二进制传输模式
* Input			: None
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::setFTPToBinaryTransferMode()
{
	int length;
	int ftpCode;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff, "TYPE I\r\n");

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending setFTPToBinaryTransferMode command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 200)
				{
					FTPLOG("type recv is error!\n");
					return -1;
				}
				else
				{
					FTPLOG("use binary mode to transmission data.\n");
				}
			}
	}

	return 0;
}

/*****************************************************************************
* Function Name : getFile
* Description   : 下载文件
* Input			: int Pasvfd
*                 char *fileName
*                 char *destination
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::getFile(int Pasvfd, char *fileName, char *destination)
{
	int fd;
	int length;
	int ftpCode;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff, "RETR %s\r\n",fileName);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 150)
				{
					FTPLOG("can get file!");
					return -1;
				}

				if(0 > (fd = open(destination, O_CREAT|O_TRUNC|O_WRONLY,S_IRUSR|S_IWUSR)))
				{
					perror("open");
					close(Pasvfd);
					return -1;
				}
				while(1)
				{
					memset(buff, '\0', MAX_BUFF_LEN);
					length = recv(Pasvfd, buff, sizeof(buff), 0);
					if(length > 0)
					{
						write(fd, buff, length);
					}
					
					if(length <= 0)
					{
						shutdown(Pasvfd, SHUT_WR);
						close(Pasvfd);
						close(fd);
						break;
					}
				}

				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, MAX_BUFF_LEN, 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 226 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

			}
	}

	return 0;
}


/*****************************************************************************
* Function Name : uploadFile
* Description   : 上传文件
* Input			: int Pasvfd
*                 char *fileName	// / URL
*                 char *destination	// / data/myuplog
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::uploadFile(int Pasvfd,char* remotepath, char *destination)
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	char tempbuff[64] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	printf("remotepath == %s\n", remotepath);

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

//	ftp_mkdir(remotepath); 
    err = ftp_cd(/*remotepath*/(char*)"logs/"); 
	if(err)return -1;

	struct tm *p_tm = NULL; //时间的处理
	time_t tmp_time;
	tmp_time = time(NULL);
	p_tm = gmtime(&tmp_time);

	sprintf(tempbuff, "%d%d%d%d%d_log.file\0",  (p_tm->tm_mon + 1), p_tm->tm_mday, p_tm->tm_hour, \
		p_tm->tm_min, p_tm->tm_sec);

	sprintf(buff, "STOR %s\r\n",/*destination*/tempbuff);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if((ftpCode != 150) && (ftpCode != 125))
				{
					FTPLOG("can get file!");
					return -1;
				}
				flogfd = fopen("/data/myuplog", "r");
				if(flogfd == NULL)
				{
					perror("open");
					return -1;

				}
				while((length = fread(buff,1,511,flogfd))>0) 
   				{ 
        			err = send(Pasvfd,buff,length,0); 
       				if(err<0)
       				{       					
						shutdown(Pasvfd, SHUT_WR);
						close(Pasvfd);
						//close(flogfd);
						fclose(flogfd);
						return -1; 
       				}
   				} 
				shutdown(Pasvfd, SHUT_WR);
				close(Pasvfd);
				//close(flogfd);
				fclose(flogfd);

				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 226 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

			}
	}

	return 0;
}

int FTPClient::ftp_cd(char* dir) 
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff,"CWD %s\r\n",dir);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 250)
				{
					FTPLOG("can get file!");
					return -1;
				}
			}
	}

	return 0;
}

  
int FTPClient::ftp_cdup() 
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff,"CDUP\r\n");

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 250) /*?????????????????????*/
				{
					FTPLOG("can get file!");
					return -1;
				}
			}
	}

	return 0;
}


int FTPClient::ftp_mkdirSingle(char* dir) 
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff,"MKD %s\r\n",dir);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 250) /*?????????????????????*/
				{
					FTPLOG("can get file!");
					return -1;
				}
			}
	}

	return 0;
}

 
int FTPClient::ftp_mkdir(char* dir) 
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	int i,j; 
	char path[300]; 



	err = ftp_cd((char*)"/"); 
    if(err)return -1; 
	for(i=1,j=0;i<strlen(dir);i++)	 //第一个字节是根目录 
	{ 
		path[j++] = dir[i]; 
		if(dir[i]=='/'){ 
			path[j++]='\0'; 
			//printf("create :%s\n",path); 
			err = ftp_mkdirSingle(path); 
			err = ftp_cd(path); 
			if(err)return -1; 
			j=0; 
		} 
	} 
    path[j++]='\0'; 

	
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);
    sprintf(buff,"MKD %s\r\n",path); 

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 250) /*?????????????????????*/
				{
					FTPLOG("can get file!");
					return -1;
				}
			}
	}

	return 0;
}

 
 
int FTPClient::ftp_rmdir(char* dir) 
{
	int fd;
	int length;
	int ftpCode;	
	FILE *flogfd;
	int err;
	char buff[MAX_BUFF_LEN] = {0};
	fd_set fds;
	struct timeval timeout;// = {0, 0};
	timeout.tv_sec	= 5;
	timeout.tv_usec = 0;

	FD_ZERO(&fds);
	FD_SET(socketfd, &fds);

	sprintf(buff,"RMD %s\r\n",dir);

	length = send(socketfd, buff, strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending getFile command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	switch(select(socketfd+1, &fds, NULL, NULL, &timeout))
	{
		case -1:
			FTPLOG("Select error.\n");
			return -1;
		case 0:
			FTPLOG("Select timeout times.\n");
			return -1;
		default:
			if(FD_ISSET(socketfd, &fds))
			{
				memset(buff, 0, MAX_BUFF_LEN);
				length = recv(socketfd, buff, sizeof(buff), 0);
				if (length < 0)
				{
					perror("Error recv.");
					return -1;
				}
				/* 227 Entering passive mode.*/
				FTPLOG("recv data:%s, length:%d",buff,length);
				
				sscanf(buff,"%d", &ftpCode);
				FTPLOG("ftpCode:%d\n", ftpCode);

				if(ftpCode != 250) /*?????????????????????*/
				{
					FTPLOG("can get file!");
					return -1;
				}
			}
	}

	return 0;
}


/*****************************************************************************
* Function Name : quitFTP
* Description   : 下载文件
* Input			: None
* Output        : None
* Return        : 0:success, -1:faild
* Auther        : ygg
* Date          : 2018.01.18
*****************************************************************************/
int FTPClient::quitFTP()
{
	int length;
	char buff[MAX_BUFF_LEN] = {0};

	sprintf(buff, "QUIT\r\n");

	length = send(socketfd, buff, (int)strlen(buff), 0);
	if (length < 0)
	{
		perror("Error sending command to server");
		return -1;
	}
	FTPLOG("Send data OK, length:%d",length);

	memset(buff, 0, MAX_BUFF_LEN);

	if(recv(socketfd, buff, sizeof(buff), 0 ) < 0)
	{
		perror("Error: can't recv");
		return -1;
	}
	else 
		printf("%s",buff);

	//close(socketfd);
	this->~FTPClient();
	
	return 0;
}


