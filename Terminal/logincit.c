#include "global.h"
#include "interface.h"

#define IMAGE_BTN_LOGIN_NOR      "images2/btnlogin_nor.png"
#define IMAGE_BTN_LOGIN_PRES     "images2/btnlogin_press.png"
#define IMAGE_CIT_BACKGROUD      "images2/cit_login.png"  //登录界面背景图路径
#define IMAGE_BTN_CITPREV_NOR    "images2/loginout.png"
#define IMAGE_BTN_CITPREV_PRES   "images2/loginout_press.png"
#define IMAGE_LOGO_CIT           "images2/cit_logo.png"

static char *g_cit_css = "citlogin.css";
static GObject *g_window = NULL;
extern GdkPixbuf *g_shutdownPress;
extern GdkPixbuf *g_shutdownNor;
GdkPixbuf *g_loginPress;
GdkPixbuf *g_loginNor;
GdkPixbuf *g_prevPress;
GdkPixbuf *g_prevNor;
GdkPixbuf *g_logoCit;
static cairo_surface_t *surface ;
static GtkBuilder *g_builder;
extern void MsgDailog(char * sMsg);
int cit_login()
{
     GObject *http;
     if (g_builder == NULL)
     {
         LogInfo("debug: cit_login  g_builder == NULL.\n");
         return -1;
     }
     http = gtk_builder_get_object (g_builder, "entry_http");
     char * ip = gtk_entry_get_text(GTK_ENTRY(http));
     LogInfo("debug: cit_login http: %s.\n", ip);
     return Run_Citrix(ip);
}

//为登录主窗口绘制背景图
static gboolean on_expose_loginevent (GtkWidget * widget,
                 GdkEventExpose * event, gpointer data)
{
    GdkWindow *window;
    cairo_t *cr;
    window = gtk_widget_get_window(widget); //gtk3中GtkWidget中已经没有window成员了
    cr = gdk_cairo_create(window);
    cairo_set_source_surface (cr, surface, 0, 0);
    cairo_paint (cr);
    cairo_destroy (cr);
    return FALSE;
}

static gboolean gtk_main_quit_login(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
      gtk_main_quit();
      return TRUE;
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

static void  on_btn_prev_pressed(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevPress);
}

static void  on_btn_prev_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_prevNor);
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
}


static void  on_btn_login_pressed(GtkButton *button,  gpointer   user_data)
{
     gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginPress);
}

static void  on_btn_login_released(GtkButton *button,  gpointer   user_data)
{
   gtk_image_set_from_pixbuf(GTK_IMAGE(user_data), g_loginNor);
   gtk_widget_hide((GtkWidget *)g_window);
   if (cit_login() < 0)
   {
        MsgDailog("lognin failed.");
        return;
   }
   gtk_widget_destroy((GtkWidget *)g_window);
   gtk_main_quit();
}

void SY_logincit_main()
{
    GtkBuilder *builder;
    GObject *window;

    builder = gtk_builder_new ();
    g_builder = builder;
    GError *errort = NULL;
    gtk_builder_add_from_file (builder, "logincit.glade", &errort);
    g_window = gtk_builder_get_object (builder, "window_cit");

    g_loginPress = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_PRES, NULL);
    g_loginNor = gdk_pixbuf_new_from_file(IMAGE_BTN_LOGIN_NOR, NULL);
    surface = cairo_image_surface_create_from_png (IMAGE_CIT_BACKGROUD);
    g_prevPress = gdk_pixbuf_new_from_file(IMAGE_BTN_CITPREV_PRES, NULL);
    g_prevNor = gdk_pixbuf_new_from_file(IMAGE_BTN_CITPREV_NOR, NULL);

    GObject *image_logo;
    image_logo = gtk_builder_get_object (builder, "image_logo");
    g_logoCit = gdk_pixbuf_new_from_file(IMAGE_LOGO_CIT, NULL);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image_logo), g_logoCit);

    GObject *label = gtk_builder_get_object (builder, "label_http");
    gtk_label_set_text(GTK_LABEL(label), "URL");

    gtk_widget_set_app_paintable((GtkWidget * )g_window, TRUE);
    //button = gtk_builder_get_object (builder, "loginbutton");
    g_signal_connect (G_OBJECT(g_window), "draw", G_CALLBACK (on_expose_loginevent), NULL);
    g_signal_connect (G_OBJECT(g_window), "destroy",
                       G_CALLBACK(gtk_main_quit_login), NULL);
   GObject *image_shutdown;
   GObject *eventbox_shutdown;
   image_shutdown = gtk_builder_get_object (builder, "image_shutdown");
   eventbox_shutdown = gtk_builder_get_object (builder, "eventbox_shutdown");
   gtk_widget_set_events((GtkWidget *)eventbox_shutdown, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件
   g_signal_connect (G_OBJECT (eventbox_shutdown), "button_press_event",
                   G_CALLBACK (shutdown_button_press_callback), (GtkWidget *)image_shutdown);
   g_signal_connect (G_OBJECT (eventbox_shutdown), "button_release_event",
                                   G_CALLBACK (shutdown_button_released_callback), (GtkWidget *)image_shutdown);
   //登录
   GObject *image_loginout;
   GObject *login_image;
   image_loginout = gtk_builder_get_object (builder, "image_loginout");
   login_image = gtk_builder_get_object (builder, "image_login");
   GObject *btn_citlogin;
   GObject *btn_citPrev;
   btn_citPrev = gtk_builder_get_object (builder, "btn_citPrev");
   btn_citlogin = gtk_builder_get_object (builder, "btn_citlogin");
   g_signal_connect(G_OBJECT(btn_citPrev), "pressed", G_CALLBACK(on_btn_prev_pressed), (GtkWidget *)image_loginout);
   g_signal_connect(G_OBJECT(btn_citPrev), "released", G_CALLBACK(on_btn_prev_released), (GtkWidget *)image_loginout);

   g_signal_connect(G_OBJECT(btn_citlogin), "pressed", G_CALLBACK(on_btn_login_pressed), (GtkWidget *)login_image);
   g_signal_connect(G_OBJECT(btn_citlogin), "released", G_CALLBACK(on_btn_login_released), (GtkWidget *)login_image);
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
                                 g_filename_to_utf8(g_cit_css, strlen(g_cit_css), &bytes_read, &bytes_written, &error),
                                 NULL);
	  //gtk_css_provider_load_from_path (provider, home, NULL);
	  g_object_unref (provider);
	  /* --------------------------------------------------------------------------------------------------------------------*/
    gtk_window_fullscreen(GTK_WINDOW(g_window));
    gtk_window_set_default_size(GTK_WINDOW(g_window), g_screen_width, g_screen_height);
    gtk_window_set_decorated(GTK_WINDOW(g_window), FALSE); /* hide the title bar and the boder */

   gtk_main ();
   g_object_unref (G_OBJECT (builder));
   cairo_surface_destroy (surface);
}
