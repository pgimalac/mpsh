typedef struct str_vec {
    int capacity;
    int size;
    char **tab;
} str_vec_t;

str_vec_t *create_str_vec();

void str_vec_add (str_vec_t *vec, char *str);

void str_vec_remove (str_vec_t *vec, int i);

struct str_list_node {
    char **value;
    struct str_list_node *next, *prev;
};

typedef struct str_list {
    int length;
    struct str_list_node *first;
    struct str_list_node *last;
} str_list_t;

str_list_t *create_str_list();

void add_first (str_list_t *lst, char **value);

void add_last (str_list_t *lst, char **value);

void free_str_list (str_list_t *lst);
