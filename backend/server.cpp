#include "crow_all.h"
#include <openssl/evp.h> // biblioteka OpenSSL
#include <iomanip>
#include <sstream>
#include <vector>

std::string sha256(const std::string str) {
    // I - tworzę kontekst dla algorytmu
    EVP_MD_CTX* context = EVP_MD_CTX_new();

    // II - wybieramy algorytm (sha256)
    const EVP_MD* md = EVP_sha256();

    // III - inicjalizacja operacji
    EVP_DigestInit_ex(context, md, nullptr);

    // IV - przetwarzanie danych
    EVP_DigestUpdate(context, str.c_str(), str.size());

    // V - pobieranie wyniku (hash ma zawsze 32 bajty dla SHA-256)
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);

    // VI - sprzątanie pamięci (ważne w C++)
    EVP_MD_CTX_free(context);

    // VII - konwersja na czytelny hex (16tkowy)
    std::stringstream ss;
    for(unsigned int i = 0; i < lengthOfHash; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

int main() {
    crow::SimpleApp app;

    // Prosty endpoint POST
    CROW_ROUTE(app, "/api/hash").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        
        // Jeśli JSON jest pusty lub błędny
        if (!x) return crow::response(400, "Brak JSON");

        // pobieram hasło
        std::string pass = x["password"].s();
        std::string result_msg = "";
        std::string strength = "";

        // logika security - walidacja po stronie serwera

        if (pass.length() < 8) {
            strength = "SŁABE (Za krótkie)";
        } else {
            // sprawdza czy są znaki specjalne
            bool hasSpecial = false;
            for (char c : pass) {
                // isalnum zwraca true, jeśli znak to litera lub cyfra
                // jeśli nie jest alfanumerycazny -> to znak specjalny
                if(!std::isalnum(c)) {
                    hasSpecial = true;
                    break; // jeden jest, wystarczy
                }
            }

            if (hasSpecial) {
                strength = "SILNE (Secure Unicorn aprobuje!)";
            } else {
                strength = "ŚREDNIE (Dodaj znak specjalny)";
            }
        }

        std::string hashed_password = sha256(pass);

        // tworzę odpowiedź
        crow::json::wvalue z;
        z["hash"] = hashed_password;
        z["strength"] = strength; // można wysłać oba pola (?)
        z["algorithm"] = "SHA-256 (OpenSSL 3.0 EVP)";

        // zwraca czysty JSON (CORS mnie nie obchodzi, bo mam Proxy)
        return crow::response(z);
    });

    // uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}