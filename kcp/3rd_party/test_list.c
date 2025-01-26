/*************************************************************************
    > File Name: test_list.c
    > Author: hsz
    > Brief:
    > Created Time: Fri 24 Jan 2025 03:52:18 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#include "list.h"

typedef struct _test_list {
    struct list_head node;
    int a;
} test_list_t;

int main(int argc, char **argv)
{
    test_list_t *test_list = (test_list_t *)malloc(sizeof(test_list_t));
    list_init(&test_list->node);
    for (int32_t i = 0; i < 10; i++) {
        test_list_t *tmp = (test_list_t *)malloc(sizeof(test_list_t));
        tmp->a = i + 1;
        list_add_tail(&tmp->node, &test_list->node);
    }

    test_list_t *test_list_2 = (test_list_t *)malloc(sizeof(test_list_t));
    list_init(&test_list_2->node);

    test_list_t *pos = NULL;
    test_list_t *next = NULL;
    list_for_each_entry_safe(pos, next, &test_list->node, node) {
        printf("a = %d\n", pos->a);
        list_move_tail(&pos->node, &test_list_2->node);
    }

    printf("-------------------\n");

    list_for_each_entry(pos, &test_list_2->node, node) {
        printf("a = %d\n", pos->a);
    }

    return 0;
}
