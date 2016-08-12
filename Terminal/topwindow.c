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

#define IMAGE_BTN_SH_NOR    "images2/sh_nor.png"
#define IMAGE_BTN_SH_PRES   "images2/sh_pres.png"

#define IMAGE_BTN_MIR_NOR    "images2/mirsoft_nor.png"
#define IMAGE_BTN_MIR_PRES   "images2/mirsoft_press.png"

static GObject *g_window = NULL;
//extern void create_surfaces();
//extern void destroy_surfaces();

extern cairo_surface_t *surface_topwindow;

GdkPixbuf *g_shPress;
GdkPixbuf *g_shNor;

GdkPixbuf *g_mirPress;
GdkPixbuf *g_mirNor;

void ShenCloud_topWindowExit()
{
    printf("it is topwindow exit. \n");
    assert(g_window);
    gtk_widget_destroy((GtkWidget *)g_window);
    //gtk_widget_hide((GtkWidget *)g_window);
    gtk_main_quit();
}

void ShenCloud_topWindowShow()
{
   gtk_widget_show_all((GtkWidget *)g_window);
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
        ShenCloud_topWindowExit();
        SY_loginwindow_main();
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
        ShenCloud_topWindowExit();
        SY_loginwindow_main();
    }
    return TRUE;
}

//为顶层窗口绘制背景图
gboolean on_draw_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    g_print("on_draw_event start.\n");
    LogInfo("debug: on_draw_event start.\n");
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
    gtk_widget_set_app_paintable((GtkWidget * )g_window, TRUE);
    g_selectProto = 0;
    LogInfo("debug: SY_topwindow_main  111111.\n");

    GObject *eventbox_sh;
    GObject *eventbox_mir;
    eventbox_sh = gtk_builder_get_object (builder, "eventbox_sh");
    gtk_widget_set_events((GtkWidget *)eventbox_sh, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    eventbox_mir = gtk_builder_get_object (builder, "eventbox_mirsoft");
    gtk_widget_set_events((GtkWidget *)eventbox_mir, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    GObject *sh_image;
    GObject *image_mirsoft;

    sh_image = gtk_builder_get_object (builder, "image_sh");
    image_mirsoft = gtk_builder_get_object (builder, "image_mirsoft");
    g_signal_connect (G_OBJECT (eventbox_sh), "button_press_event",
                    G_CALLBACK (sh_button_press_callback), (GtkWidget *)sh_image);
    g_signal_connect (G_OBJECT (eventbox_sh), "button_release_event",
                                    G_CALLBACK (sh_button_released_callback), (GtkWidget *)sh_image);
    g_signal_connect (G_OBJECT (eventbox_mir), "button_press_event",
                    G_CALLBACK (mirsoft_button_press_callback), (GtkWidget *)image_mirsoft);
    g_signal_connect (G_OBJECT (eventbox_mir), "button_release_event",
                                    G_CALLBACK (mirsoft_button_released_callback), (GtkWidget *)image_mirsoft);


    g_signal_connect (G_OBJECT(g_window), "draw", G_CALLBACK (on_draw_event), NULL);
    g_signal_connect (G_OBJECT(g_window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    LogInfo("debug: SY_topwindow_main  2222222.\n");
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
}
