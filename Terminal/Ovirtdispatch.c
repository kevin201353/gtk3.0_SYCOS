/***********
//filename: curlc
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 自定义函数，负责与ovirt-engine ovirt 服务端通信，供其它函数调用
//如：登录 ovirt 服务端，获取 ovirt 参数信息
//Date: 2016/6/24
**************/
#include <stdio.h>
#include <string.h>
#include "global.h"

char g_szUrl[MAX_DATA_SIZE] = {0};

int Ovirt_Login(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_LOGIN);
    printf("Ovirt_Login url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_Login url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt login failed.\n");
       LogInfo("Debug: Ovirt login failed.\n");
       return SY_OVIRT_LOGIN_FAILED;
    }
    return 0;
}

int Ovirt_GetVms(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_VMS);
    printf("Ovirt_getvms url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getvms url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt get vms failed.\n");
       LogInfo("Debug: Ovirt get vms failed.\n");
       return SY_OVIRT_GETVMS_FAILED;
    }
    return 0;
}

int Ovirt_GetClusters(char *url, char *user, char* password)
{
    if (url == NULL && strlen(url) <= 0)
        return -1;
    if (user == NULL && strlen(user) <= 0)
    {
        return SY_USER_ISNULL;
    }
    memset(g_szUrl, 0, MAX_DATA_SIZE);
    strcat(g_szUrl, url);
    strcat(g_szUrl, STR_OVIRT_GET_CLUSTER);
    printf("Ovirt_getclusters url :%s.\n", g_szUrl);
    LogInfo("Debug: Ovirt_getclusters url : %s.\n", g_szUrl);

    if (Http_Request(g_szUrl, user, password) < 0)
    {
       printf("Ovirt get clusters failed.\n");
       LogInfo("Debug: Ovirt get clusters failed.\n");
       return SY_OVIRT_GETCLUSTERS_FAILED;
    }
    return 0;
}
