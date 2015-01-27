/********************************************
*
* Author: Yue Yang <yueyang2010@gmail.com>
*
********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "bplustree.h"

static int
key_binary_search(int *arr, int len, int key)
{
    int low = -1;
    int high = len;
    int mid;

    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (key > arr[mid]) 
            low = mid;
        else
            high = mid;
    }
    
    if (high >= len || arr[high] != key) 
        return -high - 1;
    else 
        return high;
}

static int
_nodeInit( node_t *new, bpt_t *tree, int type )
{
    int nKeys = tree->b_factor*2-1;

    new->type = type;
    new->b_factor = tree->b_factor;
    new->n = 0;
    new->parent = NULL;
    
    new->key =  (int *) malloc ( sizeof(int)*nKeys );
    memset( new->key, 0, nKeys * sizeof(int) );

    return 0;    
}

static void
_nodeDestroy( node_t *node )
{
    free( node->key );

    return;
}

static struct non_leaf *
non_leaf_new( bpt_t *tree )
{
    int nKeys = tree->b_factor*2-1;
    int nChildren = nKeys+1;

    nonleaf_t *new = (nonleaf_t *)malloc(sizeof(nonleaf_t));

    assert(new);

    _nodeInit( &new->node, tree, BPLUS_TREE_NON_LEAF );

    new->sub_ptr = (node_t **)malloc(nChildren * sizeof(node_t *)); 
    memset(new->sub_ptr, 0, nChildren * sizeof(node_t *));

    return new;
}

static void 
non_leaf_destroy( nonleaf_t *nonleaf )
{
    free( nonleaf->sub_ptr );
    _nodeDestroy( &nonleaf->node );

    return;
}

static leaf_t *
leaf_new( bpt_t *tree )
{
    int nKeys = tree->b_factor*2-1;

    leaf_t *new = (leaf_t*)malloc(sizeof(leaf_t));
    assert(new);

    _nodeInit( &new->node, tree, BPLUS_TREE_LEAF );

    new->data = (int *) malloc ( sizeof(int)*nKeys );
    assert( new->data );
    memset( new->data, 0, nKeys * sizeof(int) );

    new->next = NULL;
    new->node.type = BPLUS_TREE_LEAF;
    
    return new;
}

static void
leaf_destroy( leaf_t *leaf )
{
    free( leaf->data );
    _nodeDestroy( &leaf->node );

    return;
}

static int
_node_search( node_t *node, int key ){
    
    int i;

    nonleaf_t *nln;
    leaf_t *ln;
    node_t *child;

    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        i = key_binary_search(ln->node.key, ln->node.n, key);
        if (i >= 0)
            return ln->data[i];
        else 
            return 0;
    }

    assert( node->type == BPLUS_TREE_NON_LEAF );
    
    nln = (nonleaf_t *)node;
    i = key_binary_search(nln->node.key, nln->node.n-1, key);

    if(i >= 0)
        child = nln->sub_ptr[i + 1];
    else{
        i = -i - 1;
        child = nln->sub_ptr[i];
    }

    return _node_search( child, key );
}

static void
non_leaf_remove(bpt_t *tree, nonleaf_t *nl, int remove, int level)
{
    int i, j, k;
    nonleaf_t *sibling;

    if (nl->node.n <= tree->b_factor ) {
        nonleaf_t *parent = nl->node.parent;
        if (parent != NULL) {
            int borrow = 0;
            
            /* find which sibling node with same parent to be borrowed from */
            i = key_binary_search(parent->node.key, parent->node.n-1, nl->node.key[0]);
            assert(i < 0);
            i = -i - 1;
            if (i == 0) {
                /* no left sibling, choose right one */
                sibling = (nonleaf_t *)parent->sub_ptr[i + 1];
                borrow = BORROW_FROM_RIGHT;
            }else if (i == parent->node.n-1) {
                /* no right sibling, choose left one */
                sibling = (nonleaf_t *)parent->sub_ptr[i - 1];
                borrow = BORROW_FROM_LEFT;
            }else{
                nonleaf_t *l_sib = (nonleaf_t *)parent->sub_ptr[i - 1];
                nonleaf_t *r_sib = (nonleaf_t *)parent->sub_ptr[i + 1];
                /* if both left and right sibling found, choose the one with more n */
                sibling = l_sib->node.n >= r_sib->node.n ? l_sib : r_sib;
                borrow = l_sib->node.n >= r_sib->node.n ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
            }

            /* locate parent node key index */
            if (i > 0) 
                i = i - 1;

            if (borrow == BORROW_FROM_LEFT) {
               if (sibling->node.n > tree->b_factor ) {
                   /* node right shift */
                   for (j = remove; j > 0; j--) 
                       nl->node.key[j] = nl->node.key[j - 1];
                   
                   for (j = remove + 1; j > 0; j--) 
                       nl->sub_ptr[j] = nl->sub_ptr[j - 1];
                   
                   /* right rotate key */
                   nl->node.key[0] = parent->node.key[i];
                   parent->node.key[i] = sibling->node.key[sibling->node.n-2];

                   /* move left sibling's last sub-node into node's first location */
                   nl->sub_ptr[0] = sibling->sub_ptr[sibling->node.n - 1];
                   sibling->sub_ptr[sibling->node.n-1]->parent = nl;
                   sibling->node.n--;
               } else {
                   /* move parent key down */
                   sibling->node.key[sibling->node.n-1] = parent->node.key[i];
                   /* merge node and left sibling */

                   for (j = sibling->node.n, k = 0; k < nl->node.n - 1; k++) {
                       if (k != remove) {
                           sibling->node.key[j] = nl->node.key[k];
                           j++;
                       }
                   }

                   for (j = sibling->node.n, k = 0; k < nl->node.n-1; k++) {
                       if (k != remove + 1) {
                           sibling->sub_ptr[j] = nl->sub_ptr[k];
                           nl->sub_ptr[k]->parent = sibling;
                           j++;
                        }
                   }

                   sibling->node.n = j;

                   /* delete merged node */
                   //sibling->next = nl->next;
                   non_leaf_destroy(nl);
                   /* trace upwards */
                   non_leaf_remove(tree, parent, i, level + 1);
               }
            } else {
                    /* remove key first in case of overflow merging with sibling node */
                    for (; remove < nl->node.n - 2; remove++) {
                            nl->node.key[remove] = nl->node.key[remove + 1];
                            nl->sub_ptr[remove + 1] = nl->sub_ptr[remove + 2];
                    }
                    nl->node.n--;

                    if (sibling->node.n > tree->b_factor) {
                        /* left rotate key */
                        nl->node.key[nl->node.n - 1] = parent->node.key[i];
                        parent->node.key[i] = sibling->node.key[0];
                        nl->node.n++;

                        /* move right sibling's first sub-node into node's last location */
                        nl->sub_ptr[nl->node.n - 1] = sibling->sub_ptr[0];
                        sibling->sub_ptr[0]->parent = nl;

                        /* right sibling left shift */
                        for (j = 0; j < sibling->node.n - 2; j++) {
                            sibling->node.key[j] = sibling->node.key[j + 1];
                        }
                        for (j = 0; j < sibling->node.n - 1; j++) {
                            sibling->sub_ptr[j] = sibling->sub_ptr[j + 1];
                        }
                        sibling->node.n--;
                    } else {
                        /* move parent key down */
                        nl->node.key[nl->node.n - 1] = parent->node.key[i];
                        nl->node.n++;

                        /* merge node and right sibling */
                        for (j = nl->node.n - 1, k = 0; k < sibling->node.n - 1; j++, k++) {
                                nl->node.key[j] = sibling->node.key[k];
                        }
                        for (j = nl->node.n - 1, k = 0; k < sibling->node.n; j++, k++) {
                                nl->sub_ptr[j] = sibling->sub_ptr[k];
                                sibling->sub_ptr[k]->parent = nl;
                        }

                        nl->node.n = j;
                        /* delete merged sibling */
                        //nl->next = sibling->next;
                        non_leaf_destroy(sibling);
                        /* trace upwards */
                        non_leaf_remove(tree, parent, i, level + 1);
                    }
            }
            /* deletion finishes */
            return;

            } 
            else {
                if (nl->node.n == 2) {
                    /* delete old root node */
                    assert(remove == 0);
                    nl->sub_ptr[0]->parent = NULL;
                    tree->root = nl->sub_ptr[0];
                    //tree->head[level] = NULL;
                    non_leaf_destroy(nl);
                    return;
                }
            }
    }
    
    /* simple deletion */
    assert(nl->node.n > 2);
    for (; remove < nl->node.n - 2; remove++) {
        nl->node.key[remove] = nl->node.key[remove + 1];
        nl->sub_ptr[remove + 1] = nl->sub_ptr[remove + 2];
    }
    
    nl->node.n--;

    return;
}

