#include "version.hpp"

#include <sstream>

namespace PixCraft {
	std::string getVersionString() {
		std::stringstream version;
		version << VERSION_MAJOR << "." << VERSION_MINOR << "." << COMMIT_HASH;
		return version.str();
	}
}