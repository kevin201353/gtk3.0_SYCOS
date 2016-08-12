#include <gtk/gtk.h>
#include "global.h"
#include <string.h>

static GtkBuilder *g_builder;

#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtn_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtn_press.png"

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;
static unsigned short g_checkrepass = 0;
static unsigned short g_bnetstatic = 0;
struct NetStaticInfo{
     char szIP[MAX_BUFF_SIZE];
     char szNetMask[MAX_BUFF_SIZE];
     char szGatWay[MAX_BUFF_SIZE];
     char szDns[MAX_BUFF_SIZE];
};

static struct NetStaticInfo g_NetStaticInfo = {0,0,0,0};

static GtkWidget *Msgdialog;
void GetNetStaticInfo();
void Msg2Dailog(char * sMsg)
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
         system("reboot");
         break;
      default:
         break;
    }
    gtk_widget_destroy(Msgdialog);
}

GObject * GetNetWorkSetLayout()
{
     return gtk_builder_get_object(g_builder, "layout2");
}

int SetDhcpNet()
{
    system("dhcp.sh");
    return 0;
}

int SetStaticNet()
{
    GetNetStaticInfo();
    if (strcmp(g_NetStaticInfo.szIP, "") == 0 || strcmp(g_NetStaticInfo.szNetMask, "") == 0 ||
        strcmp(g_NetStaticInfo.szGatWay, "") == 0 || strcmp(g_NetStaticInfo.szDns, "") == 0)
        return -1;
    char szCmd[MAX_BUFF_SIZE] = {0};
    sprintf(szCmd, "netstatic.sh %s %s %s %s", g_NetStaticInfo.szIP, g_NetStaticInfo.szNetMask, g_NetStaticInfo.szGatWay, g_NetStaticInfo.szDns);
    system(szCmd);
    return 0;
}

void savenetwork_button_clicked(GtkButton *button,  gpointer user_data)
{
    if (g_bnetstatic == 1)
    {
        SetStaticNet();
    }
    else
        SetDhcpNet();
}

void GetNetStaticInfo()
{
    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    GObject *entry_netdns;

    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    strcpy(g_NetStaticInfo.szIP, gtk_entry_get_text((GtkEntry *)entry_netip));
    strcpy(g_NetStaticInfo.szNetMask, gtk_entry_get_text((GtkEntry *)entry_netmask));
    strcpy(g_NetStaticInfo.szGatWay, gtk_entry_get_text((GtkEntry *)entry_netgatway));
    strcpy(g_NetStaticInfo.szDns, gtk_entry_get_text((GtkEntry *)entry_netdns));
}

void SetNetCtrlSensitive(unsigned short value)
{
    GObject *label_netip;
    GObject *label_netmask;
    GObject *label_netgatway;
    GObject *label_netdns;

    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    GObject *entry_netdns;

    label_netip = gtk_builder_get_object (g_builder, "label_netip");
    label_netmask = gtk_builder_get_object (g_builder, "label_netmask");
    label_netgatway = gtk_builder_get_object (g_builder, "label_netgatway");
    label_netdns = gtk_builder_get_object (g_builder, "label_netdns");
    gtk_widget_set_sensitive(GTK_LABEL(label_netip), value);
    gtk_widget_set_sensitive(GTK_LABEL(label_netmask), value);
    gtk_widget_set_sensitive(GTK_LABEL(label_netgatway), value);
    gtk_widget_set_sensitive(GTK_LABEL(label_netdns), value);
    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    gtk_widget_set_sensitive(GTK_ENTRY(entry_netip), value);
    gtk_widget_set_sensitive(GTK_ENTRY(entry_netmask), value);
    gtk_widget_set_sensitive(GTK_ENTRY(entry_netgatway), value);
    gtk_widget_set_sensitive(GTK_ENTRY(entry_netdns), value);
}

void rabtn_static_button_callback(GtkWidget *check_button, gpointer data)
{
    if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button)))
    {
        printf("check button selected\n");
        //到这里，是选这了Radio 静态IP地址
        SetNetCtrlSensitive(1);
        g_bnetstatic = 1;
    }
    else
    {
        printf("check button unselected\n");
        SetNetCtrlSensitive(0);
        g_bnetstatic = 0;
    }
}

