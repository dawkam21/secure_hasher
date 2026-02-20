#include "crow_all.h"
#include "utils.hpp"
#include "crypto_logic.hpp"
#include <chrono>
#include <thread>

int main() {
    crow::SimpleApp app; // tworzy aplikacje serwera - zaczynam od PasswordHasher

    // --- ENDPOINT: HASHING ---

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

    // --- ENDPOINT: GENERATOR ---

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

    // --- ENDPOINT: ENTROPY ---

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

        std::string verdict = "Bardzo słabe";
        
        std::string password = x["password"].s();

        bool hasLower = false;
        bool hasUpper = false;
        bool hasDigit = false;
        bool hasSpecial = false;
        int poolSize = 0;
        double entropy = (poolSize > 0) ? password.length() * std::log2(poolSize) : 0;
        
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
            entropy = password.length() * std::log2(poolSize);
        }
        
        if (entropy >= 128) {
            verdict = "Bardzo silne (God Mode)";
        } else if (entropy >= 60) {
            verdict = "Silne";
        } else if (entropy >= 36) {
            verdict = "Średnie";
        } else if (entropy >= 28) {
            verdict = "Słabe";
        } else {
            verdict = "Tragedia, zmień to";
        }
        
        // oblicza całkowitą liczbę kombinacji: 2^Entropy
        double totalCombinations = std::pow(2.0, entropy);

        crow::json::wvalue result;
        result["entropy"] = entropy; // wysyła do reacta
        result["verdict"] = verdict;

        // tworzy listę profili dla Reacta
        std::vector<crow::json::wvalue> attack_scenarios;
        for (const auto& profile : getProfiles()) {
            crow::json::wvalue scenario;

            // średni czas łamania to połowa wszystkich kombinacji (statystycznie)
            double secondsToCrack = (totalCombinations / 2.0) / profile.speed;
            
            scenario["name"] = profile.name;
            scenario["time_text"] = formatTime((secondsToCrack)); // średni czas łamania to połowa wszystkich kombinacji (statystycznie)
            attack_scenarios.push_back(std::move(scenario));


            scenario["time_text"] = formatTime(secondsToCrack);
            attack_scenarios.push_back(std::move(scenario));
        }

        result["attacks"] = std::move(attack_scenarios);
        return crow::response(result);
    });

    // --- ENDPOINT: BRUTEFORCE (Symulator) ---

    CROW_ROUTE(app, "/api/bruteforce").methods("POST"_method)
    ([](const crow::request& req) {

        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400);

        int length = body["length"].i();
        int charsetSize = body["charsetSize"].i();
        std::string hashStr = body["hashFunction"].s();

        HashFunction hf;

        if (hashStr == "SHA256")
            hf = HashFunction::SHA256;
        else if (hashStr == "BCRYPT_10")
            hf = HashFunction::BCRYPT_10;
        else if (hashStr == "BCRYPT_12")
            hf = HashFunction::BCRYPT_12;
        else
            hf = HashFunction::ARGON2ID;

        long double entropy = calculateEntropy(length, charsetSize);
        long double hashRate = getHashRate(hf);
        long double logTime = calculateLog10CrackTime(entropy, hashRate);

        crow::json::wvalue result;
        result["entropyBits"] = (double)entropy;
        result["log10Seconds"] = (double)logTime;
        result["humanReadable"] = formatHumanReadable(logTime);

        return crow::response(result);
    });

    // uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}