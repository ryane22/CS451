/*********************/
/* Ryan Evenstad HW1 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void){
	int get;
	get = system("wget -q http://www.senate.gov/general/contact_information/senators_cfm.cfm");
	
	FILE *dataFile;
	dataFile = fopen("senators_cfm.cfm", "r");
	if (dataFile == NULL)
	{
		printf("cannot open file\n");
		exit(0);
	}
	char temp[256];
	int i, count;
	count = 0;
	while(fscanf(dataFile, "%s", temp) != EOF)
	{
		if (count > 10)
		{
			if(strstr(temp, "http:")!= NULL)
			{
				if(!(strstr(temp, "javasc")!= NULL))
				{
					for(i=6; i<=strlen(temp); i++)
					{
						if(temp[i] == '?')
							break;
						if(temp[i] == '>')
							break;
						if(temp[i] == '"')
							break;
						printf("%c", temp[i]);
					}
					printf("\n");
				}
			}
		}
		count++;
	}
	unlink("senators_cfm.cfm");
	fclose(dataFile);
}