static void
leaf_remove(bpt_t *tree, leaf_t *leaf, int key)
{
    int i, j, k;
    leaf_t *sibling;

    int remove = key_binary_search(leaf->node.key, leaf->node.n, key);
    
    if (remove < 0) 
        return;

    if (leaf->node.n <= leaf->node.b_factor) {
        nonleaf_t *parent = leaf->node.parent;
        if (parent != NULL) {
            int borrow = 0;
            
            /* find which sibling node with same parent to be borrowed from */
            i = key_binary_search(parent->node.key, parent->node.n-1, leaf->node.key[0]);
            if (i >= 0) {
                i = i + 1;
                if (i == parent->node.n-1){
                    /* the last node, no right sibling, choose left one */
                    sibling = (leaf_t *)parent->sub_ptr[i-1];
                    borrow = BORROW_FROM_LEFT;
                }  
                else {
                    leaf_t *l_sib = (leaf_t *)parent->sub_ptr[i - 1];
                    leaf_t *r_sib = (leaf_t *)parent->sub_ptr[i + 1];

                    /* if both left and right sibling found, choose the one with more entries */
                    sibling = l_sib->node.n>= r_sib->node.n ? l_sib : r_sib;
                    borrow = l_sib->node.n >= r_sib->node.n ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                }
             } else {
                i = -i - 1;
                if (i == 0) {
                    /* the frist node, no left sibling, choose right one */
                    sibling = (leaf_t *)parent->sub_ptr[i + 1];
                    borrow = BORROW_FROM_RIGHT;
                } else if (i == parent->node.n-1) {
                    /* the last node, no right sibling, choose left one */
                    sibling = (leaf_t *)parent->sub_ptr[i - 1];
                    borrow = BORROW_FROM_LEFT;
                } else {
                    leaf_t *l_sib = (leaf_t *)parent->sub_ptr[i - 1];
                    leaf_t *r_sib = (leaf_t *)parent->sub_ptr[i + 1];
                    /* if both left and right sibling found, choose the one with more entries */
                    sibling = l_sib->node.n >= r_sib->node.n ? l_sib : r_sib;
                    borrow = l_sib->node.n >= r_sib->node.n ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                }
             }

             /* locate parent node key index */
             if (i > 0)
                 i = i - 1;

             if (borrow == BORROW_FROM_LEFT) {
                 if (sibling->node.n > tree->b_factor) {
                     /* leaf node right shift */
                     parent->node.key[i] = sibling->node.key[sibling->node.n - 1];
                     for (; remove > 0; remove--) {
                         leaf->node.key[remove] = leaf->node.key[remove - 1];
                         leaf->data[remove] = leaf->data[remove - 1];
                     }
                     leaf->node.key[0] = sibling->node.key[sibling->node.n - 1];
                     leaf->data[0] = sibling->data[sibling->node.n - 1];
                     sibling->node.n--;
                     /* adjust parent key */
                     parent->node.key[i] = leaf->node.key[0];
                 } else {
                         /* merge leaf and left sibling */
                     for (j = sibling->node.n, k = 0; k < leaf->node.n; k++) {
                        if (k != remove) {
                            sibling->node.key[j] = leaf->node.key[k];
                            sibling->data[j] = leaf->data[k];
                            j++;
                        }
                     }
                     sibling->node.n = j;
                     /* delete merged leaf */
                     sibling->next = leaf->next;
                     leaf_destroy(leaf);
                     /* trace upwards */
                     non_leaf_remove(tree, parent, i, 1);
                 }
             } else {
                 /* remove entry first in case of overflow merging with sibling node */
                 for (; remove < leaf->node.n - 1; remove++) {
                         leaf->node.key[remove] = leaf->node.key[remove + 1];
                         leaf->data[remove] = leaf->data[remove + 1];
                 }
                 leaf->node.n--;
                 if (sibling->node.n > tree->b_factor ) {
                         /* borrow */
                         leaf->node.key[leaf->node.n] = sibling->node.key[0];
                         leaf->data[leaf->node.n] = sibling->data[0];
                         leaf->node.n++;
                         /* right sibling node left shift */
                         for (j = 0; j < sibling->node.n - 1; j++) {
                                 sibling->node.key[j] = sibling->node.key[j + 1];
                                 sibling->data[j] = sibling->data[j + 1];
                         }
                         sibling->node.n--;
                         /* adjust parent key */
                         parent->node.key[i] = sibling->node.key[0];
                 } else {
                         /* merge leaf node */
                         for(j = leaf->node.n, k = 0; k < sibling->node.n; j++, k++) {
                                 leaf->node.key[j] = sibling->node.key[k];
                                 leaf->data[j] = sibling->data[k];
                         }
                         leaf->node.n = j;
                         /* delete merged sibling */
                         leaf->next = sibling->next;
                         leaf_destroy(sibling);
                         /* trace upwards */
                         non_leaf_remove(tree, parent, i, 1);
                 }
             }
                    /* deletion finishes */
                    return;
            } else {
                if (leaf->node.n == 1) {
                    /* delete the only last node */
                    assert(key == leaf->node.key[0]);
                    tree->root = NULL;
                    tree->head[0] = NULL;
                    leaf_destroy(leaf);
                    return;
                }
            }
}

    /* simple deletion */
    for(; remove < leaf->node.n-1; remove++) {
        leaf->node.key[remove] = leaf->node.key[remove + 1];
        leaf->data[remove] = leaf->data[remove + 1];
    }

    leaf->node.n--;

    return;
}

