#include "KeyAdminClient.h"
#include <iostream>
#include <string>
#include <unordered_map>

static void usage(){
    std::cout << "MicKeyAdminClient (por MicDog)\n"
              << "Uso:\n"
              << "  MicKeyAdminClient.exe --base https://keyadmin.online \\\n"
              << "    --cliente_hash X --api_key Y --software_id 29 --key MicDog-AAAA --hwid PC-UUID activate\n"
              << "  MicKeyAdminClient.exe --base https://keyadmin.online \\\n"
              << "    --cliente_hash X --software_id 29 --key MicDog-AAAA validate\n\n"
              << "Opcoes:\n"
              << "  --base URL_BASE               (ex.: https://keyadmin.online)\n"
              << "  --cliente_hash HASH_CLIENTE   (obrigatorio)\n"
              << "  --api_key API_KEY             (obrigatorio para activate)\n"
              << "  --software_id ID              (obrigatorio)\n"
              << "  --key CHAVE_LICENCA           (obrigatorio)\n"
              << "  --hwid HWID                   (opcional)\n"
              << "  --validar_path /caminho       (opcional; padrao /painel/api/validar.php)\n"
              << "  --ativar_path  /caminho       (opcional; padrao /painel/api/ativar.php)\n"
              << "Comando: validate | activate\n";
}

int main(int argc, char** argv){
    if (argc < 2) { usage(); return 1; }
    std::unordered_map<std::string,std::string> args;
    std::string cmd;
    for (int i=1; i<argc; ++i){
        std::string a = argv[i];
        if (a.rfind("--",0)==0 && i+1<argc) {
            args[a]=argv[++i];
        } else if (a=="validate"||a=="activate") {
            cmd = a;
        }
    }
    if (cmd.empty()) { std::cerr<<"Comando ausente (validate|activate)\n"; usage(); return 1; }

    auto base = args.count("--base") ? args["--base"] : "https://keyadmin.online";
    auto cliente_hash = args["--cliente_hash"];
    auto software_id  = args["--software_id"];
    auto key          = args["--key"];
    if (cliente_hash.empty() || software_id.empty() || key.empty()) {
        std::cerr<<"Parâmetros obrigatórios faltando.\n"; usage(); return 2;
    }

    KeyAdminClient client(base);
    if (args.count("--validar_path") || args.count("--ativar_path")) {
        client.setCustomPaths(args.count("--validar_path")?args["--validar_path"]:"/painel/api/validar.php",
                              args.count("--ativar_path") ?args["--ativar_path"] :"/painel/api/ativar.php");
    }

    if (cmd=="validate") {
        auto res = client.validate(cliente_hash, software_id, key);
        std::cout << "[HTTP " << res.status << "]\n" << res.body << "\n";
        std::cout << (res.ok ? "✅ SUCESSO\n" : "❌ FALHA\n");
        return res.ok ? 0 : 3;
    } else { // activate
        auto api_key = args["--api_key"];
        if (api_key.empty()) { std::cerr<<"--api_key obrigatório para activate\n"; return 2; }
        auto hwid = args.count("--hwid") ? args["--hwid"] : "";
        auto res = client.activate(cliente_hash, api_key, software_id, key, hwid);
        std::cout << "[HTTP " << res.status << "]\n" << res.body << "\n";
        std::cout << (res.ok ? "✅ SUCESSO\n" : "❌ FALHA\n");
        return res.ok ? 0 : 3;
    }
}
