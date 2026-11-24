//#include "WebScraper.h"


//const unordered_set<string> voidTags = {
//    "area", "base", "br", "col", "embed", "hr", "img", "input",
//    "link", "meta", "source", "track", "wbr"
//};
//
//vector<Tags> processHtml(const string& html) {
//    vector<Tags> result;
//    vector<size_t> openStack;          // indices into result of tags not yet closed
//    size_t i = 0, n = html.size();
//
//    while (i < n) {
//        if (html[i] != '<') {          // plain text
//            size_t next = html.find('<', i);
//            if (next == string::npos) next = n;
//            string text = trim(html.substr(i, next - i));
//            if (!text.empty() && !openStack.empty())
//                result[openStack.back()].text += text;
//            i = next;
//            continue;
//        }
//
//        size_t close = html.find('>', i);
//        if (close == string::npos) break;   // unterminated tag
//
//        string inside = html.substr(i + 1, close - i - 1);
//        i = close + 1;
//
//        if (inside.empty() || inside[0] == '!' || inside[0] == '?')
//            continue;                        // comments, doctype, etc.
//
//        if (inside[0] == '/') {              // closing tag: </name>
//            string name = toLower(trim(inside.substr(1)));
//            for (size_t k = openStack.size(); k-- > 0; ) {
//                if (result[openStack[k]].name == name) {
//                    openStack.resize(k);     // closes this tag and any unclosed children
//                    break;
//                }
//            }
//            continue;
//        }
//
//        bool selfClosing = inside.back() == '/';
//        if (selfClosing) inside.pop_back();
//
//        Tags t = parseOpenTag(inside);
//        result.push_back(t);
//        if (!selfClosing && !voidTags.count(t.name))
//            openStack.push_back(result.size() - 1);
//    }
//    return result;
//}
//
//
//void printChildren(Tags* ptr) {
//    for (int i = 0; i < ptr->content.size(); i++) {
//        cout << "children" << " = " << ptr->content.at(i)->name << endl;
//        // Tags *ptr  = ptr->content.at(i);
//        if (ptr->content.at(i)->content.size() > 0)
//            return printChildren(ptr->content.at(i));
//        //else continue;
//        // while(ptr->content.size() > 0){
//        //     cout << "children" << " = " << ptr->name << endl;
//        //     ptr =  ptr->content.at(i);
//        // }
//    }
//}
//

