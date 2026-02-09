import { useState } from 'react';
import { ResultBox } from '../common/ResultBox';

export function Hasher() {
  // zmienne stanu (State)
  const [input, setInput] = useState<string>("");
  const [isLoading, setIsLoading] = useState<boolean>(false);

  // pudełka na dane z C++
  const [strength, setStrength] = useState<string>("");
  const [combined, setCombined] = useState<string>("");
  const [hash, setHash] = useState<string>("");

  const sendToCpp = async () => {
    // walidacja - jeśli pole jest puste, nie wysyłaj nic
    if (!input.trim()) return;

    // włącza tryb ładowania
    setIsLoading(true);
    setStrength(""); // czyści poprzedni wynik, żeby nie mylił
    setCombined("");
    setHash("");

    try {
      // WAŻNE: Tu nie ma już "http://localhost:18080", jest tylko ścieżka
      const response = await fetch("/api/hash", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ password: input }),
      });

      const data = await response.json();
      setStrength(data.strength); // wyświetla to, co c++ odeśle
      setCombined(data.combined);
      setHash(data.hash);
    } catch (e) {
      console.error(e);
      setStrength("Błąd połączenia z backendem (sprawdź czy serwer C++ działa)"); // używam strength do pokazania błędu
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <section className="main">
      <h1>React + C++ (Proxy Mode)</h1>
      <h2>Secure Unicorn Hasher</h2>
      {/* zawijam wszystko w form*/}
      <form className='myForm' onSubmit={(e) => {
        e.preventDefault(); // zapobiega przeładowaniu strony przez przeglądarkę
        sendToCpp();
      }}>
        <div className="cpp_message">
          <label htmlFor="">Wpisz swoje hasło - musi zawierać przynajmniej 8 znaków, w tym 1 znak specjalny: </label>
          {/* łączy input ze stanem Reacta (tzw. Controlled Component) */}
          <input 
            type="text" 
            value={input} 
            onChange={(e) => setInput(e.target.value)} 
            placeholder='np. TajneHasło123'/>
        </div>

        <button 
          type="submit"
          disabled={ isLoading || !input.trim() } // przycisk staje się szary i nieklikalny podczas pracy
          className='hasher-button'
        > {isLoading ? "Analizowanie..." : "Sprawdź"}
        </button>
      </form>
      <div className="cpp_response">
        <ResultBox
          strength={strength}
          combined={combined}
          hash={hash}
        />
      </div>
    </section>
  );
}
