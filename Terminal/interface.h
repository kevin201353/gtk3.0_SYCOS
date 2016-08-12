#ifndef __INTERFACE_H_
#define __INTERFACE_H_

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

#define INT8        char
#define UIN8        unsigned char
#define INT32       int
#define UINT32      unsigned int

#define LOGSIZE_MAX     516
#define ARGVSIZE_MAX    32
#define RDPEXE_PATH        "/usr/local/bin/xfreerdp"
#define VMWAREEXE_PATH      "/usr/bin/vmware-view"
#define FIREFOX_PATH			"/usr/bin/firefox"

/*******************************
 *
 * ***************************/
typedef struct
{
    INT8 strUsrName[100];         //User name
    INT8 strPasswd[64];             //Passwd
    INT8 strIP[16];                 // IP
    INT8 strExeName[64];            //ExeName
    INT8 *Argv[ARGVSIZE_MAX];
}RDP;
typedef struct
{
    INT8 strUsrName[100];         //User name
    INT8 strPasswd[64];             //Passwd
    INT8 strIP[16];                 // IP
    INT8 strExeName[64];            //ExeName
    INT8 strDeskTop[64];            //DeskTop Name
    INT8 *Argv[ARGVSIZE_MAX];
}VMware;
typedef struct 
{
		 INT8 strDomain[100];                 // XenDeskTop IP
		 INT8 strExeName[64];            //ExeName
		 INT8 *Argv[ARGVSIZE_MAX];
}Citrix;
typedef struct
{
    RDP     sRDP;
    VMware  sVMware;
    Citrix			sCitrix;
    INT32 iLogFlag;                 //Log switch
    INT32 fd;                       //Log fd
}param;

void SetLogFlag(INT32 flag);

INT32 Run_FreeRDP(INT8 *name,INT8 *passwd,INT8 *IP);
INT32 Run_VmwareView(INT8 *name,INT8 *passwd,INT8 *IP,INT8 *desktop);
INT32 Run_Citrix(INT8 *IP);

INT32 InitLog(void);
INT32 CloseLog(void);
INT32 WriteLog(const INT8* sFormat, ... );
INT32 GetExeName(const INT8 *exe_path,INT32 Mode);
#endif
