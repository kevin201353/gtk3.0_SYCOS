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

#define WINDOW_MAX_WIDTH     1920
#define WINDOW_MAX_HEIGHT     1080

#define MAX_BUFF_SIZE      100

extern cairo_surface_t *surface1 ;
#define IMAGE_BTN_LOGIN_NOR    "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES   "images2/btnlogin_nor.png"
GdkPixbuf *g_loginPress;
GdkPixbuf *g_loginNor;

typedef struct _TAG_WINBUILD
{
    GtkBuilder *builder;
    GObject  *gobject;
}_tagWinbuild;

_tagWinbuild * g_pWin = NULL;

/*gboolean on_drawcombobox_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    g_print("on_draw_event start.\n");
    LogInfo("debug: on_draw_event start.\n");
    GdkWindow *window;
    cairo_t *cr = NULL;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
    cairo_set_source_rgb(cr, 0.0,0.0,0.0);
    //cairo_set_source_surface (cr, surface_topwindow, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}*/

/*void login_event (GtkWidget *widget,
             gpointer   data)
{
    g_print ("it is button. \n");
    _tagWinbuild *  pWin = (_tagWinbuild *)data;
    Get_ctrldata(pWin->builder, pWin->gobject);
}*/
static gboolean sh_button_press_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        printf("sh gtkimage check mouser button pressed.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_loginPress);
    }
    return TRUE;
}

static gboolean sh_button_released_callback (GtkWidget  *event_box,
                       GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_RELEASE)
    {
        printf("sh gtkimage check mouser button released.\n");
        gtk_image_set_from_pixbuf(GTK_IMAGE(data), g_loginNor);
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
gboolean on_expose_event (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    g_print("on_expose_event start.\n");
    LogInfo("debug: on_expose_event start.\n");
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

void Get_ctrldata(GtkBuilder *builder, GObject *window)
{
    GObject *object;
    int len = 0;
    printf("### builder :%d, window :%d.\n", (int)builder, (int)window);
    LogInfo("debug: ### builder :%d, window :%d.\n" , (int)builder, (int)window);
    object = gtk_builder_get_object (builder, "comboboxtext-entry");
    len = gtk_entry_get_text_length((GtkEntry *)object);
    if (len == 0)
    {
        g_print("username don't null.\n");
        LogInfo("debug: username don't null.\n");
        return;
    }
    char * szUsername = gtk_entry_get_text((GtkEntry *)object);
    printf("username: %s.\n", szUsername);
    LogInfo("debug: username: %s.\n" ,szUsername);

    object = gtk_builder_get_object (builder, "pwdentry");
    len = gtk_entry_get_text_length((GtkEntry *)object);
    if (len == 0)
    {
        g_print("password don't null.\n");
        LogInfo("debug: password don't null.\n");
        return;
    }
    char * szPassword = gtk_entry_get_text((GtkEntry *)object);
    printf("password: %s.\n", szPassword);
    LogInfo("debug: password: %s.\n" ,szPassword);
}

void SY_loginwindow_main()
{
    GObject *window;
    GObject *button;
    GtkBuilder *builder;
    GObject *label;
    GObject *comboboxpro;

    builder = gtk_builder_new ();

    gtk_builder_add_from_file (builder, "loginwindow3.glade", NULL);

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object (builder, "window_login");
    //gtk_window_resize(GTK_WINDOW(window), WINDOW_MAX_WIDTH, WINDOW_MAX_HEIGHT);
    gtk_widget_set_app_paintable((GtkWidget * )window, TRUE);
    //button = gtk_builder_get_object (builder, "loginbutton");
    g_signal_connect (G_OBJECT(window), "draw", G_CALLBACK (on_expose_event), NULL);

    g_loginPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_PRES, NULL);
    g_loginNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_NOR, NULL);

    if (g_pWin == NULL)
    {
        g_pWin = malloc(sizeof(struct _TAG_WINBUILD));
    }
    g_pWin->builder = builder;
    g_pWin->gobject = window;

    label = gtk_builder_get_object (builder, "label_pro");
    gtk_label_set_text(GTK_LABEL(label), "选择协议");
    set_widget_font_size(GTK_LABEL(label), 18, FALSE);

    label = gtk_builder_get_object (builder, "label_user");
    gtk_label_set_text(GTK_LABEL(label), "用户名");
    set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "label_pass");
    gtk_label_set_text(GTK_LABEL(label), "密码");
    set_widget_font_size(GTK_LABEL(label), 14, FALSE);

    label = gtk_builder_get_object (builder, "btncheck_pass");
    gtk_button_set_label((GtkButton *)label, "记住密码");
    set_widget_font_size((GtkButton *)label, 13, TRUE);

    label = gtk_builder_get_object (builder, "btncheck_autologin");
    gtk_button_set_label((GtkButton *)label, "自动登录");
    set_widget_font_size((GtkButton *)label, 13, TRUE);

    comboboxpro = gtk_builder_get_object (builder, "combobox_pro");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 0, "SPICE");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxpro, 1, "RDP");
    gtk_combo_box_set_active((GtkComboBox *)comboboxpro, 0);

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

    //SetFontText((GtkWidget *)label, "选择协议");
    //g_signal_connect (button, "clicked", G_CALLBACK (login_event), (gpointer)g_pWin);
    //自动关联已在UI文件配置好的信号
    //gtk_builder_connect_signals (builder, NULL);
    //手动为主窗口添加关联退出事件
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
}
