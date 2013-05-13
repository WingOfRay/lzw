/**
 * @file utils.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "utils.h"

#include <stdexcept>
#include <string>

void addOption(const std::string& option, const std::string& arg, OptionsMap& options) {
	auto iter = options.find(option);
	if (iter == options.end())
		throw std::runtime_error("Unknown option \"" + option + "\"");

	iter->second.argument = arg;
	iter->second.isPresent = true;
}

std::vector<std::string> parseCmdline(int argc, char* argv[], OptionsMap& options) {
	std::vector<std::string> leftovers;
	bool expectArg = false;
	std::string curOpt;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		// option
		if (arg.size() > 1 && arg[0] == '-') {
			curOpt = arg.substr(1);

			// if option has argument parse it in next iteration otherwise add it now
			if (options.at(curOpt).hasArgument)
				expectArg = true;
			else
				addOption(curOpt, "", options);
			// argument
		} else {
			// if there was option
			if (expectArg) {
				addOption(curOpt, arg, options);
				expectArg = false;
			} else
				leftovers.push_back(arg);
		}
	}

	return leftovers;
}