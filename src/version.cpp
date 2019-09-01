#include "version.hpp"

#include <sstream>

namespace PixCraft {
	const int VERSION_MAJOR = 0;
	const int VERSION_MINOR = 0;
	const std::string COMMIT_HASH("dev");
	
	std::string getVersionString() {
		std::stringstream version;
		version << VERSION_MAJOR << "." << VERSION_MINOR << "." << COMMIT_HASH.substr(0, 8);
		return version.str();
	}
	
	const std::string VERSION_STR = getVersionString();
}