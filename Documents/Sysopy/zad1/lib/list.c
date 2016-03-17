#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "list_header.h"

list * create_list(){
	node * head = malloc( sizeof(node) );
	node * tail = malloc( sizeof(node) );
	head->next = tail;
	tail->prev = head;
	list * new_list = malloc( sizeof(list) );
	new_list->head = head;
	new_list->tail = tail;
	new_list->length = 0;
	//Pointers to function declaration
	new_list->delete_list = delete_list;
	new_list->list_to_string = list_to_string;
	new_list->push = push;
	new_list->add = add;
	new_list->delete_node = delete_node;
	new_list->pop = pop;
	new_list->find = find;
	new_list->get_element = get_element;
	new_list->sort = sort;
	return new_list;
}
void delete_list(list * l){
	node * n = l->head->next;
	node * m;
	while(n->next != NULL){
		m=n;
		n = n->next;
		free(m);
	}
	free(l);
}
node * merge(node * l,node * r, comparator cmp){
	node * head;
	node * n;
	if(cmp(l,r)<0){
		head = l;
		l = l->next;
	}
	else{
		head = r;
		r = r->next;
	}
	n = head;
	while(l != NULL && r != NULL){
		if(cmp(l,r)<0){
			n->next = l;
			l->prev = n;
			l = l->next;
		}
		else{
			n->next = r;
			r->prev = n;
			r = r->next;
		}
		n = n->next;
	}
	if(l!=NULL){
		n->next = l;
		l->prev = n;
	};
	if(r!=NULL){
		n->next = r;
		r->prev = n;
	}
	return head;
};
node * merge_sort(node * head, int length, comparator cmp){
	if(head->next == NULL)
		return head;
	node * n = head;
	node * l;
	node * r;
	for(int i=0;i<length/2;i++){
		n = n->next;
	}
	n->prev->next = NULL;
	n->prev = NULL;
	l = merge_sort(head,length/2,cmp);
	r = merge_sort(n,length-(length/2),cmp);
	return merge(l,r,cmp);
}
void sort(list * l, comparator cmp){
	node * n = l->head->next;
	l->tail->prev->next = NULL;
	n->prev = NULL;
	l->head->next = NULL;
	l->tail->prev = NULL;
	n = merge_sort(n,l->length,cmp);
	n->prev = l->head;
	l->head->next = n;
	while(n->next != NULL){
		n = n->next;
	}
	n->next = l->tail;
	l->tail->prev = n;
}
char * list_to_string(list * l){
	if(l->length==0)
		return "";
	//Declaring a little bit to much. Just in case.
	char * str = malloc(l->length * 186 * sizeof(char));
	int i=0;
	node * n;
	n = l->head->next;
	while(n->next != NULL){
		strcat(str,n->name);
		strcat(str,",");
		strcat(str,n->surname);
		strcat(str,",");
		strcat(str,n->birthDate);
		strcat(str,",");
		strcat(str,n->email);
		strcat(str,",");
		strcat(str,n->phone);
		strcat(str,",");
		strcat(str,n->address);
		strcat(str,"\n");
		n = n->next;
	}
	return str;
}
//Adds element on the end of the list
int push(list * l, char * name,char * surname,char * birthDate,char * email,char * phone,char * address){
	node * new_node = malloc(sizeof(node));
	strcpy(new_node->name,name);
	strcpy(new_node->surname,surname);
	strcpy(new_node->birthDate,birthDate);
	strcpy(new_node->email,email);
	strcpy(new_node->phone,phone);
	strcpy(new_node->address,address);
	new_node->next = l->tail;
	new_node->prev = l->tail->prev;
	l->tail->prev->next = new_node;
	l->tail->prev = new_node;
	l->length++;
	return 1;
}
//Adds element before element specified by argument pos
int add(list * l, char * name,char * surname,char * birthDate,char * email,char * phone,char * address, int pos){
	if(pos > l->length || pos < 0){
		return 0;
	}
	node * new_node = malloc(sizeof(node));
	strcpy(new_node->name,name);
	strcpy(new_node->surname,surname);
	strcpy(new_node->birthDate,birthDate);
	strcpy(new_node->email,email);
	strcpy(new_node->phone,phone);
	strcpy(new_node->address,address);
	node * p = l->head->next;
	int i=0;
	while(i<pos){
		p = p->next;
		i++;
	}
	new_node->next = p;
	new_node->prev = p->prev;
	p->prev->next = new_node;
	p->prev = new_node;
	l->length++;
	return 1;
}
node * pop(list *l){
	node * result = l->tail->prev;
	//If list is empty
	if(result==l->head){
		return NULL;
	}
	result->prev->next = l->tail;
	l->tail->prev = result->prev;
	l->length--;
	result->next = NULL;
	result->prev = NULL;
	return result;
}
int delete_node(list * l, int position){
	if(position < 0 || position > l->length-1)
		return 0;
	int i=0;
	node * n = l->head->next;
	while(i<position){
		n = n->next;
		i++;
	}
	node * m = n;
	n->prev->next = n->next;
	n->next->prev = n->prev;
	free(n);
	l->length--;
	return 1;
}
node * find(list * l,enum node_attributes attribute, char * value){
	node * n = l->head->next;
	while(n->next != NULL){
		switch(attribute){
			case NAME:
				if(strcmp(n->name,value)==0)
					return n;
			case SURNAME:
				if(strcmp(n->surname,value)==0)
					return n;
			case BIRTHDATE:
				if(strcmp(n->birthDate,value)==0)
					return n;
			case EMAIL:
				if(strcmp(n->email,value)==0)
					return n;
			case PHONE:
				if(strcmp(n->phone,value)==0)
					return n;
			case ADDRESS:
				if(strcmp(n->address,value)==0)
					return n;
		}
		n= n->next;
	}
	return NULL;
}
node * get_element(list * l, int index){
	if(index < 0 || index > l->length)
		return NULL;
	node * n=l->head->next;
	for(int i=0;i<index;i++){
		n = n->next;
	}
	return n;
}
int compare_names(node * a, node * b){
	if(strcmp(a->name,b->name)==0)
		return strcmp(a->surname,b->surname);
	return strcmp(a->name,b->name);
}