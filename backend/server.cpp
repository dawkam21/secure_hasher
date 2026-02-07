#include "crow_all.h"

int main() {
    crow::SimpleApp app;

    // Prosty endpoint POST
    CROW_ROUTE(app, "/api/hash").methods(crow::HTTPMethod::POST)
    ([](const crow::request& req){
        auto x = crow::json::load(req.body);
        
        // Jeśli JSON jest pusty lub błędny
        if (!x) return crow::response(400, "Bad Request");

        // Pobieramy dane
        std::string pass = x["password"].s();

        // Tworzymy odpowiedź
        crow::json::wvalue z;
        z["hash"] = "Backend_Odebral_" + pass;
        
        // Zwracamy czysty JSON (CORS nas nie obchodzi, bo mamy Proxy)
        return crow::response(z);
    });

    // Uruchomienie na porcie 18080
    app.port(18080).bindaddr("0.0.0.0").multithreaded().run();
}