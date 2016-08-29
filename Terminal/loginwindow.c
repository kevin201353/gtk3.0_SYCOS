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
#include "interface.h"
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>

#define WINDOW_MAX_WIDTH     1920
#define WINDOW_MAX_HEIGHT     1080

extern cairo_surface_t *surface1 ;
#define IMAGE_BTN_LOGIN_NOR    "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES   "images2/btnlogin_press.png"
#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtn_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtn_press.png"

#define IMAGE_BTN_PREV_NOR    "images2/loginout.png"
#define IMAGE_BTN_PREV_PRES   "images2/loginout_press.png"
#define IMAGE_SETTING_NOR     "images2/set_nor.png"
#define IMAGE_SETTING_PRESS   "images2/set_press.png"

//#define IMAGE_LOGO_CIT         "images2/cit_logo.png"
#define IMAGE_LOGO_VM         "images2/wm_logo.png"
#define IMAGE_LOGO_MIR         "images2/mir_logo.png"
#define IMAGE_LOGO_SH        "images2/sh_logo.png"


GdkPixbuf *g_loginPress;
GdkPixbuf *g_loginNor;

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;

GdkPixbuf *g_setPress;
GdkPixbuf *g_setNor;
GdkPixbuf *g_prevPress;
GdkPixbuf *g_prevNor;

GdkPixbuf *g_logoVm;
//GdkPixbuf *g_logoCit;
GdkPixbuf *g_logoMir;
GdkPixbuf *g_logoSh;

extern GdkPixbuf *g_shutdownPress = NULL;
extern GdkPixbuf *g_shutdownNor = NULL;

struct LoginInfo  g_loginfo = {SY_VM_COMMON_SPICE, "", "", "127.0.0.1", 3389, 0, 0};

static GObject *g_window = NULL;
unsigned short g_checkrepass;  //0: 没有选中 1：选中

unsigned short g_checkautologin; //0: 没有选中 1：选中
static  GtkBuilder *g_builder = NULL;
extern short g_loginExit;

extern void MsgDailog(char * sMsg);
static struct ServerInfo  serverInfo;
static int showloginwindow = 0;

//CallBackFun
extern void SYmsgCaller(CallBackFun fun, char *p);
extern int SYMsgFun(char *p);
extern GdkPixbuf *create_pixbuf(const gchar * filename);
//
static pthread_t tid;
void SetLoginWindowFocus()
{
    if (g_selectProto == 0) //ShenCloud
    {
        gtk_window_set_keep_above(GTK_WINDOW(g_window), TRUE);
    }
}

int initOvirtUrl()
{
     memset(ovirt_url, 0, MAX_BUFF_SIZE);
     if ( GetServerInfo2(&serverInfo) < 0 )
     {
         //可能是第一次启动，还没有设置
         return -1;
     }
     strcpy(ovirt_url, "https://");
     strcat(ovirt_url, serverInfo.szIP);
     strcat(ovirt_url, "/ovirt-engine/");
     return 0;
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    switch(event->keyval)
    {
        case GDK_KEY_Return:
            {
              //ShenCloud_login();
              SYMsgDialog(0, "正在连接，请稍后 ... ");
            }
            break;
    }
    return FALSE;
}

static gboolean shutdown_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("mirsoft gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_shutdownPress);
    }
    return TRUE;
}

static gboolean shutdown_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_shutdownNor);
        //MsgShutDownDailog("您确定要关闭系统吗？");
        SYMsgDialog(11, "您确定要关闭系统吗？");
    }
    return TRUE;
}

static void  on_btn_setting_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_setPress);
}

static void  on_btn_setting_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_setNor);
   SY_Setting_main();
}

static void  on_btn_prev_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
}

static void  on_btn_prev_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevNor);
   g_loginExit = 1;
   showloginwindow = 0;
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
}

static gboolean gtk_main_quit_login(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      showloginwindow = 0;
      gtk_main_quit();
      g_loginExit = 1;
      return TRUE;
}

/*void *thrd_Vmarefunc(void *arg)
{
    int nRet = Run_VmwareView(g_loginfo.user, g_loginfo.pass, g_loginfo.ip, "Test");
    if (nRet < 0)
    {
        LogInfo("Debug: login window connect vmare failed, nRet: %d. \n", nRet);
        MsgDailog("connect vmare view failed.");
        return -1;
    }
}*/

