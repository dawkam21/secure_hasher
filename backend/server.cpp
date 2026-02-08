#include "crow_all.h"
#include <openssl/evp.h> // biblioteka OpenSSL
#include <iomanip>
#include <sstream>
#include <random>
#include <vector>

// funkcja generująca sól o zadanej długości
std::string generateSalt(int length) {
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    std::random_device rd; // źródło losowości (sprzętowe)
    std::mt19937 generator(rd()); // generator liczb losowych
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string salt = "";
    for (int i = 0; i < length; ++i) {
        salt += characters[distribution(generator)];
    }
    return salt;
}

// funkcja hashująca (SHA-256 z OpenSSl 3.0)
std::string sha256(const std::string& str) {
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

        // --- LOGIKA SECURE UNICORN ---

        // I - generuje unikalną sól dla konkretnego strzału
        std::string salt = generateSalt(16);

        // II - łączy hasło z solą
        std::string saltedPassword = pass + salt;

        // III - hashuje "posolone" hasło
        std::string finalHash = sha256(saltedPassword);

        // ocena siły hasła
        std::string strength = "";
        if (pass.length() < 8) {
            strength = "SŁABE (za krótkie)";
        } else {
            // sprawdzam czy ma znaki specjalne
            bool hasSpecial = false;
            for (char c : pass) {
                // isalnum zwraca true, jeśli znak to litera lub cyfra
                // jeśli NIE jest alfanumeryczny -> to znak specjalny
                if (!std::isalnum(c)) {
                    hasSpecial = true;
                    break; // znalazło jeden, starczy
                }
            }
            if (hasSpecial) {
                strength = "SILNE (Secure Unicorn aprobuje!)";
            } else {
                strength = "ŚREDNIE (Dodaj znak specjalny)";
            }
        }

        // tworzę odpowiedź
        crow::json::wvalue z;
        z["hash"] = finalHash;
        z["salt"] = salt; // odsyła sół, żebym ją widział (w produkcji zapisuje się ją w bazie)
        z["combined"] = saltedPassword; // pokazuje też, co realnie poszło do hashera
        z["strength"] = strength; // można wysłać oba pola (?)

        // zwraca czysty JSON (CORS mnie nie obchodzi, bo mam Proxy)
        return crow::response(z);
    });

    // uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}