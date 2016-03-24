#include "main.h"

int num;
int maxnum;
int up;
void strrev(char * s){
	int l=strlen(s);
	char b;
	for(int i=0;i<l/2;i++){
		b = s[i];
		s[i] = s[l-i-1];
		s[l-i-1] = b;
	}
}
void sig_handler(int signo){
	if (signo == SIGINT){
		printf("SIGINT signal received. Program ends.\n");
		exit(0);
	}
	if (signo == SIGTSTP){
		if(up==1){
			if(num <= maxnum*2){
				num++;
			}
			else{
				up=0;
				num--;
			}
		}
		else{
			if(num > 2){
				num--;
			}
			else{
				up=1;
				num++;
			}

		}
	}
}

int main(int argv,char ** argc){
	char * s = argc[1];
	char * srev = malloc(sizeof(char)*strlen(s)+1);
	strcpy(srev,s);
	strrev(srev);
	maxnum = atoi(argc[2]);
	num=2;
	up=0;
	if (signal(SIGINT, sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGINT\n");
  	if(signal(SIGTSTP,sig_handler) == SIG_ERR)
  		printf("\ncan't catch SIGSTMP\n");
	while(1){
		if(num%2==0){
			for(int i=0;i<num/2;i++)
				printf("%s",s);
			printf("\n");
		}
		else{
			for(int i=0;i<num/2;i++)
				printf("%s",srev);
			printf("\n");
		}
	}
	return 0;
}