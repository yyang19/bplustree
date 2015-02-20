/*  bplustree.c
 *  Author: Yue Yang ( yueyang2010@gmail.com )
 *
 *
* Copyright (c) 2015, Yue Yang ( yueyang2010@gmail.com )
*  * All rights reserved.
*  *
*  - Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*  Redistributions of source code must retain the above copyright notice,
*  this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright
*  notice, this list of conditions and the following disclaimer in the
*  documentation and/or other materials provided with the distribution.
*
*  - Neither the name of Redis nor the names of its contributors may be used
*  to endorse or promote products derived from this software without
*  specific prior written permission.
*  
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*                          
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "bplustree.h"

static void _descend( bpt_t *tree, node_t *node, int key );

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
    
    new->key =  (int *) malloc ( sizeof(int)*nKeys );
    memset( new->key, 0xff, nKeys * sizeof(int) );

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

    new->children = (node_t **)malloc(nChildren * sizeof(node_t *)); 
    memset(new->children, 0, nChildren * sizeof(node_t *));

    return new;
}

static void 
non_leaf_destroy( nonleaf_t **nonleaf )
{
    free( (*nonleaf)->children );
    _nodeDestroy( &(*nonleaf)->node );
    free( *nonleaf );
    *nonleaf = NULL;

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
    memset( new->data, 0xff, nKeys * sizeof(int) );

    new->next = NULL;
    new->node.type = BPLUS_TREE_LEAF;
    
    return new;
}

static void
leaf_destroy( leaf_t **leaf )
{
    free( (*leaf)->data );
    _nodeDestroy( &(*leaf)->node );
    free( *leaf );
    *leaf = NULL;

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
        i = key_binary_search(ln->node.key, ln->node.n, key );
        if (i >= 0)
            return ln->data[i];
        else 
            return DATA_NOT_EXIST;
    }

    assert( node->type == BPLUS_TREE_NON_LEAF );
    
    nln = (nonleaf_t *)node;
    i = key_binary_search(nln->node.key, nln->node.n, key );

    if(i >= 0)
        child = nln->children[i];
    else{
        i = -i - 1;
        child = nln->children[i];
    }

    return _node_search( child, key );
}

void
bptDump(bpt_t *tree)
{
    int i;
    node_t *node;
    leaf_t *leaf;

    printf("*******Tree dump*******\n");

    if( !tree->root ){
        printf("Empty tree\n");
        return;
    }

    node = tree->root;

    while( node )
        if( node->type == BPLUS_TREE_NON_LEAF )
            node = ((nonleaf_t *)node)->children[0];
        else
            break;
        
    assert( node->type == BPLUS_TREE_LEAF );

    leaf = (leaf_t *) node;

    while( leaf ){
        for(i=0; i<leaf->node.n; i++)
            printf("Key=%d, data=%d\n", leaf->node.key[i], leaf->data[i] );  

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
    leaf_t *y_ln, *z_ln;

    y = nln->children[i];
    
    if( y->type == BPLUS_TREE_LEAF ){
        y_ln = (leaf_t *)y;
        z_ln = leaf_new(tree);
        z = &z_ln->node;
    }
    else{
        y_nln = (nonleaf_t *)y;
        z_nln = non_leaf_new(tree);
        z = &z_nln->node;
    }

    z->n = t-1;

    //move the largest t-1 keys and tchildren of y into z
    for( j=0; j<z->n; j++ )
        z->key[j] = y->key[t+j];
    

    if( y->type != BPLUS_TREE_LEAF ){
        for( j=0; j<=z->n; j++ )
            z_nln->children[j] = y_nln->children[t+j];
    }
    else{
        for( j=0; j<z->n; j++ )
            z_ln->data[j] = y_ln->data[t+j];
    }

    if( y->type == BPLUS_TREE_LEAF ){
        y->n = t;
        z_ln->next = y_ln->next;
        y_ln->next = z_ln;    
    }
    else
        y->n = t-1;

    //shift child pointers in node dest the right dest create a room for z
    
    for( j=node->n+1; j>i; j-- )
        nln->children[j] = nln->children[j-1];

    nln->children[i+1] = z;

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
            ln->data[i] = ln->data[i-1];
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

        if( nln->children[i-1]->n == tree->b_factor*2-1 ){
            _split_child( tree, node, i-1 );
            if( key>node->key[i-1] )
                i=i+1;
        }

        _insert_nonfull( tree, nln->children[i-1], key, data );
    }
}


void
bptPut( bpt_t *tree, int key, int data)
{
    node_t *node;
    nonleaf_t *s;

    if (tree->root == NULL) { //empty tree
        leaf_t *leaf = leaf_new(tree);
        tree->root = &leaf->node;
    }

    node = tree->root;

    if( node->n == tree->b_factor*2-1 ){
        s = non_leaf_new(tree);
        tree->root = &s->node;
        s->children[0] = node;
        _split_child(tree,&s->node,0);
        _insert_nonfull(tree,&s->node,key,data);
    }
    else
        _insert_nonfull(tree,node,key,data);

    return;
}

static void 
_node_key_shift_left( node_t *node, int index, int ptr_shift) 
{
    int idx;
    nonleaf_t *nln;
    leaf_t *ln;
    
    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        for( idx=index; idx<node->n-1; idx++ ){
            node->key[idx] = node->key[idx+1];
            ln->data[idx] = ln->data[idx+1];
        }

    }
    else{ 

        nln = (nonleaf_t *)node;

        for( idx=index; idx<node->n-1; idx++ ){
            node->key[idx] = node->key[idx+1];
            if( ptr_shift )
                nln->children[idx]=nln->children[idx+1];
        }

        if( ptr_shift )
            nln->children[node->n-1]=nln->children[node->n];
    }

    node->n--;
}

static void 
_node_key_shift_right( node_t *node, int index, int ptr_shift )
{

    int idx;
    nonleaf_t *nln;
    leaf_t *ln;
    
    assert( index<=node->n );
    
    if( node->type == BPLUS_TREE_LEAF ){
        ln = (leaf_t *)node;
        for( idx=node->n; idx>index; idx-- ){
            node->key[idx] = node->key[idx-1];
            ln->data[idx] = ln->data[idx-1];
        }
    }
    else{
        nln = (nonleaf_t *)node;

        for( idx=node->n; idx>index; idx-- ){
            node->key[idx] = node->key[idx-1];
            if( ptr_shift )
                nln->children[idx+1]=nln->children[idx]; 
        } 
 
        if( ptr_shift )
            nln->children[idx+1]=nln->children[idx]; 
    }
    
    node->key[index] = INVALID_KEY;
    
    node->n++;
}

static void 
_move_key( int key, node_t *src, node_t *dest, int src_ptr_shift, int dest_ptr_shift )
{
    int src_pos = 0;
    int dest_pos = 0;

    while( src_pos<src->n && key>src->key[src_pos] )
        src_pos++;
    
    while( dest_pos<dest->n && key>dest->key[dest_pos] )
        dest_pos++;
    
    _node_key_shift_right( dest, dest_pos, dest_ptr_shift );
    
    dest->key[ dest_pos ] = src->key[src_pos]; 

    if( dest->type == BPLUS_TREE_LEAF )
        ((leaf_t *)dest)->data[ dest_pos ] = ((leaf_t *)src)->data[src_pos]; 
    
    _node_key_shift_left( src, src_pos, src_ptr_shift );
    
    //DISK_WRITE(src);
    //DISK_WRITE(dest);
}

static void 
_merge_node( node_t *left, node_t *right )
{
    int k;
    nonleaf_t *l_nln, *r_nln;
    leaf_t *l_ln, *r_ln;
    
    assert( left->type == right->type );

    if( left->type == BPLUS_TREE_LEAF ){
        l_ln = (leaf_t *) left;
        r_ln = (leaf_t *) right;
    }
    else{
        l_nln = (nonleaf_t *) left;
        r_nln = (nonleaf_t *) right;
    }

    if( left->type == BPLUS_TREE_LEAF ){
        for( k=0; k<right->n; k++ ){
            left->key[left->n+k] = right->key[k];
            l_ln->data[left->n+k] = r_ln->data[k];
        }
    }
    else{ 
        for( k=0; k<right->n; k++ ){
            left->key[left->n+k] = right->key[k];
            l_nln->children[left->n+k] = r_nln->children[k];
        }
    
        l_nln->children[left->n+right->n] = r_nln->children[right->n];
    }
    
    left->n += right->n;

    if( left->type == BPLUS_TREE_LEAF )
        l_ln->next = r_ln->next;
    
    if( right->type==BPLUS_TREE_NON_LEAF )
        non_leaf_destroy(&r_nln);
    else
        leaf_destroy(&r_ln);
    
    //DISK_WRITE(left);
    //DISK_WRITE(right);
}

static void
_remove_from_leaf( node_t *node, int idx )
{
    int i = idx;
    leaf_t *ln = (leaf_t *)node;
    
    assert( node->type == BPLUS_TREE_LEAF);
    assert( idx<node->n );
    
    while( i<node->n-1 ){
        node->key[i] = node->key[i+1];
        ln->data[i] = ln->data[i+1];
        i++;
    }
    
    node->n--;
    
    return;
}

static node_t *
_pre_descend_child( node_t *parent, int idx )
{
    node_t *child = NULL;
    node_t *lsibling = NULL;
    node_t *rsibling = NULL;
    nonleaf_t *nln_parent;
    nonleaf_t *nln_child, *nln_lsibling, *nln_rsibling;

    int parent_key, predecessor_key, successor_key;
    
    int t = parent->b_factor;

    nln_parent = (nonleaf_t *)parent;

    child = nln_parent->children[idx]; 
    
    if( child->n>t-1 ) // not a minimal node
        return child;

    if ( idx == parent->n ){   
        lsibling = nln_parent->children[idx-1];
        rsibling = NULL; 
    }else if ( idx == 0 ) {
        lsibling = NULL;
        rsibling = nln_parent->children[1];
    }else {
        lsibling = nln_parent->children[idx - 1]; 
        rsibling = nln_parent->children[idx + 1];
    }
    
    if( lsibling && lsibling->n > t-1 ){ 
        assert(idx>0);
        
        predecessor_key = lsibling->key[lsibling->n-1];

        if( child->type == BPLUS_TREE_LEAF ){
            parent->key[idx-1] = lsibling->key[lsibling->n-2];
            _move_key( predecessor_key, lsibling, child, 1, 1 );
        }
        else{
            parent_key = parent->key[idx-1];
            if( parent_key != child->key[0] )
                _move_key( parent_key, parent, child, 0, 1 );//move 1-level down
            //parent->key[idx-1] = lsibling->key[lsibling->n-1];

            nln_child = (nonleaf_t *)child;
            nln_lsibling = (nonleaf_t *)lsibling;
            nln_child->children[0] = nln_lsibling->children[lsibling->n];
            
            _move_key( predecessor_key, lsibling, parent, 0, 0 );
        }
    }
    else if( rsibling && rsibling->n > t-1 ){
        
        successor_key = rsibling->key[0];

        if( child->type == BPLUS_TREE_LEAF ){
            parent->key[idx] = successor_key;
            _move_key( successor_key, rsibling, child, 1, 1 );
        }
        else{
            parent_key = parent->key[idx];
            if( parent_key != child->key[child->n-1] )
                _move_key( parent_key, parent, child, 0, 1 );//move 1-level down
            //parent->key[idx] = successor_key;

            nln_child = (nonleaf_t *)child;
            nln_rsibling = (nonleaf_t *)rsibling;
            nln_child->children[child->n] = nln_rsibling->children[0];
            
            _move_key( successor_key, rsibling, parent, 1, 0 );
        }

    }
    else if( lsibling && (lsibling->n == t-1) ){
        assert(idx>0);
        
        if( child->type == BPLUS_TREE_NON_LEAF ){
            parent_key = parent->key[idx-1];
            if( parent_key != lsibling->key[lsibling->n-1] )
                _move_key( parent_key, parent, lsibling, 1, 0 );//move 1-level down
        }
        else
            _node_key_shift_left( parent, idx-1, 1 );

        _merge_node( lsibling, child );
        nln_parent->children[idx-1] = lsibling;
        
        child = lsibling;
    }
    else if( rsibling && (rsibling->n == t-1) ){
        
        if( child->type == BPLUS_TREE_NON_LEAF ){
            parent_key = parent->key[idx];
            if( parent_key != child->key[child->n-1] )
                _move_key( parent_key, parent, child, 1, 0 );//move 1-level down
        }
        else
            _node_key_shift_left( parent, idx, 1 );

        _merge_node( child, rsibling );
        nln_parent->children[idx] = child;
    }
    else
        assert(0);
    
    return child;
}


static void
_descend( bpt_t *tree, node_t *node, int key )
{
    int i;
    nonleaf_t *nln;
    leaf_t *ln;
    node_t *child;

    i = key_binary_search(node->key, node->n, key);

    if( i>= 0 ){ //key found in node 
        if( node->type == BPLUS_TREE_LEAF ){
            _remove_from_leaf( node, i );
            if( node->n==0 && node==tree->root ){
                ln = (leaf_t *)node;
                leaf_destroy(&ln);
                tree->root = NULL;
            }
            return;
        }
    }
        
    //key not found in the node
    if( node->type == BPLUS_TREE_LEAF ){
        printf(" The key %d does not exist in the tree\n", key );
        return;
    }
    
    assert( node->type == BPLUS_TREE_NON_LEAF );

    nln = (nonleaf_t *)node;
    
    if (i < 0) 
        i = -i - 1;

    child = _pre_descend_child( node, i );
    
    _descend( tree, child, key );
    
    if( node->n==0 && node==tree->root ){
        non_leaf_destroy(&nln);
        tree->root = child;
    }
    
    return;
}

void
bptRemove( bpt_t *tree, int key ){
    
    if( !tree->root )
        printf("Empty tree! No deletion\n");
    else
        return _descend( tree, tree->root, key );
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

void
bptDestroy( bpt_t *tree ){

    if( tree )
        free(tree);
}

#ifdef DEBUG
static void
_dump( node_t *node ){

    int i;

    if( !node ){
        printf("NULL node");
        return;
    }

    if( node->type == BPLUS_TREE_LEAF )
        printf("Type: leaf\n" );
    else if( node->type == BPLUS_TREE_NON_LEAF )
        printf("Type: non-leaf\n" );
    else
        assert(0);

    printf("B Factor = %d\n", node->b_factor );
    printf("# keys = %d\n", node->n );
    
    for( i=0; i<node->n; i++){
        printf("keys[%d] = %d", i, node->key[i] );
        if( node->type == BPLUS_TREE_LEAF )
            printf("\t data[%d] = %d\n", i, ((leaf_t *)node)->data[i] );
        else
            printf("\n");
    }

    return;

}


void _d( bpt_t *t )
{
    int i,j;
    int ichildren;
    node_t *node;
    leaf_t *leaf;
    nonleaf_t *nln;

    if( !t )
        return;
    
    node = t->root;

    printf( "Level root: ");
    for( i = 0; i<node->n; i++ ){
        printf(" <%d> ", node->key[i]);
    }
    printf( "\n");

    if( node->n > 0 ){
        nln = (nonleaf_t *)node;
        ichildren = node->n;
        printf( "Level 2: ");
        for( i = 0; i<=ichildren; i++ ){
            node = nln->children[i];
            for( j = 0; j<node->n; j++ ){
                printf(" <%d> ", node->key[j]);
            }
            printf( "|\t");
        }
        printf( "\n");
    }
    
    printf( "Leaf level: ");
    node = t->root;

    while( node->type == BPLUS_TREE_NON_LEAF )
        node = ((nonleaf_t *)node)->children[0];
        
    assert( node->type == BPLUS_TREE_LEAF );

    leaf = (leaf_t *) node;

    while( leaf ){
        for(i=0; i<leaf->node.n; i++){
            printf(" <%d,%d> ", leaf->node.key[i], leaf->data[i]);                
        }
        printf( "|\t");
        leaf = leaf->next;
    }
    printf("\n");
}

#endif
