/**
 * @file utils.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <map>
#include <vector>

/**
 * Utility class that allows std::map initialization.
 * Required because !!!!STUPID!!!! MSVC11 doesn't support initializer lists
 */
template <typename K, typename T>
class create_map 
{
public:
	create_map(const K& key, const T& val) {
		map[key] = val;
	}

	create_map<K, T>& operator()(const K& key, const T& val) {
		map[key] = val;
		return *this;
	}

	operator std::map<K, T>() {
		return map;
	}
private:
	std::map<K, T> map;
};

/**
 * Utility class that allows std::vector initialization.
 * Required because !!!!STUPID!!!! MSVC11 doesn't support initializer lists
 */
template <typename T>
class create_vector
{
public:
	create_vector(const T& val) {
		vec.push_back(val);
	}

	create_vector<T>& operator()(const T& val) {
		vec.push_back(val);
		return *this;
	}

	operator std::vector<T>() {
		return vec;
	}
private:
	std::vector<T> vec;
};

struct Option
{
	Option() : hasArgument(false), isPresent(false) { }
	Option(const std::string& defArg) : hasArgument(true), argument(defArg), isPresent(false) { }

	bool hasArgument;
	std::string argument;

	bool isPresent;
};

typedef std::map<std::string, Option> OptionsMap;

void addOption(const std::string& option, const std::string& arg, OptionsMap& options);
std::vector<std::string> parseCmdline(int argc, char* argv[], OptionsMap& options);

#endif // !UTILS_H
