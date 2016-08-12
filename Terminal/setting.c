#include <gtk/gtk.h>
#include "global.h"
#include <string.h>

static const gchar*  g_settingcss = "setting.css";
#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtn_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtn_press.png"

enum {
    COLUMN = 0,
    NUM_COLS
};

struct ServerInfo g_serverinfo = {"192.168.0.220", 5900, "admin", "123456"};

GObject * g_layout1;  //服务器Layout
GObject * g_layout2;  //网络

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;

GtkBuilder *g_builder;

static unsigned short g_checkrepass = 0;

void  on_button_11_clicked(GtkButton *button, gpointer   user_data)
{
	printf("on button 11 clicked.\n");
}


int initctrl()
{
    GetServerInfo(g_serverinfo);
    GObject *object;
    object = gtk_builder_get_object (g_builder, "entry_serverip");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szIP);
    object = gtk_builder_get_object (g_builder, "entry_port");
    char szTmp[MAX_BUFF_SIZE] = {0};
    sprintf(szTmp, "%d", g_serverinfo.nport);
    gtk_entry_set_text((GtkEntry *)object, szTmp);
    object = gtk_builder_get_object (g_builder, "entry_user");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szUser);
    object = gtk_builder_get_object (g_builder, "entry_pass");
    gtk_entry_set_text((GtkEntry *)object, g_serverinfo.szPass);
    g_checkrepass = g_serverinfo.resol;
    GObject *image_resolution;
    image_resolution = gtk_builder_get_object (g_builder, "image_resolution");
    if (g_checkrepass)
    {
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkPressimage);
    }else
    {
       gtk_image_set_from_pixbuf(GTK_IMAGE(image_resolution), g_checkNorimage);
    }
}

unsigned short saveServerInfo()
{
      GObject *object;
      object = gtk_builder_get_object (g_builder, "entry_serverip");
      char * szserverIP = (char *)gtk_entry_get_text((GtkEntry *)object);
      LogInfo("debug: serverIP: %s.\n" ,szserverIP);
      memset(g_serverinfo.szIP, 0, MAX_BUFF_SIZE);
      memcpy(g_serverinfo.szIP, szserverIP, strlen(szserverIP));

      object = gtk_builder_get_object (g_builder, "entry_port");
      char * szPort = (char *)gtk_entry_get_text((GtkEntry *)object);
      LogInfo("debug: serverIP: %s.\n" ,szPort);
      g_serverinfo.nport = (unsigned int)atoi(szPort);

      object = gtk_builder_get_object (g_builder, "entry_user");
      char * szUsername = (char *)gtk_entry_get_text((GtkEntry *)object);
      LogInfo("debug: username: %s.\n" ,szUsername);
      memset(g_serverinfo.szUser, 0, MAX_BUFF_SIZE);
      memcpy(g_serverinfo.szUser, szUsername, strlen(szUsername));

      object = gtk_builder_get_object (g_builder, "entry_pass");
      char * szPassword = (char *)gtk_entry_get_text((GtkEntry *)object);
      LogInfo("debug: password: %s.\n" ,szPassword);
      memset(g_serverinfo.szPass, 0, MAX_BUFF_SIZE);
      memcpy(g_serverinfo.szPass, szPassword, strlen(szPassword));
      g_serverinfo.resol = g_checkrepass;

      SaveServerInfo(g_serverinfo);
      return 0;
}

static gboolean gtk_main_quit_setting(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      saveServerInfo();
      gtk_main_quit();
      return TRUE;
}

static gboolean checkbutton_resolution_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
        printf("checkbutton resolution gtkimage check: %d.\n", g_checkrepass);
        if (g_checkrepass == 1)
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkPressimage);
        }
        else
        {
            gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_checkNorimage);
        }
    }
    return TRUE;
}

void on_changed(GtkWidget * widget, gpointer statusbar)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *value;
    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(widget), &model, &iter)){
        gtk_tree_model_get(model, &iter, COLUMN, &value, -1);
		    printf("on changed selected value :%s.\n", (gchar *)value);
        //gtk_label_set_text((GtkLabel *)g_label, (gchar *)value);
		if (strcmp((char *)(gchar *)value, "服务器") == 0)
		{
  			gtk_widget_set_visible((GtkWidget *)g_layout1, 1);
  			gtk_widget_set_visible((GtkWidget *)g_layout2, 0);
        initctrl();
		}
		if (strcmp((char *)(gchar *)value, "网络") == 0)
		{
        saveServerInfo();
  			gtk_widget_set_visible((GtkWidget *)g_layout2, 1);
  			//gtk_label_set_text((GtkLabel *)g_label_11, "dfdfdfdfdfdfdfd");
  			gtk_widget_set_visible((GtkWidget *)g_layout1, 0);
		}
        g_free(value);
    }
	printf("on changed enter.\n");
}

