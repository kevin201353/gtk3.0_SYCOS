/***********
//filename: topwindow.c
//author: zhaosenhua
//version: 1.0
//funciton list: ....
//description: ShenCloud 虚拟终端顶层界面
//Date: 2016/6/21
**************/
#include <math.h>
#include "global.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define IMAGE_BTN_SH_NOR          "images2/sh_nor.png"
#define IMAGE_BTN_SH_PRES         "images2/sh_pres.png"

#define IMAGE_BTN_MIR_NOR         "images2/mirsoft_nor.png"
#define IMAGE_BTN_MIR_PRES        "images2/mirsoft_press.png"

#define IMAGE_BTN_SHUTDWON_NOR    "images2/shutdown_nor.png"
#define IMAGE_BTN_SHUTDWON_PRES   "images2/shutdown_press.png"

#define IMAGE_BTN_VMARE_NOR       "images2/vm_nor.png"
#define IMAGE_BTN_VMARE_PRES      "images2/vm_press.png"

#define IMAGE_BTN_CIT_NOR         "images2/cit_nor.png"
#define IMAGE_BTN_CIT_PRES        "images2/cit_press.png"

#define IMAGE_SETTING_NOR     "images2/set_nor.png"
#define IMAGE_SETTING_PRESS   "images2/set_press.png"

static GObject *g_window = NULL;
//extern void create_surfaces();
//extern void destroy_surfaces();

extern cairo_surface_t *surface_topwindow;
extern void MsgShutDwonDailog(char * sMsg);

GdkPixbuf *g_shPress;
GdkPixbuf *g_shNor;

GdkPixbuf *g_mirPress;
GdkPixbuf *g_mirNor;

GdkPixbuf *g_vmarePress;
GdkPixbuf *g_vmareNor;

GdkPixbuf *g_citPress;
GdkPixbuf *g_citNor;

static GdkPixbuf *g_setPress;
static GdkPixbuf *g_setNor;

extern GdkPixbuf *g_shutdownPress;
extern GdkPixbuf *g_shutdownNor;
static pthread_t tid;

static void *thrd_Vmarefunc(void *arg)
{
    int nRet = Run_VmwareView("admin", "123456", "127.0.0.1", "Test");
    if (nRet < 0)
    {
        LogInfo("Debug: login window connect vmare failed, nRet: %d. \n", nRet);
        MsgDailog("connect vmare view failed.");
        return -1;
    }
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

void ShenCloud_topWindowExit()
{
    printf("it is topwindow exit. \n");
    assert(g_window);
    gtk_widget_destroy((GtkWidget *)g_window);
    //gtk_widget_hide((GtkWidget *)g_window);
    gtk_main_quit();
}

//关闭启动界面和主程序的进程的快捷键处理
//SHIFT + Q 快捷键退出程序
static void on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    /*switch(event->keyval)
    {
        case GDK_KEY_Escape:
            gtk_main_quit();
            break;
        default:
            break;
    }*/
    gchar temp[100] = {0};
    if(event->state & GDK_SHIFT_MASK)
      strcpy(temp, "SHIFT+");
    strcat(temp, gdk_keyval_name(event->keyval));
    LogInfo("Debug : topwindow press : %s .\n", temp);
    if (strcasecmp(temp, "shift+q") == 0)
    {
        //ShenCloud_topWindowExit();
        SY_Setting_main();
    }
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
        MsgShutDownDailog("您确定要关闭系统吗？");
    }
    return TRUE;
}

static gboolean mirsoft_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("mirsoft gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_mirPress);
    }
    return TRUE;
}

static gboolean mirsoft_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("mirsoft gtkimage check mouser button released.\n");
        g_selectProto = 1;
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_mirNor);
        //ShenCloud_topWindowExit();
        SY_loginwindow_main();
    }
    return TRUE;
}

static gboolean vmware_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("vmware gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_vmarePress);
    }
    return TRUE;
}

