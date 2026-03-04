#pragma  once
#include "databaseApi.h"
#include <string>
#include <vector>
namespace XQ {
	class database_API XDataPath {
	public:
		XDataPath();
		std::string path() const {
			std::string path = "/";

			for (auto p : names) {
				auto c = *(--path.end());
				if (c == '/') {
					if (p != "/")
						path.append(p);
				}
				else {
					path.append("/").append(p);
				}
			}
			return path;
		}

		XDataPath& push(std::string s) { names.push_back(s); return *this; }
		XDataPath& insert(std::string s) { names.insert(names.begin(), s); return *this; }
		XDataPath& pop() { names.pop_back(); return *this; }
		std::string back() {
			return names.back();
		}

		size_t size() {
			return names.size();
		}
	public:
		std::vector<std::string> names;
	};
}