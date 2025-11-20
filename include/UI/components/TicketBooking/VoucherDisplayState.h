#pragma once

#include <string>

struct VoucherDisplayState {
    std::string inputText;
    std::string appliedCode;
    std::string statusMessage;
    bool inputActive = false;
    bool caretVisible = false;
    bool userLoggedIn = false;
    bool statusIsError = false;
};