static gboolean vmware_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("vmware gtkimage check mouser button released.\n");
        g_selectProto = 3;
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_vmareNor);
        //ShenCloud_topWindowExit();
        //SY_loginwindow_main();
        // 创建线程tid，且线程函数由thrd_func指向，是thrd_func的入口点，即马上执行此线程函数
        if ( pthread_create(&tid, NULL, thrd_Vmarefunc, NULL) !=0 ) {
            printf("Create vmare thread error!\n");
        }
    }
    return TRUE;
}

static gboolean sh_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("sh gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_shPress);
    }
    return TRUE;
}

static gboolean sh_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("sh gtkimage check mouser button released.\n");
        g_selectProto = 0;
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_shNor);
        //ShenCloud_topWindowExit();
        SY_loginwindow_main();
    }
    return TRUE;
}

static gboolean cit_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("cit gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_citPress);
    }
    return TRUE;
}

static gboolean cit_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("cit gtkimage check mouser button released.\n");
        g_selectProto = 2;
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_citNor);
        //ShenCloud_topWindowExit();
        SY_logincit_main();
    }
    return TRUE;
}

//为顶层窗口绘制背景图
gboolean on_draw_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    //g_print("on_draw_event start.\n");
    //LogInfo("debug: on_draw_event start.\n");
    GdkWindow *window;
    cairo_t *cr = NULL;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface_topwindow, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

