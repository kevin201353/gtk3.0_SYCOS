#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include <math.h>

#define BUF_MAX_SZIE   1024

#define IMAGE_MAIN_BACKGROUD   "images2/login.png"  //登录界面背景图路径
#define IMAGE_TOPWINDOW_BACKGROUD   "images2/topwindow.png"
#define IMAGE_VMLISTWINDOW_BACKGROUD  "images2/vmlistwindow.png"

cairo_surface_t *surface1 = NULL;
cairo_surface_t *surface_topwindow = NULL;
cairo_surface_t *surface_vmlistwindow = NULL;

extern short g_loginExit = 0;
extern void MsgDailog(char * sMsg);

void MsgDailog(char * sMsg)
{
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new (NULL,
                                     flags,
                                     GTK_MESSAGE_ERROR,
                                     GTK_BUTTONS_CLOSE,
                                     "Error reading : %s",
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
}

void destroy_surfaces()
{
    g_print ("destroying surfaces");
    LogInfo("debug: destroying surfaces.\n");
    cairo_surface_destroy (surface1);
    cairo_surface_destroy (surface_topwindow);
    cairo_surface_destroy (surface_vmlistwindow);
}

int main(int argc, char *argv[])
{
    //system("chmod +x tmp.sh");
    //system("./tmp.sh");
    Init_Curlc();
    g_loginExit = 0;
    //deal gui
    gtk_init (&argc, &argv);
    create_surfaces();
    SY_topwindow_main();
    printf("main topwindow end.\n");
    //SY_loginwindow_main();
  /*  printf("main @@@@@@@@@@@ \n");
    if (g_loginExit == 0)
    {
        SY_vmlistwindow_main();
        printf("main vmlist window. \n");
    }*/
    destroy_surfaces();
    SY_FreeVmsList();
    Unit_Curlc();
    return 0;
}
