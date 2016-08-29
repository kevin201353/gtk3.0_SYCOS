#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <math.h>
#include "buildtime.h"

#define BUF_MAX_SZIE   1024

#define IMAGE_MAIN_BACKGROUD          "images2/login.png"  //登录界面背景图路径
#define IMAGE_TOPWINDOW_BACKGROUD     "images2/topwindow.png"
#define IMAGE_VMLISTWINDOW_BACKGROUD  "images2/vmlistwindow.png"

#define IMAGE_1680X1050_BACKGROUD     "images2/1680x1050_topwindow.png"

cairo_surface_t *surface1 = NULL;
cairo_surface_t *surface_topwindow = NULL;
cairo_surface_t *surface_vmlistwindow = NULL;
//cairo_surface_t *surface_1680x1050_topwindow = NULL;
extern short g_loginExit = 0;
static GtkWidget *Msgdialog;
extern void MsgShutDownDailog(char * sMsg);
void MsgShutDownDailog(char * sMsg)
{
    Msgdialog = gtk_message_dialog_new (NULL,
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_WARNING,
                                     GTK_BUTTONS_YES_NO,
                                     "%s",
                                     sMsg);
    int nRet = gtk_dialog_run (GTK_DIALOG (Msgdialog));
    switch (nRet)
    {
      case GTK_RESPONSE_YES:
         gtk_main_quit();
         system("shutdown -t 10 -h now \"System will shutdown 10 sencond later\" ");
         break;
      default:
         break;
    }
    gtk_widget_destroy(Msgdialog);
}
extern void MsgDailog(char * sMsg);
void MsgDailog(char * sMsg)
{
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new (NULL,
                                     flags,
                                     GTK_MESSAGE_ERROR,
                                     GTK_BUTTONS_CLOSE,
                                     "消息 : %s",
                                     sMsg);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}

void create_surfaces()
{
    LogInfo("debug: create_surfaces start load png.\n");
    surface1 = cairo_image_surface_create_from_png (IMAGE_MAIN_BACKGROUD);
    if (surface1 == NULL)
    {
        LogInfo("Error: create_surfaces load loginwindow png failed.\n");
    }
    surface_topwindow = cairo_image_surface_create_from_png (IMAGE_TOPWINDOW_BACKGROUD);
    if (surface_topwindow == NULL)
    {
        LogInfo("Error: create_surfaces load topwindow png failed.\n");
    }

    surface_vmlistwindow = cairo_image_surface_create_from_png (IMAGE_VMLISTWINDOW_BACKGROUD);
    if (surface_vmlistwindow == NULL)
    {
        LogInfo("Error: create_surfaces load vmlistwindow png failed.\n");
    }

    //surface_1680x1050_topwindow = cairo_image_surface_create_from_png(IMAGE_1680X1050_BACKGROUD);
    LogInfo("debug: create_surfaces load png end.\n");
}

void destroy_surfaces()
{
    g_print ("destroying surfaces");
    LogInfo("debug: destroying surfaces.\n");
    cairo_surface_destroy (surface1);
    cairo_surface_destroy (surface_topwindow);
    cairo_surface_destroy (surface_vmlistwindow);
    //cairo_surface_destroy (surface_1680x1050_topwindow);
}

int main(int argc, char *argv[])
{
    system("sudo chmod +x tmp.sh");
    system("sudo ./tmp.sh");
    system("sudo chmod +x netstatic.sh");
    system("sudo chmod +x dhcp.sh");
    system("sudo chmod +x netget.sh");
    system("sudo xrandr > resol.txt");
    //print version info
    LogInfo("Debug: shencloud version : %s .\n", buildtime);
    Init_Curlc();
    g_loginExit = 0;
    gtk_init (&argc, &argv);
    create_surfaces();
    SY_topwindow_main();
    printf("main topwindow end.\n");
    destroy_surfaces();
    SY_FreeVmsList();
    Unit_Curlc();
    system("rm -rf resol.txt");
    return 0;
}
