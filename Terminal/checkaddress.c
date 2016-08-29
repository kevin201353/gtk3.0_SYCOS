#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <Winsock2.h>
#else
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#include <sys/un.h>
#include <linux/types.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#ifdef WIN32
#pragma comment(lib,"wsock32.lib")
#endif

/*����UINT32��Ϊ�����ֽ�˳��*/

/*IP��ַ�Ƿ�Ϸ�, �Ϸ�����TURE��ʧ�ܷ���FALSE*/

int netIpIsValid(_UINT32 IP)
{
  int i;
  struct in_addr addr;
  addr.s_addr = IP;

  i = inet_addr(inet_ntoa(addr));

  if((i == 0)||(i == 0xffffffff))
    return FALSE;
  else
    return TRUE;
}

/*MASK���������Ƿ�Ϸ�, �Ϸ�����TURE��ʧ�ܷ���FALSE*/
int netMaskIsValid(_UINT32 mask)
{
  int i;
  unsigned long ii;
  i = netIpIsValid(mask);
  if(i==TRUE)
  {
    ii = ntohl(mask);
    if((ii|ii-1)==0xffffffff)
    {
      return TRUE;
    }
  }
  return FALSE;
}
/*MASK���������Ƿ�Ϸ�, �Ϸ�����TURE��ʧ�ܷ���FALSE*/
int netMaskAndIpIsValid(_UINT32 IP, _UINT32 mask)
{
  int i;
  int a, b = 0, c;
  i = netIpIsValid(IP);
  if(i != TRUE)
    return FALSE;
  i = netMaskIsValid(mask);
  if(i != TRUE)
    return FALSE;
  a = IP&0x000000ff;
  b = ntohl(mask);
  if( a>0 && a< 127)
  {
    if(mask < 0x000000ff)
      return FALSE;
    if(mask > 0x000000ff)
      b -= 0xff000000;
  }
  if(a >= 128 && a<= 191)
  {
    if(mask < 0x0000ffff)
      return FALSE;
    if(mask > 0x0000ffff)
      b -= 0xffff0000;
  }
  if(a >= 192 && a<= 223)
  {
    if(mask < 0x00ffffff)
      return FALSE;
    if(mask> 0x00ffffff)
       b-= 0xffffff00;
  }
  /*ÿ�������εĵ�һ���������ַ,������־�������,���һ���ǹ㲥��ַ,����������������ϵ���������.������IP��ַ��TCP/IP����,���ɷ��������ʹ��.*/
  c = ~ntohl(mask)&ntohl(IP);
  if( c == 0 || c == ~ntohl(mask))
    return FALSE;
  /*RFC 1009�й涨��������ʱ�������Ų���ȫΪ0��1���ᵼ��IP��ַ�Ķ�����*/
  if( b > 0 )
  {
    c = b&(ntohl(IP));
    if(c == 0|| c == b)
      return FALSE;
  }
  return TRUE;
}

/*���������������Ƿ�ƥ�䣬Ҳ�ɲ�����������IP�Ƿ���ͬһ������*/
int netIPAndSubnetValid(_UINT32 IP, _UINT32 subIP, _UINT32 mask)
{
  int i;
  int addr1, addr2;
  i = netMaskAndIpIsValid(IP, mask);
  if(i!=TRUE)
    return FALSE;
  i = netMaskAndIpIsValid(subIP, mask);
  if(i!=TRUE)
    return FALSE;

  addr1 = IP&mask;
  addr2 = subIP&mask;

  if(addr1!=addr2)
    return FALSE;

  return TRUE;
}

int netIPAndSubnetValid(_UINT32 IP, _UINT32 subIP, _UINT32 mask)
{
  int i;
  int addr1, addr2;
  i = netMaskAndIpIsValid(IP, mask);
  if(i!=TRUE)
    return FALSE;
  i = netMaskAndIpIsValid(subIP, mask);
  if(i!=TRUE)
    return FALSE;

  addr1 = IP&mask;
  addr2 = subIP&mask;

  if(addr1!=addr2)
    return FALSE;

  return TRUE;
}

int main(int argc, char *argv[])
{
	
}
