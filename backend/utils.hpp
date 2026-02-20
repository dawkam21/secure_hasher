// to co sprząta dane

#pragma once
#include <string>
#include <random>
#include <vector>

// słowo 'inline' pozwala trzymać kod funkcji w nagłówku

// funkcja generująca sól o zadanej długości
inline std::string generateSalt(int length) {
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

enum class HashFunction {
    SHA256,
    BCRYPT_10,
    BCRYPT_12,
    ARGON2ID
};

struct SimulationResult {
    long double entropyBits;
    long double log10Seconds;
    std::string humanReadable;
};

long double calculateEntropy(int length, int charsetSize);

long double getHashRate(HashFunction hf);

long double calculateLog10CrackTime(long double entropyBits, long double hashRate);

std::string formatHumanReadable(long double log10Seconds);

// funkcja formatująca czas
inline std::string formatTime(double seconds) {
    if (seconds < 1) return "Natychmiast";
    if (seconds < 60) return std::to_string((int)seconds) + " s";
    if (seconds < 3600) return std::to_string((int)(seconds / 60)) + " min";
    if (seconds < 86400) return std::to_string((int)(seconds / 3600)) + " h";
    
    double days = seconds / 86400;
    if (days < 365) return std::to_string((int)days) + " dni";
    
    double years = days / 365;
    if (years > 1000000) return "Wieki (1M+ lat)";
    return std::to_string((int)years) + " lat";
};
