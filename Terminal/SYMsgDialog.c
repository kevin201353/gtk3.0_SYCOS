#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <unistd.h>

static const gchar*  g_symsgcss = "systhrdmg.css";
static GtkBuilder *builder;
static GObject  *window;
static int showSyMsgDlg = 0;
GtkLabel *labeltext;

/*static pthread_t tid;
static void *thrdfunc(void *arg)
{
     SYMsgDialog();
}*/
void startSyMsgThrd()
{
  /*  if ( pthread_create(&tid, NULL, thrdfunc, NULL) !=0 )
    {
        printf("Create startSyMsgThrd error!\n");
    }*/
}
extern int SYMsgFun(char *p);
extern void SYmsgCaller(CallBackFun fun, char *p);
void DestorySYmsgDlg();
void SetSymsgContext(char* msg);
static gboolean close_button_clicked(GtkButton *button,  gpointer user_data)
{
    gtk_widget_destroy((GtkWidget *)window);
    gtk_main_quit();
}

static gboolean OK_button_clicked(GtkButton *button,  gpointer user_data)
{
    system("sudo poweroff");
    gtk_widget_destroy((GtkWidget *)window);
    gtk_main_quit();
}

static gboolean Cancel_button_clicked(GtkButton *button,  gpointer user_data)
{
    gtk_widget_destroy((GtkWidget *)window);
    gtk_main_quit();
}

int SYMsgFun(char *p)
{
    SetSymsgContext(p);
    LogInfo("DFDFDFDFDDFD  SYMsgFun, p = %s.\n", p);
    if (strcmp(p, "登录完成!") == 0 || strcmp(p, "网络配置完成！") == 0)
    {
        gtk_widget_destroy((GtkWidget *)window);
        gtk_main_quit();
        LogInfo("DestorySYmsgDlg  exit.\n");
    }
    return 0;
}

void SYmsgCaller(CallBackFun fun, char *p)
{
   fun(p);
}

void SetSymsgContext(char* msg)
{
  //  GObject * label_thrdtext;
  //  label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    gtk_label_set_text(labeltext, msg);
    LogInfo("DFDFDFDFDDFD  SYMsgFun, p = %s.\n", msg);
    if (strcmp(msg, "登录完成!") == 0 || strcmp(msg, "网络配置完成！") == 0)
    {
        showSyMsgDlg = 0;
        gtk_widget_destroy((GtkWidget *)window);
        gtk_main_quit();
        LogInfo("DestorySYmsgDlg  exit.\n");
    }
}

//nfalg 0:sh   1:mirsoft  2: cit  3:vm  4：配置网络 7：其他，不带有是和否的提示消息， 11： 带有是和否的提示消息,这里
void SYMsgDialog(int nflag, char *msg)
{
    if (showSyMsgDlg == 1)
        return;
    showSyMsgDlg = 1;
    builder = gtk_builder_new();
    GError *error1 = NULL;
    gtk_builder_add_from_file (builder, "SYthrdMsgDlg.glade", &error1);
    window = gtk_builder_get_object(builder, "window_symsg");
    GObject * label_thrdtext;
    label_thrdtext = gtk_builder_get_object(builder, "label_thrdtext");
    labeltext = (GtkLabel *)label_thrdtext;
    if (nflag == 7 || nflag == 11 || nflag == 4)
      gtk_label_set_text(GTK_LABEL(label_thrdtext), msg);
    else if (nflag == 0)
    {
        gtk_label_set_text(GTK_LABEL(label_thrdtext), "正在连接，请稍后 ... ");
    }
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
                                  g_filename_to_utf8(g_symsgcss, strlen(g_symsgcss), &bytes_read, &bytes_written, &error),
                                  NULL);
    //gtk_css_provider_load_from_path (provider, home, NULL);
    g_object_unref (provider);
    /* --------------------------------------------------------------------------------------------------------------------*/
    GObject * btnsymsg_close;
    btnsymsg_close = gtk_builder_get_object(builder, "btnsymsg_close");
    g_signal_connect(GTK_BUTTON(btnsymsg_close), "clicked", G_CALLBACK(close_button_clicked), NULL);
    g_signal_connect (G_OBJECT(window), "destroy",
                       G_CALLBACK(gtk_main_quit), NULL);
    if (nflag == 0 || nflag == 1)
        ShenCloud_login();
    //if (nflag == 4)
    //    SaveSysNetwork();
    //ok-cancel btn
    GObject * btn_OK;
    GObject * btn_Cancel;
    btn_OK = gtk_builder_get_object(builder, "btn_OK");
    btn_Cancel = gtk_builder_get_object(builder, "btn_Cancel");
    gtk_button_set_label(GTK_BUTTON(btn_OK), "是");
    gtk_button_set_label(GTK_BUTTON(btn_Cancel), "否");
    if (nflag == 11)
    {
        gtk_widget_set_visible(GTK_WIDGET(btn_OK), TRUE);
        gtk_widget_set_visible(GTK_WIDGET(btn_Cancel), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_OK), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_Cancel), TRUE);
    }else
    {
        gtk_widget_set_visible(GTK_WIDGET(btn_OK), FALSE);
        gtk_widget_set_visible(GTK_WIDGET(btn_Cancel), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_OK), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(btn_Cancel), FALSE);
    }
    g_signal_connect(GTK_BUTTON(btn_OK), "clicked", G_CALLBACK(OK_button_clicked), NULL);
    g_signal_connect(GTK_BUTTON(btn_Cancel), "clicked", G_CALLBACK(Cancel_button_clicked), NULL);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    showSyMsgDlg = 0;
}

void DestorySYmsgDlg()
{
    gtk_widget_destroy((GtkWidget *)window);
    gtk_main_quit();
    LogInfo("DestorySYmsgDlg  exit.\n");
}
