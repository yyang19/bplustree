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

static struct tree *bplus_tree;

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

static node_t *
node_new( bpt_t *tree, int type )
{
    node_t *new;
    int nKeys = tree->b_factor*2-1;

    struct node_t *new = (node_t *)malloc(sizeof(*node));

    assert(new != NULL);
    
    new->type = type;
    new->b_factor = tree->b_factor;
    new->node.n = 0;
    new->node.parent = NULL;
    
    new->key =  (int *) malloc ( sizeof(int)*nKeys );
    memset( new->key, 0, nKeys * sizeof(int) );

    return new;    
}

static void
node_destroy( node_t *node )
{
    free( node->key );
    free( node );

    return;
}

static struct non_leaf *
non_leaf_new( bpt_t *tree )
{
    int nKeys = tree->b_factor*2-1;
    int nChildren = nKeys+1;

    non_leaf_t *new = (nonleaf_t *)malloc(sizeof(nonleaf_t));

    assert(new);

    new->node = node_new( tree, BPLUS_TREE_NON_LEAF );

    assert(new->node);

    new->sub_ptr = (node_t **)malloc(nChildren * sizeof(node_t *)); 
    memset(node->sub_ptr, 0, nChildren * sizeof(node_t *));

    return new;
}

static struct leaf *
leaf_new( bpt_t *tree )
{
    int nKeys = tree->b_factor*2-1;

    struct leaf *new = malloc(sizeof(struct leaf));
    assert(new != NULL);

    new->node.type = BPLUS_TREE_LEAF;
    node->node.b_factor = tree->b_factor;
    new->node.parent = NULL;
    new->node.parent = NULL;

    node->key =  (int *) malloc ( sizeof(int)*nKeys );
    node->data = (int *) malloc ( sizeof(int)*nKeys );

    assert( node->key );
    assert( node->data );

    memset( node->data, 0, nKeys * sizeof(int) );

    node->parent = NULL;
    node->next = NULL;

    node->n = 0;
    
    return node;
}

static void
non_leaf_delete(struct non_leaf *node)
{
        free(node);
}

static void
leaf_delete(struct leaf *node)
{
        free(node);
}

int
bplus_tree_search(struct tree *tree, int key)
{
        int i;
        struct node *node = tree->root;
        struct non_leaf *nln;
        struct leaf *ln;

        while (node != NULL) {
                switch (node->type) {
                        case BPLUS_TREE_NON_LEAF:
                                nln = (struct non_leaf *)node;
                                i = key_binary_search(nln->key, nln->n - 1, key);
                                if (i >= 0) {
                                        node = nln->sub_ptr[i + 1];
                                } else {
                                        i = -i - 1;
                                        node = nln->sub_ptr[i];
                                }
                                break;
                        case BPLUS_TREE_LEAF:
                                ln = (struct leaf *)node;
                                i = key_binary_search(ln->key, ln->entries, key);
                                if (i >= 0) {
                                        return ln->data[i];
                                } else {
                                        return 0;
                                }
                        default:
                                assert(0);
                }
        }

        return 0;
}

