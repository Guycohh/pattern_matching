String Pattern Matching Algorithm 
Authored by Guy Cohen

==Description==
String pattern matching algorithms are very useful for several purposes, like simple search for a pattern in a text or looking for attacks with predefined signatures.
This program implement a dictionary-matching algorithm that locates elements of a finite set of strings (the "dictionary") within an input text. It matches all patterns "at once", so the complexity of the algorithm is linear in the length of the patterns plus the length of the searched text plus the number of output matches. 
The algorithm matches multiple patterns simultaneously, by first constructing a Deterministic Finite Automaton (DFA) representing the patterns set, and then, with this DFA on its disposal, processing the text in a single pas. 
==Program DATABASE:==
DFA= will be implement like a tree, that is presented with pointers.  Each node of this tree will have some attributes like transitions list, output list and and some more “dry” attributes like id and more.
double list= all of the nodes of this program are use a double list. for example the output of the final state will be save in a list. In addition the outpur of this program, the matching patterns will use this kind of data base. 

==functions:==
-int pm_init(pm_t * p) --> initiate the DFA, such as the paramaters of it. return 0 as success and -1 as failure.

-pm_state_t* pm_goto_get(pm_state_t *state,unsigned char symbol)--> Returns the transition state.  If no such state exists, returns NULL.

-int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state)-->this function porpose is to set a transition arrow from from state, via a symbol, to a another state.retern 0 as success and -1 as failure.

-int pm_addstring(pm_t * p,unsigned char * word, size_t size)-->Adds a new string to the fsm, given that the string is of length n. Retern 0 as success and -1 as failure.

-int pm_makeFSM(pm_t * p)-->Finalizes construction by setting up the failrue transitions, as well as the goto transitions of the zerostate. Retern 0 as success and -1 as failure.

-dbllist_t* pm_fsm_search(pm_state_t * state,unsigned char * text,size_t size)-->Search for matches in a string of size n in the FSM. If there are no matches return empty list.

-void pm_destroy(pm_t * p) && void destroy_recursive(pm_state_t * state)-->Destroys the fsm, deallocating memory. It make it by using recursive, with  the"post-order" techniqe.

==Input:==
Patterns from the user and text in order to search on it.

==Output:==
-Each time when a  new state is created the int the FSM the program 
  will print the message: "Allocating state i\n".
-Each time when edge is created for the goto function from state i to state j in the FSM' it will print the message: 
 "I -> a -> j\n", where 'a' is the character that generates this edge.

==Program Files==
pattern_matching.c , slist.c

==How to compile?==
compile: gcc pattern_matching.c slist.c main.c -o main
run: ./main




