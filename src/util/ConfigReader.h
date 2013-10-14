/*
 * Author: guoning
 * Date:   2009.07.31
 */

#ifndef __CONFIG_READER_H__
#define __CONFIG_READER_H__

#include <string>
#include <map>
#include <fstream>

namespace utils {

typedef std::map<std::string, std::string> SessionMap;
typedef std::string SessionKey;

class ConfigReader : public std::map<SessionKey, SessionMap>
{
public:
	ConfigReader() {}
	ConfigReader(const char *pConfig) { open(pConfig); }
	virtual ~ConfigReader() {}

	bool init(const char *pConfig) { open(pConfig); return true; }
	bool has(
			const SessionKey &session,
			const SessionMap::key_type &option = SessionMap::key_type()) const
	{
		std::map<SessionKey, SessionMap>::const_iterator it = this->find(session);
		return it != this->end() && (
				option.empty() || it->second.find(option) != it->second.end());
	}

private:
	int open(const char *pConfig)
	{
		std::ifstream ifile(pConfig);
		if (!ifile) return 1;
		std::string line, key, value;

		SessionKey _key;
		SessionMap _session;
		SessionMap _varMap;

		while (getline(ifile, line)) {
			if (line[0] == '[' && line[line.length() - 1]  == ']') {
				if (!_session.empty() && !_key.empty()) {
					this->insert(make_pair(_key, _session));	
				}
				_key = line.substr(1, line.length() - 2);
				_session.clear();
			} else if (line[0] == '[' || line[0] == '#')
				continue;
			else if (separateString(line, key, value) == 0) {
				if (key[0] == '$')
					_varMap.insert(make_pair(key, value));
				else
					_session.insert(make_pair(key, replaceVariable(value, _varMap)));
			}
		}
		if (!_session.empty() && !_key.empty()) {
			this->insert(make_pair(_key, _session));
		}
		return 0;
	}

	const std::string replaceVariable(const std::string &src, const SessionMap &varmap) const
	{
		std::string result = src;
		SessionMap::const_iterator it = varmap.begin();
		while (it != varmap.end()) {
			for (it = varmap.begin(); it != varmap.end(); ++ it) {
				size_t pos = result.find(it->first);
				if (pos == std::string::npos) continue;
				size_t end = pos + it->first.size();
				if (end != result.size() && (result[end] == '_'
						|| result[end] >= '0' && result[end] <= '9'
						|| result[end] >= 'A' && result[end] <= 'Z'
						|| result[end] >= 'a' && result[end] <= 'z'))
					continue;
				result.replace(pos, it->first.size(), it->second);
				break;
			}
		}
		return result;
	}

	int separateString(const std::string &src, std::string &key, std::string &value) const
	{
		size_t pos = src.find("=");
		if (pos == std::string::npos)
			return 1;
		std::string str1 = src.substr(0, pos);
		std::string str2 = src.substr(pos + 1);
		if (str1[0] == '#' || (str1.size() > 1 && str1[0] == '/' && str1[1] == '/'))
			return 1;
		key = trim(str1);
		value = trim(str2);
		return 0;
	}

	const std::string trim(const std::string &src) const
	{
		const char* pignore = " \t\r\n;";
		size_t pos1 = src.find_first_not_of(pignore);
		size_t pos2 = src.find_last_not_of(pignore);
		if (pos1 == std::string::npos || pos2 == std::string::npos)
			return "";
		else return src.substr(pos1, pos2 - pos1 + 1);
	}
};

} // namespace utils

#endif /* __CONFIG_READER_H__ */
