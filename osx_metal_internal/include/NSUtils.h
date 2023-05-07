#pragma once

#include <Foundation/Foundation.hpp>
#include <string>

namespace NSUtils {
    inline NS::String* createUtf8String(const char* str) {
        return NS::String::string(str, NS::StringEncoding::UTF8StringEncoding);
    }
}