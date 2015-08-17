#ifndef CAF_PRACTICE_UTIL_HPP
#define CAF_PRACTICE_UTIL_HPP

#include <sstream>
#include <thread>

namespace std {

inline std::string to_string(const std::thread::id& tid) {
	std::stringstream ss;
	ss << tid;
	return ss.str();
}

}

#endif	// CAF_PRACTICE_UTIL_HPP
