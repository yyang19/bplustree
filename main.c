#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

static void
_help( void ){
    printf( "B+ Tree options:\n" );
    printf("\t args: [1]branching factor r \n");
}

int main( int argc, char *argv[] ){
    
    int result = 0;
    int b;
    bpt_t *t; 

    if( argc!=2 ){
        _help();
        result = -1;
    }

    if( result == 0 ){
        b = atoi( argv[1] );
        t = bptInit(b);
    }

    if( !t )
        return -1;    

#if 0
     bptPut(t, 24, 24);
     bptPut(t, 72, 72);
     bptPut(t, 1, 1);
     bptPut(t, 39, 39);
     bptPut(t, 53, 53);
     bptPut(t, 63, 63);
     bptPut(t, 90, 90);
     bptPut(t, 88, 88);
     bptPut(t, 15, 15);
     bptPut(t, 10, 10);
     bptPut(t, 44, 44);
     bptPut(t, 68, 68);
     bptPut(t, 74, 74);
     bplus_tree_dump(bplus_tree);
#endif
#if 0
     bptPut(t, 10, 10);
     bptPut(t, 15, 15);
     bptPut(t, 18, 18);
     bptPut(t, 22, 22);
     bptPut(t, 27, 27);
     bptPut(t, 34, 34);
     bptPut(t, 40, 40);
     bptPut(t, 44, 44);
     bptPut(t, 47, 47);
     bptPut(t, 54, 54);
     bptPut(t, 67, 67);
     bptPut(t, 72, 72);
     bptPut(t, 74, 74);
     bptPut(t, 78, 78);
     bptPut(t, 81, 81);
     bptPut(t, 84, 84);
     bplus_tree_dump(bplus_tree);
#endif
#if 0
     printf("key:24 data:%d\n", get(24));
     printf("key:72 data:%d\n", get(72));
     printf("key:01 data:%d\n", get(1));
     printf("key:39 data:%d\n", get(39));
     printf("key:53 data:%d\n", get(53));
     printf("key:63 data:%d\n", get(63));
     printf("key:90 data:%d\n", get(90));
     printf("key:88 data:%d\n", get(88));
     printf("key:15 data:%d\n", get(15));
     printf("key:10 data:%d\n", get(10));
     printf("key:44 data:%d\n", get(44));
     printf("key:68 data:%d\n", get(68));
     printf("key:74 data:%d\n", get(74));
     printf("key:44 data:%d\n", get(44));
     printf("key:45 data:%d\n", get(45));
     printf("key:46 data:%d\n", get(46));
     printf("key:47 data:%d\n", get(47));
     printf("key:48 data:%d\n", get(48));
#endif
#if 0
     bptPut(t, 90, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 88, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 74, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 72, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 68, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 63, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 53, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 44, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 39, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 24, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 15, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 10, 0);
     bplus_tree_dump(bplus_tree);
     bptPut(t, 1, 0);
     bplus_tree_dump(bplus_tree);
#endif
#define MAX_KEY 100
     int i;
     /* Ordered insertion and deletion */
     for (i = 1; i <= MAX_KEY; i++) {
         bptPut(t, i, i);
     }
     bplus_tree_dump(bplus_tree);
     for (i = 1; i <= MAX_KEY; i++) {
         bptRemove(t, i);
     }
     bplus_tree_dump(bplus_tree);
     /* Ordered insertion and reversed deletion */
     for (i = 1; i <= MAX_KEY; i++) {
         bptPut(t, i, i);
     }
     bplus_tree_dump(bplus_tree);
     while (--i > 0) {
         bptRemove(t, i);
     }
     bplus_tree_dump(bplus_tree);
     /* Reversed insertion and ordered deletion */
     for (i = MAX_KEY; i > 0; i--) {
         bptPut(t, i, i);
     }
     bplus_tree_dump(bplus_tree);
     for (i = 1; i <= MAX_KEY; i++) {
         bptRemove(t, i);
     }
     bplus_tree_dump(bplus_tree);
     /* Reversed insertion and reversed deletion */
     for (i = MAX_KEY; i > 0; i--) {
         bptPut(t, i, i);
     }
     bplus_tree_dump(bplus_tree);
     for (i = MAX_KEY; i > 0; i--) {
         bptRemove(t, i);
     }
     bplus_tree_dump(bplus_tree);
     return 0;
}

    return result;
}

