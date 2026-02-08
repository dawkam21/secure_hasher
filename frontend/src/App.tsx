import { useState } from 'react';

function App() {
  const [input, setInput] = useState<string>("");
  const [result, setResult] = useState<string>("");
  const [isLoading, setIsLoading] = useState<boolean>(false);

  const sendToCpp = async () => {
    // walidacja - jeśli pole jest puste, nie wysyłaj nic
    if (!input.trim()) return;

    // włącza tryb ładowania
    setIsLoading(true);
    setResult(""); // czyści poprzedni wynik, żeby nie mylił

    try {
      // WAŻNE: Tu nie ma już "http://localhost:18080", jest tylko ścieżka
      const response = await fetch("/api/hash", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ password: input }),
      });

      const data = await response.json();
      setResult(data.strength); // wyświetla to, co c++ odeśle
    } catch (e) {
      console.error(e);
      setResult("Błąd połączenia z backendem (sprawdź czy serwer C++ działa)");
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
          disabled={ isLoading } // przycisk staje się szary i nieklikalny podczas pracy
          className='hasher-button'
        > {isLoading ? "Analizowanie..." : "Sprawdź"}
        </button>
      </form>
      <div className="cpp_response">
        <strong>Odpowiedź z backendu:</strong>
        <p style={{ 
          fontSize: "20px",
          fontWeight: "bold",
          color: result.includes("SILNE") 
            ? "#2ecc71" 
            : result.includes("ŚREDNIE")
              ? "#f1c40f"
              :"#e74c3c" 
        }}>
          {result || "Czekam na hasło..."}
        </p>
      </div>
    </section>
  );
}

export default App;