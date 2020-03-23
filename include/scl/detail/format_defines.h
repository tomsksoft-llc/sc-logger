#pragma once

#include <type_traits>
#include <cstddef>

#include <scl/process_id.h>

namespace scl::detail {

/**
 * Number trait that provide the way to detect
 * number of digits of the maximum possible integer of the specified type.
 * @tparam T - integral type
 */
template<
    typename T,
    std::enable_if_t<std::is_integral_v<T>> * = nullptr // type T must be an integral
>
class NumberTrait {
private:
    /**
     * Detect count of digits of the number.
     * @param number - number for which we are looking for the count of digits
     * @return - count of digits of the number.
     */
    static constexpr std::size_t detect_digits_count(T number) {
        return number
               ? detect_digits_count(number / 10) + 1
               : 0;
    }

public:
    /**
     * Number of digits of the maximum possible integer of the specified T type.
     */
    static constexpr std::size_t max_digits_count = detect_digits_count(std::numeric_limits<T>::max());
};


namespace file_name_formatting {
// Specifier can be only '%t' or '%n' - only 2 characters
const std::size_t specifier_size = 2;

const char start_of_spec_subseq_k = '%';
const char current_time_spc_k = 't';
const char rotation_iteration_number_spc_k = 'n';
} // end of file_name_formatting


namespace log_formatting {
const auto time_format_k = "%Y-%m-%d-%H-%M-%S";

// Y-4, m-2, d-2, H-2, M-2, S-2
// '-' character occurs 5 times
const auto time_length_k = 19;

const auto pid_length_k = NumberTrait<ProcessId>::max_digits_count;
} // end of log_formatting

} // end of scl::detail

