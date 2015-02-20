/*  bplustree.h
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


#ifndef _HEADER_BPLUSTREE_
#define _HEADER_BPLUSTREE_

#define MAX_LEVEL (20)
#define KEY_NOT_FOUND (-1)
#define DATA_NOT_EXIST (-1)
#define INVALID_KEY (-1)
enum {
    BPLUS_TREE_LEAF,
    BPLUS_TREE_NON_LEAF = 1,
};

enum {
    BORROW_FROM_LEFT,
    BORROW_FROM_RIGHT = 1,
};

typedef struct node {
    int *key;
    int type;
    int n;

    int wr_count;
}node_t;

typedef struct non_leaf {
    node_t node;
    node_t **children;
}nonleaf_t;

typedef struct leaf {
    node_t node;
    struct leaf *next;
    int *data;
}leaf_t;

struct tree {
    int b_factor;
    struct node *root;

    int leaf_size;
    int nonleaf_size;
};

typedef struct tree bpt_t;

bpt_t * bptInit( int );
void bptDestroy( bpt_t * );
int bptGet( bpt_t *, int );
void bptPut( bpt_t *, int, int );
void bptRemove( bpt_t *, int );
void bptDump( bpt_t * );
#endif
