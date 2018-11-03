/**
 * File managing the parsing of a command line
 */

typedef struct command {

    /**
     * number of arguments
     */
    int argc;

    /**
     * values of arguments
     * index 0 is the name of the command
     */
    char **argv;

} command;

/**
 * types of binding between simple commands
 */
enum b_type
    {
     AND,
     OR,
     REDL,
     DREDL,
     REDL1,
     REDL2,
     REDR,
     DREDR,
     PIPE,
     SEMI
    };

/**
 * Element of the command_queue structure
 * double linked list to implement FIFO structure
 * with additionnal information on the transition to the next element
 */
typedef struct command_node {

    enum b_type type;

    command *cmd;
    
    struct command_node *next, *prev;
    
} command_node;
    
/**
 * Represents the full command line
 */
typedef struct command_queue {

    command_node *first, *last;
    
} command_queue;

/**
 * return the first node inserted into the queue
 * (the first readed while parsing)
 */
command_node* get(command_queue *queue);

/**
 * return 1 of the queue is empty
 * 0 otherwise
 */
short remain_command (command_queue *queue);


/**
 * Create a queue of commands from the string given by the user
 */
command_queue *command_parser (char *str);
