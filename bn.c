#include <linux/slab.h>

#include "bn.h"
// cppcheck-suppress unusedFunction
bool bn_new(bn_t *bn_ptr, size_t size)
{
    bn_ptr->num = kmalloc(sizeof(unsigned long long) * size, GFP_KERNEL);
    return !!bn_ptr->num;
}
// cppcheck-suppress unusedFunction
void bn_free(bn_t *bn_ptr)
{
    kfree(bn_ptr->num);
    bn_ptr->num = NULL;
}