int ShenCloud_login()
{
    LogInfo("it is login_window exit. \n");
    if (Get_ctrldata() < 0)
      return -1;
    //test
    //SYmsgCaller(SYMsgFun, "正在连接，请稍后 ... ");
    SetSymsgContext("正在连接，请稍后 ... ");
    //test
    if (Ovirt_Login(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
    {
        LogInfo("main Ovirt login failed.\n");
        //MsgDailog("登录失败！");
        //SYmsgCaller(SYMsgFun, "登录失败！");
        SetSymsgContext("登录失败！");
        g_loginExit = 1;
        return -1;
    }
    //write login info
    if (g_selectProto == 0)
        SaveLogin(g_loginfo);
    if (g_selectProto == 1)
        SaveMirLogin(g_loginfo);
    /*if (g_selectProto == 3)
        SaveVMareLogin(g_loginfo);*/
    if (Ovirt_GetVms(ovirt_url, g_loginfo.user, g_loginfo.pass) < 0)
    {
        LogInfo("main Ovirt get vms failed.\n");
        //MsgDailog("获取虚拟机信息失败！");
        //SYmsgCaller(SYMsgFun, "获取虚拟机信息失败！");
        SetSymsgContext("获取虚拟机信息失败！");
        return -1;
    }
    //获取服务器虚拟机列表数据
    if (g_selectProto == 0)  //shencloud
    {
        if (SY_GetVms() < 0)
        {
            //MsgDailog("登录失败，用户名或密码错误！");
            SYmsgCaller(SYMsgFun, "登录失败，用户名或密码错误！");
            return -1;
        }
        //SYmsgCaller(SYMsgFun, "连接成功，获取虚拟机信息 ... ");
        SetSymsgContext("连接成功，获取虚拟机信息 ... ");
        LogInfo("login server shencloud, get vm  count : %d.\n", g_nVmCount);
        if (g_nVmCount == 1)
        {
            //直接连接虚拟机
            list_for_each(plist, &head)
            {
                struct Vms_Node *node = list_entry(plist, struct Vms_Node, list);
                LogInfo("Debug: login window directly connectVms g_loginfo.pass: = %s,  g_loginfo.pass = %s.\n", g_loginfo.user, g_loginfo.pass);
                Ovirt_GetVmTicket(ovirt_url, g_loginfo.user, g_loginfo.pass, node->val.vmid);
                char szTicket[MAX_BUFF_SIZE] = {0};
                char shellcmd[MAX_BUFF_SIZE] = {0};
                SY_GetVmsTicket(szTicket);
                //find vm
                sprintf(shellcmd, "spicy -h %s -p %d -w %s -f", node->val.ip, node->val.port, szTicket);
                LogInfo("Debug: login window directly connect vms  : %s. \n", shellcmd);
                system(shellcmd);
            }
        }
    }
    int nRet = 0;
    SetLogFlag(1);
    if (g_selectProto == 1)
    {
        LogInfo("login server mirfeerdp , directly connect vm.\n");
        char szError[MAX_BUFF_SIZE] = {0};
        nRet = Run_FreeRDP(g_loginfo.user, g_loginfo.pass, g_loginfo.ip);
        if (nRet < 0)
        {
            switch (abs(nRet)) {
              case 7:
                 strcpy(szError, "连接失败，请确认信息！");
                 break;
              case 2:
                 strcpy(szError, "IP地址不正确！");
                 break;
            }
            LogInfo("Debug: login window mirsoft, freeredp directly connect vms failed, nRet: %d. \n", nRet);
            //MsgDailog(szError);
            //SYmsgCaller(SYMsgFun, szError);
            SetSymsgContext(szError);
            return -1;
        }
    }
    /*if (g_selectProto == 3)
    {
        // 创建线程tid，且线程函数由thrd_func指向，是thrd_func的入口点，即马上执行此线程函数
        if ( pthread_create(&tid, NULL, thrd_Vmarefunc, NULL) !=0 ) {
            printf("Create vmare thread error!\n");
        }
    }*/
    //SYmsgCaller(SYMsgFun, "登录完成!");
    SetSymsgContext("登录完成!");
    CloseLog();
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
    showloginwindow = 0;
    g_loginExit = 0;
    if (g_selectProto == 0 && g_nVmCount > 1)
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
        LogInfo("checkbutton repass gtkimage check: %d.\n", g_checkrepass);
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
        //ShenCloud_login();
        SYMsgDialog(0, "正在连接，请稍后 ... ");
    }
    return TRUE;
}

/* 功能:      设置控件字体大小
* widget:    需要改变字体的控件
* size:      字体大小
* is_button: TRUE代表控件为按钮，FALSE为其它控件
*/
/*static void set_widget_font_size(GtkWidget *widget, int size, gboolean is_button)
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
}*/

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
    //g_print("on_expose_logineven start.\n");
    //LogInfo("debug: on_expose_logineven start.\n");
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface1, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

int Get_ctrldata()
{
    GObject *object;
    int len = 0;
    if (g_builder == NULL)
    {
        LogInfo("debug: Get_ctrldata  g_builder == NULL.\n");
        return -1;
    }
    if (g_selectProto == 1 || g_selectProto == 3)// freeRdp
    {
        object = gtk_builder_get_object (g_builder, "entry_ip");
        len = gtk_entry_get_text_length(GTK_ENTRY(object));
        if (len == 0)
        {
            LogInfo("debug: IP don't null.\n");
            //MsgDailog("IP 不能为空！");
            //SYMsgDialog(7, "IP 不能为空！");
            //SYmsgCaller(SYMsgFun, "IP 不能为空！");
            SetSymsgContext("IP 不能为空！");
            return -1;
        }
        char * ip = gtk_entry_get_text(GTK_ENTRY(object));
        //printf("username: %s.\n", szUsername);
        LogInfo("debug: ip: %s.\n" , ip);
        memset(g_loginfo.ip, 0, MAX_BUFF_SIZE);
        memcpy(g_loginfo.ip, ip, strlen(ip));

        object = gtk_builder_get_object (g_builder, "entry_port");
        len = gtk_entry_get_text_length(GTK_ENTRY(object));
        if (len == 0)
        {
            LogInfo("debug: port don't null.\n");
            //MsgDailog("端口不能为空！");
            //SYMsgDialog(7, "端口不能为空！");
            //SYmsgCaller(SYMsgFun, "端口不能为空！");
            SetSymsgContext("端口不能为空！");
            return -1;
        }
        char * port = gtk_entry_get_text(GTK_ENTRY(object));
        LogInfo("debug: ip: %s.\n" , port);
        g_loginfo.port = atoi(port);
    }
    object = gtk_builder_get_object (g_builder, "entry_user");
    if (object == NULL)
    {
        LogInfo("debug: 444444444 object  null.\n");
        return -1;
    }
    len = gtk_entry_get_text_length(GTK_ENTRY(object));
    if (len == 0)
    {
        LogInfo("debug: username don't null.\n");
        //MsgDailog("用户名不能为空！");
        //SYMsgDialog(7, "用户名不能为空！");
        //SYmsgCaller(SYMsgFun, "用户名不能为空！");
        SetSymsgContext("用户名不能为空！");
        return -1;
    }
    char * szUsername = gtk_entry_get_text(GTK_ENTRY(object));
    //printf("username: %s.\n", szUsername);
    LogInfo("debug: username: %s.\n" ,szUsername);
    memset(g_loginfo.user, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.user, szUsername, strlen(szUsername));
    //printf("username 222: %s.\n", g_loginfo.user);

    object = gtk_builder_get_object (g_builder, "entry_pass");
    len = gtk_entry_get_text_length(GTK_ENTRY(object));
    if (len == 0)
    {
        LogInfo("debug: password don't null.\n");
        //MsgDailog("密码不能为空！");
        //SYMsgDialog(7, "密码不能为空！");
        //SYmsgCaller(SYMsgFun, "密码不能为空！");
        SetSymsgContext("密码不能为空！");
        return -1;
    }
    char * szPassword = gtk_entry_get_text(GTK_ENTRY(object));
    //printf("password: %s.\n", szPassword);
    LogInfo("debug: password: %s.\n" ,szPassword);
    memset(g_loginfo.pass, 0, MAX_BUFF_SIZE);
    memcpy(g_loginfo.pass, szPassword, strlen(szPassword));
    //printf("password 222: %s.\n", g_loginfo.pass);
    g_loginfo.repass = g_checkrepass;
    LogInfo("debug: reapss: %d.\n" , g_loginfo.repass);
    return 0;
}

void SY_loginwindow_main()
{
    if (showloginwindow == 1)
       return;
    showloginwindow = 1;
    GObject *window;
    GObject *button;
    GtkBuilder *builder;
    GObject *label;
    GObject *comboboxpro;

    g_loginExit = 0;
    builder = gtk_builder_new ();

    GError *errort = NULL;
    gtk_builder_add_from_file (builder, "loginwindow3.glade", &errort);

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

    g_setPress = gdk_pixbuf_new_from_file(IMAGE_SETTING_PRESS, NULL);
    g_setNor = gdk_pixbuf_new_from_file(IMAGE_SETTING_NOR, NULL);
    g_prevPress = gdk_pixbuf_new_from_file(IMAGE_BTN_PREV_PRES, NULL);
    g_prevNor = gdk_pixbuf_new_from_file(IMAGE_BTN_PREV_NOR, NULL);


    g_builder = builder;

    g_checkrepass = 0;
    g_checkautologin = 0;
    initOvirtUrl();

    label = gtk_builder_get_object (builder, "label_pro");
    gtk_label_set_text(GTK_LABEL(label), "IP");
    //set_widget_font_size(GTK_LABEL(label), 13, FALSE);

    label = gtk_builder_get_object (builder, "label_port");
    gtk_label_set_text(GTK_LABEL(label), "端口");
    //set_widget_font_size(GTK_LABEL(label), 13, FALSE);


    label = gtk_builder_get_object (builder, "label_user");
    gtk_label_set_text(GTK_LABEL(label), "用户名");
    //set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "label_pass");
    gtk_label_set_text(GTK_LABEL(label), "密码");
    //set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "label_repass");
    gtk_label_set_text(GTK_LABEL(label), "记住密码");
    //set_widget_font_size(GTK_LABEL(label), 13, FALSE);

    label = gtk_builder_get_object (builder, "label_autologin");
    gtk_label_set_text(GTK_LABEL(label), "自动登录");
    //set_widget_font_size(GTK_LABEL(label), 13, FALSE);

    comboboxpro = gtk_builder_get_object (builder, "combobox_pro");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 0, "SPICE");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 1, "RDP");
    gtk_combo_box_set_active((GtkComboBox *)comboboxpro, 0);

    g_signal_connect(G_OBJECT(comboboxpro), "changed",
                         G_CALLBACK(comboboxpro_changed), NULL);

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
    GObject *entry_user;
    entry_user = gtk_builder_get_object (builder, "entry_user");
    memset(g_loginfo.proto, 0, MAX_BUFF_SIZE);
    memset(g_loginfo.user, 0, MAX_BUFF_SIZE);
    memset(g_loginfo.pass, 0, MAX_BUFF_SIZE);
    memset(g_loginfo.ip, 0, MAX_BUFF_SIZE);
    //printf(" login window wwwwwwww  user: %s\n", serverInfo.szUser);
    if (g_selectProto == 0) //ShenCloud
    {
        GObject *label;
        GObject *entry;
        label = gtk_builder_get_object (builder, "label_pro");
        gtk_widget_hide((GtkWidget *)label);
        label = gtk_builder_get_object (builder, "label_port");
        gtk_widget_hide((GtkWidget *)label);
        entry = gtk_builder_get_object (builder, "entry_ip");
        gtk_widget_hide((GtkWidget *)entry);
        entry = gtk_builder_get_object (builder, "entry_port");
        gtk_widget_hide((GtkWidget *)entry);
        struct LoginInfo info = {"", "", "", "", 3389, 0, 0};
        GetLoginInfo(&info);
        if (strcmp(info.user, "") == 0)
            strcpy(info.user, "admin@internal");
        gtk_entry_set_text(GTK_ENTRY(entry_user), info.user);
        g_checkrepass = info.repass;
        if (g_checkrepass == 1)
        {
            GObject *entry_pass;
            entry_pass = gtk_builder_get_object (builder, "entry_pass");
            gtk_entry_set_text(GTK_ENTRY(entry_pass), info.pass);
            gtk_image_set_from_pixbuf(GTK_IMAGE(repass_image), g_checkPressimage);
        }
    }

    if (g_selectProto == 1 || g_selectProto == 3) //mirsoft
    {
        GObject *label;
        GObject *entry;
        g_checkrepass = 0;
        struct LoginInfo info = {"", "", "", "", 3389, 0, 0};
        if (g_selectProto == 1)
            GetMirLoginInfo(&info);
        if (g_selectProto == 3)
            GetVmareLoginInfo(&info);
        label = gtk_builder_get_object (builder, "label_pro");
        gtk_widget_show((GtkWidget *)label);
        label = gtk_builder_get_object (builder, "label_port");
        gtk_widget_show((GtkWidget *)label);
        entry = gtk_builder_get_object (builder, "entry_ip");
        gtk_widget_show((GtkWidget *)entry);
        gtk_entry_set_text(GTK_ENTRY(entry), info.ip);
        entry = gtk_builder_get_object (builder, "entry_port");
        gtk_widget_show((GtkWidget *)entry);
        if (info.port == 0)
            info.port = 3389;
        {
           char szTmp[MAX_BUFF_SIZE] = {0};
           sprintf(szTmp, "%d", info.port);
           gtk_entry_set_text(GTK_ENTRY(entry), szTmp);
        }
        gtk_entry_set_text(GTK_ENTRY(entry_user), info.user);
        g_checkrepass = info.repass;
        if (g_checkrepass == 1)
        {
            GObject *entry_pass;
            entry_pass = gtk_builder_get_object (builder, "entry_pass");
            gtk_entry_set_text(GTK_ENTRY(entry_pass), info.pass);
            gtk_image_set_from_pixbuf(GTK_IMAGE(repass_image), g_checkPressimage);
        }
    }
    GObject *btn_set;
    GObject *btn_Prev;
    GObject *image_loginout;
    GObject *image_set;

    btn_set = gtk_builder_get_object (builder, "btn_set");
    btn_Prev = gtk_builder_get_object (builder, "btn_Prev");
    image_loginout = gtk_builder_get_object (builder, "image_loginout");
    image_set = gtk_builder_get_object (builder, "image_set");
    //配置
    if (g_selectProto == 0)
    {
        g_signal_connect(G_OBJECT(btn_set), "pressed", G_CALLBACK(on_btn_setting_pressed), (GtkWidget *)image_set);
        g_signal_connect(G_OBJECT(btn_set), "released", G_CALLBACK(on_btn_setting_released), (GtkWidget *)image_set);
        gtk_widget_set_visible(GTK_WIDGET(image_set), 0);
    }
    if (g_selectProto == 1 || g_selectProto == 3)
    {
        gtk_widget_set_visible(GTK_WIDGET(image_set), 0);
    }
    //还回到启动界面
    g_signal_connect(G_OBJECT(btn_Prev), "pressed", G_CALLBACK(on_btn_prev_pressed), (GtkWidget *)image_loginout);
    g_signal_connect(G_OBJECT(btn_Prev), "released", G_CALLBACK(on_btn_prev_released), (GtkWidget *)image_loginout);

    GObject *image_shutdown;
    GObject *eventbox_shutdown;
    image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
    eventbox_shutdown = gtk_builder_get_object (builder, "eventbox_shutdown");
    gtk_widget_set_events((GtkWidget *)eventbox_shutdown, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件
    g_signal_connect (G_OBJECT (eventbox_shutdown), "button_press_event",
                    G_CALLBACK (shutdown_button_press_callback), (GtkWidget *)image_shutdown);
    g_signal_connect (G_OBJECT (eventbox_shutdown), "button_release_event",
                                    G_CALLBACK (shutdown_button_released_callback), (GtkWidget *)image_shutdown);
    //全屏显示
    gtk_window_fullscreen(GTK_WINDOW(g_window));
    gtk_window_set_default_size(GTK_WINDOW(g_window), g_screen_width, g_screen_height);
    gtk_window_set_decorated(GTK_WINDOW(g_window), FALSE); /* hide the title bar and the boder */
    LogInfo("loginwindow screen width: %d, height: %d\n", g_screen_width, g_screen_height);
    gtk_window_set_keep_above(GTK_WINDOW(g_window), TRUE);
    g_signal_connect(G_OBJECT(g_window), \
                      "key-press-event", \
                      G_CALLBACK(on_key_press), NULL);
    //全屏显示结束
    //手动为主窗口添加关联退出事件
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit_login), NULL);
    gtk_window_set_icon(GTK_WINDOW(g_window), create_pixbuf("images2/logo.png"));

    //根据选择的协议的不同，改变登录时的LOGO
    GObject *image_logo;
    image_logo = gtk_builder_get_object (builder, "image_logo");
    g_logoVm = gdk_pixbuf_new_from_file(IMAGE_LOGO_VM, NULL);
    //g_logoCit = gdk_pixbuf_new_from_file(IMAGE_LOGO_CIT, NULL);
    g_logoMir = gdk_pixbuf_new_from_file(IMAGE_LOGO_MIR, NULL);
    g_logoSh = gdk_pixbuf_new_from_file(IMAGE_LOGO_SH, NULL);
    if (g_selectProto == 0)
    {
        //int x, y = 0;
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoSh);
        //LogInfo("Debug:  loginwindow  image_logo position:　x: = %d, y: =%d .\n", image_logo.allocation.x, image_logo.allocation.y);
    }
    if (g_selectProto == 1)
    {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoMir);
    }
  /*  if (g_selectProto == 2)
    {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoCit);
    }*/
    if (g_selectProto == 3)
    {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoVm);
    }
    //LOGO end
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    g_builder = NULL;
    showloginwindow = 0;
}
