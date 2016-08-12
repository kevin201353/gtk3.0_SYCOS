#include <stdio.h>
#include <linux/input.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_PATH  "/dev/input/event5"   //difference is possible

int main()
{
	int keys_fd;
	char ret[2];
	struct input_event t;
	keys_fd = open(DEV_PATH, O_RDONLY);
	if(keys_fd <= 0)
	{
		printf("open /dev/input/event5 device error!\n");
		return -1;
	}
	while(1)
	{
  		if(read(keys_fd, &t, sizeof(t)) == sizeof(t))
  		{
  			if(t.type==EV_KEY)
  				if(t.value==0)
  				{
            if (t.code == 62)  //F4
            {
              	printf("key %d %s\n", t.code, (t.value) ? "Pressed" : "Released");
                system("./run_main.sh");
            }
  					//if(t.code == KEY_ESC)
  					//	break;
  				}
  		}
      sleep(0);
	}
	close(keys_fd);
	return 0;
}
