#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "global.h"
#include <string.h>

static GtkBuilder *g_builder;

#define IMAGE_CHECKBUTTON_NOR  "images2/checkbtnset_nor.png"
#define IMAGE_CHECKBUTTON_PRESS  "images2/checkbtnset_press.png"

GdkPixbuf *g_checkNorimage;
GdkPixbuf *g_checkPressimage;
static unsigned short g_checkrepass = 0;
static unsigned short g_bnetstatic = 0;
struct NetStaticInfo{
     char szIP[MAX_BUFF_SIZE];
     char szNetMask[MAX_BUFF_SIZE];
     char szGatWay[MAX_BUFF_SIZE];
     char szDns[3][MAX_BUFF_SIZE];
};
//CallBackFun
extern void SYmsgCaller(CallBackFun fun, char *p);
extern int SYMsgFun(char *p);
//

static struct NetStaticInfo g_NetStaticInfo = {0,0,0,{0,0,0}};

static GtkWidget *Msgdialog;
void GetNetStaticInfo();
//字符串去空格
extern char *rtspace(char *str);
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
     return gtk_builder_get_object(g_builder, "layout_network");
}

//还回值 ， 1： 找到   0: 没找到
int findstr(char * szfile, char * str)
{
    system("sudo cat /etc/network/interfaces > net_tmp.txt");
    FILE* fp=fopen("net_tmp.txt", "r");
    if (fp == NULL)
       return -1;
    char buf[MAX_BUFF_SIZE] = {0};
    while(fgets(buf, MAX_BUFF_SIZE, fp) != NULL)
    {
        char * p = strstr(buf, str);//p为dhcp的出现位置,NULL则为没找到
        if (p)
        {
            LogInfo("Debug: Network findstr find.\n");
            fclose(fp);
            return 1; //找到， 网络为dhcp
        }
    }
    fclose(fp);
    system("sudo rm -rf net_tmp.txt");
    return 0;
}

int GetStaticNet()
{
    int nRet = system("sudo ./netget.sh");
    if (nRet < 0)
        return -1;

    FILE* fp=fopen("nettmp.txt", "r");
    if (fp == NULL)
       return -1;

    memset(g_NetStaticInfo.szIP, 0, MAX_BUFF_SIZE);
    memset(g_NetStaticInfo.szNetMask, 0, MAX_BUFF_SIZE);
    memset(g_NetStaticInfo.szGatWay, 0, MAX_BUFF_SIZE);

    char szTmp[MAX_BUFF_SIZE] = {0};
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szIP, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szNetMask, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    fgets(szTmp, MAX_BUFF_SIZE, fp);
    szTmp[strlen(szTmp) - 1] = '\0';
    strcpy(g_NetStaticInfo.szGatWay, rtspace(szTmp));
    memset(szTmp, 0, MAX_BUFF_SIZE);
    LogInfo("Debug: Network info Get static net ip :%s.\n", g_NetStaticInfo.szIP);
    LogInfo("Debug: Network info Get static net mask :%s.\n", g_NetStaticInfo.szNetMask);
    LogInfo("Debug: Network info Get static net gatway :%s.\n", g_NetStaticInfo.szGatWay);
    //LogInfo("Debug: Network info Get static net dns :%s.\n", g_NetStaticInfo.szDns);
    fclose(fp);
    system("rm -rf nettmp.txt");
    return 0;
}

int isDhcp()
{
    return findstr("net_tmp.txt", "dhcp");
}

int isStatic()
{
    return findstr("net_tmp.txt", "static");
}

int SetDhcpNet()
{
    system("sudo ./dhcp.sh");
    return 0;
}