void
bptDump(bpt_t *tree)
{
    int i;
    int key,data;
    node_t *node;
    leaf_t *leaf;

    if( !tree->root ){
        printf("Empty tree\n");
        return;
    }

    while( node->type == BPLUS_TREE_NON_LEAF )
        node = ((nonleaf_t *)node)->sub_ptr[0];
        
    assert( node->type == BPLUS_TREE_LEAF );

    leaf = (leaf_t *) node;

    while( leaf->next ){
        for(i=0; i<leaf->node.n; i++){
            key = leaf->node.key[i];
            data = leaf->data[i];
            printf("Key=%d, data=%d\n", key, data);                
        }

        leaf = leaf->next;
    }

    return;
}

int
bptGet( bpt_t *tree, int key )
{
    if( !tree->root )
        return 0;

    return _node_search( tree->root, key );
}

static void
_split_child( bpt_t *tree, node_t *node, int i )
{
    int j;
    int t = tree->b_factor;
    node_t *y, *z;
    nonleaf_t *nln = (nonleaf_t *)node;
    nonleaf_t *y_nln, *z_nln;
    leaf_t *z_ln;

    y = nln->sub_ptr[i];
    
    if( y->type == BPLUS_TREE_LEAF ){
        z_ln = leaf_new(tree);
        z = &z_ln->node;
    }
    else{
        z_nln = non_leaf_new(tree);
        z = &z_nln->node;
    }

    z->n = t-1;

    //move the largest t-1 keys and tchildren of y into z
    for( j=0; j<z->n; j++ )
        z->key[j] = y->key[t+j];

    if( y->type != BPLUS_TREE_LEAF ){
        y_nln = (nonleaf_t *)y;
        for( j=0; j<=z->n; j++ )
            z_nln->sub_ptr[j] = y_nln->sub_ptr[t+j];
    }
    
    y->n = t-1;

    //shift child pointers in node dest the right dest create a room for z
    
    for( j=node->n+1; j>i; j-- )
        nln->sub_ptr[j] = nln->sub_ptr[j-1];

    nln->sub_ptr[i+1] = z;

    for( j=node->n; j>i; j-- )
        node->key[j] = node->key[j-1];

    node->key[i] = y->key[t-1];
    node->n++;

    //NODE_WRITE(y);
    //NODE_WRITE(z);
    //NODE_WRITE(node);

}

