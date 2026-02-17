import { useState } from "react"

export function Generator() { 

    const [passLength, setPassLength] = useState<number>(8);
    const [wantSpecial, setWantSpecial] = useState<boolean>(false);
    const [wantNumbers, setWantNumbers] = useState<boolean>(false);

    const [generatedPassword, setGeneratedPassword] = useState<string>("");

    const [isLoading, setIsLoading] = useState<boolean>(false);

    const handleGenerate = async() => {
        setIsLoading(true);
        setGeneratedPassword(""); // czyszczę stare hasło

        try {
            const response = await fetch("/api/generator", {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify ({
                    // tu wysyła aktualne wartości ze stanu
                    "length": passLength,
                    "numbers": wantNumbers,
                    "special": wantSpecial
                }),
            });

            const data = await response.json();

            // C++ odsyła obiekt { "password: "..." }, więc tak go odbieram
            setGeneratedPassword(data.password)
        } catch (e) {
            console.error("Błąd generatora: ", e);
            setGeneratedPassword("Błąd połączenia z serwerem C++");
        } finally {
            setIsLoading(false);
        }
    };

    return <section className="container">
        <h2>Password Generator 🦄</h2>

        {/* I - Suwak długości (range) */}
        <div className="control-group">
            <label htmlFor="">Długość hasła: {passLength}</label>
            <input 
                type="range"
                value={passLength}
                onChange={(e) => setPassLength(+e.target.value)}
                min="8"
                max="32" 
            />
        </div>
        
        {/* Checkboxy */}
        <div className="control-group">
            <label htmlFor="">
                <input 
                    type="checkbox" 
                    checked={wantSpecial}
                    onChange={(e) => setWantSpecial(e.target.checked)}
                />
                Znaki specjalne (!@#$%)
            </label>
        </div>

        <div className="control-group">
            <label htmlFor="">
                <input 
                    type="checkbox"
                    checked={wantNumbers}
                    onChange={(e) => setWantNumbers(e.target.checked)}
                />
                Cyfry (0-9)
            </label>
        </div>

        <button 
            className="generator-button"
            onClick={handleGenerate}
            disabled={isLoading} // to sprawia, że przycisk gaśnie
        >
            {isLoading ? "Generowanie..." : "Generuj bezpieczne hasło"}
        </button>

        {/* wyświetlanie wyniku */}
        {generatedPassword && ( // warunkowe renderowanie {generatedPassword && ...}: Dzięki temu napis "Twoje nowe hasło" nie straszy na ekranie, dopóki ktoś faktycznie go nie wygeneruje.
            <div className="result-container">
                <label>Twoje nowe hasło: </label>
                <div className="password-display">
                    <input 
                        type="text" 
                        readOnly
                        value={generatedPassword}
                        className="password-input"
                    />
                    <button
                        onClick={() => navigator.clipboard.writeText(generatedPassword)}
                        className="copy-button"
                    >
                        Kopiuj📋
                    </button>
                </div>
            </div>
        )}
    </section> 
}