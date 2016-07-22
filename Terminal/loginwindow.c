/***********
//filename: loginwindow.c
//author: zhaosenhua
//version: 1.0
//funciton list: create_surfaces, destroy_surfaces ,login_event, main etc.
//description: ShenCloud 虚拟终端登录界面主程序，负责启动登录界面
//Date: 2016/6/21
**************/
#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <string.h>

#define WINDOW_MAX_WIDTH     1920
#define WINDOW_MAX_HEIGHT     1080

extern cairo_surface_t *surface1 ;
#define IMAGE_BTN_LOGIN_NOR    "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES   "images2/btnlogin_press.png"
#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtn_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtn_press.png"

GdkPixbuf *g_loginPress;
GdkPixbuf *g_loginNor;

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;

struct LoginInfo  g_loginfo = {SY_VM_COMMON_SPICE, "", "", "127.0.0.1", 8080, 0};

static GObject *g_window = NULL;
unsigned short g_checkrepass;  //0: 没有选中 1：选中

unsigned short g_checkautologin; //0: 没有选中 1：选中
GtkBuilder *g_builder = NULL;
extern short g_loginExit;

extern void MsgDailog(char * sMsg);

static gboolean gtk_main_quit_login(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      gtk_main_quit();
      g_loginExit = 1;
      return TRUE;
}

int ShenCloud_login()
{
    printf("it is login_window exit. \n");
    if (Get_ctrldata() < 0)
      return -1;
    strcat(g_loginfo.user, "@internal");
    //Start_Session();
    if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
    {
        printf("main Ovirt login failed.\n");
        MsgDailog("Login failed.");
        g_loginExit = 1;
        //Close_Session();
        return -1;
    }

    //write login info
    SaveLogin(g_loginfo);
    if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
    {
        printf("main Ovirt get vms failed.\n");
        MsgDailog("Get Vms info failed.");
        //Close_Session();
        return -1;
    }

    //获取服务器虚拟机列表数据
    if (SY_GetVms() < 0)
    {
        MsgDailog("login failed, Please ensure that the user name and password are correct.");
        //Close_Session();
        return -1;
    }
    //打印从服务器获取的虚拟机列表数据
  /*  //test use
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
    }*/
    //test
    gtk_widget_destroy((GtkWidget *)g_window);
    //gtk_widget_hide((GtkWidget *)g_window);
    gtk_main_quit();
    g_loginExit = 0;
    SY_vmlistwindow_main();
    return 0;
}

static gboolean comboboxpro_changed(GtkComboBoxText *comboBox, gpointer data) {
    gchar *active = gtk_combo_box_text_get_active_text((GtkComboBoxText *)comboBox);
    printf("loin selected combox item : %s\n", (char *)active);
    memset(g_loginfo.proto, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.proto, active, strlen(active));
    printf("loin selected combox item 2222: %s\n", g_loginfo.proto);
    return TRUE;
}

static gboolean checkbutton_repass_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
        printf("checkbutton repass gtkimage check: %d.\n", g_checkrepass);
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

static gboolean checkbutton_autologin_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkautologin = !g_checkautologin;
        printf("checkbutton autologin gtkimage check :%d .\n", g_checkautologin);
        if (g_checkautologin == 1)
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

static gboolean sh_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("sh loginwindow loginbutton check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_loginPress);
    }
    return TRUE;
}

static gboolean sh_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("sh loginwindow loginbutton check mouser button released.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_loginNor);
        ShenCloud_login();
    }
    return TRUE;
}

/* 功能:      设置控件字体大小
* widget:    需要改变字体的控件
* size:      字体大小
* is_button: TRUE代表控件为按钮，FALSE为其它控件
*/
static void set_widget_font_size(GtkWidget *widget, int size, gboolean is_button)
{
   GtkWidget *labelChild;
   PangoFontDescription *font;
   gint fontSize = size;

   font = pango_font_description_from_string("Microsoft YaHei");          //"Sans"字体名
   pango_font_description_set_size(font, fontSize*PANGO_SCALE);//设置字体大小

   if(is_button){
       labelChild = gtk_bin_get_child(GTK_BIN(widget));//取出GtkButton里的label
   }else{
       labelChild = widget;
   }

   //设置label的字体，这样这个GtkButton上面显示的字体就变了
   gtk_widget_modify_font(GTK_WIDGET(labelChild), font);
   //gtk_widget_override_font(GTK_WIDGET(labelChild), font);

   PangoAttrList *attrls = NULL;
   attrls = pango_attr_list_new();
   PangoAttribute *attr = NULL;
   attr = pango_attr_foreground_new(65535,65535,65535); //rgb
   pango_attr_list_change(attrls, attr);
   gtk_label_set_attributes(GTK_LABEL(labelChild), attrls);
   pango_attr_list_unref(attrls); // 释放
   pango_font_description_free(font);
}

