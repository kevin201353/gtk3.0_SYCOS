#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int main( void )
{
	FILE   *stream;
	FILE    *wstream;
	char   buf[1024];
	memset( buf, '/0', sizeof(buf) );//��ʼ��buf,�������д�����뵽�ļ���
	stream = popen( "ip addr", "r" ); //����ls ��l���������� ͨ���ܵ���ȡ����r����������FILE* stream
	wstream = fopen( "test_popen.txt", "w+"); //�½�һ����д���ļ�
	fread( buf, sizeof(char), sizeof(buf), stream); //���ո�FILE* stream����������ȡ��buf��
	fwrite( buf, 1, sizeof(buf), wstream );//��buf�е�����д��FILE    *wstream��Ӧ�����У�Ҳ��д���ļ���
	pclose( stream );
	fclose( wstream );
	return 0;
}