static GtkTreeModel *create_and_fill_model(void)
{
    GtkTreeStore *treestore;
    GtkTreeIter toplevel, child;
    treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING);
    gtk_tree_store_append(treestore,&toplevel,NULL);
    gtk_tree_store_set(treestore,&toplevel, COLUMN,"服务器",-1);
    gtk_tree_store_append(treestore,&toplevel,NULL);
    gtk_tree_store_set(treestore,&toplevel, COLUMN,"网络",-1);
  /*  gtk_tree_store_append(treestore,&child,&toplevel);
    gtk_tree_store_set(treestore,&child, COLUMN,"PHP",-1);
    gtk_tree_store_append(treestore,&child,&toplevel);
    gtk_tree_store_set(treestore,&child, COLUMN,"Perl",-1);*/
    return GTK_TREE_MODEL(treestore);
}

static GtkWidget *create_view_and_model(void)
{
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkWidget *view;
    GtkTreeModel *model;
    view = gtk_tree_view_new();
    col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(col,"配置");
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer,TRUE);
    gtk_tree_view_column_add_attribute(col, renderer,"text", COLUMN);
    model = create_and_fill_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);
    return view;
}


static void print_hello (GtkWidget *widget,
             gpointer   data)
{
  g_print ("Hello World\n");
}

int  SY_Setting_main ()
{
  GtkBuilder *builder;
  GObject *window;
  GObject *boxmenu;
  GObject *boxwindow;
  GObject *button_11;

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new ();
  gtk_builder_add_from_file (builder, "setting.glade", NULL);
  boxmenu = gtk_builder_get_object (builder, "box_menu");
  boxwindow = gtk_builder_get_object (builder, "box_window");

  SY_NetworkSet_main();
  g_layout2 = GetNetWorkSetLayout();

  g_builder = builder;

  initctrl();

  GtkWidget *view;
  GtkTreeSelection *selection;
  GtkWidget *vbox1;
  view = create_view_and_model();
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
  gtk_box_pack_start(GTK_BOX(boxmenu), view, TRUE, TRUE, 1);

  g_layout1 = gtk_builder_get_object (builder, "layout1");
  gtk_box_pack_start(GTK_BOX(boxwindow), (GtkWidget *)g_layout2, TRUE, TRUE, 1);

  GObject * label_tip;
  GObject * label_serverip;
  GObject * label_port;
  GObject * label_user;
  GObject * label_pass;
  GObject * label_resolution;
  label_tip = gtk_builder_get_object (builder, "label_tip");
  label_serverip = gtk_builder_get_object (builder, "label_serverip");
  label_port = gtk_builder_get_object (builder, "label_port");
  label_user = gtk_builder_get_object (builder, "label_user");
  label_pass = gtk_builder_get_object (builder, "label_pass");
  label_resolution = gtk_builder_get_object (builder, "label_resolution");

  gtk_label_set_text(GTK_LABEL(label_tip), "在首次使用之前请正确设置服务器地址和端口，\n 如果您不了解这些设置，请联系你的管理员。");
  gtk_label_set_text(GTK_LABEL(label_serverip), "服务器地址");
  gtk_label_set_text(GTK_LABEL(label_port), "端口");
  gtk_label_set_text(GTK_LABEL(label_user), "用户名");
  gtk_label_set_text(GTK_LABEL(label_pass), "密码");
  gtk_label_set_text(GTK_LABEL(label_resolution), "根据客户端分辨率调整");

  g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
  g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);

  g_checkrepass = 1;

  //绘制控件
  /*----- CSS ----------- */
  GtkCssProvider *provider;
  GdkDisplay *display;
  GdkScreen *screen;
 /*-----------------------*/
 /* ----------------- CSS ----------------------------------------------------------------------------------------------*/
  provider = gtk_css_provider_new ();
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  gsize bytes_written, bytes_read;
  GError *error = 0;

  gtk_css_provider_load_from_path (provider,
                                g_filename_to_utf8(g_settingcss, strlen(g_settingcss), &bytes_read, &bytes_written, &error),
                                NULL);
  //gtk_css_provider_load_from_path (provider, home, NULL);
  g_object_unref (provider);
  /* --------------------------------------------------------------------------------------------------------------------*/

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object (builder, "window1");
  g_signal_connect(selection, "changed", G_CALLBACK(on_changed), NULL);
//  g_signal_connect (button_11, "clicked", G_CALLBACK (on_button_11_clicked), NULL);
  GObject *eventbox_resolution;
  eventbox_resolution = gtk_builder_get_object (builder, "eventbox_resolution");
  gtk_widget_set_events((GtkWidget *)eventbox_resolution, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

  GObject *image_resolution;
  image_resolution = gtk_builder_get_object (builder, "image_resolution");
  g_signal_connect (G_OBJECT (eventbox_resolution), "button_press_event",
                  G_CALLBACK (checkbutton_resolution_press_callback), (GtkWidget *)image_resolution);

  g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit_setting), NULL);
  gtk_widget_show_all((GtkWidget *)window);
  gtk_widget_set_visible((GtkWidget *)g_layout2, 0);

  gtk_main ();

  return 0;
}
