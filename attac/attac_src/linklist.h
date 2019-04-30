#ifndef __linklist_h__
#define __linklist_h__

#include "structs.h"
#include "util.h"

void print_link_list (long i);
void delete_link_list (long i);
void insert_link_list_node(char *str, long i);
int get_link_list_length(long i);
char * get_link_list_nth_data(long i, int nth);

#endif
