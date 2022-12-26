#include "slist.h"
#include <stdio.h>
#include <stdlib.h>

void dbllist_init(dbllist_t * list){
//    if(list!=NULL)
//        return;

    dbllist_head(list) = NULL;
    dbllist_tail(list) = NULL;
    dbllist_size(list) = 0;

}

int dbllist_append(dbllist_t * list,void *data){
    if(list==NULL || data==NULL){
        return -1; //failed
    }
    /*allocate new node*/
    dbllist_node_t *new_node= (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    if(new_node==NULL){
        perror("problem with memory allocate: dbllist_append function!");
        return -1;
    }

    dbllist_data(new_node)=data;
//    dbllist_prev(new_node)=NULL;

    /*if the list is empty,same as the prepend function. */
    if(dbllist_head(list)==NULL){
        dbllist_head(list)=new_node;
        dbllist_tail(list)=new_node;
        dbllist_prev(new_node)=NULL;
        dbllist_next(new_node)=NULL;
    }
        /* new node will be added after the tail*/
    else{
        dbllist_prev(new_node)=dbllist_tail(list);
        dbllist_next(dbllist_tail(list))=new_node;
        dbllist_tail(list)=new_node;
        dbllist_next(new_node)=NULL;

    }
    dbllist_size(list)++;
    return 0; //success
}

int dbllist_prepend(dbllist_t * list, void *data){
    if(list==NULL || data==NULL){
        return -1; //failed
    }

    /*allocate new node*/
    dbllist_node_t *new_node= (dbllist_node_t*) malloc(sizeof(dbllist_node_t));
    if(new_node==NULL){
        perror("problem with memory allocate: dbllist_append function!");
        return -1; //failed
    }

    dbllist_data(new_node)=data;
    dbllist_next(new_node)=NULL;
    dbllist_prev(new_node)=NULL;

    /*if the list is empty,same as the append function. */
    if(dbllist_size(list)==0){
        dbllist_head(list)=new_node;
        dbllist_tail(list)=new_node;
    }
    else{
        dbllist_next(new_node)=dbllist_head(list);
        dbllist_prev(new_node)=NULL;
        dbllist_head(list)=new_node;
    }
    dbllist_size(list)++;
    return 0; //success
}

void dbllist_destroy(dbllist_t * list,dbllist_destroy_t des){
    /*if so, the list has not been built yet.*/
    if(list==NULL || dbllist_size(list)==0){
        return;
    }

    dbllist_node_t *ptr_node;
    while (dbllist_head(list)!=NULL){
        ptr_node=dbllist_head(list);
        dbllist_head(list)=dbllist_next(dbllist_head(list));

        if(des==DBLLIST_FREE_DATA ) {
            free(ptr_node->data);
            ptr_node->data=NULL;
        }
        free(ptr_node);
        ptr_node=NULL;

    }
    dbllist_size(list)=0;

}

int dbllist_remove(dbllist_t * list, dbllist_node_t* node,dbllist_destroy_t des){
    if(list==NULL||dbllist_head(list)==NULL)//list_head==NULL is same as list_size==0
        return -1;//failed
    /*if this list has only one parameter, it is a specific case so...*/
    if(dbllist_size(list)==1 && dbllist_data(dbllist_head(list))==dbllist_data(node)){
        dbllist_tail(list)=NULL;
        dbllist_head(list)=NULL;

        if( des==DBLLIST_FREE_DATA){
            free(dbllist_data(node));
            dbllist_data(node)=NULL;
        }
        free(node);
        node=NULL;
        dbllist_size(list)--;

        return 0;
    }
    /*if this node is the head of the list so...*/
    if(dbllist_prev(node)==NULL && dbllist_next(node)!=NULL){
        dbllist_head(list)=dbllist_next(node);
        dbllist_prev(dbllist_head(list))=NULL;
        if( des==DBLLIST_FREE_DATA){
            free(dbllist_data(node));
            dbllist_data(node)=NULL;
        }
        free(node);
        node=NULL;
        dbllist_size(list)--;

        return 0;
    }
    /*if this node is the tail of the list so...*/
    if(dbllist_prev(node)!=NULL && dbllist_next(node)==NULL){
        dbllist_tail(list)=dbllist_prev(node);
        dbllist_next(dbllist_tail(list))=NULL;
        dbllist_size(list)--;
        if( des==DBLLIST_FREE_DATA){
            free(dbllist_data(node));
            dbllist_data(node)=NULL;
        }
        free(node);
        node=NULL;
        dbllist_size(list)--;
        return 0;
    }
    /*if this node is somewhere in the list so...*/
    if(dbllist_prev(node)!=NULL && dbllist_next(node)!=NULL){
        dbllist_next(dbllist_prev(node))=dbllist_next(node);
        dbllist_prev(dbllist_next(node))=dbllist_prev(node);
        if( des==DBLLIST_FREE_DATA){
            free(dbllist_data(node));
            dbllist_data(node)=NULL;
        }
        free(node);
        node=NULL;
        dbllist_size(list)--;
        return 0;
    }
    //so there are some problems...
    return -1;
}