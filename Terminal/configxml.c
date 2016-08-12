#include <mxml.h>  //minixml 解析库
#include <string.h>
#include <stdlib.h>
#include "global.h"
//login.xml
/*<?xml version="1.0"?>
<login>
    <user>val1</user>
    <password>val2</password>
    <ip>val3</ip>
    <port>val4</port>
</login>*/
#define  FILE_CONFIG_LOGIN    "login.xml"
#define  FILE_CONFIG_SETTING   "setting.xml"

void Parsexml(char * element,  char * value)
{
  	FILE *fp;
  	fp = fopen(FILE_CONFIG_LOGIN, "r");
    if (fp)
    {
      	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
      	if (g_tree != NULL)
      	{
        		mxml_node_t *node;
        		node = mxmlFindElement(g_tree, g_tree, element,
        												 NULL, NULL,
        												 MXML_DESCEND);
        		if (node != NULL)
        		{
        			 LogInfo("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
               //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
               strcpy(value, node->child->value.text.string);
        		}
      	}
      	fclose(fp);
    }
}

void SaveLogin(struct LoginInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */

    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "login");
    node = mxmlNewElement(data, "user");
    mxmlNewText(node, 0, info.user);
    node = mxmlNewElement(data, "password");
    mxmlNewText(node, 0, info.pass);
    node = mxmlNewElement(data, "repass");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", info.repass);
    mxmlNewText(node, 0, szTmp);
    FILE *fp;
    fp = fopen(FILE_CONFIG_LOGIN, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

void SaveServerInfo(struct ServerInfo info)
{
    mxml_node_t *xml;    /* <?xml ... ?> */
    mxml_node_t *data;   /* <data> */
    mxml_node_t *node;   /* <node> */
    mxml_node_t *node_server;

    xml = mxmlNewXML("1.0");
    data = mxmlNewElement(xml, "setting");
    node_server = mxmlNewElement(data, "server");
    node = mxmlNewElement(node_server, "address");
    mxmlNewText(node, 0, info.szIP);
    node = mxmlNewElement(node_server, "port");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", info.nport);
    mxmlNewText(node, 0, szTmp);
    node = mxmlNewElement(node_server, "user");
    mxmlNewText(node, 0, info.szUser);
    node = mxmlNewElement(node_server, "password");
    mxmlNewText(node, 0, info.szPass);
    node = mxmlNewElement(node_server, "resolution_client");
    memset(szTmp, 0, sizeof(MAX_BUFF_SIZE));
    sprintf(szTmp, "%d", info.resol);
    mxmlNewText(node, 0, szTmp);
    FILE *fp;
    fp = fopen(FILE_CONFIG_SETTING, "w");
    if (fp)
    {
        mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
        fclose(fp);
    }
}

int GetServerInfo(struct ServerInfo info)
{
     FILE *fp;
     fp = fopen(FILE_CONFIG_SETTING, "r");
     if (fp == NULL)
        return -1;
     mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
     if (g_tree != NULL)
     {
         mxml_node_t *node = NULL;
         mxml_node_t *heading = NULL;
         node = mxmlFindElement(g_tree, g_tree, "setting",
                              NULL, NULL,
                              MXML_DESCEND);
         if (node != NULL)
         {
             //printf("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
             //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
             for (heading = mxmlGetFirstChild(node);
                  heading;
                  heading = mxmlGetNextSibling(heading))
              {
                    mxml_node_t * tmp_node = mxmlFindElement(heading, node, "address",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(info.szIP, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, address : %s\n", info.szIP);
                    }

                    tmp_node = mxmlFindElement(heading, node, "port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        info.nport = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, port : %d.\n", info.nport);
                    }

                    tmp_node = mxmlFindElement(heading, node, "user",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(info.szUser, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, user : %s.\n", info.szUser);
                    }

                    tmp_node = mxmlFindElement(heading, node, "password",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(info.szPass, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, password : %s.\n", info.szPass);
                    }

                    tmp_node = mxmlFindElement(heading, node, "resolution_client",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        info.resol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_client : %d.\n", info.resol);
                    }
              }//for
         }//if
     }
     fclose(fp);
     return 0;
}

int GetServerInfo2(struct ServerInfo *pInfo)
{
     FILE *fp;
     fp = fopen(FILE_CONFIG_SETTING, "r");
     if (fp == NULL)
        return -1;
     mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
     if (g_tree != NULL)
     {
         mxml_node_t *node = NULL;
         mxml_node_t *heading = NULL;
         node = mxmlFindElement(g_tree, g_tree, "setting",
                              NULL, NULL,
                              MXML_DESCEND);
         if (node != NULL)
         {
             //printf("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
             //memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
             for (heading = mxmlGetFirstChild(node);
                  heading;
                  heading = mxmlGetNextSibling(heading))
              {
                    mxml_node_t * tmp_node = mxmlFindElement(heading, node, "address",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(pInfo->szIP, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, address : %s\n", pInfo->szIP);
                    }

                    tmp_node = mxmlFindElement(heading, node, "port",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        pInfo->nport = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, port : %d.\n", pInfo->nport);
                    }

                    tmp_node = mxmlFindElement(heading, node, "user",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(pInfo->szUser, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, user : %s.\n", pInfo->szUser);
                    }

                    tmp_node = mxmlFindElement(heading, node, "password",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        strcpy(pInfo->szPass, tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, password : %s.\n", pInfo->szPass);
                    }

                    tmp_node = mxmlFindElement(heading, node, "resolution_client",
                                          NULL, NULL,
                                          MXML_DESCEND);
                    if (tmp_node)
                    {
                        pInfo->resol = atoi(tmp_node->child->value.text.string);
                        LogInfo("configxml Get server info, resolution_client : %d.\n", pInfo->resol);
                    }
              }//for
         }//if
     }
     fclose(fp);
     return 0;
}

int GetLoginInfo(struct LoginInfo *pInfo)
{
    Parsexml("user",  pInfo->user);
    Parsexml("password",  pInfo->pass);
    char szTmp[MAX_BUFF_SIZE] = {0};
    Parsexml("repass",  szTmp);
    pInfo->repass = atoi(szTmp);
}
