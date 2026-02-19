#include "crow_all.h"
#include <openssl/evp.h> // biblioteka OpenSSL
#include <iomanip>
#include <sstream>
#include <random>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>

// funkcja generująca sól o zadanej długości
std::string generateSalt(int length) {
    const std::string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    std::random_device rd; // źródło losowości (sprzętowe)

    // mt19937 to algorytm (Mersenne Twister), który przerabia ten impuls z procesora na liczby
    std::mt19937 generator(rd()); // generator liczb losowych

    // distribution mówi: "losuj liczby tylko z zakresu od 0 do (liczba znaków - 1)"
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
        // setw(2) - każdy bajt musi mieć 2 znaki (np. "0a" zamiast "a")
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

int main() {
    crow::SimpleApp app; // tworzy aplikacje serwera - zaczynam od PasswordHasher

    // Prosty endpoint POST - definiuję trase "/api/hash", akceptuje tylko metodę POST (wysyłanie danych)
    CROW_ROUTE(app, "/api/hash").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        std::this_thread::sleep_for(std::chrono::milliseconds(2000)); // zatrzymaj na 2 sekundy
        // zamienia to, co przyszło (body), na obiekt (JSON)
        auto x = crow::json::load(req.body);
        
        // Jeśli JSON jest pusty lub błędny
        if (!x) return crow::response(400, "Brak JSON");

        // pobieram hasło - wyciągam z JSON-a jako string (.s())
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

        // tworzę odpowiedź (wartości, które wrócą do Reacta)
        crow::json::wvalue z;
        z["hash"] = finalHash;
        z["salt"] = salt; // odsyła sół, żebym ją widział (w produkcji zapisuje się ją w bazie)
        z["strength"] = strength; // można wysłać oba pola (?)
        z["combined"] = saltedPassword; // pokazuje też, co realnie poszło do hashera

        // zwraca czysty JSON (CORS mnie nie obchodzi, bo mam Proxy) - odsyła JSON do Reacta
        return crow::response(z);
    });

    // generator

    CROW_ROUTE(app, "/api/generator").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Błąd JSON");

        int length = x["length"].i();
        bool useNumbers = x["numbers"].b();
        bool useSpecial = x["special"].b();

        // I - buduję pule znaków
        std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        if (useNumbers) charset += "0123456789";
        if (useSpecial) charset += "!@#$%^&*()-_=+[]{}";

        // II - losowanie (style secure unicorn)
        std::string password = "";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, charset.size() - 1);

        for (int i = 0; i < length; ++i) {
            password += charset[distribution(generator)];
        }

        // III - odsyłam wynik do reacta
        crow::json::wvalue z;
        z["password"] = password;
        return crow::response(z);
    });

    // entropy guardian
    
    // E = L x log2(R) - L (Length): długość hasła [liczba znaków] - R (Range/Pool): wielkość puli znaków, z której korzystamy 

    // R: 
    // * tylko małe litery (a-z): 26
    // * tylko duże litery (A-Z): 26
    // * cyfry (0-9): 10
    // * znaki specjalne (!@#$%): ok. 32

    CROW_ROUTE(app, "/api/entropy-analysis").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req) {
        auto x = crow::json::load(req.body);
        if (!x) return crow::response(400, "Błąd JSON");

        bool hasLower = false;
        bool hasUpper = false;
        bool hasDigit = false;
        bool hasSpecial = false;
        int poolSize = 0;
        double Entropy = 0;
        std::string verdict = "Bardzo słabe";

        std::string password = x["password"].s();

        for (char c : password) {
            if (c >= '0' && c <= '9') { hasDigit = true; } 
            else if (c >= 'a' && c <= 'z') { hasLower = true; } 
            else if (c >= 'A' && c <= 'Z') { hasUpper = true; } 
            else { hasSpecial = true; };
        }

        if (hasLower) poolSize += 26;
        if (hasUpper) poolSize += 26;
        if (hasDigit) poolSize += 10;
        if (hasSpecial) poolSize += 32;

        if (poolSize > 0) {
            Entropy = password.length() * std::log2(poolSize);
        }

        if (Entropy >= 128) {
            verdict = "Bardzo silne (God Mode)";
        } else if (Entropy >= 60) {
            verdict = "Silne";
        } else if (Entropy >= 36) {
            verdict = "Średnie";
        } else if (Entropy >= 28) {
            verdict = "Słabe";
        } else {
            verdict = "Tragedia, zmień to";
        }

        crow::json::wvalue result;
        result["entropy"] = Entropy; // wysyła do reacta
        result["verdict"] = verdict;
        return crow::response(result);
    });

    // uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}