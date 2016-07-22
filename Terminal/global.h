/***********
//filename: global
//author: zhaosenhua
//version: 1.0
//funciton list:LogInfo
//description: 自定义函数，供其它函数调用
//Date: 2016/6/23
**************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "c_list.h"
#include <gtk/gtk.h>

#define MAX_DATA_SIZE  1024
#define ERROR_START_VALUE   -3000
#define FILE_OVIRT_INFO_PATH  "ovirtInfo.xml"
#define FILE_OVIRT_TICKET_PATH   "ticket.xml"
#define FILE_OVIRT_INFOTMP_PATH  "ovirtInfoTmp.xml"

#define SY_VM_COMMON_SPICE    "SPICE"
#define SY_VM_COMMON_RDP      "RDP"


#define  STR_OVIRT_LOGIN            "api"
#define  STR_OVIRT_GET_VMS          "api/vms"
#define  STR_OVIRT_GET_CLUSTER      "api/clusters"

#define MAX_BUFF_SIZE      100

struct list_head head, *plist;
static char ovirt_url[] = "https://192.168.110.200/ovirt-engine/";  //test
static const gchar* g_home_css = "/home/kevin/sycos/Terminal/mygtk.css";

//全局错误码定义
enum Error{
  SY_USER_ISNULL = ERROR_START_VALUE + 1,
  SY_PASSWORD_ISNULL,
  SY_OVIRT_LOGIN_FAILED,
  SY_OVIRT_GETVMS_FAILED,
  SY_OVIRT_GETCLUSTERS_FAILED,
  SY_OVIRT_STARTVMS_FAILED,
  SY_OVIRT_SHUTDOWNVMS_FAILED,
  SY_OVIRT_SUSPENDVMS_FAILED,
  SY_OVIRT_GETVMSTICKET_FAILED,
};

//虚拟机列表
struct Vms{
   char name[MAX_BUFF_SIZE];  //虚拟机名称
   char os[MAX_BUFF_SIZE];  //系统名称
   char tab[MAX_BUFF_SIZE]; //标签
   unsigned int status; //运行状态
   unsigned int vcpu;  //cpu 个数
   unsigned long memory; //内存  MB
   char ip[MAX_BUFF_SIZE]; //IP 地址
   unsigned int usb;   // sub策略
   char vmid[MAX_BUFF_SIZE];  //vm id
   int  port; //vm
};

struct Vms_Node {
    struct Vms  val;
    struct list_head list;
};


//
struct Vms  g_vmsComUpdate[MAX_BUFF_SIZE];
int  g_vmsComCount;

//登录用户信息
struct LoginInfo{
   char proto[20];
   char user[MAX_BUFF_SIZE];
   char pass[MAX_BUFF_SIZE];
   char ip[MAX_BUFF_SIZE];
   int  port;
   short protype;  //0:spice 1: rdp
};

//写日志
void LogInfo(const char* ms, ... );

void Init_Curlc();

void Unit_Curlc();

void Xml_Open();

void Xml_Close();

//调用libcurl 库发送 http 请求
int Http_Request(char *url, char *user, char *password);
int Http_Request2(char *url, char *user, char* password, char *path);

//登录Ovirt
int Ovirt_Login(char *url, char *user, char *password);

//获取Ovirt上的虚拟机列表
int Ovirt_GetVms(char *url, char *user, char *password);

//获取Ovirt 上的集群列表
int Ovirt_GetClusters(char *url, char *user, char *password);

//服务器网络配置
//终端配置
//系统配置

int SY_GetVms();
void SY_FreeVmsList();

void SY_topwindow_main();
void SY_loginwindow_main();
void SY_vmlistwindow_main();
void ShenCloud_topWindowShow();

int Ovirt_StartVms(char *url, char *user, char *password, char *vm);
int Ovirt_ShutdownVms(char *url, char *user, char *password, char *vm);
int Ovirt_SuspendVms(char *url, char *user, char *password, char *vm);
int Ovirt_GetVm2(char *url, char *user, char* password, char *vm);

void Parsexml(char *element,  char *value);
void SaveLogin(struct LoginInfo info);
int Http_Post(char *url, char *user, char* password, char *data);
void SY_GetVmsTicket(char * szTicket);

void Start_Session();
void Close_Session();
unsigned short SY_GetVmState(char* vmid);
int SY_GetVms2();
int Ovirt_GetVmsTmp(char *url, char *user, char* password);

#endif //_GLOBAL_H