static void
non_leaf_insert(struct tree *tree, struct non_leaf *node, struct node *sub_node, int key, int level)
{
        int i, j, split_key;
        int split = 0;
        struct non_leaf *sibling;

        int insert = key_binary_search(node->key, node->n - 1, key);
        assert(insert < 0);
        insert = -insert - 1;

        /* node full */
        if (node->n == ORDER) {
                /* split key index */
                split = (ORDER + 1) / 2;
                /* splited sibling node */
                sibling = non_leaf_new();
                sibling->next = node->next;
                node->next = sibling;
                node->n = split + 1;
                /* sibling node replication */
                if (insert < split) {
                        i = split - 1, j = 0;
                        split_key = node->key[i];
                        sibling->sub_ptr[j] = node->sub_ptr[i + 1];
                        node->sub_ptr[i + 1]->parent = sibling;
                        i++;
                        for (; i < ORDER - 1; i++, j++) {
                                sibling->key[j] = node->key[i];
                                sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                node->sub_ptr[i + 1]->parent = sibling;
                        }
                        sibling->n = j + 1;
                        /* node insertion and its n count stays unchanged(split + 1) */
                        for (i = node->n - 1; i > insert; i--) {
                                node->key[i] = node->key[i - 1];
                                node->sub_ptr[i + 1] = node->sub_ptr[i];
                        }
                        node->key[i] = key;
                        node->sub_ptr[i + 1] = sub_node;
                } else if (insert == split) {
                        i = split, j = 0;
                        split_key = key;
                        sibling->sub_ptr[j] = sub_node;
                        sub_node->parent = sibling;
                        i++;
                        for (; i < ORDER - 1; i++, j++) {
                                sibling->key[j] = node->key[i];
                                sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                node->sub_ptr[i + 1]->parent = sibling;
                        }
                        sibling->n = j + 1;
                } else {
                        i = split, j = 0;
                        split_key = node->key[i];
                        sibling->sub_ptr[j] = node->sub_ptr[i + 1];
                        node->sub_ptr[i + 1]->parent = sibling;
                        i++;
                        while (i < ORDER - 1) {
                                if (j != insert - split) {
                                        sibling->key[j] = node->key[i];
                                        sibling->sub_ptr[j + 1] = node->sub_ptr[i + 1];
                                        node->sub_ptr[i + 1]->parent = sibling;
                                        i++;
                                }
                                j++;
                        }
                        /* sibling node n count */
                        if (j > insert - split) {
                                sibling->n = j + 1;
                        } else {
                                sibling->n = insert - split + 1;
                        }
                        /* insert new key */
                        j = insert - split - 1;
                        sibling->key[j] = key;
                        sibling->sub_ptr[j + 1] = sub_node;
                        sub_node->parent = sibling;
                }
        } else {
                /* simple insertion */
                for (i = node->n - 1; i > insert; i--) {
                        node->key[i] = node->key[i - 1];
                        node->sub_ptr[i + 1] = node->sub_ptr[i];
                }
                node->key[i] = key;
                node->sub_ptr[i + 1] = sub_node;
                node->n++;
        }

        if (split) {
                struct non_leaf *parent = node->parent;
                if (parent == NULL) {
                        /* new parent */
                        parent = non_leaf_new();
                        parent->key[0] = split_key;
                        parent->sub_ptr[0] = (struct node *)node;
                        parent->sub_ptr[1] = (struct node *)sibling;
                        parent->n = 2;
                        /* new root */
                        tree->root = (struct node *)parent;
                        if (++level >= MAX_LEVEL) {
                                fprintf(stderr, "!!Panic: Level exceeded, please expand the non-leaf order or leaf capacity of the tree!\n");
                                assert(0);
                        }
                        tree->head[level] = (struct node *)parent;
                        node->parent = parent;
                        sibling->parent = parent;
                } else {
                        /* Trace upwards */
                        sibling->parent = parent;
                        non_leaf_insert(tree, parent, (struct node *)sibling, split_key, level + 1);
                }
        }
}

