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
#include <cstring>
#include <string>

#include "tinyxml2.h"

// Fuzz the tinyxml2 XML parser.
// Exercises: Parse, element/attribute traversal, text retrieval,
// printing back to string, and error handling paths.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    // Create a null-terminated copy of the input.
    std::string input(reinterpret_cast<const char *>(data), size);

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(input.c_str(), input.size());

    if (err == tinyxml2::XML_SUCCESS) {
        // Walk the document tree to exercise element/attribute accessors.
        tinyxml2::XMLElement *root = doc.RootElement();
        if (root) {
            // Traverse first-level children.
            for (tinyxml2::XMLElement *child = root->FirstChildElement();
                 child != nullptr;
                 child = child->NextSiblingElement()) {
                // Access text content.
                const char *text = child->GetText();
                (void)text;

                // Iterate attributes.
                for (const tinyxml2::XMLAttribute *attr = child->FirstAttribute();
                     attr != nullptr;
                     attr = attr->Next()) {
                    (void)attr->Name();
                    (void)attr->Value();
                }
            }

            // Exercise ShallowClone and DeepClone.
            tinyxml2::XMLDocument doc2;
            tinyxml2::XMLNode *clone = root->ShallowClone(&doc2);
            (void)clone;
        }

        // Print the document back to a string (tests serialisation path).
        tinyxml2::XMLPrinter printer;
        doc.Print(&printer);
    }

    return 0;
}
