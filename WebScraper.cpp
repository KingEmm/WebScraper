// WebScraper.cpp : Defines the entry point for the application.
//

#include "WebScraper.h"
#include "StrManipulator.cpp"
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

struct Tags {
	std::string name;
	std::map<std::string, std::string> attributes;
	vector<Tags*> content;
	Tags* parent;
	std::string text;

	~Tags() {
		for (Tags* child : content) {
			delete child;
		}
	}
};

std::string trim(const std::string& s) {
	const char* ws = " \t\n\r\f\v";
	size_t start = s.find_first_not_of(ws);
	if (start == std::string::npos) return "";  // string is all whitespace
	size_t end = s.find_last_not_of(ws);
	return s.substr(start, end - start + 1);
}

std::string toLower(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

static string getHtml(const string *response) {
	size_t headerEnd = response->find("\r\n\r\n");
	return response->substr(headerEnd, response->length());
}

const unordered_set<string> voidTags = {
	"area", "base", "br", "col", "embed", "hr", "img", "input",
	"link", "meta", "source", "track", "wbr"
};

Tags parseOpenTag(const string& s) {
	Tags t;
	size_t i = 0, n = s.size();

	while (i < n && !isspace((unsigned char)s[i])) i++;
	t.name = toLower(s.substr(0, i));

	while (i < n) {
		while (i < n && isspace((unsigned char)s[i])) i++;

		size_t keyStart = i;
		while (i < n && !isspace((unsigned char)s[i]) && s[i] != '=') i++;
		string key = s.substr(keyStart, i - keyStart);
		if (key.empty()) break;

		string value;
		while (i < n && isspace((unsigned char)s[i])) i++;
		if (i < n && s[i] == '=') {
			i++;
			while (i < n && isspace((unsigned char)s[i])) i++;
			if (i < n && (s[i] == '"' || s[i] == '\'')) {
				char quote = s[i++];
				size_t vs = i;
				while (i < n && s[i] != quote) i++;
				value = s.substr(vs, i - vs);
				if (i < n) i++;          // skip closing quote
			}
			else {
				size_t vs = i;
				while (i < n && !isspace((unsigned char)s[i])) i++;
				value = s.substr(vs, i - vs);
			}
		}
		t.attributes[key] = value;
	}
	return t;
}


vector<Tags> processHtml(const string& html) {
	vector<Tags> result;
	vector<size_t> openStack;          // indices into result of tags not yet closed
	size_t i = 0, n = html.size();

	while (i < n) {
		if (html[i] != '<') {          // plain text
			size_t next = html.find('<', i);
			if (next == string::npos) next = n;
			string text = trim(html.substr(i, next - i));
			if (!text.empty() && !openStack.empty())
				result[openStack.back()].text += text;
			i = next;
			continue;
		}

		size_t close = html.find('>', i);
		if (close == string::npos) break;   // unterminated tag

		string inside = html.substr(i + 1, close - i - 1);
		i = close + 1;

		if (inside.empty() || inside[0] == '!' || inside[0] == '?')
			continue;                        // comments, doctype, etc.

		if (inside[0] == '/') {              // closing tag: </name>
			string name = toLower(trim(inside.substr(1)));
			for (size_t k = openStack.size(); k-- > 0; ) {
				if (result[openStack[k]].name == name) {
					openStack.resize(k);     // closes this tag and any unclosed children
					break;
				}
			}
			continue;
		}

		bool selfClosing = inside.back() == '/';
		if (selfClosing) inside.pop_back();

		Tags t = parseOpenTag(inside);
		result.push_back(t);
		if (!selfClosing && !voidTags.count(t.name))
			openStack.push_back(result.size() - 1);
	}
	return result;
}


void printChildren(Tags* ptr) {
	for (int i = 0; i < ptr->content.size(); i++) {
		cout << "children" << " = " << ptr->content.at(i)->name << endl;
		// Tags *ptr  = ptr->content.at(i);
		if (ptr->content.at(i)->content.size() > 0)
			return printChildren(ptr->content.at(i));
		//else continue;
		// while(ptr->content.size() > 0){
		//     cout << "children" << " = " << ptr->name << endl;
		//     ptr =  ptr->content.at(i);
		// }
	}
}



int main()
{
	MySocket mySocket;

	string url;
	cout << "Enter URL: ";
	cin >> url;

	if (!mySocket.load()) {
		std::cout << "Failed to load necessary socket at main\n";
		return -1;
	}
	if (!mySocket.initialize()) {
		std::cout << "Failed to Initialize socket at main\n";
		return -1;
	}

	//SOCKET sock = mySocket.createSocket("192.168.0.1", 80);
	//SOCKET sock = mySocket.createSocket(url.c_str(), 5000, true);
	SOCKET sock = mySocket.createSocket(url.c_str(), 443, true);

	if (sock == INVALID_SOCKET) {
		std::cout << "Failed to create socket at main\n";
		return -1;
	}
	std::cout << "Socket created successfully at main: " << sock << "\n";

	if(!mySocket.connectToServer(sock)) {
		mySocket.CloseSocket_Ptr(sock);
		mySocket.cleanup();
		std::cout << "Failed to connect to server." << std::endl;
		return -1;
	}

	//url.insert(0, "https://");

	SSL_CTX* context = nullptr;
	SSL* ssl = nullptr;
	
	cout << "Establishing TLS connection to server: " << url << std::endl;
	if(!mySocket.establishTLS(sock, url, context, ssl)) {
		mySocket.CloseSocket_Ptr(sock);
		mySocket.cleanup();
		std::cout << "Failed to establish TLS connection to server." << std::endl;
		return -1;
	}
	
	std::string requestData = "GET / HTTP/1.1\r\n"
		"Connection: close\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3\r\n"
		"Content-Type: text/html\r\n";
		//"Content-Length: 18\r\n"

	std::ostringstream requestBuilder;
	//requestBuilder << "GET " << path << "HTTP/1.1\r\n";
	requestBuilder << "Host: " << url << "\r\n";

	requestData.append(requestBuilder.str());
	requestData.append("\r\n");

	//mySocket.Send_Ptr(sock, requestData.c_str(), requestData.size(), 0);
	mySocket.sendTLSData(ssl, requestData);

	//cout << "\n\n" << requestData << "\n\n\n";
	std::string data;
	//vector<char> data;
	//char* buffer = new char[4096];
	//char buffer[4096];

	//mySocket.receiveData(sock, buffer, sizeof(buffer), &data);
	mySocket.receiveTLSData(ssl, &data);
		//std::cout << "Hello Sent: " << data.at(0) << std::endl;

	cout << "\n\n" << data << endl;

	//WebHeader header;

	//StrManipulator mani;
	//string response(data.begin(), data.end());
	//std::cout << response << std::endl;

		//std::cout << "Request Sent: " << string(data.begin(), data.end()) << std::endl;
	string html = getHtml(&data);

	//std::string response(data.begin(), data.end());
	//string html = getHtml(&data); 

	cout << "\n\n" << html << endl;

	auto ele = processHtml(html);

	std::ofstream file("output.csv");

	if (!file.is_open()) {
		cout << "Error While Opening File" << endl;
	}

	file << "Tag, Text, Attributes\n";

	for (const auto item : ele) {
		cout << item.name;
		if (item.text.size() > 0)
			cout << " | " << item.text << " | ";
		if (item.attributes.size() > 0) {
			auto len = item.attributes.size();
			for (int i = 0; i < len; i++) {
				auto it = std::next(item.attributes.begin(), i);
				if (i == len - 1)
					std::cout << ' ' << it->first << " : " << it->second;
				else
					std::cout << ' ' << it->first << " : " << it->second << " & ";
			}
		}
		cout << "" << endl;//<< endl;
	}
	std::cout << "Hello CMake." << std::endl;
	file.close();
	mySocket.cleanup();
	mySocket.CloseSocket_Ptr(sock);
	return 0;
 }
