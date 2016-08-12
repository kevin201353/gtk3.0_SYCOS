#include "interface.h"

param s;

/***************************************
 * flag = 1;Open Log
 * flag = 0;Close Log
 * ************************************/
void SetLogFlag(INT32 flag)
{
    if(!flag)
    {
        s.iLogFlag = 0;
    }
    else
    {
        s.iLogFlag =1;
        if(InitLog() < 0)
        {
            printf("Log no use!Error[%d]\n",errno);
        }
    }
}
INT32 InitLog(void)
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    if(s.fd > 2)
    {
        close(s.fd);
    }
    if(access("Interface.log",F_OK) < 0)
    {
        s.fd = open("Interface.log",O_CREAT|O_APPEND|O_RDWR,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    }
    else
    {
        s.fd = open("Interface.log",O_APPEND|O_RDWR);
    }
    if(s.fd < 0)
    {
        perror("Open LogFile Error!");
        s.iLogFlag = 0;
        return -1;
    }
    return 0;
}
INT32 CloseLog(void)
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    if(s.fd > 0)
    {
        close(s.fd);
        s.fd = 0;
    }
    return 0;
}
INT32 WriteLog(const INT8* sFormat, ... )
{
    if(!s.iLogFlag)
    {
        return 0;
    }
    va_list arg;
    struct tm *pTm;
    time_t Time;
    Time = time(NULL);
    pTm = localtime(&Time);
    int iLen = 0;
    INT8 LogBuf[LOGSIZE_MAX] = {0};

    sprintf(LogBuf,"[%04d-%02d-%02d %02d:%02d:%02d] ",1900+pTm->tm_year, pTm->tm_mon+1, pTm->tm_mday, pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
    if(write(s.fd,LogBuf,22) < 0)
    {
        perror("Write Log Time Error");
    }
    printf("%s",LogBuf);

    memset(LogBuf,0,LOGSIZE_MAX);
    va_start(arg,sFormat);
    vsnprintf(LogBuf,LOGSIZE_MAX-2,sFormat,arg);
    va_end(arg);
    iLen = strlen(LogBuf);
    if(iLen < LOGSIZE_MAX-1)
    {
        LogBuf[iLen] = '\n';
    }
    LogBuf[iLen+1] = 0;
    if(write(s.fd,LogBuf,iLen+1) < 0)
    {
        perror("Write Log Error");
    }
    printf("%s",LogBuf);
}
/***************************************************
 * exe_path             Exe Path
 * Mode          1      RDP Mode
 * Mode          0      Vmware Mode
 * Mode			2       Citrix Mode
 * ************************************************/
INT32 GetExeName(const INT8 *exe_path,INT32 Mode)
{
    INT8 *p = exe_path;
    INT8 *dp = NULL;
    if(strlen(p) == 0)
    {
        WriteLog("Not Find EXE.");
        return -1;
    }
    dp = strrchr(p,'/');
    if(dp)
    {
        switch(Mode)
        {
			case 0:
			{
				strcpy(s.sVMware.strExeName,dp+1);
			}break;
		   case 1:
		   {
			  strcpy(s.sRDP.strExeName,dp+1);
			}break;
			case 2:
			{
			strcpy(s.sCitrix.strExeName,dp+1);
			}break;
			default:
				break;
       }
    }
    else
    {
        switch(Mode)
        {
			case 0:
			{
				strcpy(s.sVMware.strExeName,p+1);
			}break;
		   case 1:
		   {
			  strcpy(s.sRDP.strExeName,p+1);
			}break;
			case 2:
			{
			strcpy(s.sCitrix.strExeName,p+1);
			}break;
			default:
				break;
       }
    }
    return 0;
}
/*********************************************
 * Name:user name
 * Passwod:user passwd
 * IP:server ip
 * *******************************************/
INT32 Run_FreeRDP(INT8 *name,INT8 *passwd,INT8 *IP)
{
    INT32 iRet = -1;
    INT32 iStatus = -1;
    WriteLog("Login With FreeRDP------------------------");
    WriteLog("Name[%s] Passwd[%s] IP[%s]",name,passwd,IP);
    if(name==0||passwd==0||IP==NULL)
    {
        WriteLog("Error:Please Check Name,Passwd,IP");
        return -1;
    }
    //Information Right Start Run
    strncpy(s.sRDP.strUsrName,name,100);
    strncpy(s.sRDP.strPasswd,passwd,64);
    strncpy(s.sRDP.strIP,IP,16);
    if(GetExeName(RDPEXE_PATH,1) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sRDP.strExeName);
        return -2;
    }

    s.sRDP.Argv[0] = s.sRDP.strExeName;
    s.sRDP.Argv[1] = "-u";
    s.sRDP.Argv[2] = s.sRDP.strUsrName;
    s.sRDP.Argv[3] = "-p";
    s.sRDP.Argv[4] = s.sRDP.strPasswd;
    s.sRDP.Argv[5] = "-v";
    s.sRDP.Argv[6] = s.sRDP.strIP;
    s.sRDP.Argv[7] = "-f";
    s.sRDP.Argv[8] = "-usb";
    s.sRDP.Argv[9] = "auto";
    s.sRDP.Argv[10] = "-sound";
    s.sRDP.Argv[11] = "-rfx";
    s.sRDP.Argv[12] = "-t";
    s.sRDP.Argv[13] = "ShenCloud";
    s.sRDP.Argv[14] = "-from-stdin";
    s.sRDP.Argv[15] = NULL;
printf("-------ExeName[%s] Path[%s]\n",s.sRDP.strExeName,RDPEXE_PATH);
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -2;
    }
    if(pid == 0)
    {
        iRet = execv(RDPEXE_PATH,s.sRDP.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
     WriteLog("-------------------------------------------");
    return 0;
}
INT32 Run_VmwareView(INT8 *name,INT8 *passwd,INT8 *IP,INT8 *desktop)
{
    INT32 iStatus;
    INT32 iRet;
    WriteLog("Login With VMwareView..................");
    WriteLog("Name[%s] Passwd[%s] IP[%s] DeskTop[%s]",name,passwd,IP,desktop);
    if(name == NULL||passwd == NULL||IP == NULL||desktop == NULL)
    {
        WriteLog("Error:Please Check Name,Passwd,IP,DeskTop");
        return -1;
    }

    strncpy(s.sVMware.strUsrName,name,100);
    strncpy(s.sVMware.strPasswd,passwd,64);
    strncpy(s.sVMware.strIP,IP,16);
    if(GetExeName(VMWAREEXE_PATH,0) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sVMware.strExeName);
        return -2;
    }

    s.sVMware.Argv[0] = s.sVMware.strExeName;
    s.sVMware.Argv[1] = NULL;
//    s.sVMware.Argv[1] = "-u";
    s.sVMware.Argv[2] = s.sVMware.strUsrName;
    s.sVMware.Argv[3] = "-p";
    s.sVMware.Argv[4] = s.sVMware.strPasswd;
    s.sVMware.Argv[5] = "-s";
    s.sVMware.Argv[6] = s.sVMware.strIP;
    s.sVMware.Argv[7] = "-n";
    s.sVMware.Argv[8] = s.sVMware.strDeskTop;
    s.sVMware.Argv[9] = "protocol=BLAST";
    s.sVMware.Argv[10] = "fullscreen";
    s.sVMware.Argv[11] = "-q";
    s.sVMware.Argv[12] = NULL;

    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -2;
    }
    if(pid == 0)
    {
        iRet = execv(VMWAREEXE_PATH,s.sVMware.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
     WriteLog("-------------------------------------------");
}
INT32 Run_Citrix(INT8 *IP)
{
	 INT32 iStatus;
    INT32 iRet;
    WriteLog("Login With Citrix..................");
    WriteLog("IP[%s]",IP);
    if(IP == NULL)
    {
        WriteLog("Error:Please IP");
        return -1;
    }
    strncpy(s.sCitrix.strDomain,IP,100);
    if(GetExeName(FIREFOX_PATH,2) < 0)
    {
        WriteLog("Unknow ExeName[%s]",s.sCitrix.strExeName);
        return -2;
    }
	s.sCitrix.Argv[0] = s.sCitrix.strExeName;
	s.sCitrix.Argv[1] = "-URL";
	s.sCitrix.Argv[2] = s.sCitrix.strDomain;
	s.sCitrix.Argv[3] = NULL;


    pid_t pid = 0;
    //pid = fork();
    if(pid < 0)
    {
        WriteLog("Error:Fork Error!");
        return -2;
    }
    if(pid == 0)
    {
        iRet = execv(FIREFOX_PATH,s.sCitrix.Argv);
        if(iRet < 0)
        {
            WriteLog("execv ret:%d error:%d error:%s",iRet,errno,strerror(errno));
        }
        exit(0);
    }
    if(pid > 0)
    {
        pid = wait(&iStatus);
        WriteLog("Wait Pid[%u] return Status[%d].",pid,iStatus);
    }
     WriteLog("-------------------------------------------");
}
