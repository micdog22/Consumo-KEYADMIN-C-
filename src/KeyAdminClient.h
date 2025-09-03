#pragma once
#include <string>

class KeyAdminClient {
public:
    explicit KeyAdminClient(std::string baseUrl) : base(std::move(baseUrl)) {}

    struct Result {
        int status;
        bool ok;
        std::string body;
    };

    Result validate(const std::string& clienteHash,
                    const std::string& softwareId,
                    const std::string& key);

    Result activate(const std::string& clienteHash,
                    const std::string& apiKey,
                    const std::string& softwareId,
                    const std::string& key,
                    const std::string& hwid = "");

    void setCustomPaths(std::string validarPath, std::string ativarPath) {
        pathValidar = std::move(validarPath);
        pathAtivar  = std::move(ativarPath);
    }

private:
    std::string base;
    std::string pathValidar = "/painel/api/validar.php";
    std::string pathAtivar  = "/painel/api/ativar.php";

    static std::string encodeKV(const std::string& k, const std::string& v);
};
