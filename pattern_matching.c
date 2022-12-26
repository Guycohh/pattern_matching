#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pattern_matching.h"
#include <stddef.h>


void destroy_recursive(pm_state_t * state);
/* Initializes the fsm parameters (the fsm itself should be allocated).  Returns 0 on success, -1 on failure.
*  this function should init zero state
*/
int pm_init(pm_t * p) {
    if(p==NULL){
        return -1;
    }

    p->zerostate= (pm_state_t*)malloc(sizeof (pm_state_t));

    if((p->zerostate)==NULL){
        perror("problem with memory allocate: pm_init function!");
        return -1;
    }

    p->zerostate->id = 0;// like S0.
    p->zerostate->depth = 0;// at the first line.
    p->newstate=1;//the id of the next state.
    p->zerostate->fail=NULL; //failure function, pointer to the state that we should go when there is no transition. in this case' it is the root, so it should point to NULL.

    /*malloc a double list for the output*/
    p->zerostate->output=(dbllist_t *) malloc(sizeof(dbllist_t));
    if(p->zerostate->output==NULL){
        perror("problem with memory allocate: pm_init function!");
        return -1;
    }
    dbllist_init(p->zerostate->output);

    /*list of edges all the next passes*/
    p->zerostate->_transitions=(dbllist_t *) malloc(sizeof(dbllist_t));
    if(p->zerostate->_transitions==NULL){
        perror("problem with memory allocate: pm_init function!");
        return -1;
    }
    dbllist_init(p->zerostate->_transitions);

    return 0;//success.
}

/* Adds a new string to the fsm, given that the string is of length n.
   Returns 0 on success, -1 on failure.*/
int pm_addstring(pm_t * p,unsigned char * word, _size_t size){
    /*some extreme cases*/
    if((p==NULL) || (word==NULL) || (p->zerostate)==NULL)
        return -1;//failure.
    if(size==0)
        return 0;//success.
    int y;
    for(y=0 ;word[y]!=0;y++);
    if(y!=size)
        return -1;//failure.

    /*First, I want to check if the pattern is already in the tree */
    pm_state_t *current_state= p->zerostate;
    /*First, I will try to go from the root.
     * If there is no success, I will go to the goto_set function*/

    int i=0;
    for(;i<size && pm_goto_get(current_state,word[i])!=NULL; i++){
        current_state=pm_goto_get(current_state,word[i]);// give the next state.
    }

    /*if(i<size),
     I am going to create new states for the other character with a loop,
    from the last word[i]*/

    for(;i<size;i++){
        pm_state_t *to_state = (pm_state_t*) malloc(sizeof(pm_state_t));// crate new state.
        if(to_state ==NULL){
            perror("problem with memory allocate: pm_addstring function!");
            return -1;//failure
        }
        printf ("Allocating state %d\n", p->newstate);//Print the next state.
        to_state->id = p->newstate;// The id that should be for the new state.
        p->newstate+=1;

        /*Now I have to add a specific list for each state I create*/
        to_state->output=(dbllist_t*) malloc(sizeof (dbllist_t));
        if((to_state->output)==NULL){
            perror("problem with memory allocate: pm_addstring function!");
            return -1;//failure
        }
        dbllist_init(to_state->output);

        /*Another necessary lis is the transitions list */
        to_state->_transitions=(dbllist_t*)malloc(sizeof(dbllist_t));
        if((to_state->_transitions)==NULL){
            perror("problem with memory allocate: pm_addstring function!");
            return -1;//failure
        }
        dbllist_init(to_state->_transitions);

        if(pm_goto_set(current_state, word[i],to_state)==-1){
            return -1;//failure
        }
        current_state=to_state;//Now, the current state will be the next state.
    }

    if(dbllist_size(current_state->output)<=0 ){
        if(dbllist_append(current_state->output,word)==-1)
            return -1;//failure
    }

    return 0;//success
}

/* Set a transition arrow from this from_state, via a symbol, to a
   to_state. will be used in the pm_addstring and pm_makeFSM functions.
   Returns 0 on success, -1 on failure.
   The function will create a new edge, put the character and this state in it.
   And in addition you will add him to the transition list of From State*/
int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    if(from_state==NULL || to_state==NULL)//the function have to make a connection between two states.
        return -1;//failure.

    pm_labeled_edge_t * edge=(pm_labeled_edge_t*) malloc(sizeof(pm_labeled_edge_t));//new state=0.
    if(edge==NULL){
        perror("problem with memory allocate: pm_goto_func function!");
        return -1;
    }

    edge->label=symbol;
    edge->state=to_state;
    //new
//    to_state->id=(from_state->id)+1;
    to_state->depth=(from_state->depth)+1;
    to_state->fail=NULL;
    //new
    if(dbllist_append(from_state->_transitions,edge)==-1)// add to the transitions list.
        return -1;


    printf("%d -> %c -> %d\n",from_state->id, symbol, to_state->id);
    return 0;//success

}

/* Returns the transition state.  If no such state exists, returns NULL.
   will be used in pm_addstring, pm_makeFSM, pm_fsm_search, pm_destroy functions.
   Given symbol, which is the state, give me the next set, of where the goto leads.*/
pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol){
    if(state==NULL)
        return NULL;//failure.
    dbllist_node_t * node_ptr=dbllist_head(state->_transitions);//pointer to the head of the transitions list

    pm_labeled_edge_t *edge_ptr;
    while(node_ptr!=NULL){//while this is not the end of the list
        edge_ptr=dbllist_data(node_ptr);
        if(edge_ptr!=NULL &&(edge_ptr->label)==symbol){
            return (edge_ptr->state);
        }
        node_ptr=dbllist_next(node_ptr);

    }
    return NULL;
}

