#ifndef list_header_h
#define list_header_h

typedef struct node node;
typedef enum node_attributes node_attributes;
typedef struct list list;
typedef int (*comparator)(node*,node*);
struct node{
	char name[30];
	char surname[30];
	char birthDate[30];
	char email[30];
	char phone[30];
	char address[30];
	node * next;
	node * prev;
};
struct list{
	node * head;
	node * tail;
	int length;
	void (*delete_list)(list *);
	void (*sort)(list *, comparator);
	char * (*list_to_string)(list *);
	int (*push)(list *, char *,char *,char *,char *,char *,char *);
	int (*add)(list *, char *,char *,char *,char *,char *,char *, int);
	int (*delete_node)(list *, int);
	node * (*pop)(list *);
	node * (*find)(list *,node_attributes, char *);
	node * (*get_element)(list *, int);
};
enum node_attributes{NAME,SURNAME,BIRTHDATE,EMAIL,PHONE,ADDRESS};

list * create_list();
void delete_list(list * l);
void sort(list * l, comparator cmp);
char * list_to_string(list * l);
int push(list * l, char * name,char * surname,char * birthDate,char * email,char * phone,char * address);
int add(list * l, char * name,char * surname,char * birthDate,char * email,char * phone,char * address, int pos);
int delete_node(list * l, int position);
int compare_names(node * a, node * b);
node * pop(list *l);
node * find(list * l,enum node_attributes attribute, char * value);
node * get_element(list * l, int index);
node * merge_sort(node * head, int length, comparator cmp);
node * merge(node * l,node * r, comparator cmp);
#endif