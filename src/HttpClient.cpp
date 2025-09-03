#include "HttpClient.h"
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include <sstream>
#include <iomanip>
#include <cctype>

std::string HttpClient::urlEncode(const std::string& s) {
    std::ostringstream oss;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c=='-'||c=='_'||c=='.'||c=='~') oss<<c;
        else if (c==' ') oss<<'+';
        else oss<<'%'<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(int)c;
    }
    return oss.str();
}

std::wstring HttpClient::toW(const std::string& s){
    if (s.empty()) return L"";
    int sz = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring ws(sz-1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, ws.data(), sz);
    return ws;
}
std::string HttpClient::toA(const std::wstring& ws){
    if (ws.empty()) return std::string();
    int sz = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(sz-1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, s.data(), sz, nullptr, nullptr);
    return s;
}

bool HttpClient::parseUrl(const std::string& url, bool& https, std::wstring& host, int& port, std::wstring& path){
    https = false; port = 80; host.clear(); path = L"/";
    std::string u = url;
    if (u.rfind("https://",0)==0){ https=true; port=443; u = u.substr(8); }
    else if (u.rfind("http://",0)==0){ https=false; port=80; u = u.substr(7); }
    size_t slash = u.find('/');
    std::string hostport = (slash==std::string::npos) ? u : u.substr(0,slash);
    std::string pathA    = (slash==std::string::npos) ? "/" : u.substr(slash);
    size_t colon = hostport.find(':');
    if (colon!=std::string::npos) {
        host = toW(hostport.substr(0, colon));
        port = std::stoi(hostport.substr(colon+1));
    } else {
        host = toW(hostport);
    }
    path = toW(pathA);
    return !host.empty();
}

HttpResponse HttpClient::postForm(const std::string& url, const std::string& form,
                                  const std::vector<std::pair<std::string,std::string>>& headers) {
    HttpResponse resp;
    bool https; std::wstring host; int port; std::wstring path;
    if (!parseUrl(url, https, host, port, path)) { resp.status = -1; return resp; }

    HINTERNET hSession = WinHttpOpen(L"MicKeyAdminClient/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) { resp.status = -2; return resp; }
    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); resp.status = -3; return resp; }
    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), nullptr,
                                            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            https ? WINHTTP_FLAG_SECURE : 0);
    if (!hRequest) {
        WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); resp.status = -4; return resp;
    }

    std::wstring hdr = L"Content-Type: application/x-www-form-urlencoded\r\n";
    for (auto& kv : headers) {
        hdr += toW(kv.first + ": " + kv.second + "\r\n");
    }

    BOOL sent = WinHttpSendRequest(hRequest,
                                   hdr.c_str(), (DWORD)-1L,
                                   (LPVOID)form.data(), (DWORD)form.size(),
                                   (DWORD)form.size(), 0);
    if (!sent) {
        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        resp.status = -5; return resp;
    }

    if (!WinHttpReceiveResponse(hRequest, nullptr)) {
        WinHttpCloseHandle(hRequest); WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession);
        resp.status = -6; return resp;
    }

    DWORD statusCode=0; DWORD size=sizeof(statusCode);
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX,
                        &statusCode, &size, WINHTTP_NO_HEADER_INDEX);
    resp.status = (int)statusCode;

    std::string body;
    DWORD dwSize=0;
    do {
        DWORD dwDownloaded = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
        if (!dwSize) break;
        std::string buffer; buffer.resize(dwSize);
        if (!WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded)) break;
        body.append(buffer.data(), dwDownloaded);
    } while (dwSize>0);
    resp.body = body;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return resp;
}
