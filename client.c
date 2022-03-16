#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define RUNS 50
#define OFFSET 100

int bn_to_string(unsigned long long *bn, int bn_len, char **str_ptr);

void reverse(char *str, int len);

int main()
{
    long long sz;

    unsigned long long buf[200];
    char write_buf[] = "testing writing";
    int offset = OFFSET; /* TODO: try test something bigger than the limit */

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        sz = write(fd, write_buf, strlen(write_buf));
        printf("Writing to " FIB_DEV ", returned the sequence %lld\n", sz);
    }
    int kt[OFFSET][RUNS] = {};
    unsigned long ut[OFFSET][RUNS] = {};
    int k_to_ut[OFFSET][RUNS] = {};
    unsigned long total[OFFSET][RUNS] = {};
    FILE *kt_fp = fopen("ktime.txt", "w");
    FILE *k_to_u_fp = fopen("k_to_utime.txt", "w");
    FILE *ut_fp = fopen("utime.txt", "w");
    FILE *total_fp = fopen("total_time.txt", "w");
    for (int j = 0; j < RUNS; j++) {
        for (int i = 0; i < offset; i++) {
            lseek(fd, i, SEEK_SET);
            sz = read(fd, buf, 200 * sizeof(unsigned long long));
            char *str;
            struct timespec tp_start, tp_end;
            clock_gettime(CLOCK_MONOTONIC, &tp_start);
            bn_to_string(buf, sz / sizeof(*buf), &str);
            clock_gettime(CLOCK_MONOTONIC, &tp_end);
            free(str);
            ut[i][j] = tp_end.tv_nsec - tp_start.tv_nsec;

            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%s.\n",
                   i, str);
            lseek(fd, 0, SEEK_SET);
            kt[i][j] = write(fd, write_buf, strlen(write_buf));
            lseek(fd, 1, SEEK_SET);
            k_to_ut[i][j] = write(fd, write_buf, strlen(write_buf));
            total[i][j] = kt[i][j] + k_to_ut[i][j] + ut[i][j];
        }
    }
    for (int i = 0; i < offset; i++) {
        for (int j = 0; j < RUNS; j++) {
            fprintf(ut_fp, "%lu ", ut[i][j]);
            fprintf(kt_fp, "%d ", kt[i][j]);
            fprintf(k_to_u_fp, "%d ", k_to_ut[i][j]);
            fprintf(total_fp, "%lu ", total[i][j]);
        }
        fprintf(kt_fp, "\n");
        fprintf(ut_fp, "\n");
        fprintf(k_to_u_fp, "\n");
        fprintf(total_fp, "\n");
    }
    fclose(ut_fp);
    fclose(kt_fp);
    fclose(k_to_u_fp);
    fclose(total_fp);
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
            if (bn[i] & (1ULL << j))
                carry = 1;

            for (int k = 0; k < len && k < total_len; k++) {
                str[k] += str[k] + carry;
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