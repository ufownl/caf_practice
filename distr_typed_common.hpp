#ifndef CAF_PRACTICE_DISTR_TYPED_COMMON_HPP
#define CAF_PRACTICE_DISTR_TYPED_COMMON_HPP

#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>
#include <thread>
#include "util.hpp"

using mirror_type = caf::typed_actor<caf::replies_to<std::string>::with<std::string>>;

#endif	// CAF_PRACTICE_DISTR_TYPED_COMMON_HPP
