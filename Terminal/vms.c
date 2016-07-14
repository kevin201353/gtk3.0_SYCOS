/***********
//filename: vms.c
//author: zhaosenhua
//version: 1.0
//funciton list: ...
//description: 获取服务器虚拟机列表处理模块
//Date: 2016/6/23
**************/
#include "vms.h"

mxml_node_t *g_tree = NULL;  //用来解析xml文件

void SY_Loadxml(char *file)
{
    FILE *fp;
    fp = fopen(file, "r");
    printf("xml file: %s.\n", file);
    g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
    if (g_tree == NULL)
    {
        printf("sy_loadxml failed.\n");
        fclose(fp);
        return;
    }
    fclose(fp);
    return ;
}

void SY_Unloadxml(mxml_node_t * node)
{
   if (node != NULL)
      mxmlDelete(node);
}

void SY_FreeVmsList()
{
  /*list_for_each(plist, &head)
  {
      printf("SY_FreeList start.\n");
      struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
      if (node)
      {
          free(node);
      }
      printf("SY_FreeList end.\n");
  }*/
  list_del(&head);
}

void FindNode(char* value)
{
    if (g_tree == NULL)
    {
        printf("findNode g_tree == NULL , return.\n");
        return ;
    }
    mxml_node_t *node = NULL;
    mxml_node_t *heading = NULL;
    node = mxmlFindElement(g_tree, g_tree, value,
                              NULL, NULL,
                              MXML_DESCEND);
   printf("1111  node %s\n", mxmlGetElement(node));
   if (node)
   {
     for (heading = mxmlGetFirstChild(node);
          heading;
          heading = mxmlGetNextSibling(heading))
          {
            //printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
          //  printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));
            //LogInfo("Debug: [xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
             if (mxmlGetElement(heading) != NULL)
             {
                 //printf("11111111111111.\n");
                 if (strcmp(mxmlGetElement(heading), "vm") == 0)
                 {
                     printf("[xml :xxx node: %s].\n", mxmlElementGetAttr(heading, "id"));
                     printf("[xml :xxx node 22 : %s].\n", mxmlGetElement (heading));

                     struct Vms_Node *pNode = malloc(sizeof(struct Vms_Node));
                     if (pNode == NULL)
                     {
                        printf("findnode malloc Vms_node failed.\n");
                        LogInfo("Debug: findnode malloc Vms_node failed. \n");
                        return ;
                     }
                     //get vms name
                     mxml_node_t * tmp_node = mxmlFindElement(heading, node, "name",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     printf("222222222, get vms attribute.\n");
                     if (tmp_node)
                     {
                        strcpy(pNode->val.name, tmp_node->child->value.text.string);
                        printf("pNode-val.name : %s.\n", pNode->val.name);
                        printf(" vms name : %s. \n", tmp_node->child->value.text.string);
                        LogInfo("Debug: pNode-val.name : %s.\n", pNode->val.name);
                     }//if name

                     //get vms os
                     tmp_node = mxmlFindElement(heading, node, "os",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                        strcpy(pNode->val.os, mxmlElementGetAttr(tmp_node, "type"));
                        printf("pNode-val.os : %s.\n", pNode->val.os);
                        LogInfo("Debug: pNode-val.os :%s.\n", pNode->val.os);
                     }//os

                     //get status
                     tmp_node = mxmlFindElement(heading, node, "state",
                                               NULL, NULL,
                                               MXML_DESCEND);
                     if (tmp_node)
                     {
                      //  strcpy(pNode->val.os, mxmlElementGetAttr(tmp_node, "type"));
                      //  printf("pNode-val.os : %s.\n", tmp_node->child->value.text.string);
                      //  LogInfo("Debug: pNode-val.os :%s.\n", tmp_node->child->value.text.string);
                        printf(" vms status : %s. \n", tmp_node->child->value.text.string);
                        if (strcmp(tmp_node->child->value.text.string, VMS_STATE_UP) == 0)
                          pNode->val.status = 1;
                        else if (strcmp(tmp_node->child->value.text.string, VMS_STATE_DOWN) == 0)
                          pNode->val.status = 0;
                     }//status

                     //get cpu count
                     tmp_node = mxmlFindElement(heading, node, "cpu",
                                               NULL, NULL,
                                               MXML_DESCEND);
                    if (tmp_node)
                    {
                        mxml_node_t *cpuNode = mxmlFindElement(tmp_node, heading, "topology",
                                                NULL, NULL,
                                                MXML_DESCEND);
                        if (cpuNode)
                        {
                            const char *strcpu = mxmlElementGetAttr(cpuNode, "sockets");
                            if (strcmp(strcpu, "") >= 0)
                            {
                                printf(" vms cpu count : %s. \n", strcpu);
                                pNode->val.vcpu = atoi(strcpu);
                            }
                        }
                    }//get cpu count

                    //get memory
                    tmp_node = mxmlFindElement(heading, node, "memory",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        printf(" vms memory : %s. \n", tmp_node->child->value.text.string);
                        pNode->val.memory = strtod(tmp_node->child->value.text.string, NULL)/(1024*1024);
                        printf(" vms memory long: %ld. \n", pNode->val.memory);
                    }//get memory

                    //get usb strategy
                    tmp_node = mxmlFindElement(heading, node, "usb",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        mxml_node_t * typeNode = mxmlFindElement(tmp_node, heading, "enabled",
                                                 NULL, NULL,
                                                 MXML_DESCEND);
                        if (typeNode)
                        {
                            printf(" vms usb strategy : %s.\n", typeNode->child->value.text.string);
                            //strcpy(pNode->val.usb, typeNode->child->value.text.string);
                            if (strcmp(typeNode->child->value.text.string, "true") == 0)
                                pNode->val.usb = 1;
                            else
                                pNode->val.usb = 0;
                        }else{
                           //strcpy(pNode->val.usb, "");
                        }
                    }//usb strategy

                    //get address
                    tmp_node = mxmlFindElement(heading, node, "address",
                                              NULL, NULL,
                                              MXML_DESCEND);
                    if (tmp_node)
                    {
                        printf(" vms address : %s. \n", tmp_node->child->value.text.string);
                        strcpy(pNode->val.ip, tmp_node->child->value.text.string);
                    }

                    list_add(&pNode->list, &head);
                 }//if vm
             }//if null
          }
   }
    return ;
}

void SY_GetVms()
{
    INIT_LIST_HEAD(&head);
    SY_Loadxml(FILE_OVIRT_INFO_PATH);
    FindNode("vms");
    SY_Unloadxml(g_tree);
}
