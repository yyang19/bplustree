#ifndef _HEADER_BPLUSTREE_
#define _HEADER_BPLUSTREE_

#define MAX_LEVEL (20)
enum {
    BPLUS_TREE_LEAF,
    BPLUS_TREE_NON_LEAF = 1,
};

enum {
    BORROW_FROM_LEFT,
    BORROW_FROM_RIGHT = 1,
};

typedef struct node {
    int type;
    int b_factor;
    int n;
    
    int *key;
    struct non_leaf *parent;
}node_t;

typedef struct non_leaf {
    node_t node;
    node_t **sub_ptr;
}nonleaf_t;

typedef struct leaf {
    node_t node;
    struct leaf *next;
    int *data;
}leaf_t;

struct tree {
    int b_factor;
    struct node *root;
    struct node *head[MAX_LEVEL]; //to be removed
};

typedef struct tree bpt_t;

bpt_t * bptInit( int );
int bptGet( bpt_t *, int );
void bptPut( bpt_t *, int, int );
void bptRemove( bpt_t *, int );
void bptDump( bpt_t * );
#endif
