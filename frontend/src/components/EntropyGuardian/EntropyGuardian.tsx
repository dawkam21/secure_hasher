import { useState } from "react";

interface EntropyResponse {
    entropy: number;
    verdict: string;
}

export function EntropyGuardian() {

    const [result, setResult] = useState<EntropyResponse | null>(null);
    const [loading, setLoading] = useState<boolean>(false);

    const handleCheckEntropy = async (password: string) => {
        if (!password) { // chroni przed pustym requestem i czyści wynik, gdy input zostanie wyczyszczony
            setResult(null);
            return;
        }

        setLoading(true); // natychmiast wywołuje re-render
        setResult(null); // usuwa poprzedni wynik - zapobiega sytuacji -> loading + stary wynik widoczny jednocześnie

        try {
            // 2 sekundy opóźnienia
            await new Promise(resolve => setTimeout(resolve, 2000));

            const response = await fetch('/api/entropy-guardian', {
                method: "POST",
                headers: { "Content-Type": "application/json" },
                body: JSON.stringify ({ password })
            });

            if (!response.ok) throw new Error('Błąd serwera C++');

            const data: EntropyResponse = await response.json();
            setResult(data);

        } catch (error) {
            console.error("Layet 7 Error: ", error);
        } finally {
            setLoading(false);
        }
    };

    return (
        <section className="container">
            <h2>🛡️ Entropy Guardian</h2>
            <input 
                type="password"
                onChange={(e) => handleCheckEntropy(e.target.value)}
                placeholder="Wpisz hasło..."
            />
            {/* 
                true && coś -> coś
                false && coś -> false
                jeśli loading === true -> renderuje <p>Sprawdzanie...</p>
                jeśli loading === false -> nie renderuje nic 

                skrót dla -> {loading ? <p>Sprawdzanie...</p> : null}
            */}
            {loading && <p>Sprawdzanie...</p>}

            
            {/* na starcie result = null, więc nie wyświetla nic */}
            {result && (
                <div className="box">
                    <p>Entropia: <b>{result.entropy.toFixed(2)} bitów</b></p>
                    <p>Werdykt: <span className="span">{result.verdict}</span></p>
                </div>
            )}
        </section>
    )
}