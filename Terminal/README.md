SYCOS_9.0/Terminal :
//author:zhaosenhua
//date:2016/6.28

1.��ʹ�õĿ⣺ gtk, ovirt-engine Api, libcurl, minixml
2.ʹ��C ����Gtk�⿪�����棬 ʹ��Linux ubuntu ���а���п���, gtk ������ http://www.gtk.org/ �� ��ϵͳʹ��GTK3.xx�İ汾�� ��GTK3.XX �İ汾�� GTK2.XX �汾�����ݡ���ʹ��ʱ����ȷ�����Ļ�����װ����GTK3.XX�İ汾��
ubuntu �ϰ�װgtk������İ�װ
$sudo apt-get install libgtk3-dev
����
$sudo apt-get install libgtk3*

3.ʹ��C ����������ovirt-engine Api , ʹ�õĿ⺯����libcurl   һ��Ĭ�ϵ�linux ���а����Ѱ�װ��curl�� ��ʹ��libcurl��ʱ�������ʾ�Ҳ���<curl/curl.h>, ���յ���װlibcurl-devel ���ļ���
�й�libcurl����ϸ������������ https://curl.haxx.se/
 
4.libcurl ʹ��http ���� ovirt-engine api ���� ��ʹ��header ��ʹ��Application/xml , һ��Ĭ�������ָ�ʽ��ovirt-engine ����Ϊxml��ʽ�Ľ����
5.C ����minixml ������xml�ļ���minixml �İ�װ���Լ�������http://www.msweet.org/