int SetDns()
{
    GObject *entry_netdns;
    GObject *entry_netdns2;
    GObject *entry_netdns3;
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    strcpy(g_NetStaticInfo.szDns[0], gtk_entry_get_text(GTK_ENTRY(entry_netdns)));
    entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
    strcpy(g_NetStaticInfo.szDns[1], gtk_entry_get_text(GTK_ENTRY(entry_netdns2)));
    entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
    strcpy(g_NetStaticInfo.szDns[2], gtk_entry_get_text(GTK_ENTRY(entry_netdns3)));

    if (strcmp(g_NetStaticInfo.szDns[0], "") >= 0 ||
        strcmp(g_NetStaticInfo.szDns[1], "") >= 0 ||
        strcmp(g_NetStaticInfo.szDns[2], "") >= 0)
    {
        char szTmp[MAX_BUFF_SIZE] = {0};
        sprintf(szTmp, "sudo ./dns.sh %s %s %s", g_NetStaticInfo.szDns[0], g_NetStaticInfo.szDns[1], g_NetStaticInfo.szDns[2]);
        LogInfo("Debug: set net dns cmd ---- %s .\n", szTmp);
        system(szTmp);
    }
    return 0;
}

int GetDns()
{
    LogInfo("Debug: get net dns, open get dns value  000000.\n");
    FILE* fp = fopen("/etc/resolv.conf", "r");
    LogInfo("Debug: get net dns, open get dns value 44444.\n");
    if (fp == NULL)
    {
       LogInfo("Debug: get net dns, open /etc/resolv.conf failed.\n");
       return -1;
    }
    char *delim = " ";
    char szTmp[MAX_BUFF_SIZE] = {0};
    int nCount = 0;
    while (fgets(szTmp, MAX_BUFF_SIZE, fp) != NULL) {
        LogInfo("Debug: get net dns, open get dns value  111111,  fgets string = %s, len = %d.\n", szTmp, strlen(szTmp));
        if (strcmp(szTmp, "") != 0)
        {
          LogInfo("Debug: get net dns, open get dns value  222222.\n");
          if (strlen(szTmp) <= 0)
              break;
          szTmp[strlen(szTmp) - 1] = '\0';
          LogInfo("Debug: get net dns, open get dns value  222222dddd.\n");
          if (strstr(szTmp, "#") == NULL)//p为dhcp的出现位置,NULL则为没找到
          {
              if (strstr(szTmp, "nameserver") != NULL && strlen(szTmp) > 12 )
              {
                if (nCount < 3)
                {
                  char *pt = strtok(szTmp, delim);
                  pt  = strtok(NULL, delim);
                  strcpy(szTmp, pt);
                  LogInfo("Debug: get net dns, open get dns value = %s.\n", szTmp);
                  strcpy(g_NetStaticInfo.szDns[nCount], rtspace(szTmp));
                  LogInfo("Debug: get net dns, netstatic info szDns = %s.\n", g_NetStaticInfo.szDns[nCount]);
                  nCount++;
                }
              }
          }
        }
    }
    fclose(fp);
    return 0;
}

int SetStaticNet()
{
    GetNetStaticInfo();
    if (strcmp(g_NetStaticInfo.szIP, "") == 0 || strcmp(g_NetStaticInfo.szNetMask, "") == 0 ||
        strcmp(g_NetStaticInfo.szGatWay, "") == 0)
        return -1;
    char szCmd[MAX_BUFF_SIZE] = {0};
    sprintf(szCmd, "sudo ./netstatic.sh %s %s %s", g_NetStaticInfo.szIP, g_NetStaticInfo.szNetMask, g_NetStaticInfo.szGatWay);
    LogInfo("Debug: set static net ---- %s .\n", szCmd);
    system(szCmd);
    return 0;
}

