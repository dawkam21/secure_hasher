#include "crow_all.h"

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

        // tworzę odpowiedź
        crow::json::wvalue z;
        z["hash"] = strength;
        
        // zwraca czysty JSON (CORS mnie nie obchodzi, bo mam Proxy)
        return crow::response(z);
    });

    // uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}