#ifndef  _SY_VMS_H
#define  _SY_VMS_H

#include <mxml.h>  //minixml 解析库
#include <string.h>
#include "global.h"
#include <stdlib.h>

#define   VMS_STATE_UP    "up"    // 1
#define   VMS_STATE_DOWN   "down"   //0


void SY_Loadxml(char *file);
void SY_Unloadxml(mxml_node_t * node);
void FindNode(char* value);
void SY_GetVms();
void SY_FreeVmsList();

#endif  //_SY_VMS_H
