/***********
//filename: vmlistwindow.c
//author: zhaosenhua
//version: 1.0
//funciton list: 虚拟机列表
//description: ShenCloud 虚拟终端登录界面主程序，负责启动登录界面
//Date: 2016/6/21
**************/
#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum {
    NAME = 0,
    NAME_1,
    OS,
    TAB,
    STATUS,
    VCPU,
    MEMORY,
    IP,
    USB,
    COLUMNS
};

#define IMAGE_TREE_VMICON_NOR    "images2/vm_icon.png"
GdkPixbuf *g_vmIconPix;
extern cairo_surface_t *surface_vmlistwindow ;
GtkListStore *g_store;

static void  on_vmlist_changed(GtkWidget *widget,  GtkTreeModel *model)
{
   GtkTreeIter iter;
   char *value;
   if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)) {
       gtk_tree_model_get(model, &iter, OS, &value, -1);
       printf("list changed 4444444  OS: %s\n", value);
       gtk_tree_model_get(model, &iter, IP, &value, -1);
      printf("list changed 4444444 IP: %s\n", value);
       //gtk_label_set_text(GTK_LABEL(label), value);
       g_free(value);
   }
}

//为登录主窗口绘制背景图
gboolean on_expose_vmlistevent (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    g_print("vmlist on_expose_event start.\n");
    LogInfo("debug: vmlist on_expose_event start.\n");
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
  //pattern1 = cairo_pattern_create_for_surface (surface1);

    /*
    cairo_set_source (cr, pattern1);
    cairo_pattern_set_extend (cairo_get_source (cr),
                              CAIRO_EXTEND_REPEAT);
    cairo_rectangle (cr, 20, 20, 100, 100);
    cairo_fill (cr);*/
    cairo_set_source_surface (cr, surface_vmlistwindow, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

static void setup_tree_view(GtkWidget*);
static void adddata();

void SY_vmlistwindow_main()
{
    GObject *window;
    GtkBuilder *builder;
    GObject *treeview;

    GtkTreeModel *model;
    GtkTreeSelection *selection;

    g_vmIconPix = gdk_pixbuf_new_from_file(IMAGE_TREE_VMICON_NOR, NULL);
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "vmlistwindow.glade", NULL);
    window = gtk_builder_get_object (builder, "vmlist_window");
    gtk_widget_set_app_paintable((GtkWidget * )window, TRUE);
    treeview = gtk_builder_get_object (builder, "treeview_vm");
    setup_tree_view(GTK_WIDGET(treeview));
    adddata();
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(g_store));

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
    g_signal_connect(selection, "changed", G_CALLBACK(on_vmlist_changed), model);
    g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_expose_vmlistevent), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    g_object_unref (G_OBJECT (g_store));
}

/* 功能:      设置控件字体大小
* widget:    需要改变字体的控件
* size:      字体大小
* is_button: TRUE代表控件为按钮，FALSE为其它控件
*/
static void set_widget_font_size(GtkWidget *widget, int size, GtkTreeViewColumn *column, GtkCellRenderer *renderer)
{
   GtkWidget *labelChild;
   PangoFontDescription *font;
   gint fontSize = size;

   font = pango_font_description_from_string("Microsoft YaHei");          //"Sans"字体名
   pango_font_description_set_size(font, fontSize*PANGO_SCALE);//设置字体大小

   //设置label的字体，这样这个GtkButton上面显示的字体就变了
   gtk_widget_modify_font(GTK_WIDGET(column), font);
   //gtk_widget_override_font(GTK_WIDGET(labelChild), font);

   PangoAttrList *attrls = NULL;
   attrls = pango_attr_list_new();
   PangoAttribute *attr = NULL;
   attr = pango_attr_foreground_new(65535,65535,65535); //rgb
   pango_attr_list_change(attrls, attr);
   //gtk_label_set_attributes(GTK_LABEL(labelChild), attrls);
   gtk_tree_view_column_set_attributes(column, renderer, attrls);
   pango_attr_list_unref(attrls); // 释放
   pango_font_description_free(font);
}

static void setup_tree_view(GtkWidget *treeview) {
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer_icon;

    /* Create a new GtkCellRendererText, add it to the tree view column
     * and append the column to the tree view.
     */
    renderer_icon = gtk_cell_renderer_pixbuf_new();
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("", renderer_icon, "pixbuf", NAME, NULL);
    //g_object_set(G_OBJECT(renderer), "foreground", "red", NULL);
    gtk_tree_view_column_set_fixed_width (column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("虚拟机", renderer, "text", NAME_1, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_column_set_fixed_width (column, 200);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("OS", renderer, "text", OS, NULL);
    gtk_tree_view_column_set_fixed_width (column, 200);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("标签", renderer, "text", TAB, NULL);
    gtk_tree_view_column_set_fixed_width (column, 100);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("状态", renderer, "text", STATUS, NULL);
    gtk_tree_view_column_set_fixed_width (column, 150);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("vCpu", renderer, "text", VCPU, NULL);
    gtk_tree_view_column_set_fixed_width (column, 100);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("内存", renderer, "text", MEMORY, NULL);
    gtk_tree_view_column_set_fixed_width (column, 100);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("IP地址", renderer, "text", IP, NULL);
    gtk_tree_view_column_set_fixed_width (column, 200);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("USB策略", renderer, "text", USB, NULL);
    gtk_tree_view_column_set_fixed_width (column, 200);
    gtk_tree_view_column_set_resizable(column, TRUE); //将列设置手动改变大小
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

void adddata()
{
  /*
   * Create a new tree model with three columns, as string,
   * gint and guint.
   */
  GtkTreeIter  iter;
  g_store = gtk_list_store_new(COLUMNS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  /* Add all of the products to the GtkListStore. */
  //vms data deal with
  //insert vms data into list
  list_for_each(plist, &head)
  {
      struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
      printf("main get vms name = %s.\n", node->val.name);
      printf("main get vms os = %s.\n", node->val.os);
      printf("main get vms status = %d.\n", node->val.status);
      printf("main get vms cpu count = %d.\n", node->val.vcpu);
      printf("main get vms memory = %ld.\n", node->val.memory);
      printf("main get vms ip = %s.\n", node->val.ip);
      printf("main get vms usb strategy = %d.\n", node->val.usb);
      char szStatus[MAX_BUFF_SIZE] = {0};
      char szMemory[MAX_BUFF_SIZE] = {0};
      char szUsb[MAX_BUFF_SIZE] = {0};
      char szvcpu[MAX_BUFF_SIZE] = {0};
      if (node->val.status == 1)
         memcpy(szStatus, "运行中", MAX_BUFF_SIZE);
      else if (node->val.status == 0)
         memcpy(szStatus, "停止", MAX_BUFF_SIZE);

      sprintf(szMemory, "%ld", node->val.memory);

      if (node->val.usb == 1)
        memcpy(szUsb, "支持", MAX_BUFF_SIZE);
      else if (node->val.usb == 0)
        memcpy(szUsb, "不支持", MAX_BUFF_SIZE);

      sprintf(szvcpu, "%d", node->val.vcpu);


      gtk_list_store_append(g_store, &iter);
      gtk_list_store_set(g_store, &iter,
                        NAME, g_vmIconPix,
                        NAME_1, node->val.name,
                        OS, node->val.os,
                        TAB, "测试",
                        STATUS, szStatus,
                        VCPU, szvcpu,
                        MEMORY, szMemory,
                        IP,  node->val.ip,
                        USB, szUsb,
                        -1);
  }
}
