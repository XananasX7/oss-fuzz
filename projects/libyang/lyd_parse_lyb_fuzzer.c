/*
 * Copyright (C) 2024 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Fuzzer: lyd_parse_lyb_fuzzer
 *
 * Gap addressed: The existing JSON and XML data parsers are fuzzed, but the
 * LYB (LibYANG Binary) format has zero fuzzing coverage. LYB is a compact
 * binary wire format used to efficiently serialize YANG data trees.  Corrupted
 * LYB input can reach complex length-decoding and tree-reconstruction paths
 * not exercised by any text-format fuzzer.
 *
 * Strategy:
 *   1. Parse the fuzz input as a LYD_LYB binary document.
 *   2. On success, round-trip: print the tree back to LYB format using
 *      ly_out (to correctly retrieve the binary output length), then
 *      re-parse the printed bytes, exercising both printer and parser.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include "libyang.h"
#include "out.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    static struct ly_ctx *ctx = NULL;
    static bool initialized = false;

    /* One-time context + schema initialisation */
    if (!initialized) {
        ly_log_options(0);

        if (ly_ctx_new(LY_SRC_DIR "/modules", 0, &ctx) != LY_SUCCESS) {
            fprintf(stderr, "Failed to create libyang context\n");
            exit(EXIT_FAILURE);
        }

        /* Load a representative set of IETF modules so the parser has enough
         * schema context.  Failure here is non-fatal: the fuzzer still
         * exercises schema-less / opaque-node code paths. */
        lys_parse_path(ctx,
                       LY_SRC_DIR "/modules/ietf-inet-types@2025-12-22.yang",
                       LYS_IN_YANG, NULL);
        lys_parse_path(ctx,
                       LY_SRC_DIR "/modules/ietf-yang-types@2025-12-22.yang",
                       LYS_IN_YANG, NULL);

        initialized = true;
    }

    if (size == 0) {
        return 0;
    }

    /* --- Pass 1: write input to a temp file, then parse as LYB ---
     * ly_in_new_memory() only accepts null-terminated strings; LYB is
     * binary so we must go through an fd-backed ly_in instead.         */
    char tmppath[] = "/tmp/lyb_fuzz_XXXXXX";
    int tmpfd = mkstemp(tmppath);
    if (tmpfd < 0)
        return 0;
    if (write(tmpfd, data, size) != (ssize_t)size) {
        close(tmpfd);
        unlink(tmppath);
        return 0;
    }
    lseek(tmpfd, 0, SEEK_SET);

    struct ly_in *in = NULL;
    if (ly_in_new_fd(tmpfd, &in) != LY_SUCCESS) {
        close(tmpfd);
        unlink(tmppath);
        return 0;
    }

    struct lyd_node *tree = NULL;
    LY_ERR ret = lyd_parse_data(ctx, NULL, in, LYD_LYB,
                                LYD_PARSE_ONLY | LYD_PARSE_NO_STATE,
                                0, &tree);
    ly_in_free(in, 0);
    close(tmpfd);
    unlink(tmppath);

    if (ret != LY_SUCCESS || !tree) {
        return 0;
    }

    /* --- Pass 2: print back to LYB, then re-parse (roundtrip) ---
     * Use ly_out_new_memory / ly_out_printed() to get the real byte count
     * of the binary output, then feed it to a new fd-backed ly_in.      */
    char *buf = NULL;
    struct ly_out *out = NULL;
    if (ly_out_new_memory(&buf, 0, &out) == LY_SUCCESS) {
        if (lyd_print_all(out, tree, LYD_LYB, LYD_PRINT_SHRINK) == LY_SUCCESS) {
            size_t printed = ly_out_printed(out);
            if (printed > 0 && buf != NULL) {
                /* Write printed LYB to a temp file to get a binary-safe ly_in */
                char tmp2[] = "/tmp/lyb_rt_XXXXXX";
                int fd2 = mkstemp(tmp2);
                if (fd2 >= 0) {
                    if (write(fd2, buf, printed) == (ssize_t)printed) {
                        lseek(fd2, 0, SEEK_SET);
                        struct ly_in *in2 = NULL;
                        if (ly_in_new_fd(fd2, &in2) == LY_SUCCESS) {
                            struct lyd_node *tree2 = NULL;
                            lyd_parse_data(ctx, NULL, in2, LYD_LYB,
                                           LYD_PARSE_ONLY | LYD_PARSE_NO_STATE,
                                           0, &tree2);
                            ly_in_free(in2, 0);
                            lyd_free_all(tree2);
                        }
                    }
                    close(fd2);
                    unlink(tmp2);
                }
            }
        }
        ly_out_free(out, NULL, 1);  /* 1 = free the memory buffer */
    }

    lyd_free_all(tree);
    return 0;
}
