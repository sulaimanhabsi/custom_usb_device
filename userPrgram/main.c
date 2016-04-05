#include <stdio.h>
#include <stdint.h>

int main()
{
	char buff[64];
	buff[0]= 0x30;

	FILE* fp = fopen("/dev/Oman0","rw+");
	if(fp !=NULL)
	{
		int res = fwrite(buff, 1, 64, fp);
		printf("write returned %d \n\r", res);
		fclose(fp);
	}
	else
	{
		printf("unable to open the file \n\r");
	}

	return 0;
}