static void
_insert_nonfull( bpt_t *tree, node_t *node, int key, int data )
{
    int i = node->n;
    nonleaf_t *nln;
    leaf_t *ln;
    
    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        while( i>=1 && key<node->key[i-1] ){
            node->key[i] = node->key[i-1];
            i--;    
        }

        node->key[i] = key;
        ln->data[i] = data;

        node->n ++;
        //NODE_WRITE(node);
    }
    else{
        assert( node->type == BPLUS_TREE_NON_LEAF );

        while( i>=1 && key<node->key[i-1] )
            i--;

        i++;
        //NODE_READ(node);
        
        nln = (nonleaf_t *)node;

        if( nln->sub_ptr[i-1]->n == tree->b_factor*2-1 ){
            _split_child( tree, node, i-1 );
            if( key>node->key[i-1] )
                i=i+1;
        }

        _insert_nonfull( tree, nln->sub_ptr[i-1], key, data );
    }
}


void
bptPut( bpt_t *tree, int key, int data)
{
    node_t *node = tree->root;
    nonleaf_t *s;

    if (tree->root == NULL) { //empty tree
        leaf_t *leaf = leaf_new(tree);
        tree->root = &leaf->node;
    }

    node = tree->root;

    if( node->n == tree->b_factor*2-1 ){
        s = non_leaf_new(tree);
        tree->root = &s->node;
        s->sub_ptr[0] = node;
        _split_child(tree,&s->node,0);
        _insert_nonfull(tree,&s->node,key,data);
    }
    else
        _insert_nonfull(tree,node,key,data);

    return;
}

