#include "KeyAdminClient.h"
#include "HttpClient.h"
#include <sstream>
#include <iomanip>
#include <cctype>

static std::string urlJoin(const std::string& base, const std::string& path) {
    if (base.empty()) return path;
    if (base.back()=='/' && !path.empty() && path.front()=='/')
        return base.substr(0, base.size()-1)+path;
    if (base.back()!='/' && !path.empty() && path.front()!='/')
        return base + "/" + path;
    return base + path;
}

static std::string urlEncodeLocal(const std::string& s){
    std::ostringstream oss;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c=='-'||c=='_'||c=='.'||c=='~') oss<<c;
        else if (c==' ') oss<<'+';
        else oss<<'%'<<std::uppercase<<std::setw(2)<<std::setfill('0')<<(int)c;
    }
    return oss.str();
}

std::string KeyAdminClient::encodeKV(const std::string& k, const std::string& v) {
    return urlEncodeLocal(k) + "=" + urlEncodeLocal(v);
}

KeyAdminClient::Result KeyAdminClient::validate(const std::string& clienteHash,
                                                const std::string& softwareId,
                                                const std::string& key) {
    HttpClient http;
    std::ostringstream body;
    body << encodeKV("cliente_hash", clienteHash) << "&"
         << encodeKV("software_id", softwareId)   << "&"
         << encodeKV("key", key);

    auto resp = http.postForm(urlJoin(base, pathValidar), body.str(), {{"Accept","application/json"}});
    bool ok = (resp.body.find("\"ok\":true")!=std::string::npos) || (resp.body.find("\"success\":true")!=std::string::npos);
    return {resp.status, ok, resp.body};
}

KeyAdminClient::Result KeyAdminClient::activate(const std::string& clienteHash,
                                                const std::string& apiKey,
                                                const std::string& softwareId,
                                                const std::string& key,
                                                const std::string& hwid) {
    HttpClient http;
    std::ostringstream body;
    body << encodeKV("cliente_hash", clienteHash) << "&"
         << encodeKV("api_key", apiKey)           << "&"
         << encodeKV("software_id", softwareId)   << "&"
         << encodeKV("key", key);
    if (!hwid.empty()) body << "&" << encodeKV("hwid", hwid);

    auto resp = http.postForm(urlJoin(base, pathAtivar), body.str(), {{"Accept","application/json"}});
    bool ok = (resp.body.find("\"ok\":true")!=std::string::npos) || (resp.body.find("\"success\":true")!=std::string::npos);
    return {resp.status, ok, resp.body};
}