//#pragma once
//#include <string>
//#include <map>
//#include <sstream>
//
////#include "WebScraper.h"
//
//typedef struct{
//	std::string method;
//	std::string url;
//	std::string version;
//	std::map<std::string, std::string> headers;
//} WebHeader;
//
//struct Tags {
//	std::string name;
//	std::map<std::string, std::string> attributes;
//	Tags* content;
//	std::string text;
//};
//
//class StrManipulator {
//public:
//	std::string trim(const std::string& str) {
//		size_t first = str.find_first_not_of(" \t\n\r");
//		if (first == std::string::npos) {
//			return ""; // String is all whitespace
//		}
//		size_t last = str.find_last_not_of(" \t\n\r");
//		return str.substr(first, (last - first + 1));
//	}
//
//	std::string toLower(const std::string& str) {
//		std::string result = str;
//		for (char& c : result) {
//			c = std::tolower(static_cast<unsigned char>(c));
//		}
//		return result;
//	}
//
//	std::string toUpper(const std::string& str) {
//		std::string result = str;
//		for (char& c : result) {
//			c = std::toupper(static_cast<unsigned char>(c));
//		}
//		return result;
//	}
//
//	std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
//		std::string result = str;
//		size_t startPos = 0;
//		while ((startPos = result.find(from, startPos)) != std::string::npos) {
//			result.replace(startPos, from.length(), to);
//			startPos += to.length(); // Move past the replacement
//		}
//		return result;
//	}
//
//	std::string grabAndReturnHtmlTag(const std::string& html, const std::string& tagName) {
//		std::string lowerHtml = toLower(html);
//		std::string lowerTagName = toLower(tagName);
//		std::string openTag = "<" + lowerTagName;
//		std::string closeTag = "</" + lowerTagName + ">";
//		size_t startPos = lowerHtml.find(openTag);
//		if (startPos == std::string::npos) {
//			return ""; // Tag not found
//		}
//		size_t endPos = lowerHtml.find(closeTag, startPos);
//		if (endPos == std::string::npos) {
//			return ""; // Closing tag not found
//		}
//		endPos += closeTag.length(); // Move to the end of the closing tag
//		return html.substr(startPos, endPos - startPos);
//	}
//
//	std::string parseHttpResponse(const std::string& response, WebHeader& header) {
//		//WebHeader header;
//		size_t pos = 0;
//		size_t endOfLine = response.find("\r\n", pos);
//		if (endOfLine == std::string::npos) {
//			return ""; // Invalid response
//		}
//		std::string statusLine = response.substr(pos, endOfLine - pos);
//		pos = endOfLine + 2; // Move past the status line
//		// Parse the status line
//		size_t methodEnd = statusLine.find(' ');
//		if (methodEnd == std::string::npos) {
//			return ""; // Invalid status line
//		}
//		header.method = statusLine.substr(0, methodEnd);
//		size_t urlEnd = statusLine.find(' ', methodEnd + 1);
//		if (urlEnd == std::string::npos) {
//			return "header"; // Invalid status line
//		}
//		header.url = statusLine.substr(methodEnd + 1, urlEnd - methodEnd - 1);
//		header.version = statusLine.substr(urlEnd + 1);
//		// Parse headers
//		while (true) {
//			endOfLine = response.find("\r\n", pos);
//			if (endOfLine == std::string::npos || endOfLine == pos) {
//				break; // End of headers or empty line
//			}
//			std::string line = response.substr(pos, endOfLine - pos);
//			pos = endOfLine + 2; // Move past the line
//			size_t colonPos = line.find(':');
//			if (colonPos != std::string::npos) {
//				std::string key = trim(line.substr(0, colonPos));
//				std::string value = trim(line.substr(colonPos + 1));
//				header.headers[key] = value;
//			}
//		}
//		//std::cout << pos <<std::endl;
//		return response.substr(pos, response.length);
//	}
//
//	std::string extractTextFromHtml(const std::string& html) {
//		std::string text;
//		bool inTag = false;
//		for (char c : html) {
//			if (c == '<') {
//				inTag = true;
//			}
//			else if (c == '>') {
//				inTag = false;
//			}
//			else if (!inTag) {
//				text += c;
//			}
//		}
//		return text;
//	}
//
//	std::string extractTagName(const std::string& tag) {
//		size_t start = tag.find('<');
//		if (start == std::string::npos) return "";
//		size_t end = tag.find_first_of(" >", start);
//		if (end == std::string::npos) return "";
//		return tag.substr(start + 1, end - start - 1);
//	}
//
//	std::map<std::string, std::string> extractAttributes(const std::string& tag) {
//		std::map<std::string, std::string> attributes;
//		size_t start = tag.find('<');
//		if (start == std::string::npos) return attributes;
//		size_t end = tag.find('>', start);
//		if (end == std::string::npos) return attributes;
//		std::string content = tag.substr(start + 1, end - start - 1);
//		std::istringstream iss(content);
//		std::string token;
//		while (iss >> token) {
//			size_t equalPos = token.find('=');
//			if (equalPos != std::string::npos) {
//				std::string key = token.substr(0, equalPos);
//				std::string value = token.substr(equalPos + 1);
//				// Remove quotes from value if present
//				if (!value.empty() && value.front() == '"' && value.back() == '"') {
//					value = value.substr(1, value.size() - 2);
//				}
//				attributes[key] = value;
//			}
//		}
//		return attributes;
//	}
//
//	std::map<std::string, std::string> parseQueryString(const std::string& query)
//	{
//		std::map<std::string, std::string> params;
//		size_t start = 0;
//		while (start < query.length()) {
//			size_t end = query.find('&', start);
//			if (end == std::string::npos) {
//				end = query.length();
//			}
//			std::string pair = query.substr(start, end - start);
//			size_t equalPos = pair.find('=');
//			if (equalPos != std::string::npos) {
//				std::string key = pair.substr(0, equalPos);
//				std::string value = pair.substr(equalPos + 1);
//				params[key] = value;
//			}
//			start = end + 1;
//		}
//		return params;
//	}
//
//	std::string buildQueryString(const std::map<std::string, std::string>& params) {
//		std::string query;
//		for (const auto& pair : params) {
//			if (!query.empty()) {
//				query += "&";
//			}
//			query += pair.first + "=" + pair.second;
//		}
//		return query;
//	}
//
//	//std::string urlEncode(const std::string& value) {
//	//	std::ostringstream escaped;
//	//	escaped.fill('0');
//	//	escaped << std::hex;
//	//	for (char c : value) {
//	//		if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
//	//			escaped << c;
//	//		}
//	//		else {
//	//			escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
//	//		}
//	//	}
//	//	return escaped.str();
//	//}
//
//	//std::string urlDecode(const std::string& value) {
//	//	std::ostringstream unescaped;
//	//	for (size_t i = 0; i < value.length(); ++i) {
//	//		if (value[i] == '%' && i + 2 < value.length()) {
//	//			std::string hex = value.substr(i + 1, 2);
//	//			char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
//	//			unescaped << decodedChar;
//	//			i += 2;
//	//		}
//	//		else if (value[i] == '+') {
//	//			unescaped << ' ';
//	//		}
//	//		else {
//	//			unescaped << value[i];
//	//		}
//	//	}
//	//	return unescaped.str();
//	//}
//
//	std::string extractDomainFromUrl(const std::string& url) {
//		size_t protocolEnd = url.find("://");
//		size_t domainStart = (protocolEnd == std::string::npos) ? 0 : protocolEnd + 3;
//		size_t domainEnd = url.find('/', domainStart);
//		if (domainEnd == std::string::npos) {
//			domainEnd = url.length();
//		}
//		return url.substr(domainStart, domainEnd - domainStart);
//	}
//
//	std::string extractPathFromUrl(const std::string& url) {
//		size_t protocolEnd = url.find("://");
//		size_t pathStart = (protocolEnd == std::string::npos) ? 0 : protocolEnd + 3;
//		pathStart = url.find('/', pathStart);
//		if (pathStart == std::string::npos) {
//			return "/"; // No path, return root
//		}
//		return url.substr(pathStart);
//	}
//
//	std::string extractQueryFromUrl(const std::string& url) {
//		size_t queryStart = url.find('?');
//		if (queryStart == std::string::npos) {
//			return ""; // No query string
//		}
//		return url.substr(queryStart + 1);
//	}
//
//	std::string extractFragmentFromUrl(const std::string& url) {
//		size_t fragmentStart = url.find('#');
//		if (fragmentStart == std::string::npos) {
//			return ""; // No fragment
//		}
//		return url.substr(fragmentStart + 1);
//	}
//
//	std::map<std::string, Tags> parseHtml(const std::string& html) {
//		std::map<std::string, Tags> tagMap;
//		size_t pos = 0;
//		while (pos < html.length()) {
//			size_t openTagStart = html.find('<', pos);
//			if (openTagStart == std::string::npos) break;
//			size_t openTagEnd = html.find('>', openTagStart);
//			if (openTagEnd == std::string::npos) break;
//			std::string tagContent = html.substr(openTagStart, openTagEnd - openTagStart + 1);
//			std::string tagName = extractTagName(tagContent);
//			Tags tag;
//			tag.name = tagName;
//			tag.attributes = extractAttributes(tagContent);
//			size_t closeTagStart = html.find("</" + tagName + ">", openTagEnd);
//			if (closeTagStart != std::string::npos) {
//				tag.text = extractTextFromHtml(html.substr(openTagEnd + 1, closeTagStart - openTagEnd - 1));
//				pos = closeTagStart + tagName.length() + 3; // Move past the closing tag
//			}
//			else {
//				pos = openTagEnd + 1; // Move past the opening tag
//			}
//			tagMap[tagName] = tag;
//		}
//		return tagMap;
//	}
//};