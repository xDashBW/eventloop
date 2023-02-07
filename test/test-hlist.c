
#include "../hlist.h"

#include <stdlib.h>
#include <stdio.h>

struct val_obj {
    int val;
    struct hlist_node head;
};

int main() {
    struct hlist_head head;
    hlist_head_init(&head);

    struct val_obj *p;
    for (int i = 0; i < 100000; ++i) {
        p = malloc(sizeof(struct val_obj));
        p->val = i;
        hlist_add_head(&p->head, &head);
    }

    int sum = 0;

    for (int i = 0; i < 100000; ++i) {
        p = hlist_entry(head.first, struct val_obj, head);
        sum += p->val;
        hlist_del(head.first);
    }

    int sum2 = 0;
    for (int i = 0; i< 100000; ++i) {
        sum2 += i;
    }

    printf("list sum1 : %d\n", sum);
    printf("list sum2 : %d\n", sum2);
}