/*void SetFontText(GtkWidget *widget, char * str)
{
    char *markup;
    gtk_label_set_text(GTK_LABEL(widget), str);
    markup = g_markup_printf_escaped ("<span foreground='white' size='9' font_desc='Microsoft YaHei' style='normal'>%s</span>", str);
    printf("set font text : %s\n", str);
    gtk_label_set_markup (GTK_LABEL(widget), markup);
    g_free (markup);
}*/
//为登录主窗口绘制背景图
gboolean on_expose_loginevent (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    g_print("on_expose_logineven start.\n");
    LogInfo("debug: on_expose_logineven start.\n");
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
    cairo_set_source_surface (cr, surface1, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

int Get_ctrldata()
{
    GObject *object;
    int len = 0;
    object = gtk_builder_get_object (g_builder, "entry_user");
    len = gtk_entry_get_text_length((GtkEntry *)object);
    if (len == 0)
    {
        g_print("username don't null.\n");
        LogInfo("debug: username don't null.\n");
        MsgDailog("Login user don't null.");
        return -1;
    }
    char * szUsername = gtk_entry_get_text((GtkEntry *)object);
    printf("username: %s.\n", szUsername);
    LogInfo("debug: username: %s.\n" ,szUsername);
    memset(g_loginfo.user, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.user, szUsername, strlen(szUsername));
    printf("username 222: %s.\n", g_loginfo.user);

    object = gtk_builder_get_object (g_builder, "entry_pass");
    len = gtk_entry_get_text_length((GtkEntry *)object);
    if (len == 0)
    {
        g_print("password don't null.\n");
        LogInfo("debug: password don't null.\n");
        MsgDailog("Login password don't null.");
        return -1;
    }
    char * szPassword = gtk_entry_get_text((GtkEntry *)object);
    printf("password: %s.\n", szPassword);
    LogInfo("debug: password: %s.\n" ,szPassword);
    memset(g_loginfo.pass, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.pass, szPassword, strlen(szPassword));
    printf("password 222: %s.\n", g_loginfo.pass);
    return 0;
}

void SY_loginwindow_main()
{
    GObject *window;
    GObject *button;
    GtkBuilder *builder;
    GObject *label;
    GObject *comboboxpro;

    g_loginExit = 0;
    builder = gtk_builder_new ();

    gtk_builder_add_from_file (builder, "loginwindow3.glade", NULL);

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object (builder, "window_login");
    g_window = window;
    //gtk_window_resize(GTK_WINDOW(window), WINDOW_MAX_WIDTH, WINDOW_MAX_HEIGHT);
    gtk_widget_set_app_paintable((GtkWidget * )window, TRUE);
    //button = gtk_builder_get_object (builder, "loginbutton");
    g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_expose_loginevent), NULL);

    g_loginPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_PRES, NULL);
    g_loginNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_NOR, NULL);

    g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
    g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);
    g_builder = builder;

    g_checkrepass = 0;
    g_checkautologin = 0;

    label = gtk_builder_get_object (builder, "label_pro");
    gtk_label_set_text(GTK_LABEL(label), "选择协议");
    set_widget_font_size(GTK_LABEL(label), 18, FALSE);

    label = gtk_builder_get_object (builder, "label_user");
    gtk_label_set_text(GTK_LABEL(label), "用户名");
    set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "label_pass");
    gtk_label_set_text(GTK_LABEL(label), "密码");
    set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "label_repass");
    gtk_label_set_text(GTK_LABEL(label), "记住密码");
    set_widget_font_size(GTK_LABEL(label), 13, FALSE);

    label = gtk_builder_get_object (builder, "label_autologin");
    gtk_label_set_text(GTK_LABEL(label), "自动登录");
    set_widget_font_size(GTK_LABEL(label), 13, FALSE);

    comboboxpro = gtk_builder_get_object (builder, "combobox_pro");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 0, "SPICE");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 1, "RDP");
    gtk_combo_box_set_active((GtkComboBox *)comboboxpro, 0);

    g_signal_connect(G_OBJECT(comboboxpro), "changed",
                         G_CALLBACK(comboboxpro_changed), NULL);

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
                                  g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error),
                                  NULL);
	  //gtk_css_provider_load_from_path (provider, home, NULL);
	  g_object_unref (provider);
	  /* --------------------------------------------------------------------------------------------------------------------*/


  /*  g_signal_connect (G_OBJECT(comboboxpro), "expose-event", G_CALLBACK (on_drawcombobox_event), (GtkWidget *)comboboxpro);
    GdkWindow *window2 = gtk_widget_get_window(GTK_WIDGET(comboboxpro));
    gtk_widget_set_app_paintable(window2, TRUE);*/

    GObject *eventbox_login;
    eventbox_login = gtk_builder_get_object (builder, "eventbox_login");
    gtk_widget_set_events((GtkWidget *)eventbox_login, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件
    GObject *login_image;
    login_image = gtk_builder_get_object (builder, "image_login");
    g_signal_connect (G_OBJECT (eventbox_login), "button_press_event",
                    G_CALLBACK (sh_button_press_callback), (GtkWidget *)login_image);
    g_signal_connect (G_OBJECT (eventbox_login), "button_release_event",
                                    G_CALLBACK (sh_button_released_callback), (GtkWidget *)login_image);

    //checkbutton rember pass
    GObject *eventbox_rempass;
    GObject *eventbox_autologin;
    eventbox_rempass = gtk_builder_get_object (builder, "eventbox_rempass");
    gtk_widget_set_events((GtkWidget *)eventbox_rempass, GDK_BUTTON_PRESS_MASK);
    eventbox_autologin = gtk_builder_get_object (builder, "eventbox_autologin");
    gtk_widget_set_events((GtkWidget *)eventbox_autologin, GDK_BUTTON_PRESS_MASK);

    //checkbutton image signal
    GObject *repass_image;
    GObject *autologin_image;
    repass_image = gtk_builder_get_object (builder, "image_repass");
    autologin_image = gtk_builder_get_object (builder, "image_autologin");
    g_signal_connect (G_OBJECT (eventbox_rempass), "button_press_event",
                    G_CALLBACK (checkbutton_repass_press_callback), (GtkWidget *)repass_image);
    g_signal_connect (G_OBJECT (eventbox_autologin), "button_press_event",
                                    G_CALLBACK (checkbutton_autologin_press_callback), (GtkWidget *)autologin_image);
    //手动为主窗口添加关联退出事件
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit_login), NULL);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
}
