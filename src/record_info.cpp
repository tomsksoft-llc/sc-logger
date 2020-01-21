#include <sstream>
#include <scl/record_info.h>

namespace scl {

const std::size_t max_tokens_count = 7;

struct AlignToken {
    AlignToken(const std::string &_token, std::size_t _align) : token(_token), align(_align) {}

    std::string token{};
    std::size_t align = 0;
};

using AlignedTokenCont = std::vector<AlignToken>;
using TokenCont = std::vector<std::string>;

std::string CompileRecord(const AlignedTokenCont &aligned_tokens) {
    std::stringstream ss;
    for (const auto &[token, align_length] : aligned_tokens) {
        const auto token_length
            = align_length < token.size()
              ? align_length
              : token.size();

        const auto difference = align_length - token_length;
        const std::string left_indent(difference, ' ');

        ss << left_indent << token.substr(0, token_length) << " | ";
    }

    return ss.str();
}

std::string CompileRecord(const TokenCont &tokens) {
    std::stringstream ss;
    for (const auto &token : tokens) {
        ss << token << " | ";
    }

    return ss.str();
}

AlignedTokenCont AsAlignedTokens(const RecordInfo &record, const AlignInfo &align_info) {
    namespace Fmt = detail::log_formatting;

    AlignedTokenCont result;
    result.reserve(max_tokens_count);

    result.emplace_back(record.time_str, Fmt::time_length_k);
    result.emplace_back(std::to_string(record.parent_pid), Fmt::pid_length_k);
    result.emplace_back(std::to_string(record.pid), Fmt::pid_length_k);

    if (record.session_id) {
        result.emplace_back(*record.session_id, align_info.session_id_length);
    }

    if (record.action) {
        result.emplace_back(*record.action, align_info.action_length);
    }

    return result;
}

TokenCont AsTokens(const RecordInfo &record) {
    TokenCont result;
    result.reserve(max_tokens_count);

    result.push_back(record.time_str);
    result.emplace_back(std::to_string(record.parent_pid));
    result.emplace_back(std::to_string(record.pid));

    if (record.session_id) {
        result.emplace_back(*record.session_id);
    }

    if (record.action) {
        result.emplace_back(*record.action);
    }

    return result;
}

std::string ToString(const RecordInfo &record, const AlignInfo &align_info) {
    std::stringstream ss;
    ss << CompileRecord(AsAlignedTokens(record, align_info)) << record.message;
    return ss.str();
}

std::string ToString(const RecordInfo &record) {
    std::stringstream ss;
    ss << CompileRecord(AsTokens(record)) << record.message;
    return ss.str();
}

}