static void
leaf_insert(struct tree *tree, struct leaf *leaf, int key, int data)
{
        int i, j, split = 0;
        struct leaf *sibling;

        int insert = key_binary_search(leaf->key, leaf->entries, key);
        if (insert >= 0) {
                /* Already exists */
                return;
        }
        insert = -insert - 1;

        /* node full */
        if (leaf->entries == ENTRIES) {
                /* split = [m/2] */
                split = (ENTRIES + 1) / 2;
                /* splited sibling node */
                sibling = leaf_new();
                sibling->next = leaf->next;
                leaf->next = sibling;
                leaf->entries = split;
                /* sibling leaf replication */
                if (insert < split) {
                        for (i = split - 1, j = 0; i < ENTRIES; i++, j++) {
                                sibling->key[j] = leaf->key[i];
                                sibling->data[j] = leaf->data[i];
                        }
                        sibling->entries = j;
                        /* leaf insertion and its entry count stays unchanged(split + 1) */
                        for (i = leaf->entries; i > insert; i--) {
                                leaf->key[i] = leaf->key[i - 1];
                                leaf->data[i] = leaf->data[i - 1];
                        }
                        leaf->key[i] = key;
                        leaf->data[i] = data;
                } else {
                        i = split, j = 0;
                        while (i < ENTRIES) {
                                if (j != insert - split) {
                                        sibling->key[j] = leaf->key[i];
                                        sibling->data[j] = leaf->data[i];
                                        i++;
                                }
                                j++;
                        }
                        /* sibling leaf entries */
                        if (j > insert - split) {
                                sibling->entries = j;
                        } else {
                                sibling->entries = insert - split + 1;
                        }
                        /* insert new key */
                        j = insert - split;
                        sibling->key[j] = key;
                        sibling->data[j] = data;
                }
        } else {
                /* simple insertion */
                for (i = leaf->entries; i > insert; i--) {
                        leaf->key[i] = leaf->key[i - 1];
                        leaf->data[i] = leaf->data[i - 1];
                }
                leaf->key[i] = key;
                leaf->data[i] = data;
                leaf->entries++;
        }

        if (split) {
                struct non_leaf *parent = leaf->parent;
                if (parent == NULL) {
                        parent = non_leaf_new();
                        parent->key[0] = sibling->key[0];
                        parent->sub_ptr[0] = (struct node *)leaf;
                        parent->sub_ptr[1] = (struct node *)sibling;
                        parent->n = 2;
                        /* new root */
                        tree->root = (struct node *)parent;
                        tree->head[1] = (struct node *)parent;
                        leaf->parent = parent;
                        sibling->parent = parent;
                } else {
                        /* trace upwards */
                        sibling->parent = parent;
                        non_leaf_insert(tree, parent, (struct node *)sibling, sibling->key[0], 1);
                }
        }
}

void
_insert( bpt_t *tree, int key, int data )
{
    int i;
    struct node *node = tree->root;
    struct non_leaf *nln;
    struct leaf *ln;

    if (node == NULL) {
        struct leaf *leaf = leaf_new();
        leaf->key[0] = key;
        leaf->data[0] = data;
        leaf->entries = 1;
        tree->head[0] = (struct node *)leaf;
        tree->root = (struct node *)leaf;
        return;
    }

    while (node != NULL) {
            switch (node->type) {
                    case BPLUS_TREE_NON_LEAF:
                            nln = (struct non_leaf *)node;
                            i = key_binary_search(nln->key, nln->n - 1, key);
                            if (i >= 0) {
                                    node = nln->sub_ptr[i + 1];
                            } else {
                                    i = -i - 1;
                                    node = nln->sub_ptr[i];
                            }
                            break;
                    case BPLUS_TREE_LEAF:
                            ln = (struct leaf *)node;
                            leaf_insert(tree, ln, key, data);
                            return;
                    default:
                            break;
            }
    }
}