void initStaticNetctrl()
{
    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    GObject *entry_netdns;
    GObject *entry_netdns2;
    GObject *entry_netdns3;

    GObject *rabtn_static;  //radio button 静态IP
    GObject *rabtn_dhcp;    //dhcp ip
    rabtn_static = gtk_builder_get_object (g_builder, "rabtn_static");
    rabtn_dhcp = gtk_builder_get_object (g_builder, "rabtn_dhcp");
    if (isDhcp() == 1)
    {
        gtk_toggle_button_set_active(rabtn_static, FALSE);
        LogInfo("Debug: initStaticNetctrl isDhcp .\n");
    }

    if (isStatic() == 1)
    {
        gtk_toggle_button_set_active(rabtn_static, TRUE);
        entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
        entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
        entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");

        gtk_entry_set_text(GTK_ENTRY(entry_netip), g_NetStaticInfo.szIP);
        gtk_entry_set_text(GTK_ENTRY(entry_netmask), g_NetStaticInfo.szNetMask);
        gtk_entry_set_text(GTK_ENTRY(entry_netgatway), g_NetStaticInfo.szGatWay);
        LogInfo("Debug: initStaticNetctrl isStatic .\n");
    }
    GetDns();
    LogInfo("initStaticNetctrl  get dns, init control , dns1=%s, dns2=%s, dns3=%s.\n", g_NetStaticInfo.szDns[0], g_NetStaticInfo.szDns[1],g_NetStaticInfo.szDns[2]);
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    gtk_entry_set_text(GTK_ENTRY(entry_netdns), g_NetStaticInfo.szDns[0]);
    entry_netdns2 = gtk_builder_get_object (g_builder, "entry_netdns2");
    gtk_entry_set_text(GTK_ENTRY(entry_netdns2), g_NetStaticInfo.szDns[1]);
    entry_netdns3 = gtk_builder_get_object (g_builder, "entry_netdns3");
    gtk_entry_set_text(GTK_ENTRY(entry_netdns3), g_NetStaticInfo.szDns[2]);
}

void SaveSysNetwork()
{
   //if(vfork() == 0)
   {
      if (g_bnetstatic == 1)
      {
          SetStaticNet();
      }
      else
      {
          LogInfo("Debug: save set dhcp net 11 .\n");
          SetDhcpNet();
      }
      SetDns();
      LogInfo("网络配置完成！.\n");
      //SYmsgCaller(SYMsgFun, "网络配置完成！");
      SetSymsgContext("网络配置完成！");
    }
}
void savenetwork_button_clicked(GtkButton *button,  gpointer user_data)
{
    //if(vfork() == 0)
    {
       SYMsgDialog(4, "正在配置系统网络，请稍后 ... ");
       //SaveSysNetwork();
    }

}

void GetNetStaticInfo()
{
    GObject *entry_netip;
    GObject *entry_netmask;
    GObject *entry_netgatway;
    //GObject *entry_netdns;

    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    //entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    strcpy(g_NetStaticInfo.szIP, gtk_entry_get_text(GTK_ENTRY(entry_netip)));
    strcpy(g_NetStaticInfo.szNetMask, gtk_entry_get_text(GTK_ENTRY(entry_netmask)));
    strcpy(g_NetStaticInfo.szGatWay, gtk_entry_get_text(GTK_ENTRY(entry_netgatway)));
    //strcpy(g_NetStaticInfo.szDns, gtk_entry_get_text(GTK_ENTRY(entry_netdns)));
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
    gtk_widget_set_sensitive(GTK_WIDGET(label_netip), value);
    gtk_widget_set_sensitive(GTK_WIDGET(label_netmask), value);
    gtk_widget_set_sensitive(GTK_WIDGET(label_netgatway), value);
    gtk_widget_set_sensitive(GTK_WIDGET(label_netdns), value);
    entry_netip = gtk_builder_get_object (g_builder, "entry_netip");
    entry_netmask = gtk_builder_get_object (g_builder, "entry_netmask");
    entry_netgatway = gtk_builder_get_object (g_builder, "entry_netgatway");
    entry_netdns = gtk_builder_get_object (g_builder, "entry_netdns");
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netip), value);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netmask), value);
    gtk_widget_set_sensitive(GTK_WIDGET(entry_netgatway), value);
    //gtk_widget_set_sensitive(GTK_WIDGET(entry_netdns), value);
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

