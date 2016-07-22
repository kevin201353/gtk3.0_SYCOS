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

static GObject *g_window = NULL;
//extern void create_surfaces();
//extern void destroy_surfaces();

extern cairo_surface_t *surface_topwindow;

GdkPixbuf *g_shPress;
GdkPixbuf *g_shNor;

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
    GObject *window;
    GtkBuilder *builder;
    builder = gtk_builder_new ();
    //load png
    g_shPress = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_PRES, NULL);
    g_shNor = gdk_pixbuf_new_from_file(IMAGE_BTN_SH_NOR, NULL);
    //
    gtk_builder_add_from_file (builder, "topwindow.glade", NULL);
    g_window = gtk_builder_get_object (builder, "window1");
    gtk_widget_set_app_paintable((GtkWidget * )g_window, TRUE);

    GObject *eventbox_sh;
    eventbox_sh = gtk_builder_get_object (builder, "eventbox_sh");
    gtk_widget_set_events((GtkWidget *)eventbox_sh, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件
    GObject *sh_image;
    sh_image = gtk_builder_get_object (builder, "image_sh");
    g_signal_connect (G_OBJECT (eventbox_sh), "button_press_event",
                    G_CALLBACK (sh_button_press_callback), (GtkWidget *)sh_image);
    g_signal_connect (G_OBJECT (eventbox_sh), "button_release_event",
                                    G_CALLBACK (sh_button_released_callback), (GtkWidget *)sh_image);


    g_signal_connect (G_OBJECT(g_window), "draw", G_CALLBACK (on_draw_event), NULL);
    g_signal_connect (G_OBJECT(g_window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
}