static void
non_leaf_remove(struct tree *tree, struct non_leaf *node, int remove, int level)
{
        int i, j, k;
        struct non_leaf *sibling;

        if (node->n <= (ORDER + 1) / 2) {
                struct non_leaf *parent = node->parent;
                if (parent != NULL) {
                        int borrow = 0;
                        /* find which sibling node with same parent to be borrowed from */
                        i = key_binary_search(parent->key, parent->n - 1, node->key[0]);
                        assert(i < 0);
                        i = -i - 1;
                        if (i == 0) {
                                /* no left sibling, choose right one */
                                sibling = (struct non_leaf *)parent->sub_ptr[i + 1];
                                borrow = BORROW_FROM_RIGHT;
                        } else if (i == parent->n - 1) {
                                /* no right sibling, choose left one */
                                sibling = (struct non_leaf *)parent->sub_ptr[i - 1];
                                borrow = BORROW_FROM_LEFT;
                        } else {
                                struct non_leaf *l_sib = (struct non_leaf *)parent->sub_ptr[i - 1];
                                struct non_leaf *r_sib = (struct non_leaf *)parent->sub_ptr[i + 1];
                                /* if both left and right sibling found, choose the one with more n */
                                sibling = l_sib->n >= r_sib->n ? l_sib : r_sib;
                                borrow = l_sib->n >= r_sib->n ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                        }

                        /* locate parent node key index */
                        if (i > 0) {
                                i = i - 1;
                        }

                        if (borrow == BORROW_FROM_LEFT) {
                                if (sibling->n > (ORDER + 1) / 2) {
                                        /* node right shift */
                                        for (j = remove; j > 0; j--) {
                                                node->key[j] = node->key[j - 1];
                                        }
                                        for (j = remove + 1; j > 0; j--) {
                                                node->sub_ptr[j] = node->sub_ptr[j - 1];
                                        }
                                        /* right rotate key */
                                        node->key[0] = parent->key[i];
                                        parent->key[i] = sibling->key[sibling->n - 2];
                                        /* move left sibling's last sub-node into node's first location */
                                        node->sub_ptr[0] = sibling->sub_ptr[sibling->n - 1];
                                        sibling->sub_ptr[sibling->n - 1]->parent = node;
                                        sibling->n--;
                                } else {
                                        /* move parent key down */
                                        sibling->key[sibling->n - 1] = parent->key[i];
                                        /* merge node and left sibling */
                                        for (j = sibling->n, k = 0; k < node->n - 1; k++) {
                                                if (k != remove) {
                                                        sibling->key[j] = node->key[k];
                                                        j++;
                                                }
                                        }
                                        for (j = sibling->n, k = 0; k < node->n - 1; k++) {
                                                if (k != remove + 1) {
                                                        sibling->sub_ptr[j] = node->sub_ptr[k];
                                                        node->sub_ptr[k]->parent = sibling;
                                                        j++;
                                                }
                                        }
                                        sibling->n = j;
                                        /* delete merged node */
                                        sibling->next = node->next;
                                        non_leaf_delete(node);
                                        /* trace upwards */
                                        non_leaf_remove(tree, parent, i, level + 1);
                                }
                        } else {
                                /* remove key first in case of overflow merging with sibling node */
                                for (; remove < node->n - 2; remove++) {
                                        node->key[remove] = node->key[remove + 1];
                                        node->sub_ptr[remove + 1] = node->sub_ptr[remove + 2];
                                }
                                node->n--;
                                if (sibling->n > (ORDER + 1) / 2) {
                                        /* left rotate key */
                                        node->key[node->n - 1] = parent->key[i];
                                        parent->key[i] = sibling->key[0];
                                        node->n++;
                                        /* move right sibling's first sub-node into node's last location */
                                        node->sub_ptr[node->n - 1] = sibling->sub_ptr[0];
                                        sibling->sub_ptr[0]->parent = node;
                                        /* right sibling left shift */
                                        for (j = 0; j < sibling->n - 2; j++) {
                                                sibling->key[j] = sibling->key[j + 1];
                                        }
                                        for (j = 0; j < sibling->n - 1; j++) {
                                                sibling->sub_ptr[j] = sibling->sub_ptr[j + 1];
                                        }
                                        sibling->n--;
                                } else {
                                        /* move parent key down */
                                        node->key[node->n - 1] = parent->key[i];
                                        node->n++;
                                        /* merge node and right sibling */
                                        for (j = node->n - 1, k = 0; k < sibling->n - 1; j++, k++) {
                                                node->key[j] = sibling->key[k];
                                        }
                                        for (j = node->n - 1, k = 0; k < sibling->n; j++, k++) {
                                                node->sub_ptr[j] = sibling->sub_ptr[k];
                                                sibling->sub_ptr[k]->parent = node;
                                        }
                                        node->n = j;
                                        /* delete merged sibling */
                                        node->next = sibling->next;
                                        non_leaf_delete(sibling);
                                        /* trace upwards */
                                        non_leaf_remove(tree, parent, i, level + 1);
                                }
                        }
                        /* deletion finishes */
                        return;
                } else {
                        if (node->n == 2) {
                                /* delete old root node */
                                assert(remove == 0);
                                node->sub_ptr[0]->parent = NULL;
                                tree->root = node->sub_ptr[0];
                                tree->head[level] = NULL;
                                non_leaf_delete(node);
                                return;
                        }
                }
        }
        
        /* simple deletion */
        assert(node->n > 2);
        for (; remove < node->n - 2; remove++) {
                node->key[remove] = node->key[remove + 1];
                node->sub_ptr[remove + 1] = node->sub_ptr[remove + 2];
        }
        node->n--;
}

