#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define ORDER        7
#define ENTRIES       10
#define MAX_LEVEL     5

static void
_help( void ){
    printf( "B+ Tree package options:\n" );
    printf("\t args: [1] fan out t \n" );
}

int 
main( int argc, char *argv[] ){
{
    int result = 0;
    int t;
    
    if( argc!=2 ){
        _help();
        result = -1 ;
    }
    
    if( result == 0 ){
        t = atoi( argv[1] );
        bpt = init(r);
    }

    if( !bpt )
        result = -1 ;

    if( result == -1 )
        return result;
#if 0
        put(24, 24);
        put(72, 72);
        put(1, 1);
        put(39, 39);
        put(53, 53);
        put(63, 63);
        put(90, 90);
        put(88, 88);
        put(15, 15);
        put(10, 10);
        put(44, 44);
        put(68, 68);
        put(74, 74);
        bplus_tree_dump(bplus_tree);
#endif

#if 0
        put(10, 10);
        put(15, 15);
        put(18, 18);
        put(22, 22);
        put(27, 27);
        put(34, 34);
        put(40, 40);
        put(44, 44);
        put(47, 47);
        put(54, 54);
        put(67, 67);
        put(72, 72);
        put(74, 74);
        put(78, 78);
        put(81, 81);
        put(84, 84);
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
        put(90, 0);
        bplus_tree_dump(bplus_tree);
        put(88, 0);
        bplus_tree_dump(bplus_tree);
        put(74, 0);
        bplus_tree_dump(bplus_tree);
        put(72, 0);
        bplus_tree_dump(bplus_tree);
        put(68, 0);
        bplus_tree_dump(bplus_tree);
        put(63, 0);
        bplus_tree_dump(bplus_tree);
        put(53, 0);
        bplus_tree_dump(bplus_tree);
        put(44, 0);
        bplus_tree_dump(bplus_tree);
        put(39, 0);
        bplus_tree_dump(bplus_tree);
        put(24, 0);
        bplus_tree_dump(bplus_tree);
        put(15, 0);
        bplus_tree_dump(bplus_tree);
        put(10, 0);
        bplus_tree_dump(bplus_tree);
        put(1, 0);
        bplus_tree_dump(bplus_tree);
#endif

#define MAX_KEY  100

        int i;

/* Ordered insertion and deletion */
        for (i = 1; i <= MAX_KEY; i++) {
                put(i, i);
        }
        bplus_tree_dump(bplus_tree);

        for (i = 1; i <= MAX_KEY; i++) {
                put(i, 0);
        }
        bplus_tree_dump(bplus_tree);

/* Ordered insertion and reversed deletion */
        for (i = 1; i <= MAX_KEY; i++) {
                put(i, i);
        }
        bplus_tree_dump(bplus_tree);

        while (--i > 0) {
                put(i, 0);
        }
        bplus_tree_dump(bplus_tree);

/* Reversed insertion and ordered deletion */
        for (i = MAX_KEY; i > 0; i--) {
                put(i, i);
        }
        bplus_tree_dump(bplus_tree);

        for (i = 1; i <= MAX_KEY; i++) {
                put(i, 0);
        }
        bplus_tree_dump(bplus_tree);

/* Reversed insertion and reversed deletion */
        for (i = MAX_KEY; i > 0; i--) {
                put(i, i);
        }
        bplus_tree_dump(bplus_tree);

        for (i = MAX_KEY; i > 0; i--) {
                put(i, 0);
        }
        bplus_tree_dump(bplus_tree);

out:

    return result ;
}