static gboolean checkbutton_wlan_press_callback(GtkWidget *event_box, GdkEventButton *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS)
    {
        g_checkrepass = !g_checkrepass;
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

int SY_NetworkSet_main()
{
    g_builder = gtk_builder_new();
    gtk_builder_add_from_file (g_builder, "network.glade", NULL);

    GObject *label_wan;
    GObject *label_hotspot;  //无线热点
    GObject *label_netinter;
    GObject *label_netip;
    GObject *label_netmask;
    GObject *label_netgatway;
    GObject *label_netdns;

    GObject *btn_connect;
    GObject *rabtn_static;  //radio button 静态IP
    GObject *rabtn_dhcp;    //dhcp ip
    GObject *btn_advanced;
    GObject *btn_savenetwork;

    label_wan = gtk_builder_get_object (g_builder, "label_wan");
    label_hotspot = gtk_builder_get_object (g_builder, "label_hotspot");
    label_netinter = gtk_builder_get_object (g_builder, "label_netinter");
    label_netip = gtk_builder_get_object (g_builder, "label_netip");
    label_netmask = gtk_builder_get_object (g_builder, "label_netmask");
    label_netgatway = gtk_builder_get_object (g_builder, "label_netgatway");
    label_netdns = gtk_builder_get_object (g_builder, "label_netdns");

    btn_connect = gtk_builder_get_object (g_builder, "btn_connect");
    rabtn_static = gtk_builder_get_object (g_builder, "rabtn_static");
    rabtn_dhcp = gtk_builder_get_object (g_builder, "rabtn_dhcp");
    btn_advanced = gtk_builder_get_object (g_builder, "btn_advanced");
    btn_savenetwork = gtk_builder_get_object (g_builder, "btn_savenetwork");

    gtk_label_set_text(GTK_LABEL(label_wan), "WLAN");
    gtk_label_set_text(GTK_LABEL(label_hotspot), "无线热点");
    gtk_label_set_text(GTK_LABEL(label_netinter), "网络接口");
    gtk_label_set_text(GTK_LABEL(label_netip), "IP地址");
    gtk_label_set_text(GTK_LABEL(label_netmask), "子网掩码");
    gtk_label_set_text(GTK_LABEL(label_netgatway), "默认网关");
    gtk_label_set_text(GTK_LABEL(label_netdns), "DNS服务器");

    gtk_button_set_label((GtkButton *)btn_connect, "连接");
    gtk_button_set_label((GtkButton *)rabtn_static, "使用静态IP地址");
    gtk_button_set_label((GtkButton *)rabtn_dhcp, "使用DHCP自动获取");
    gtk_button_set_label((GtkButton *)btn_advanced, "高级");
    gtk_button_set_label((GtkButton *)btn_savenetwork, "保存");

    GObject * comboboxtext_netinter = gtk_builder_get_object (g_builder, "comboboxtext_netinter");
    gtk_combo_box_text_insert_text((GtkComboBoxText *)comboboxtext_netinter, 0, "eth0");
    gtk_combo_box_set_active((GtkComboBox *)comboboxtext_netinter, 0);

    g_checkNorimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_NOR, NULL);
    g_checkPressimage = gdk_pixbuf_new_from_file(IMAGE_CHECKBUTTON_PRESS, NULL);

    GObject *eventbox_wlan;
    eventbox_wlan = gtk_builder_get_object (g_builder, "eventbox_wlan");
    gtk_widget_set_events((GtkWidget *)eventbox_wlan, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK); // 捕获鼠标点击事件

    GObject *image_wlan;
    image_wlan = gtk_builder_get_object (g_builder, "image_wlan");
    g_signal_connect (G_OBJECT (eventbox_wlan), "button_press_event",
                    G_CALLBACK (checkbutton_wlan_press_callback), (GtkWidget *)image_wlan);

    g_signal_connect(G_OBJECT(rabtn_static), "toggled", G_CALLBACK(rabtn_static_button_callback), NULL);
    g_signal_connect(G_OBJECT(btn_savenetwork), "clicked", G_CALLBACK(savenetwork_button_clicked), NULL);
    SetNetCtrlSensitive(0);
    gtk_widget_set_sensitive(GTK_BUTTON(btn_advanced), 0);
    g_bnetstatic = 0;
}
