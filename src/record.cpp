#include <sstream>
#include <scl/record.h>

namespace scl {

std::string IRecord::ToString() const {
    std::stringstream ss;
    ss << CompileRecord(AsTokens()) << Message();
    return ss.str();
}

std::string IRecord::ToAlignedString() const {
    std::stringstream ss;
    ss << CompileRecord(AsAlignedTokens()) << Message();
    return ss.str();
}

std::string IRecord::CompileRecord(const AlignedTokenCont &aligned_tokens) {
    std::stringstream ss;
    for (const auto &[token, align_length] : aligned_tokens) {
        const auto token_length
            = align_length < token.size()
              ? align_length
              : token.size();

        const auto difference = align_length - token_length;
        const auto left_indent_length = difference / 2;
        const auto right_indent_length = left_indent_length + difference % 2;

        const std::string left_indent(left_indent_length, ' ');
        const std::string right_indent(right_indent_length, ' ');

        ss << left_indent << token.substr(0, token_length) << right_indent << " | ";
    }

    return ss.str();
}

std::string IRecord::CompileRecord(const TokenCont &tokens) {
    std::stringstream ss;
    for (const auto &token : tokens) {
        ss << token << " | ";
    }

    return ss.str();
}

}
