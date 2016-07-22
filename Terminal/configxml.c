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

void Parsexml(char * element,  char * value)
{
  	FILE *fp;
  	fp = fopen(FILE_CONFIG_LOGIN, "r");
  	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
  	if (g_tree != NULL)
  	{
    		mxml_node_t *node;
    		node = mxmlFindElement(g_tree, g_tree, element,
    												 NULL, NULL,
    												 MXML_DESCEND);
    		if (node != NULL)
    		{
    			 printf("login xml get element :%s  value: %s.\n", element, node->child->value.text.string);
           memcpy(value, node->child->value.text.string, strlen(node->child->value.text.string));
    		}
  	}
  	fclose(fp);
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
    FILE *fp;
    fp = fopen(FILE_CONFIG_LOGIN, "w");
    mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
    fclose(fp);
}
