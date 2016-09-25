#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "IntSet.h"
#include "dfa.h"
#include <math.h>
#include "LinkedList.h"

DFA *NFA_to_DFA(NFA *nfa);

DFA_State *DFA_State_new();

DFA_State *DFA_State_new() {
	DFA_State *x = (DFA_State*)malloc(sizeof(DFA_State));
	x->is_accepting = FALSE;
	for(int sym=0; sym<NFA_NSYMBOLS; sym++) {
		x->transitions[sym] = -1;
	}
	return x;
}

DFA *NFA_to_DFA(NFA *nfa) {
	//printf("ok1 \n");
	LinkedList *states = LinkedList_new(); //creates new linked list for states
	LinkedList *intsets = LinkedList_new(); //creates corresponding linked list for the intsets of the nfa state corresponding to dfa state
	//printf("ok2 \n");

	DFA_State *state0 = DFA_State_new();
	LinkedList_add_at_end(states, state0); //adds a first initial state 0 to the linked list of dfa states
	IntSet *first_IS = IntSet_new();
	IntSet_add(first_IS, 0);
	LinkedList_add_at_end(intsets, first_IS);
	//printf("ok3 \n");

	int num_states=1;
	LinkedListIterator *listIterate = LinkedList_iterator(states);
	LinkedListIterator *isIterate = LinkedList_iterator(intsets);

	while(LinkedListIterator_has_next(listIterate)) {
		//printf("%d number of states \n", num_states);
		DFA_State *currentState = LinkedListIterator_value(listIterate);
		IntSet *currentIS = LinkedListIterator_value(isIterate);
		for(int sym=0; sym<NFA_NSYMBOLS; sym++) {
			IntSet *dst = IntSet_new();
			IntSetIterator *current_s_iterator = IntSet_iterator(currentIS);
			while(IntSetIterator_has_next(current_s_iterator)) {
				int tempState = IntSetIterator_next(current_s_iterator);
				IntSet_union(dst, nfa->states[tempState].transitions[sym]);

				if(nfa->states[tempState].is_accepting) {
					currentState->is_accepting = TRUE; //if any state in the nfa is accepting, then states[i] should be too
				}
			}
			free(current_s_iterator);

			//printf("%c", sym);
			//IntSet_print(dst);

			int containInt = -1;
			int iterint = 0;
			LinkedListIterator *tempIterate = LinkedList_iterator(intsets);
			while(LinkedListIterator_has_next(tempIterate)) {
				IntSet *tempIntSet = LinkedListIterator_next(tempIterate);
				if(IntSet_equals(tempIntSet, dst)) {
					containInt=iterint;
				}
				iterint++;
			}
			free(tempIterate);

			//printf("if %d %d satisfied? \n", !IntSet_is_empty(dst), containInt == -1);

			if(containInt < 0 && (!(IntSet_is_empty(dst)))) {
				//printf("ok i got here \n");
				DFA_State *state_k = DFA_State_new();
				currentState->transitions[sym]=num_states;
				//printf("transition to %d on %c \n", num_states, sym);
				LinkedList_add_at_end(states, state_k);
				LinkedList_add_at_end(intsets, dst);
				num_states+=1;
			}

			else if(!(IntSet_is_empty(dst))) {
				currentState->transitions[sym]=containInt;
			}
		}
		DFA_State *nextState = LinkedListIterator_next(listIterate);
		IntSet *next = LinkedListIterator_next(isIterate);
	}

	DFA *new_DFA = DFA_new(num_states); //generates dfa
	LinkedListIterator *finalStates = LinkedList_iterator(states);
	for(int l=0; l<num_states; l++) { //copies over all of the states
		DFA_State *oneState = LinkedListIterator_next(finalStates);
		new_DFA->states[l] = *oneState;
		free(oneState);
	}

	free(finalStates);
	free(listIterate);
	free(isIterate);
	return new_DFA;
}

