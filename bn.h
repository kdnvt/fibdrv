#include <linux/types.h>

typedef struct _bn {
    size_t size;
    unsigned long long *num;
} bn_t;

bool bn_new(bn_t *bn_ptr, size_t size);

void bn_free(bn_t *bn_ptr);

void bn_add(const bn_t *bn1, const bn_t *bn2, bn_t *res);

void bn_sub(const bn_t *bn1, const bn_t *bn2, bn_t *res);

// ops == 0 for addition, 1 for subtraction
void bn_add_or_sub(const bn_t *bn1, const bn_t *bn2, bn_t *res, int op);

void bn_mult(const bn_t *bn1, const bn_t *bn2, bn_t *res);