static void
_remove( bpt_t *tree, node_t *node, int key )
{
    int i;
    nonleaf_t *nln;
    leaf_t *ln;
        
    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        leaf_remove(tree, ln, key);
        return;
    }

    assert( node->type == BPLUS_TREE_NON_LEAF );

    nln = (nonleaf_t *)node;
    i = key_binary_search(nln->node.key, nln->node.n - 1, key);
    
    if (i >= 0) {
        node = nln->sub_ptr[i + 1];
    } else {
        i = -i - 1;
        node = nln->sub_ptr[i];
    }

    return _remove( tree, node, key );

}

void
bptRemove( bpt_t *tree, int key ){
    
    if( !tree->root )
        printf("Empty tree! No deletion\n");
    else
        return _remove( tree, tree->root, key );
    
}

bpt_t *
bptInit( int b )
{
    bpt_t *t;

    assert( b>2 );

    t = ( bpt_t * ) malloc ( sizeof(bpt_t) );
    
    if( t ){
        t->b_factor = b;
        t->root = NULL;
    }
    
    //to be removed
    //memset(bplus_tree->head, 0, MAX_LEVEL * sizeof(struct node *));

    return t;
}

/* code backup 
 
static void
_insert( bpt_t *tree, node_t *node, int key, int data )
{
    int i;
    nonleaf_t *nln;
    leaf_t *ln;

    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        leaf_insert(tree, ln, key, data);
        return ;
    }
    
    assert( node->type == BPLUS_TREE_NON_LEAF );

    nln = (nonleaf_t *)node;
    i = key_binary_search(nln->node.key, nln->node.n - 1, key);
    if (i >= 0) 
        node = nln->sub_ptr[i + 1];
    else{ 
        i = -i - 1;
        node = nln->sub_ptr[i];
    }
    
    return _insert( tree, node, key, data );    
}

static void
leaf_insert(bpt_t *tree, leaf_t *leaf, int key, int data)
{
    int i, j, split = 0;
    leaf_t *sibling;

    int insert = key_binary_search(leaf->node.key, leaf->node.n, key);
    
    if (insert >= 0) {
        leaf->data[insert] = data;
        //NODE_WRITE( node );
        return;
    }

    insert = -insert - 1;

    if (leaf->node.n == tree->b_factor*2-1 ) {
        split = tree->b_factor;;
        
        sibling = leaf_new(tree);
        sibling->next = leaf->next;
        leaf->next = sibling;
        leaf->node.n = split;

        if (insert < split) {
            for (i = split-1, j=0; i<tree->b_factor*2-1; i++, j++) {
                sibling->node.key[j] = leaf->node.key[i];
                sibling->data[j] = leaf->data[i];
            }
            
            sibling->node.n = j;
            
            for (i = leaf->node.n; i>insert; i--) {
                leaf->node.key[i] = leaf->node.key[i - 1];
                leaf->data[i] = leaf->data[i - 1];
            }

            leaf->node.key[i] = key;
            leaf->data[i] = data;
        } else {
            i = split;
            j = 0;
            while (i < leaf->node.n ) {
                if (j != insert - split) {
                    sibling->node.key[j] = leaf->node.key[i];
                    sibling->data[j] = leaf->data[i];
                    i++;
                }
                j++;
            }
            
            if (j > insert - split) 
                sibling->node.n = j;
            else
                sibling->node.n = insert - split + 1;

            j = insert - split;
            sibling->node.key[j] = key;
            sibling->data[j] = data;
        }
    } else {
        for (i = leaf->node.n; i > insert; i--) {
            leaf->node.key[i] = leaf->node.key[i - 1];
            leaf->data[i] = leaf->data[i - 1];
        }
        leaf->node.key[i] = key;
        leaf->data[i] = data;
        leaf->node.n++;
    }

    if (split) {
        nonleaf_t *parent = leaf->node.parent;
        if (parent == NULL) {
            parent = non_leaf_new(tree);
            parent->node.key[0] = sibling->node.key[0];
            parent->sub_ptr[0] = (node_t *)leaf;
            parent->sub_ptr[1] = (node_t *)sibling;
            parent->node.n = 2;
            
            tree->root = (node_t *)parent;
            leaf->node.parent = parent;
            sibling->node.parent = parent;
        } else {
            sibling->node.parent = parent;
            non_leaf_insert(tree, parent, (node_t *)sibling, sibling->node.key[0], 1);
        }
    }
}
static void
non_leaf_insert( bpt_t *tree, nonleaf_t *nl, node_t *sub_node, int key, int level )
{
        int i, j, split_key;
        int split = 0;
        nonleaf_t *sibling;

        int insert = key_binary_search(nl->node.key, nl->node.n - 1, key);
        assert(insert < 0);
        insert = -insert - 1;

        if (nl->node.n == nl->node.b_factor*2+1) {
                split = nl->node.b_factor;
                sibling = non_leaf_new(tree);
                //sibling->next = nl->node.next;
                //nl->node.next = sibling;
                nl->node.n = split + 1;
                if (insert < split) {
                        i = split - 1, j = 0;
                        split_key = nl->node.key[i];
                        sibling->sub_ptr[j] = nl->sub_ptr[i + 1];
                        nl->sub_ptr[i + 1]->parent = sibling;
                        i++;
                        for (; i < nl->node.b_factor*2; i++, j++) {
                                sibling->node.key[j] = nl->node.key[i];
                                sibling->sub_ptr[j+1] = nl->sub_ptr[i+1];
                                nl->sub_ptr[i+1]->parent = sibling;
                        }
                        sibling->node.n = j + 1;
                        for (i = nl->node.n - 1; i > insert; i--) {
                                nl->node.key[i] = nl->node.key[i - 1];
                                nl->sub_ptr[i + 1] = nl->sub_ptr[i];
                        }
                        nl->node.key[i] = key;
                        nl->sub_ptr[i + 1] = sub_node;
                } else if (insert == split) {
                        i = split, j = 0;
                        split_key = key;
                        sibling->sub_ptr[j] = sub_node;
                        sub_node->parent = sibling;
                        i++;
                        for (; i < nl->node.b_factor*2; i++, j++) {
                                sibling->node.key[j] = nl->node.key[i];
                                sibling->sub_ptr[j + 1] = nl->sub_ptr[i + 1];
                                nl->sub_ptr[i + 1]->parent = sibling;
                        }
                        sibling->node.n = j + 1;
                } else {
                        i = split, j = 0;
                        split_key = nl->node.key[i];
                        sibling->sub_ptr[j] = nl->sub_ptr[i + 1];
                        nl->sub_ptr[i + 1]->parent = sibling;
                        i++;
                        while (i < nl->node.b_factor*2) {
                                if (j != insert - split) {
                                        sibling->node.key[j] = nl->node.key[i];
                                        sibling->sub_ptr[j + 1] = nl->sub_ptr[i + 1];
                                        nl->sub_ptr[i + 1]->parent = sibling;
                                        i++;
                                }
                                j++;
                        }
                        if (j > insert - split) {
                                sibling->node.n = j + 1;
                        } else {
                                sibling->node.n = insert - split + 1;
                        }
                        j = insert - split - 1;
                        sibling->node.key[j] = key;
                        sibling->sub_ptr[j + 1] = sub_node;
                        sub_node->parent = sibling;
                }
        } else {
                for (i = nl->node.n - 1; i > insert; i--) {
                        nl->node.key[i] = nl->node.key[i - 1];
                        nl->sub_ptr[i + 1] = nl->sub_ptr[i];
                }
                nl->node.key[i] = key;
                nl->sub_ptr[i + 1] = sub_node;
                nl->node.n++;
        }

        if (split) {
                nonleaf_t *parent = nl->node.parent;
                if (parent == NULL) {
                        parent = non_leaf_new(tree);
                        parent->node.key[0] = split_key;
                        parent->sub_ptr[0] = &nl->node;
                        parent->sub_ptr[1] = &sibling->node;
                        parent->node.n = 2;
                        tree->root = (node_t *)parent;
                        
                        //tree->head[level] = (struct node *)parent;
                        nl->node.parent = parent;
                        sibling->node.parent = parent;
                } else {
                        sibling->node.parent = parent;
                        non_leaf_insert(tree, parent, (node_t*)sibling, split_key, level + 1);
                }
        }
}

*/
