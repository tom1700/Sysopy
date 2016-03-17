#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <dlfcn.h>
#include "list_header.h"
char * random_email(){
	char * result = malloc(sizeof(char)*20);
	strcpy(result,"aaaaaaaaa@gmail.com");
	for(int i=0;i<9;i++){
		result[i]=rand()%('z'-'a')+'a';
	}
	return result;
}
char * random_phone(){
	char * result = malloc(sizeof(char)*10);
	for(int i=0;i<9;i++){
		result[i]=rand()%10+'0';
	}
	result[9]='\0';
	return result;
}
char * random_address(char**cities){
	char * result = malloc(sizeof(char)*30);
	strcpy(result,cities[rand()%20]);
	strcat(result," randomstreet");
	char number[4] = {rand()%10+'0',rand()%10+'0',rand()%10+'0','\0'};
	strcat(result, number);
	return result;
}
void generate_contact(list* l, char**names, char**surnames,char**birthdates,char**cities){
	l->push(l,names[rand()%20],
	surnames[rand()%20],
	birthdates[rand()%20],
	random_email(),
	random_phone(),
	random_address(cities));
}

void generate_contact2(list* l, char**names, char**surnames,char**birthdates,char**cities,int i){
	l->add(l,names[rand()%20],
	surnames[rand()%20],
	birthdates[rand()%20],
	random_email(),
	random_phone(),
	random_address(cities),i);
}
int main(){
	char * name[]={"Karlyn","Dionna","Arlette","Felisha",
"Neda","Mohamed","Laurie","Krystina","Kira","Jason","Alida",
"Annice","Patrice","Lelah","Lynwood","Layla","Tiffani",
"Syreeta","Cathi","Milagro"};
	char * surname[]={"Dunmore","Hause","Skeens","Pease",
"Fulp","Sutterfield","Kindred","Morrell","Hoopes","Smyth",
"Belser","Bottorff","Watlington","Baldree","Corliss",
"Erbe","Kaylor","Houk","Berrios","Mayweather"};
	char * birthDate[]={"1961-10-28","1962-11-26","1963-08-22",
"1963-12-22","1965-03-06","1973-09-10","1974-12-09",
"1976-09-07","1976-10-21","1977-06-12","1978-07-14",
"1979-04-24","1981-05-03","1981-08-19","1982-10-26",
"1987-03-21","1990-03-25","1992-03-11","1994-07-02",
"1995-02-28"};
char * cities[]={"Yangon","Santiago","Bangalore","Seoul",
"Tehran","Durban","New York","Petersburg",
"Dongguan","Guangzhou","Hanoi","Moscow",
"Suzhou","Shenzhen","Sao Paulo","Lahore",
"Pyongyang","Kinshasa","Dhaka","Mumbai"};

	struct timespec * real0 = malloc(sizeof(struct timespec));
	struct timespec * real1 = malloc(sizeof(struct timespec));
	struct timespec * real2 = malloc(sizeof(struct timespec));
	struct timespec * real3 = malloc(sizeof(struct timespec));
	struct timeval user0;
	struct timeval user1;
	struct timeval user2;
	struct timeval user3;
	struct timeval system0;
	struct timeval system1;
	struct timeval system2;
	struct timeval system3; 
	struct rusage * usage = malloc(sizeof(struct timespec));


	//checkpoint 0
	clock_gettime(CLOCK_REALTIME, real0);
	getrusage(RUSAGE_SELF,usage);
	user0 = usage->ru_utime;
	system0 = usage->ru_stime;
	printf("\nStarting program\nreal:%lld\nuser:%ld\nsystem:%ld\n",
		(long long)real0->tv_sec*1000000+real0->tv_nsec/1000,
		user0.tv_sec*1000000+user0.tv_usec,
		system0.tv_sec*1000000+system0.tv_usec
	);

	srand(time(NULL));
	void * handle = dlopen ("lib/libshlist.so", RTLD_LAZY);
	list * (*create_list)() = dlsym (handle, "create_list");
	list * l = (*create_list)();
	
	for(int i=0;i<100000;i++){
		generate_contact(l,name,surname,birthDate,cities);
	}

	//checkpoint 1
	clock_gettime(CLOCK_REALTIME, real1);
	getrusage(RUSAGE_SELF,usage);
	user1 = usage->ru_utime;
	system1 = usage->ru_stime;
	printf("\n100000 nodes generated\nreal:%lld\nuser:%ld\nsystem:%ld\nreal from start:%ld\nuser from start:%ld\nsystem from start:%ld\n",
		(long long)real1->tv_sec*1000000+real1->tv_nsec/1000,
		user1.tv_sec*1000000+user1.tv_usec,
		system1.tv_sec*1000000+system1.tv_usec,
		(real1->tv_sec-real0->tv_sec)*1000000+(real1->tv_nsec-real0->tv_nsec)/1000,
		(user1.tv_sec-user0.tv_sec)*1000000+user1.tv_usec-user0.tv_usec,
		(system1.tv_sec-system0.tv_sec)*1000000+system1.tv_usec-system0.tv_usec
	);

	int (*compare_names)() = dlsym (handle, "compare_names");
	l->sort(l,compare_names);

	//checkpoint 2
	clock_gettime(CLOCK_REALTIME, real2);
	getrusage(RUSAGE_SELF,usage);
	user2 = usage->ru_utime;
	system2 = usage->ru_stime;
	printf("\n100000 nodes sorted\nreal:%lld\nuser:%ld\nsystem:%ld\nreal from start:%ld\nuser from start:%ld\nsystem from start:%ld\nreal from previous:%ld\nuser from previous:%ld\nsystem from previous:%ld\n",
		(long long)real2->tv_sec*1000000+real2->tv_nsec/1000,
		user2.tv_sec*1000000+user2.tv_usec,
		system2.tv_sec*1000000+system2.tv_usec,
		(real2->tv_sec-real0->tv_sec)*1000000+(real2->tv_nsec-real0->tv_nsec)/1000,
		(user2.tv_sec-user0.tv_sec)*1000000+user2.tv_usec-user0.tv_usec,
		(system2.tv_sec-system0.tv_sec)*1000000+system2.tv_usec-system0.tv_usec,
		(real2->tv_sec-real1->tv_sec)*1000000+(real2->tv_nsec-real1->tv_nsec)/1000,
		(user2.tv_sec-user1.tv_sec)*1000000+user2.tv_usec-user1.tv_usec,
		(system2.tv_sec-system1.tv_sec)*1000000+system2.tv_usec-system1.tv_usec
	);

	for(int i=0;i<2500;i++){
		free(l->pop(l));
	}
	printf("\n2500 node poped\n");
	for(int i=0;i<2500;i++){
		l->delete_node(l,i);
	}
	printf("\n2500 node deleted from the middle\n");
	for(int i=0;i<5000;i++){
		generate_contact2(l,name,surname,birthDate,cities,i);
	}
	printf("\n5000 nodes added in the middle\n");
	l->delete_list(l);

	//checkpoint 3
	clock_gettime(CLOCK_REALTIME, real3);
	getrusage(RUSAGE_SELF,usage);
	user3 = usage->ru_utime;
	system3 = usage->ru_stime;
	printf("\n100000 nodes deleted. Program ended.\nreal:%lld\nuser:%ld\nsystem:%ld\nreal from start:%ld\nuser from start:%ld\nsystem from start:%ld\nreal from previous:%ld\nuser from previous:%ld\nsystem from previous:%ld\n",
	(long long)real3->tv_sec*1000000+real3->tv_nsec/1000,
	user3.tv_sec*1000000+user3.tv_usec,
	system3.tv_sec*1000000+system3.tv_usec,
	(real3->tv_sec-real0->tv_sec)*1000000+(real3->tv_nsec-real0->tv_nsec)/1000,
	(user3.tv_sec-user0.tv_sec)*1000000+user3.tv_usec-user0.tv_usec,
	(system3.tv_sec-system0.tv_sec)*1000000+system3.tv_usec-system0.tv_usec,
	(real3->tv_sec-real2->tv_sec)*1000000+(real3->tv_nsec-real2->tv_nsec)/1000,
	(user3.tv_sec-user2.tv_sec)*1000000+user3.tv_usec-user2.tv_usec,
	(system3.tv_sec-system2.tv_sec)*1000000+system3.tv_usec-system2.tv_usec
	);
	return 0;
}