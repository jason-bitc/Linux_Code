#pragma once

#include <iostream>
#include <cstdio>
#include <iterator>
#include <ratio>
#include <string>
#include <sstream>
#include <strings.h>
#include <unordered_map>
#include <fstream>
#include <functional>
#include "Logger.hpp"

const std::string linesep = "\r\n";
const std::string spacesep = " ";
const std::string headersep = ": ";
const std::string suffixsep = ".";
const std::string argsep = "?";
const std::string g_http_version = "HTTP/1.0";
const std::string g_first_page = "index.html";
const std::string g_wwwroot = "wwwroot";
const std::string page_404 = "404.html";

using namespace NS_LOG_MODULE;

class Util {
public:
    static std::string ReadLine(std::string &str) {
        auto pos = str.find(linesep);
        if(pos == std::string::npos)
            return std::string();
        std::string line = str.substr(0, pos);
        str.erase(0, line.size() + linesep.size());
        if(line.empty())
            return linesep;
        return line;
    }
    static std::string ReadFile(const std::string &filename) {
        std::ifstream in(filename, std::ios::binary);
        if(in.is_open())
            return std::string();
        in.seekg(0, in.end);
        int filesize = in.tellg();
        in.seekg(0, in.beg); // 0是offset,beg是基准位置参照点

        std::string content;
        content.resize(filesize);
        in.read((char *)content.c_str(), filesize);
        in.close();
        return content;
    }
    // static 为了?
};
class HttpRequest {
public:
    bool ParseReqLine(std::string &httpstr) {
        std::string req_line = Util::ReadLine(httpstr);
        if(req_line.empty() || req_line == linesep)
            ; // 第二种是什么情况呢
        return false;
        std::stringstream ss(req_line);
        ss >> _method >> _uri >> _version;
        if(_uri == "/")
            _uri += g_first_page;
        _uri = g_wwwroot + _uri;

        auto pos = _uri.rfind(suffixsep);
        if(pos == std::string::npos)
            _suffix = ".html";
        else
            _suffix = _uri.substr(pos);
        return true;
    }
    bool ParseHeaderkv(std::string &httpstr) {
        std::string header_line;
        do
        {
            header_line = Util::ReadLine(httpstr);
            if(header_line.empty())
                return false;
            if(header_line != linesep)
            {
                auto pos = header_line.find(headersep);
                if(pos == std::string::npos)
                    return false;
                std::string key = header_line.substr(0, pos);
                std::string value = header_line.substr(pos + headersep.size());
                _header.insert({key, value});
            }
        } while(header_line != linesep);
        return true;
    }
    bool ParseText(std::string &httpstr) {
        if(strcasecmp(_method.c_str(), "GET") == 0)
        {
            auto pos = _uri.find(argsep);
            if(pos == std::string::npos)
            {
                _text = std::string();
                return true;
            }
            else
            {
                std::cout << "_URI: " << _uri << std::endl;
                std::string temp = _uri;
                _uri = temp.substr(0, pos);
                std::cout << "_URI: " << _uri << std::endl;
                _text = temp.substr(pos + argsep.size());
                std::cout << "_text: " << _text << std::endl;
            }
        }
        else
        {
            if(_header.find("Content-Length") == _header.end())
            {
                _text = "";
            }
            else
            {
                int content_len = std::stoi(_header["Content-Length"]);
                _text = httpstr.substr(0, content_len);
                httpstr.erase(0, content_len);
            }
        }
        return true;
    }

public:
    bool Deserialize(std::string &httpstr) {
        std::cout << httpstr;

        bool n = ParseReqLine(httpstr);

        n = ParseHeaderkv(httpstr);
        _blank_line = linesep;

        n = ParseText(httpstr);
        return true;
    }
    std::string Uri() {
        return _uri;
    }
    std::string RequestContent() {
        return Util::ReadFile(_uri);
    }
    std::string Suffix() {
        return _suffix;
    }
    void DebugPrint() {
        std::cout << "_version: " << _version << std::endl;
        std::cout << "_uri: " << _uri << std::endl;
        std::cout << "_method: " << _method << std::endl;
        for(auto &header : _header)
        {
            std::cout << "==>" << header.first << " # " << header.second << std::endl;
        }
        std::cout << _blank_line;
        std::cout << _text << std::endl;
    }
    std::string Text() {
        return _text;
    }
    HttpRequest() {}
    ~HttpRequest() {}

private:
    std::string _method;
    std::string _uri;
    std::string _version;
    std::unordered_map<std::string, std::string> _header;
    std::string _blank_line;
    std::string _text;
    std::string _suffix;
};

class HttpResponse {
private:
    std::string CodeToDesc(int code) {
        switch(code)
        {
            // 1xx
            case 100:
                return "Continue";
            case 101:
                return "Switching Protocols";

            // 2xx
            case 200:
                return "OK";
            case 201:
                return "Created";
            case 202:
                return "Accepted";
            case 204:
                return "No Content";
            case 206:
                return "Partial Content";

            // 3xx
            case 301:
                return "Moved Permanently";
            case 302:
                return "Found";
            case 304:
                return "Not Modified";
            case 307:
                return "Temporary Redirect";
            case 308:
                return "Permanent Redirect";

            // 4xx
            case 400:
                return "Bad Request";
            case 401:
                return "Unauthorized";
            case 403:
                return "Forbidden";
            case 404:
                return "Not Found";
            case 405:
                return "Method Not Allowed";
            case 408:
                return "Request Timeout";
            case 409:
                return "Conflict";
            case 413:
                return "Payload Too Large";
            case 429:
                return "Too Many Requests";

            // 5xx
            case 500:
                return "Internal Server Error";
            case 501:
                return "Not Implemented";
            case 502:
                return "Bad Gateway";
            case 503:
                return "Service Unavailable";
            case 504:
                return "Gateway Timeout";
            case 505:
                return "HTTP Version Not Supported";

            // 默认情况
            default:
                return "Unknown Status Code";
        }
    }

public:
private:
    std::string _version;
    int  _code;
    std::string _code_desc;
    std::unordered_map<std::string,std::string> _header;
    std::string _blank_line;
    std::string _text;
};