// Copyright 2026 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "yyjson.h"

// Fuzz the yyjson JSON parser.
// Exercises: read/parse path, value type queries, object/array
// traversal, number parsing, and write-back (serialisation).
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    yyjson_doc *doc = yyjson_read(
        reinterpret_cast<const char *>(data), size, 0);

    if (doc) {
        yyjson_val *root = yyjson_doc_get_root(doc);
        if (root) {
            // Exercise type checks.
            (void)yyjson_get_type(root);
            (void)yyjson_get_type_desc(root);

            if (yyjson_is_obj(root)) {
                yyjson_obj_iter iter;
                yyjson_obj_iter_init(doc, root, &iter);
                yyjson_val *key;
                while ((key = yyjson_obj_iter_next(&iter)) != nullptr) {
                    yyjson_val *val = yyjson_obj_iter_get_val(key);
                    (void)yyjson_get_str(key);
                    (void)yyjson_get_type(val);
                }
            } else if (yyjson_is_arr(root)) {
                yyjson_arr_iter iter;
                yyjson_arr_iter_init(doc, root, &iter);
                yyjson_val *val;
                while ((val = yyjson_arr_iter_next(&iter)) != nullptr) {
                    (void)yyjson_get_type(val);
                }
            }

            // Serialise back to string to exercise writer.
            yyjson_write_err werr;
            size_t wlen;
            char *json_str = yyjson_write_opts(
                doc, YYJSON_WRITE_NOFLAG, nullptr, &wlen, &werr);
            if (json_str) {
                free(json_str);
            }
        }
        yyjson_doc_free(doc);
    }

    return 0;
}
