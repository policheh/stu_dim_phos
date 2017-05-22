#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dic.h>

int received = 0;
char str[132] = {'\0'};

void rout(int *tag, int *code)
{
  int silent;

  silent = *tag;
  if(!silent)
      dim_print_date_time();
  if(*code)
  {
	  if(!silent)
		printf(" Command %s Successfully Sent\n", str);
  }
  else
  {
	  if(!silent)
		printf(" Command %s Unsuccessfull\n", str);
  }
  received = 1;
#ifdef WIN32
  wake_up();
#endif
}
int main(int argc, char **argv)                    
{
int i;
int silent = 0;
unsigned char data[1024] = {'\0'};
char dns_node[128], *ptr;
int dns_port = 0;

int nb_trouve=0;
//unsigned char * pEnd;
unsigned long data_int[100];

	dns_node[0] = '\0'; 
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i],"-dns"))
		{
			strcpy(dns_node,argv[i+1]);
			if((ptr = strchr(dns_node,':')))
			{
				*ptr = '\0';
				ptr++;
				sscanf(ptr,"%d",&dns_port);
			}
			i++;
		}
		else if(!strcmp(argv[i],"-s"))
		{
			silent = 1;
		}
		else
		{
			if(!str[0])
			{
				printf("command name: %s\n",argv[i]);
				strcpy(str, argv[i]);
			}
			else 
				{
				
				//data_int[i]=strtoul(argv[i], &pEnd, 0);
				data_int[i]=strtoul(argv[i], NULL, 0);
				
				//strcpy(data,argv[i]);
				
				nb_trouve++;
				
				/*
				if (data[0]=='I'){
				nb_int++;
				printf("nb_int %d\n",nb_int);
				
				}
				*/
				}
		}
	}
	if(dns_node[0])
	{
		printf("dns node(0)\n");
		//dic_set_dns_node(dns_node);
	}
	if(dns_port)
	{
		printf("dns port\n");
		//dic_set_dns_port(dns_port);
	}
	if(!str[0])
	{
		printf("dim_send_command: Insufficient parameters\n");
		printf("usage: dim_send_command <cmnd_name> [<data>] [-dns <dns_node>] [-s]\n");
		exit(0);
	}
	if(!data[0])
		data[0] = '\0';
	printf("number of args: %d\n",nb_trouve);
	//essai de commande a la mano
	//strcpy(str,"/STU_EMCAL/Configure");
	
	for (i=0;i<nb_trouve;i++)
		{
		data[4*i]=data_int[i+2] & 0xFF;
				
		data[4*i+1]=(data_int[i+2]>>8) & 0xFF;
				
		data[4*i+2]=(data_int[i+2]>>16) & 0xFF;
			
		data[4*i+3]=(data_int[i+2]>>24) & 0xFF;
	
		}

	data[4*nb_trouve]=0;
	

	dic_cmnd_callback(str,data,4*nb_trouve+1, rout, silent);
	while(!received)
	  dim_wait();
	  
	sleep(1);
	

	return(1);
}