extern void SY_NetworkDisableCtrl();
void SY_NetworkDisableCtrl()
{
   GObject *btn_advanced;
   btn_advanced = gtk_builder_get_object (g_builder, "btn_advanced");
   gtk_widget_set_sensitive(GTK_WIDGET(btn_advanced), 0);
   gtk_widget_set_visible(GTK_WIDGET(btn_advanced), 0);
}

int SY_NetworkSet_main()
{
    g_builder = gtk_builder_new();
    GError *error = NULL;
    gtk_builder_add_from_file (g_builder, "network.glade", &error);

    GObject *label_wan;
    GObject *label_hotspot;  //无线热点
    GObject *label_netinter;
    GObject *label_netip;
    GObject *label_netmask;
    GObject *label_netgatway;
    GObject *label_netdns;
    GObject *label_netdns2;
    GObject *label_netdns3;

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
    label_netdns2 = gtk_builder_get_object (g_builder, "label_netdns2");
    label_netdns3 = gtk_builder_get_object (g_builder, "label_netdns3");

    btn_connect = gtk_builder_get_object (g_builder, "btn_connect");
    rabtn_static = gtk_builder_get_object (g_builder, "rabtn_static");
    rabtn_dhcp = gtk_builder_get_object (g_builder, "rabtn_dhcp");
    btn_advanced = gtk_builder_get_object (g_builder, "btn_advanced");
    btn_savenetwork = gtk_builder_get_object (g_builder, "btn_savenetwork");

    gtk_label_set_text(GTK_LABEL(label_wan), "WLAN");
    gtk_label_set_text(GTK_LABEL(label_hotspot), "无线热点");
    gtk_label_set_text(GTK_LABEL(label_netinter), "网络接口");
    gtk_label_set_text(GTK_LABEL(label_netip), " IP地址");
    gtk_label_set_text(GTK_LABEL(label_netmask), "子网掩码");
    gtk_label_set_text(GTK_LABEL(label_netgatway), "默认网关");
    gtk_label_set_text(GTK_LABEL(label_netdns), "DNS1服务器");
    gtk_label_set_text(GTK_LABEL(label_netdns2), "DNS2服务器");
    gtk_label_set_text(GTK_LABEL(label_netdns3), "DNS3服务器");
    gtk_label_set_justify(GTK_LABEL(label_netip), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netmask), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netgatway), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns2), GTK_JUSTIFY_RIGHT);
    gtk_label_set_justify(GTK_LABEL(label_netdns3), GTK_JUSTIFY_RIGHT);

    gtk_button_set_label(GTK_BUTTON(btn_connect), "连接");
    gtk_button_set_label(GTK_BUTTON(rabtn_static), "使用静态IP地址");
    gtk_button_set_label(GTK_BUTTON(rabtn_dhcp), "使用DHCP自动获取");
    gtk_button_set_label(GTK_BUTTON(btn_advanced), "高级");
    gtk_button_set_label(GTK_BUTTON(btn_savenetwork), "保存");

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
    gtk_image_set_from_pixbuf(GTK_IMAGE(image_wlan), g_checkNorimage);
    g_signal_connect (G_OBJECT (eventbox_wlan), "button_press_event",
                    G_CALLBACK (checkbutton_wlan_press_callback), (GtkWidget *)image_wlan);

    g_signal_connect(G_OBJECT(rabtn_static), "toggled", G_CALLBACK(rabtn_static_button_callback), NULL);
    g_signal_connect(G_OBJECT(btn_savenetwork), "clicked", G_CALLBACK(savenetwork_button_clicked), NULL);
    SetNetCtrlSensitive(0);
    //gtk_widget_set_sensitive(GTK_WIDGET(btn_advanced), 0);
    //gtk_widget_set_visible(GTK_WIDGET(btn_advanced), 0);
    GetStaticNet();
    initStaticNetctrl();
}
