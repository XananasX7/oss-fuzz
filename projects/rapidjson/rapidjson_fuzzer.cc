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
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// Fuzz the RapidJSON parser.
// Exercises: DOM parse, in-situ parse, SAX parse, value traversal,
// serialisation via Writer and PrettyWriter, and error handling.

// SAX handler that discards all events (exercises SAX parse path).
struct DiscardHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>,
                                          DiscardHandler> {
    bool Default() { return true; }
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    // --- DOM parse (copy-mode) ---
    {
        rapidjson::Document doc;
        std::string s(reinterpret_cast<const char *>(data), size);
        doc.Parse(s.c_str(), s.size());
        if (!doc.HasParseError()) {
            // Traverse top-level members.
            if (doc.IsObject()) {
                for (auto it = doc.MemberBegin();
                     it != doc.MemberEnd(); ++it) {
                    (void)it->name.GetString();
                    (void)it->value.GetType();
                }
            } else if (doc.IsArray()) {
                for (rapidjson::SizeType i = 0; i < doc.Size(); i++) {
                    (void)doc[i].GetType();
                }
            }

            // Serialise back with both writers.
            {
                rapidjson::StringBuffer sb;
                rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
                doc.Accept(writer);
            }
            {
                rapidjson::StringBuffer sb;
                rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
                doc.Accept(writer);
            }
        }
    }

    // --- In-situ parse (operates on a mutable copy) ---
    {
        std::string in_situ(reinterpret_cast<const char *>(data), size);
        in_situ.push_back('\0');
        rapidjson::Document doc;
        doc.ParseInsitu(in_situ.data());
        (void)doc.HasParseError();
    }

    // --- SAX parse ---
    {
        std::string s(reinterpret_cast<const char *>(data), size);
        rapidjson::Reader reader;
        rapidjson::StringStream ss(s.c_str());
        DiscardHandler handler;
        reader.Parse(ss, handler);
    }

    return 0;
}