static void
leaf_remove(struct tree *tree, struct leaf *leaf, int key)
{
        int i, j, k;
        struct leaf *sibling;

        int remove = key_binary_search(leaf->key, leaf->entries, key);
        if (remove < 0) {
                return;
        }

        if (leaf->entries <= (ENTRIES + 1) / 2) {
                struct non_leaf *parent = leaf->parent;
                if (parent != NULL) {
                        int borrow = 0;
                        /* find which sibling node with same parent to be borrowed from */
                        i = key_binary_search(parent->key, parent->n - 1, leaf->key[0]);
                        if (i >= 0) {
                                i = i + 1;
                                if (i == parent->n - 1) {
                                        /* the last node, no right sibling, choose left one */
                                        sibling = (struct leaf *)parent->sub_ptr[i - 1];
                                        borrow = BORROW_FROM_LEFT;
                                }  else {
                                        struct leaf *l_sib = (struct leaf *)parent->sub_ptr[i - 1];
                                        struct leaf *r_sib = (struct leaf *)parent->sub_ptr[i + 1];
                                        /* if both left and right sibling found, choose the one with more entries */
                                        sibling = l_sib->entries >= r_sib->entries ? l_sib : r_sib;
                                        borrow = l_sib->entries >= r_sib->entries ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                                }
                        } else {
                                i = -i - 1;
                                if (i == 0) {
                                        /* the frist node, no left sibling, choose right one */
                                        sibling = (struct leaf *)parent->sub_ptr[i + 1];
                                        borrow = BORROW_FROM_RIGHT;
                                } else if (i == parent->n - 1) {
                                        /* the last node, no right sibling, choose left one */
                                        sibling = (struct leaf *)parent->sub_ptr[i - 1];
                                        borrow = BORROW_FROM_LEFT;
                                } else {
                                        struct leaf *l_sib = (struct leaf *)parent->sub_ptr[i - 1];
                                        struct leaf *r_sib = (struct leaf *)parent->sub_ptr[i + 1];
                                        /* if both left and right sibling found, choose the one with more entries */
                                        sibling = l_sib->entries >= r_sib->entries ? l_sib : r_sib;
                                        borrow = l_sib->entries >= r_sib->entries ? BORROW_FROM_LEFT : BORROW_FROM_RIGHT;
                                }
                        }

                        /* locate parent node key index */
                        if (i > 0) {
                                i = i - 1;
                        }

                        if (borrow == BORROW_FROM_LEFT) {
                                if (sibling->entries > (ENTRIES + 1) / 2) {
                                        /* leaf node right shift */
                                        parent->key[i] = sibling->key[sibling->entries - 1];
                                        for (; remove > 0; remove--) {
                                                leaf->key[remove] = leaf->key[remove - 1];
                                                leaf->data[remove] = leaf->data[remove - 1];
                                        }
                                        leaf->key[0] = sibling->key[sibling->entries - 1];
                                        leaf->data[0] = sibling->data[sibling->entries - 1];
                                        sibling->entries--;
                                        /* adjust parent key */
                                        parent->key[i] = leaf->key[0];
                                } else {
                                        /* merge leaf and left sibling */
                                        for (j = sibling->entries, k = 0; k < leaf->entries; k++) {
                                                if (k != remove) {
                                                        sibling->key[j] = leaf->key[k];
                                                        sibling->data[j] = leaf->data[k];
                                                        j++;
                                                }
                                        }
                                        sibling->entries = j;
                                        /* delete merged leaf */
                                        sibling->next = leaf->next;
                                        leaf_delete(leaf);
                                        /* trace upwards */
                                        non_leaf_remove(tree, parent, i, 1);
                                }
                        } else {
                                /* remove entry first in case of overflow merging with sibling node */
                                for (; remove < leaf->entries - 1; remove++) {
                                        leaf->key[remove] = leaf->key[remove + 1];
                                        leaf->data[remove] = leaf->data[remove + 1];
                                }
                                leaf->entries--;
                                if (sibling->entries > (ENTRIES + 1) / 2) {
                                        /* borrow */
                                        leaf->key[leaf->entries] = sibling->key[0];
                                        leaf->data[leaf->entries] = sibling->data[0];
                                        leaf->entries++;
                                        /* right sibling node left shift */
                                        for (j = 0; j < sibling->entries - 1; j++) {
                                                sibling->key[j] = sibling->key[j + 1];
                                                sibling->data[j] = sibling->data[j + 1];
                                        }
                                        sibling->entries--;
                                        /* adjust parent key */
                                        parent->key[i] = sibling->key[0];
                                } else {
                                        /* merge leaf node */
                                        for (j = leaf->entries, k = 0; k < sibling->entries; j++, k++) {
                                                leaf->key[j] = sibling->key[k];
                                                leaf->data[j] = sibling->data[k];
                                        }
                                        leaf->entries = j;
                                        /* delete merged sibling */
                                        leaf->next = sibling->next;
                                        leaf_delete(sibling);
                                        /* trace upwards */
                                        non_leaf_remove(tree, parent, i, 1);
                                }
                        }
                        /* deletion finishes */
                        return;
                } else {
                        if (leaf->entries == 1) {
                                /* delete the only last node */
                                assert(key == leaf->key[0]);
                                tree->root = NULL;
                                tree->head[0] = NULL;
                                leaf_delete(leaf);
                                return;
                        }
                }
        }

        /* simple deletion */
        for (; remove < leaf->entries - 1; remove++) {
                leaf->key[remove] = leaf->key[remove + 1];
                leaf->data[remove] = leaf->data[remove + 1];
        }
        leaf->entries--;
}