void SY_topwindow_main()
{
    LogInfo("debug: SY_topwindow_main  0000000.\n");
    GObject *window;
    GtkBuilder *builder;
    builder = gtk_builder_new ();
    //load png
    g_shPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_PRES, NULL);
    g_shNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_NOR, NULL);

    g_mirPress = gdk_pixbuf_new_from_file(IMAGE_BTN_MIR_PRES, NULL);
    g_mirNor = gdk_pixbuf_new_from_file(IMAGE_BTN_MIR_NOR, NULL);

    g_vmarePress = gdk_pixbuf_new_from_file(IMAGE_BTN_VMARE_PRES, NULL);
    g_vmareNor = gdk_pixbuf_new_from_file(IMAGE_BTN_VMARE_NOR, NULL);

    g_shutdownPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_PRES, NULL);
    g_shutdownNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SHUTDWON_NOR, NULL);

    g_citPress = gdk_pixbuf_new_from_file(IMAGE_BTN_CIT_PRES, NULL);
    g_citNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CIT_NOR, NULL);

    g_setPress = gdk_pixbuf_new_from_file(IMAGE_SETTING_PRESS, NULL);
    g_setNor = gdk_pixbuf_new_from_file(IMAGE_SETTING_NOR, NULL);
    //
    GError *error = NULL;
    int nRet = gtk_builder_add_from_file (builder, "topwindow.glade", &error);
    LogInfo("debug: SY_topwindow_main  gtk_builder_add_from_file return :%d.\n", nRet);
    LogInfo("debug: SY_topwindow_main  gtk_builder_add_from_file error :%d.\n", error);
    g_window = gtk_builder_get_object (builder, "window1");
    if (g_window == NULL)
    {
        LogInfo("debug: SY_topwindow_main  g_window == null.\n");
    }
    g_mainWindow = g_window;
    gtk_widget_set_app_paintable((GtkWidget * )g_window, TRUE);
    g_selectProto = 0;
    LogInfo("debug: SY_topwindow_main  111111.\n");

    GObject *eventbox_sh;
    GObject *eventbox_mir;
    GObject *eventbox_vm;
    GObject *eventbox_cit;
    eventbox_sh = gtk_builder_get_object (builder, "eventbox_sh");
    gtk_widget_set_events((GtkWidget *)eventbox_sh, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    eventbox_mir = gtk_builder_get_object (builder, "eventbox_mirsoft");
    gtk_widget_set_events((GtkWidget *)eventbox_mir, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    eventbox_vm = gtk_builder_get_object (builder, "eventbox_vm");
    gtk_widget_set_events((GtkWidget *)eventbox_vm, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    eventbox_cit = gtk_builder_get_object (builder, "eventbox_cit");
    gtk_widget_set_events((GtkWidget *)eventbox_cit, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件


    GObject *sh_image;
    GObject *image_mirsoft;
    GObject *image_vm;
    GObject *image_cit;
    sh_image = gtk_builder_get_object (builder, "image_sh");
    image_mirsoft = gtk_builder_get_object (builder, "image_mirsoft");
    image_vm = gtk_builder_get_object (builder, "image_vm");
    image_cit = gtk_builder_get_object (builder, "image_cit");
    //shencloud
    g_signal_connect (G_OBJECT (eventbox_sh), "button_press_event",
                    G_CALLBACK (sh_button_press_callback), (GtkWidget *)sh_image);
    g_signal_connect (G_OBJECT (eventbox_sh), "button_release_event",
                                    G_CALLBACK (sh_button_released_callback), (GtkWidget *)sh_image);
    //mirsoft
    g_signal_connect (G_OBJECT (eventbox_mir), "button_press_event",
                    G_CALLBACK (mirsoft_button_press_callback), (GtkWidget *)image_mirsoft);
    g_signal_connect (G_OBJECT (eventbox_mir), "button_release_event",
                                    G_CALLBACK (mirsoft_button_released_callback), (GtkWidget *)image_mirsoft);
    //vmware
    g_signal_connect (G_OBJECT (eventbox_vm), "button_press_event",
                    G_CALLBACK (vmware_button_press_callback), (GtkWidget *)image_vm);
    g_signal_connect (G_OBJECT (eventbox_vm), "button_release_event",
                                    G_CALLBACK (vmware_button_released_callback), (GtkWidget *)image_vm);
    //cit
    g_signal_connect (G_OBJECT (eventbox_cit), "button_press_event",
                    G_CALLBACK (cit_button_press_callback), (GtkWidget *)image_cit);
    g_signal_connect (G_OBJECT (eventbox_cit), "button_release_event",
                                    G_CALLBACK (cit_button_released_callback), (GtkWidget *)image_cit);

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
    //根据分辨率
    GdkScreen* screen;
    screen = gdk_screen_get_default();
    g_screen_width = gdk_screen_get_width(screen);
    g_screen_height = gdk_screen_get_height(screen);
    gtk_window_set_default_size(GTK_WINDOW(g_window), g_screen_width, g_screen_height);
    gtk_window_set_decorated(GTK_WINDOW(g_window), FALSE); /* hide the title bar and the boder */
    LogInfo("topwindow screen width: %d, height: %d\n", g_screen_width, g_screen_height);
    g_signal_connect(G_OBJECT(g_window), \
                      "key-press-event", \
                      G_CALLBACK(on_key_press), NULL);
    //全屏显示结束
    g_signal_connect (G_OBJECT(g_window), "draw", G_CALLBACK (on_draw_event), NULL);
    g_signal_connect (G_OBJECT(g_window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    LogInfo("debug: SY_topwindow_main  2222222.\n");

    GObject *btn_set;
    GObject *image_set;
    image_set = gtk_builder_get_object (builder, "image_setting");
    btn_set = gtk_builder_get_object (builder, "btn_setting");
    g_signal_connect(G_OBJECT(btn_set), "pressed", G_CALLBACK(on_btn_setting_pressed), (GtkWidget *)image_set);
    g_signal_connect(G_OBJECT(btn_set), "released", G_CALLBACK(on_btn_setting_released), (GtkWidget *)image_set);

    //绘制控件
    /*----- CSS ----------- */
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen1;
   /*-----------------------*/
   /* ----------------- CSS ----------------------------------------------------------------------------------------------*/
    provider = gtk_css_provider_new ();
    display = gdk_display_get_default ();
    screen1 = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen1, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gsize bytes_written, bytes_read;
    GError *error1 = 0;

    gtk_css_provider_load_from_path (provider,
                                  g_filename_to_utf8(g_home_css, strlen(g_home_css), &bytes_read, &bytes_written, &error1),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
    /* --------------------------------------------------------------------------------------------------------------------*/
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
}
