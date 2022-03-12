#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int bn_to_string(unsigned long long *bn, int bn_len, char **str_ptr);

void reverse(char *str, int len);

int main()
{
    long long sz;

    unsigned long long buf[200];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 200 * sizeof(unsigned long long));
        char *str;
        bn_to_string(buf, sz / sizeof(*buf), &str);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, str);
    }

    for (int i = offset; i >= 0; i--) {
        lseek(fd, i, SEEK_SET);
        sz = read(fd, buf, 200 * sizeof(unsigned long long));
        char *str;
        bn_to_string(buf, sz / sizeof(*buf), &str);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, str);
    }

    // bn_to_string(buf)

    close(fd);
    return 0;
}

int bn_to_string(unsigned long long *bn, int bn_len, char **str_ptr)
{
    int width = sizeof(*bn) * 8;
    int bn_bits = bn_len * width;
    int len = bn_bits / 3 + 2;
    int total_len = 1;
    char *str = calloc(len, 1);
    if (!str)
        return 0;

    for (int i = bn_len - 1; i >= 0; i--) {
        for (int j = width - 1; j >= 0; j--) {
            int carry = 0;
            for (int k = 0; k < len && k < total_len; k++) {
                str[k] += str[k];
            }
            if (bn[i] & (1ULL << j))
                carry = 1;

            for (int k = 0; k < len && k < total_len; k++) {
                str[k] += carry;
                carry = 0;
                if (str[k] > 9) {
                    carry = 1;
                    str[k] -= 10;
                }
            }
            if (carry)
                str[total_len++] = 1;
        }
    }
    for (int k = 0; k < total_len; k++) {
        str[k] += '0';
    }

    reverse(str, total_len);
    *str_ptr = str;
    return len;
}

void reverse(char *str, int len)
{
    int half = len / 2;
    for (int i = 0; i < half; i++) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}