void
bplus_tree_delete(struct tree *tree, int key)
{
        int i;
        struct node *node = tree->root;
        struct non_leaf *nln;
        struct leaf *ln;

        while (node != NULL) {
                switch (node->type) {
                        case BPLUS_TREE_NON_LEAF:
                                nln = (struct non_leaf *)node;
                                i = key_binary_search(nln->key, nln->n - 1, key);
                                if (i >= 0) {
                                        node = nln->sub_ptr[i + 1];
                                } else {
                                        i = -i - 1;
                                        node = nln->sub_ptr[i];
                                }
                                break;
                        case BPLUS_TREE_LEAF:
                                ln = (struct leaf *)node;
                                leaf_remove(tree, ln, key);
                                return;
                        default:
                                break;
                }
        }
}

void
bplus_tree_dump(struct tree *tree)
{
        int i, j;

        for (i = MAX_LEVEL; i > 0; i--) {
                struct non_leaf *node = (struct non_leaf *)tree->head[i];
                if (node != NULL) {
                        printf("LEVEL %d:\n", i);
                        while (node != NULL) {
                                printf("node: ");
                                for (j = 0; j < node->n - 1; j++) {
                                        printf("%d ", node->key[j]);
                                }
                                printf("\n");
                                node = node->next;
                        }
                }
        }

        struct leaf *leaf = (struct leaf *)tree->head[0];
        if (leaf != NULL) {
                printf("LEVEL 0:\n");
                while (leaf != NULL) {
                        printf("leaf: ");
                        for (j = 0; j < leaf->entries; j++) {
                                printf("%d ", leaf->key[j]);
                        }
                        printf("\n");
                        leaf = leaf->next;
                }
        } else {
                printf("Empty tree!\n");
        }
}

int
bptGet( bpt_t *t, int key )
{
    return bplus_tree_search(t, key); 
}

void
bptPut( bpt_t *t, int key, int data)
{
    _insert(t, key, data);
    return;
}

void
bptRemove( bpt_t *t, int key ){
    bplus_tree_delete(t, key);    
    return;
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
    memset(bplus_tree->head, 0, MAX_LEVEL * sizeof(struct node *));

    return t;
}