int main (int argc, char **argv) {

	printf("-----------------------------PROBLEM 1----------------------------- \n"); 

	printf("Problem 1 (a) \n");
    printf("Exactly the string \"ab\" \n");
	DFA *problem1_a = DFA_new(3);
	DFA_set_transition(problem1_a, 0, 'a', 1);
	DFA_set_transition(problem1_a, 1, 'b', 2);
	DFA_set_accepting(problem1_a, 2, TRUE);
    DFA_print(problem1_a);
    printf("Test for \"ab\": %d \n", DFA_execute(problem1_a, "ab"));
    printf("Test for \"cde\": %d \n", DFA_execute(problem1_a, "cde"));
    printf("Test for \"\": %d \n", DFA_execute(problem1_a, ""));

	printf("\n Problem 1 (b) \n");
    printf("Any string that starts with the characters \"ab\" \n");
	DFA *problem1_b = DFA_new(3);
	DFA_set_transition(problem1_b, 0, 'a', 1);
	DFA_set_transition(problem1_b, 1, 'b', 2);
	DFA_set_transition_all(problem1_b, 2, 2);
	DFA_set_accepting(problem1_b, 2, TRUE);
    DFA_print(problem1_b);
    printf("Test for \"ab\": %d \n", DFA_execute(problem1_b, "ab"));
    printf("Test for \"abcd\": %d \n", DFA_execute(problem1_b, "abcd"));
    printf("Test for \"cde\": %d \n", DFA_execute(problem1_b, "cde"));
    printf("Test for \"\": %d \n", DFA_execute(problem1_b, ""));


	printf("\n Problem 1 (c) \n");
    printf("Binary input with an even number of 1\'s \n");
	DFA *problem1_c = DFA_new(2);
	DFA_set_transition(problem1_c, 0, '1', 1);
	DFA_set_transition(problem1_c, 1, '1', 0);
	DFA_set_transition(problem1_c, 0, '0', 0);
	DFA_set_transition(problem1_c, 1, '0', 1);
	DFA_set_accepting(problem1_c, 0, TRUE);
    DFA_print(problem1_c);
    printf("Test for \"11\": %d \n", DFA_execute(problem1_c, "11"));
    printf("Test for \"1010101011\": %d \n", DFA_execute(problem1_c, "1010101011"));
    printf("Test for \"\": %d \n", DFA_execute(problem1_c, ""));
    printf("Test for \"1\": %d \n", DFA_execute(problem1_c, "1"));
    printf("Test for \"0001011\": %d \n", DFA_execute(problem1_c, "0001011"));

	printf("\n Problem 1 (d) \n");
    printf("Binary input with an even number of both 0\'s and 1\'s \n");
	DFA *problem1_d = DFA_new(4);
	DFA_set_transition(problem1_d, 0, '1', 1);
	DFA_set_transition(problem1_d, 0, '0', 2);
	DFA_set_transition(problem1_d, 1, '1', 0);
	DFA_set_transition(problem1_d, 1, '0', 3);
	DFA_set_transition(problem1_d, 2, '0', 0);
	DFA_set_transition(problem1_d, 2, '1', 3);
	DFA_set_transition(problem1_d, 3, '0', 1);
	DFA_set_transition(problem1_d, 3, '1', 2);
	DFA_set_accepting(problem1_d, 0, TRUE);
    DFA_print(problem1_d);
    printf("Test for \"11\": %d \n", DFA_execute(problem1_d, "11"));
    printf("Test for \"00\": %d \n", DFA_execute(problem1_d, "00"));
    printf("Test for \"1010101011\": %d \n", DFA_execute(problem1_d, "1010101011"));
    printf("Test for \"\": %d \n", DFA_execute(problem1_d, ""));
    printf("Test for \"1\": %d \n", DFA_execute(problem1_d, "1"));
    printf("Test for \"100\": %d \n", DFA_execute(problem1_d, "100"));
    printf("Test for \"101\": %d \n", DFA_execute(problem1_d, "101"));
    printf("Test for \"0001011\": %d \n", DFA_execute(problem1_d, "0001011"));

	printf("\n Problem 1 (e) \n");


	printf("\n-----------------------------PROBLEM 2----------------------------- \n");

	printf("Problem 2 (a) \n");
    printf("Strings ending in \"man\" \n");
	NFA *problem2_a = NFA_new(4);
    NFA_add_transition(problem2_a, 0,'m',1);
    NFA_add_transition(problem2_a, 1,'a',2);
    NFA_add_transition(problem2_a, 2,'n',3);
    NFA_add_transition_all(problem2_a, 0,0);
    NFA_set_accepting(problem2_a, 3, TRUE);
    printf("Test for \"man\": %d \n", NFA_execute(problem2_a, "man"));
    printf("Test for \"dczeman\": %d \n", NFA_execute(problem2_a, "dczeman"));
    printf("Test for \"manmann\": %d \n", NFA_execute(problem2_a, "manmann"));
    printf("Test for \"jwiefo\": %d \n", NFA_execute(problem2_a, "jwiefo"));
    printf("Test for \"\": %d \n", NFA_execute(problem2_a, ""));


    printf(" \n Problem 2 (b) \n");
    printf("Strings that contain more than one each of the letters in \"washington\" other than n, or more than two n\'s \n");

    NFA *problem2_b = NFA_new(20);
    NFA_add_transition_all(problem2_b, 0, 0);

    NFA_add_transition(problem2_b, 0, 'a', 1);
    NFA_add_transition_all(problem2_b, 1, 1);
    NFA_add_transition(problem2_b, 1, 'a', 2);
    NFA_add_transition_all(problem2_b, 2, 2);

    NFA_add_transition(problem2_b, 0, 'g', 3);
    NFA_add_transition_all(problem2_b, 3, 3);
    NFA_add_transition(problem2_b, 3, 'g', 4);
    NFA_add_transition_all(problem2_b, 4, 4);

    NFA_add_transition(problem2_b, 0, 'h', 5);
    NFA_add_transition_all(problem2_b, 5, 5);
    NFA_add_transition(problem2_b, 5, 'h', 6);
    NFA_add_transition_all(problem2_b, 6, 6);

    NFA_add_transition(problem2_b, 0, 'i', 7);
    NFA_add_transition_all(problem2_b, 7, 7);
    NFA_add_transition(problem2_b, 7, 'i', 8);
    NFA_add_transition_all(problem2_b, 8, 8);

    NFA_add_transition(problem2_b, 0, 'n', 9);
    NFA_add_transition_all(problem2_b, 9, 9);
    NFA_add_transition(problem2_b, 9, 'n', 10);
    NFA_add_transition_all(problem2_b, 10, 10);
    NFA_add_transition(problem2_b, 10, 'n', 11);
    NFA_add_transition_all(problem2_b, 11, 11);

    NFA_add_transition(problem2_b, 0, 'o', 12);
    NFA_add_transition_all(problem2_b, 12, 12);
    NFA_add_transition(problem2_b, 12, 'o', 13);
    NFA_add_transition_all(problem2_b, 13, 13);

    NFA_add_transition(problem2_b, 0, 's', 14);
    NFA_add_transition_all(problem2_b, 14, 14);
    NFA_add_transition(problem2_b, 14, 's', 15);
    NFA_add_transition_all(problem2_b, 15, 15);

    NFA_add_transition(problem2_b, 0, 't', 16);
    NFA_add_transition_all(problem2_b, 16, 16);
    NFA_add_transition(problem2_b, 16, 't', 17);
    NFA_add_transition_all(problem2_b, 17, 17);

    NFA_add_transition(problem2_b, 0, 'w', 18);
    NFA_add_transition_all(problem2_b, 18, 18);
    NFA_add_transition(problem2_b, 18, 'w', 19);
    NFA_add_transition_all(problem2_b, 19, 19);

    NFA_set_accepting(problem2_b, 2, TRUE);
    NFA_set_accepting(problem2_b, 4, TRUE);
    NFA_set_accepting(problem2_b, 6, TRUE);
    NFA_set_accepting(problem2_b, 8, TRUE);
    NFA_set_accepting(problem2_b, 11, TRUE);
    NFA_set_accepting(problem2_b, 13, TRUE);
    NFA_set_accepting(problem2_b, 15, TRUE);
    NFA_set_accepting(problem2_b, 17, TRUE);
    NFA_set_accepting(problem2_b, 19, TRUE);

    printf("Test for \"washingtonw\": %d \n", NFA_execute(problem2_b, "washingtonw"));
    printf("Test for \"axyzuioa\": %d \n", NFA_execute(problem2_b, "axyzuioa"));
    printf("Test for \"nann\": %d \n", NFA_execute(problem2_b, "nann"));
    printf("Test for \"washington\": %d \n", NFA_execute(problem2_b, "washington"));
    printf("Test for \"\": %d \n", NFA_execute(problem2_b, ""));

    printf("Problem 2 (c) \n");

    NFA *problem2_c = NFA_new(9);
    printf("a\'s, b\'s, and c\'s only, contains \"abc\", \"ba\" or \"bba\" anywhere\n");
    NFA_add_transition_str(problem2_c, 0, "abc", 0);
    NFA_add_transition(problem2_c, 0, 'a', 1);
    NFA_add_transition(problem2_c, 1, 'b', 2);
    NFA_add_transition(problem2_c, 2, 'c', 3);
    NFA_add_transition_str(problem2_c, 3, "abc", 3);
    NFA_add_transition(problem2_c, 0, 'b', 4);
    NFA_add_transition(problem2_c, 4, 'a', 5);
    NFA_add_transition_str(problem2_c, 5, "abc", 5);
    NFA_add_transition(problem2_c, 0, 'b', 6);
    NFA_add_transition(problem2_c, 6, 'b', 7);
    NFA_add_transition(problem2_c, 7, 'a', 8);
    NFA_add_transition_str(problem2_c, 8, "abc", 8);
    NFA_set_accepting(problem2_c, 3, TRUE);
    NFA_set_accepting(problem2_c, 5, TRUE);
    NFA_set_accepting(problem2_c, 8, TRUE);

    printf("Test for \"abc\": %d \n", NFA_execute(problem2_c, "abc"));
    printf("Test for \"ba\": %d \n", NFA_execute(problem2_c, "ba"));
    printf("Test for \"bba\": %d \n", NFA_execute(problem2_c, "bba"));
    printf("Test for \"cccabcccc\": %d \n", NFA_execute(problem2_c, "cccabcccc"));

    printf("Test for \"aacc\": %d \n", NFA_execute(problem2_c, "aacc"));
    printf("Test for \"abdddd\": %d \n", NFA_execute(problem2_c, "abdddd"));
    printf("Test for \"\": %d \n", NFA_execute(problem2_c, ""));


	printf("\n-----------------------------PROBLEM 3-----------------------------");

    printf("Conversion of problem 2_a \n");
    DFA *problem2_a_dfa = NFA_to_DFA(problem2_a);
    DFA_print(problem2_a_dfa);
    printf("Test for \"man\": %d \n", DFA_execute(problem2_a_dfa, "man"));
    printf("Test for \"dczeman\": %d \n", DFA_execute(problem2_a_dfa, "dczeman"));
    printf("Test for \"manmann\": %d \n", DFA_execute(problem2_a_dfa, "manmann"));
    printf("Test for \"jwiefo\": %d \n", DFA_execute(problem2_a_dfa, "jwiefo"));
    printf("Test for \"\": %d \n", DFA_execute(problem2_a_dfa, ""));


    /*printf("Conversion of problem 2_b \n");
    DFA *problem2_b_dfa = NFA_to_DFA(problem2_b);
    DFA_print(problem2_b_dfa);
    printf("Test for \"washingtonw\": %d \n", DFA_execute(problem2_b_dfa, "washingtonw"));
    printf("Test for \"axyzuioa\": %d \n", DFA_execute(problem2_b_dfa, "axyzuioa"));
    printf("Test for \"nann\": %d \n", DFA_execute(problem2_b_dfa, "nann"));
    printf("Test for \"washington\": %d \n", DFA_execute(problem2_b_dfa, "washington"));
    printf("Test for \"\": %d \n", DFA_execute(problem2_b_dfa, ""));*/

    printf("Conversion of problem 2_c \n");
    DFA *problem2_c_dfa = NFA_to_DFA(problem2_c);
    DFA_print(problem2_c_dfa);
    printf("Test for \"abc\": %d \n", DFA_execute(problem2_c_dfa, "abc"));
    printf("Test for \"ba\": %d \n", DFA_execute(problem2_c_dfa, "ba"));
    printf("Test for \"bba\": %d \n", DFA_execute(problem2_c_dfa, "bba"));
    printf("Test for \"cccabcccc\": %d \n", DFA_execute(problem2_c_dfa, "cccabcccc"));

    printf("Test for \"aacc\": %d \n", DFA_execute(problem2_c_dfa, "aacc"));
    printf("Test for \"abdddd\": %d \n", DFA_execute(problem2_c_dfa, "abdddd"));
    printf("Test for \"\": %d \n", DFA_execute(problem2_c_dfa, ""));



	return 0;
}