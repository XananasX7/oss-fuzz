/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 *
 * OSS-Fuzz harness for Vim's regular expression engine (regexp.c).
 * Feeds arbitrary patterns and subjects into vim_regcomp/vim_regexec.
 */
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * We build Vim as a library. The entry points are:
 *   vim_regcomp(char_u *pattern, int magic)
 *   vim_regexec(regmatch_T *rmp, char_u *line, colnr_T col)
 *   vim_regfree(regprog_T *prog)
 */

/* Include minimal Vim types */
typedef unsigned char char_u;
typedef unsigned int colnr_T;

/* Forward declarations */
typedef struct regprog_S regprog_T;
typedef struct regmatch_S {
    regprog_T   *regprog;
    char_u      *startp[10];
    char_u      *endp[10];
    int          rm_ic;
} regmatch_T;

regprog_T *vim_regcomp(char_u *pattern, int magic);
int        vim_regexec(regmatch_T *rmp, char_u *line, colnr_T col);
void       vim_regfree(regprog_T *prog);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 2) return 0;

    /* Split: first byte is split point; first part = pattern, rest = subject */
    size_t split = data[0] % (size - 1) + 1;
    if (split >= size) split = size / 2;

    char *pattern = (char *)malloc(split + 1);
    char *subject = (char *)malloc(size - split + 1);
    if (!pattern || !subject) { free(pattern); free(subject); return 0; }

    memcpy(pattern, data + 1, split - 1);
    pattern[split - 1] = '\0';
    memcpy(subject, data + split, size - split);
    subject[size - split] = '\0';

    regprog_T *prog = vim_regcomp((char_u *)pattern, 1 /* magic */);
    if (prog) {
        regmatch_T rm;
        memset(&rm, 0, sizeof(rm));
        rm.regprog = prog;
        rm.rm_ic = 0;
        vim_regexec(&rm, (char_u *)subject, 0);
        vim_regfree(prog);
    }

    free(pattern);
    free(subject);
    return 0;
}
