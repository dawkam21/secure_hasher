// mózg aplikacji

#pragma once
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <vector>
#include "utils.hpp"

// funkcja hashująca (SHA-256 z OpenSSl 3.0)
inline std::string sha256(const std::string& str) {
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

// obliczanie entropii: L * log2(R)
inline long double calculateEntropy(int length, int charsetSize) {
    if (charsetSize <= 0) return 0;
    return (long double)length * std::log2((long double)charsetSize);
}

// pobieranie szybkości hashowania
inline long double getHashRate(HashFunction hf) {
    switch (hf) {
        case HashFunction::SHA256:    return 1e11L; // 100 miliardów/s (GPU)
        case HashFunction::BCRYPT_10: return 1000.0L;
        case HashFunction::BCRYPT_12: return 200.0L;
        case HashFunction::ARGON2ID:  return 50.0L;
        default:                      return 1e8L;
    }
}

// matematyka logarytmiczna dla ogromnych liczb
inline long double calculateLog10CrackTime(long double entropyBits, long double hashRate) {
    // Statystycznie łamiemy w połowie czasu (entropy - 1)
    return (entropyBits - 1.0L) * std::log10(2.0L) - std::log10(hashRate);
}

inline std::string formatHumanReadable(long double log10Seconds) {

    long double seconds = std::pow(10.0L, log10Seconds);

    long double years = seconds / (60 * 60 * 24 * 365);

    if (years > 1e6)
        return "> 1 million years";

    if (years >= 1)
        return std::to_string((long long)years) + " years";

    long double days = seconds / (60 * 60 * 24);

    if (days >= 1)
        return std::to_string((long long)days) + " days";

    long double hours = seconds / (60 * 60);

    if (hours >= 1)
        return std::to_string((long long)hours) + " hours";

    return std::to_string((long long)seconds) + " seconds";
}

// model dla profilu ataku
struct AttackProfile {
    std::string name;
    double speed; // próby na sekundę
};

inline std::vector<AttackProfile> getProfiles() {
    return {
        {"Zwykły procesor (CPU)", 1e8},
        {"Karta graficzna RTX 4090", 1e12},
        {"Farma serwerów (Botnet)", 1e15}
    };
}
