
CFLAGS=`pkg-config --cflags gtk+-3.0 mxml libcurl`
LIBS=`pkg-config --libs gtk+-3.0 mxml libcurl`

TIME=`date +"%Y-%m-%d %T"`
.INIT:
	@echo $(TIME)
	@echo "char buildtime[32]=\"$(TIME)\";" > buildtime.h

all: .INIT clean shencloudui

shencloudui:
	gcc -std=c99 -c resol.c logincit.c interface.c topwindow.c networkSet.c setting.c loginwindow.c curlc.c Ovirtdispatch.c vmlistwindow.c Loginfo.c vms.c configxml.c main.c $(CFLAGS)
	gcc -std=c99 -o shencloud-ui resol.o logincit.o interface.o topwindow.o networkSet.o setting.o loginwindow.o curlc.o Ovirtdispatch.o vmlistwindow.o Loginfo.o vms.o configxml.o main.o  -Wall $(LIBS)  -linterface

clean:  
	rm -rf *.o shencloud-ui

