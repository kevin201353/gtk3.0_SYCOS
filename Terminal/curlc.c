/***********
//filename: curlc
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 自定义函数，负责与ovirt-engine ovirt 服务端通信的低层库，供其它函数调用
//如：登录 ovirt 服务端，获取 ovirt 参数信息
//Date: 2016/6/24
**************/
#include <stdio.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "global.h"

FILE *g_fp = NULL;  //定义FILE类型指针
CURL *g_curl = NULL;

//这个函数是为了符合CURLOPT_WRITEFUNCTION而构造的
//完成数据保存功能
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int written = fwrite(ptr, size, nmemb, (FILE *)g_fp);
    return written;
}

void Init_Curlc()
{
    //在使用该库时， 必须首先调用该函数进行初始化，
    //程序结束时，使用 'uinit_curlc()' 进行释放
    curl_global_init(CURL_GLOBAL_ALL);
    //在使用该库时，应先获取一个 CURL handler， 在后面的库函数中，都要输入该 handler
    //作为参数来调用。否则， 会失败或出错。
    g_curl = curl_easy_init();
}

void Unit_Curlc()
{
  /* always cleanup */
   if (g_curl != NULL)
   {
       curl_easy_cleanup(g_curl);
       g_curl = NULL;
   }
   curl_global_cleanup();
}

void Xml_Open()
{
    if((g_fp = fopen(FILE_OVIRT_INFO_PATH,"w")) == NULL)
    {
        printf("http_request fopen ovirt info file failed.\n");
        return;
    }
}

void Xml_Close()
{
    if (g_fp != NULL)
    {
      fclose(g_fp);
      g_fp = NULL;
    }
}

int Http_Request(char *url, char *user, char* password)
{
    if (g_curl == NULL)
    {
        printf("Http_request g_curl == null ,return.\n");
        return -1;
    }
    Xml_Open();
    char szbuf[512] = {0};
    strcat(szbuf, user);
    strcat(szbuf, ":");
    strcat(szbuf, password);
    curl_easy_setopt(g_curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(g_curl, CURLOPT_URL, url);
    curl_easy_setopt(g_curl, CURLOPT_USERPWD, szbuf);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(g_curl, CURLOPT_SSL_VERIFYHOST, 0);
    //curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(szbuf));
    //curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, szbuf);
    printf("Http_request url :%s.\n", url);
    printf("Http_request user and password :%s.\n", szbuf);

    CURLcode res = curl_easy_perform(g_curl);
   /* Check for errors */
   if(res != CURLE_OK)
   {
     fprintf(stderr, "curl_easy_perform() failed: %s\n",
             curl_easy_strerror(res));
     return -1;
   }
   Xml_Close();
    return 0;
}
