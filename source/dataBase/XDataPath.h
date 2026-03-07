#pragma  once
#include "dataBaseApi.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
namespace XQ {
	class DATABASE_API XDataPath {
	public:
		static XDataPath fromStr(const std::string&str);
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
		XDataPath& pop_front() { names.erase(names.begin()); return *this; }
		XDataPath& reverse() {
			std::reverse(names.begin(),names.end());
			return *this;
		}
		std::string& back() {
			return names.back();
		}

		std::string back() const {
			return names.back();
		}
		
		std::string first() const{
			return names.front();
		}

		std::string& first() {
			return names.front();
		}

		size_t size()const {
			return names.size();
		}

		XDataPath& merge(const XDataPath& other) {
			names.insert(names.end(),other.names.begin(),other.names.end());
			return *this;
		}

		void for_each(std::function<void(std::string& v)> fn) {
			std::for_each(names.begin(),names.end(),fn);
		}
	public:
		std::vector<std::string> names;
	};
}