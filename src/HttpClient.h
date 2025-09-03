#pragma once
#include <string>
#include <vector>

struct HttpResponse {
    int status = 0;
    std::string body;
    std::string contentType;
};

class HttpClient {
public:
    HttpResponse postForm(const std::string& url, const std::string& formUrlEncoded,
                          const std::vector<std::pair<std::string,std::string>>& headers = {});

private:
    static std::string urlEncode(const std::string& s);
    static bool parseUrl(const std::string& url, bool& https, std::wstring& host, int& port, std::wstring& path);
    static std::wstring toW(const std::string& s);
    static std::string  toA(const std::wstring& ws);
};
