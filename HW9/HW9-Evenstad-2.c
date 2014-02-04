/*********************/
/* Ryan Evenstad HW9 */
/*********************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//global variables for threads
char population[24];
char debt[24];
char warCost[24];
//monitors
int t1, t2, t3;

getPopulationThread()
{
	while(1)
	{
		if(t1!=1)
		{
			int get;
			get = system("wget -q --no-cache http://www.census.gov/main/www/popclock.html");
			if (get != 0)
				continue;
				//printf("Error with getting population\n");
			else
			{
				FILE *dataFile;
				dataFile = fopen("popclock.html", "r");
				if (dataFile == NULL)
				{
					//printf("cannot open popclock.html\n");
					//exit(0);
					unlink("popclock.html");
					continue;
				}
				char temp[256];
				char buf[24] = "";
				while(fscanf(dataFile, "%s", temp) != EOF)
				{
					if(strstr(temp, "usclocknum\">")!= NULL)
					{
						int place = 0;
						char i = temp[place];
						while(i != '\0')
						{
							if (isdigit(i))
								sprintf(buf, "%s%c", buf, i);
							place++;
							i = temp[place];
						}
						strcpy(population, buf);
						//printf("population is %s\n", population);
						break;
					}
				}
			}
			unlink("popclock.html");
			t1=1;
		}
	}
}

getDebtThread ()
{
	while(1)
	{
		if(t2!=1)
		{
			int get;
			get = system("wget -q --no-cache --output-document=debt.html http://www.brillig.com/debt_clock/");
			if (get != 0)
				continue;
				//printf("Error with getting debt\n");
			else
			{
				FILE *dataFile;
				dataFile = fopen("debt.html", "r");
				if (dataFile == NULL)
				{
					//printf("cannot open debt.html\n");
					//exit(0);
					unlink("debt.html");
					continue;
				}
				char temp[256];
				char buf[24] = "";
				//while(fscanf(dataFile, "%s", temp) != EOF)
				while (fgets (temp, sizeof temp, dataFile) != NULL)
				{
					
					if(strstr(temp, "debtiv.gif")!= NULL)
					{
						//printf("%s\n", temp);
						int place = 0, count = 0;
						char i = temp[place];
						while(i != '\0')
						{
							if (isdigit(i))
							{
								count++;
								if (count > 5)
								{
									if (count == 20)
									{
										sprintf(buf,"%s.", buf);
									}
									sprintf(buf, "%s%c", buf, i);
								}
							}
							place++;
							i = temp[place];
						}
						strcpy(debt, buf);
						//printf("debt is %s\n", debt);
						break;
					}
				}
			}
			unlink("debt.html");
			t2=1;
		}
	}
}

getWarCostThread()
{
	while(1)
	{
		if(t3!=1)
		{
			int get;
			get = system("wget -q --no-cache --output-document=war.html http://costofwar.com/");
			if (get != 0)
				continue;
				//printf("Error with getting the cost of war\n");
			else
			{
				FILE *dataFile;
				dataFile = fopen("war.html", "r");
				if (dataFile == NULL)
				{
					//printf("cannot open war.html\n");
					//exit(0);
					unlink("war.html");
					continue;
				}
				char temp[256];
				char buf[24] = "";
				while(fscanf(dataFile, "%s", temp) != EOF)
				{
					if(strstr(temp, "costOfTotal\">")!= NULL)
					{
						int place = 0;
						char i = temp[place];
						while(i != '\0')
						{
							if (isdigit(i))
								sprintf(buf, "%s%c", buf, i);
							place++;
							i = temp[place];
						}
						strcpy(warCost, buf);
						//printf("warCost is %s\n", warCost);
						break;
					}
				}
			}
			unlink("war.html");
			t3=1;
		}
	}
}

printThread()
{
	while(1)
	{
		if (t1==1 && t2==1 && t3==1)
		{
			double pop = atof(population);
			double deb = atof(debt);
			double war = atof(warCost);
			
			system("clear");
			printf("U.S. Population:       %s\n", population);
			printf("U.S. Debt:            $%s\n", debt);
			printf("U.S. War Cost:        $%s.00\n\n", warCost);
			printf("Debt Per Person:      $%.2f\n", deb/pop);
			printf("War Cost Per Person:  $%.2f\n", war/pop);
			t1=t2=t3=0;
		}
	}
}

int main()
{
	unlink("popclock.html");
	unlink("debt.html");
	unlink("war.html");
	
	t1=t2=t3=0;
	
	pthread_t getPopulation, getDebt, getWarCost, print;
	
	pthread_create(&getPopulation, NULL, (void*)&getPopulationThread, NULL);
	pthread_detach(getPopulation);
	
	pthread_create(&getDebt, NULL, (void*)&getDebtThread, NULL);
	pthread_detach(getDebt);
	
	pthread_create(&getWarCost, NULL, (void*)&getWarCostThread, NULL);
	pthread_detach(getWarCost);
	
	pthread_create(&print, NULL, (void*)&printThread, NULL);
	pthread_detach(print);
	
	while(1);
	//sleep(5);
	
}