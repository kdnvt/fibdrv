#ifndef _FIB_BN_H
#define _FIB_BN_H
#include <linux/types.h>

typedef struct _bn {
    unsigned long long length;
    unsigned long long *num;
} bn_t;

bool bn_new(bn_t *bn_ptr, unsigned long long length);

bool bn_znew(bn_t *bn_ptr, unsigned long long length);

void bn_free(bn_t *bn_ptr);

bool bn_add(const bn_t *a, const bn_t *b, bn_t *res);

bool bn_sub(const bn_t *a, const bn_t *b, bn_t *res);
bool bn_toggle_move(const bn_t *a, bn_t *res);
bool bn_move(const bn_t *a, bn_t *res);
void bn_add_carry(const bn_t *b, bn_t *res, int carry);

void bn_swap(bn_t *a, bn_t *b);

void bn_mult(const bn_t *a, const bn_t *b, bn_t *res);

#endif /* _FIB_BN_H */