/* Finalizes construction by setting up the failrue transitions, as
   well as the goto transitions of the zerostate.
   Returns 0 on success, -1 on failure.*/
int pm_makeFSM(pm_t * p){
    if(p==NULL)
        return -1;//failure

    dbllist_t * fail_queue=(dbllist_t *)malloc(sizeof(dbllist_t));//Create queue.
    if(fail_queue==NULL){
        perror("problem with memory allocate: pm_makeFSM function!");
        return -1;
    }
    dbllist_init(fail_queue);

    pm_state_t *current_state=p->zerostate;//Pointer to the current state in the tree.
    dbllist_node_t *node_ptr= dbllist_head( p->zerostate->_transitions);
    unsigned int i;
    for(i=0;(i<dbllist_size( current_state->_transitions));i++, node_ptr = dbllist_next(node_ptr)) {//Insert to the queue all states with depth 1.
        if (dbllist_append(fail_queue, dbllist_data(node_ptr)) == -1)
            return -1;//failure.
        ((pm_labeled_edge_t *) dbllist_data(node_ptr))->state->fail=  p->zerostate;
    }
    /*Now my list have the states with depth 1 inside */

    pm_state_t *r;
    dbllist_node_t * edge_node;
    dbllist_node_t * next_edge;

    pm_state_t *state;
    while(dbllist_size(fail_queue)> 0){
        void* temp=dbllist_data(dbllist_head(fail_queue));
        node_ptr=dbllist_head(fail_queue);
        if(dbllist_remove(fail_queue,node_ptr,DBLLIST_LEAVE_DATA)==-1){
            return -1;//failure
        }
        r=((pm_labeled_edge_t *) temp)->state;//r is the head of the fail_queue list.
        edge_node=dbllist_head(r->_transitions);

        while(edge_node!=NULL){
            pm_labeled_edge_t * ptr_edge=((pm_labeled_edge_t *)(dbllist_data(edge_node)));
            next_edge=dbllist_next(edge_node);//the next state.
            if(dbllist_append(fail_queue,dbllist_data(edge_node)) == -1){
                return -1;//failure
            }
            state= r->fail;
            pm_state_t *s=ptr_edge->state;// s->fail is the fail of the previous state
            while(s->fail==NULL) {
                if(state==NULL){
                    s->fail = p->zerostate;
                    break;
                }
                s->fail = pm_goto_get(state, ptr_edge->label);
                state = state->fail;

            }
            dbllist_node_t *ptr=dbllist_head(s->fail->output);
            while(ptr){
                if(dbllist_append(s->output,dbllist_data(ptr))==-1){
                    return -1;
                }
                ptr=dbllist_next(ptr);
            }
            printf("Setting f(%d) = %d\n", s->id, s->fail->id);
            edge_node = next_edge;
        }
    }

    free(fail_queue);
    fail_queue=NULL;

    return 0;
}

/* Search for matches in a string of size n in the FSM.
   if there are no matches return empty list */
dbllist_t* pm_fsm_search(pm_state_t * state,unsigned char * text,_size_t size){
    if(state==NULL || text==NULL || size==0 )
        return NULL;

    dbllist_t * matching_list=(dbllist_t*) malloc(sizeof (dbllist_t));
    if(matching_list==NULL){
        perror("problem with memory allocate: pm_fsm_search function!");
        return NULL;
    }
    dbllist_init(matching_list);

    int flag;
    for(int j=0; j<size;j++){
        flag=0;
        while (state!=NULL && pm_goto_get(state,text[j])==NULL){
            if( state->id==0){//so this is the root
                flag=1;
                break;
            }
            state=state->fail;
        }
        if(flag==1 ){
            continue;
        }
        state= pm_goto_get(state, text[j]);
        dbllist_node_t *ptr;

        if(state !=NULL && dbllist_size(state->output)>0) {
            ptr = dbllist_head(state->output);
            for (int i = 0; i < dbllist_size(state->output); i++) {
                pm_match_t *newMatch = (pm_match_t *) malloc(sizeof(pm_match_t));//create struct of match
                if (newMatch == NULL) {
                    perror("problem with memory allocate: pm_fsm_search function!");
                    return NULL;
                }
                newMatch->pattern = (char *) dbllist_data(ptr);
                newMatch->start_pos = j - strlen(newMatch->pattern) + 1;
                newMatch->end_pos = j;
                newMatch->fstate = state;
                dbllist_append(matching_list, newMatch);//connect the struct to my list
                ptr = dbllist_next(ptr);
            }
        }

    }

    return matching_list;
}

/* Destroys the fsm, deallocating memory. */
void pm_destroy(pm_t * p){
    if(p==NULL)// there is no allocated fsm
        return;
    destroy_recursive(p->zerostate);

}

void destroy_recursive(pm_state_t * state){


    if(dbllist_size(state->_transitions) == 0){
        dbllist_destroy(state->output, DBLLIST_LEAVE_DATA);
        free(state->output);
        free(state->_transitions);
        free(state);
        return;
    }
    dbllist_node_t * prev = dbllist_head(state->_transitions);
    while(prev!= NULL){
        dbllist_node_t * temp = prev->next;
        destroy_recursive(((pm_labeled_edge_t *)prev->data)->state);
        free(prev->data);
        free(prev);
        prev = temp;
    }

    dbllist_destroy(state->output, DBLLIST_LEAVE_DATA);
    free(state->output);
    free(state->_transitions);
    free(state);
}

