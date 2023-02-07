
#include "../list.h"

#include <stdio.h>
#include <stdlib.h>

struct val_obj {
    int val;
    struct list_head head;
};

int main()
{
    struct list_head head;
    list_head_init(&head);

    struct val_obj *p;
    for (int i = 0; i < 100000; ++i) {
        p = malloc(sizeof(struct val_obj));
        p->val = i;
        /* list_head_init(&p->head); */
        list_add_head(&p->head, &head);
    }

    int sum = 0;

    list_foreach(p, &head, head) {
        sum += p->val;
    }

    /* for (int i = 0; i < 100000; ++i) */
    /*   { */
    /*     p = list_entry(head.next, struct val_obj, head); */
    /*     sum += p->val; */
    /*     list_del(head.next); */
    /*   } */

    int sum2 = 0;
    for (int i = 0; i< 100000; ++i) {
        sum2 += i;
    }

    printf("list sum1 : %d\n", sum);
    printf("list sum2 : %d\n", sum2);
}
