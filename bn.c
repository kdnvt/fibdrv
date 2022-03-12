#include "bn.h"
#include <linux/minmax.h>
#include <linux/slab.h>
// cppcheck-suppress unusedFunction
bool bn_new(bn_t *bn_ptr, unsigned long long length)
{
    bn_ptr->length = 0;
    if ((bn_ptr->num =
             kmalloc(sizeof(unsigned long long) * length, GFP_KERNEL)))
        bn_ptr->length = length;
    return !!bn_ptr->num;
}

bool bn_znew(bn_t *bn_ptr, unsigned long long length)
{
    bn_ptr->length = 0;
    if ((bn_ptr->num =
             kzalloc(sizeof(unsigned long long) * length, GFP_KERNEL)))
        bn_ptr->length = length;
    return !!bn_ptr->num;
}

// cppcheck-suppress unusedFunction
void bn_free(bn_t *bn_ptr)
{
    kfree(bn_ptr->num);
    bn_ptr->num = NULL;
}
// cppcheck-suppress unusedFunction
bool bn_add(const bn_t *a, const bn_t *b, bn_t *res)
{
    if (a->length < b->length ||
        (a->length == b->length &&
         b->num[b->length - 1] > a->num[a->length - 1]))
        swap(a, b);
    res->length = a->length + a->num[a->length - 1] > 0;
    bn_free(res);
    if (!bn_znew(res, res->length))
        return false;
    bn_move(a, res);
    bn_add_carry(b, res, 0);
    return true;
}

// The bn_t is a unsigned big number, so caller must make sure that
// a is bigger than b before calling bn_sub.
// cppcheck-suppress unusedFunction
bool bn_sub(const bn_t *a, const bn_t *b, bn_t *res)
{
    res->length = max(a->length, b->length);
    bn_free(res);
    if (!bn_znew(res, res->length))
        return false;
    if (a->length > b->length) {
        bn_toggle_move(a, res);
        bn_add_carry(b, res, 0);   // res = (~a+b) = b-a-1
        bn_toggle_move(res, res);  // res = ~(b-a-1) = (a-b+1) -1 = a-b
    } else {
        bn_toggle_move(b, res);
        bn_add_carry(a, res, 1);
    }
    return true;
}

bool bn_toggle_move(const bn_t *a, bn_t *res)
{
    if (a->length < res->length)
        return false;
    int i;
    for (i = 0; i < a->length; i++)
        res->num[i] = ~a->num[i];
    for (; i < res->length; i++)
        res->num[i] = ~res->num[i];
    return true;
}

bool bn_move(const bn_t *a, bn_t *res)
{
    if (a->length < res->length)
        return false;
    int i;
    for (i = 0; i < a->length; i++)
        res->num[i] = a->num[i];
    return true;
}

void bn_add_carry(const bn_t *b, bn_t *res, int carry)
{
    int i;
    /*
     * Use bit operation to check overflow.
     * There are four cases which overflow:
     *
     * msb b        : 0 1 1 1
     * msb res      : 1 0 1 1
     * msb b + res  : 0 0 0 1
     *
     * If overflow occur, it means the carry will
     * be one on next loop.
     */
    for (i = 0; i < b->length; i++) {
        int overflow = b->num[i] & res->num[i];
        int msb = b->num[i] | res->num[i];
        res->num[i] += b->num[i] + carry;
        msb &= ~res->num[i];
        overflow |= msb;
        carry = !!(overflow & 1ULL << 63);
    }
    for (; i < res->length && carry; i++) {
        int overflow = b->num[i] & res->num[i];
        int msb = b->num[i] | res->num[i];
        res->num[i] += carry;
        msb &= ~res->num[i];
        overflow |= msb;
        carry = !!(overflow & 1ULL << 63);
    }
}

void bn_swap(bn_t *a, bn_t *b)
{
    swap(a->length, b->length);
    swap(a->num, b->num);
}
