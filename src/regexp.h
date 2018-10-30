/**
 * This file implements regular expression matching.
 *
 */

#define CHAR_SIZE 256

/**
 * Structure representing a state of a
 * finite deterministic automata.
 */
typedef struct state {
    int is_terminal;

    /**
     * nexts[c] = d means that from this state, 
     * letter c goes to state d.
     * null if there is no transition for the given char
     */
    struct state* nexts[CHAR_SIZE];
} state;


/**
 * Test if a string match the given regular expression
 */
int match (const char* expr, state* inital_state);

