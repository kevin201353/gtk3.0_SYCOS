#include <mxml.h>  //minixml 解析库
#include <string.h>
#include <stdlib.h>

/*<?xml version="1.0"?>
	 <data>
			 <node>val1</node>
			 <node>val2</node>
			 <node>val3</node>
			 <group>
					 <node>val4</node>
					 <node>val5</node>
					 <node>val6</node>
			 </group>
			 <node>val7</node>
			 <node>val8</node>
	 </data>*/

void parsexml()
{
	FILE *fp;
	fp = fopen("login.xml", "r");
	mxml_node_t *g_tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	if (g_tree != NULL)
	{
		mxml_node_t *node;
		node = mxmlFindElement(g_tree, g_tree, "user",
												 NULL, NULL,
												 MXML_DESCEND);
		if (node != NULL)
		{
			 printf("login xml user: %s.\n", node->child->value.text.string);
		}
	}
	fclose(fp);
}

int main(int argc, char *argv[])
{
	mxml_node_t *xml;    /* <?xml ... ?> */
  mxml_node_t *data;   /* <data> */
  mxml_node_t *node;   /* <node> */
  mxml_node_t *group;  /* <group> */

	xml = mxmlNewXML("1.0");
	data = mxmlNewElement(xml, "login");
	node = mxmlNewElement(data, "user");
  mxmlNewText(node, 0, "val1");
	node = mxmlNewElement(data, "password");
	mxmlNewText(node, 0, "val22");
	FILE *fp;
	fp = fopen("login.xml", "w");
	mxmlSaveFile(xml, fp, MXML_NO_CALLBACK);
	fclose(fp);

	parsexml();
	return 0;
}
