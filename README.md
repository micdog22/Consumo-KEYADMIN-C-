# MicKeyAdminClient — Exemplo C++ para consumir a KeyAdmin 

Projeto C++ completo (CMake) para **ativar** e **validar** licenças usando a sua API KeyAdmin.
Sem dependências externas — usa **WinHTTP** (Windows). Ótimo como exemplo de integração nativa.

Autor: **MicDog (Michael Douglas)**

## Recursos
- `POST` x-www-form-urlencoded para endpoints `ativar.php` e `validar.php`
- Configurável por **linha de comando**
- Saída com o corpo bruto da resposta (JSON/texto) e checagem de `ok`/`success` simples
- Sem libs externas (JSON opcional; aqui fazemos parse leve apenas para `ok`)

## Compilação (Windows)
Requer: **CMake 3.20+** e **MSVC/MinGW**

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
# binário em: build/Release/MicKeyAdminClient.exe
```

Ou MinGW:
```bash
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
```

## Uso
```bash
MicKeyAdminClient.exe --base https://keyadmin.online   --cliente_hash SEU_CLIENTE_HASH   --api_key SUA_API_KEY   --software_id 29   --key MicDog-XXXX-YYYY   --hwid PC-UUID-OPCIONAL validate
```

Ativar (ativa uma key nova ou vincula HWID conforme sua API):
```bash
MicKeyAdminClient.exe --base https://keyadmin.online   --cliente_hash SEU_CLIENTE_HASH   --api_key SUA_API_KEY   --software_id 29   --key MicDog-XXXX-YYYY   --hwid ABCD-1234-EFGH activate
```

> Endpoints esperados (ajuste se o seu painel usar caminhos diferentes):
> - `POST {base}/painel/api/validar.php`
> - `POST {base}/painel/api/ativar.php`
> Campos comuns: `cliente_hash`, `api_key`, `software_id`, `key`, `hwid` (opcional) 
> Alguns painéis exigem somente `cliente_hash` para validar e `api_key` para operações protegidas — adapte nos args.

## Exemplos
- Validar apenas com `cliente_hash` + `key` (se sua API permitir):
```bash
MicKeyAdminClient.exe --base https://keyadmin.online --cliente_hash X --software_id 29 --key MicDog-AAAA validate
```
- Ativar com `api_key` obrigatória (escopo Revendedor):
```bash
MicKeyAdminClient.exe --base https://keyadmin.online --cliente_hash X --api_key Y --software_id 29 --key MicDog-AAAA --hwid HOST-UUID activate
```

## Retorno
- Código HTTP + corpo (impresso no stdout)
- Checagem simples: se o corpo contém `"ok":true` ou `"success":true` exibe "✅ SUCESSO"; caso contrário, "❌ FALHA".
A resposta completa é mantida para debug.

## Estrutura
```
MicKeyAdminClient/
├─ src/
│  ├─ HttpClient.h/.cpp        # wrapper WinHTTP
│  ├─ KeyAdminClient.h/.cpp    # cliente de alto nível
│  └─ main.cpp                 # CLI
├─ CMakeLists.txt
├─ LICENSE (MIT)
└─ README.md
```

## Observações
- Se o seu painel usa rotas amigáveis (ex.: `/painel/api/ativar`), altere em `KeyAdminClient.h`.
- Este cliente **não armazena** tokens ou HWID — é somente exemplo de consumo.
- Você pode incorporar em launchers/instaladores em C++.

## Licença
MIT © 2025 **MicDog (Michael Douglas)**
