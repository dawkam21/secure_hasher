import { useState } from 'react';

function App() {
  const [input, setInput] = useState<string>("");
  const [result, setResult] = useState<string>("");

  const sendToCpp = async () => {
    // walidacja - jeśli pole jest puste, nie wysyłaj nic
    if (!input.trim()) {
      alert("Trzeba coś wpisać!");
      return;
    }

    try {
      // WAŻNE: Tu nie ma już "http://localhost:18080", jest tylko ścieżka
      const response = await fetch("/api/hash", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ password: input }),
      });

      const data = await response.json();
      setResult(data.hash); // wyświetla to, co c++ odeśle
    } catch (e) {
      console.error(e);
      setResult("Błąd połączenia z backendem (sprawdź czy serwer C++ działa)");
    }
  };

  return (
    <section className="main">
      <h1>React + C++ (Proxy Mode)</h1>
      <h2>Secure Unicorn: Krok I</h2>
      <div className="cpp_message">
        <label htmlFor="">Wpisz wiadomość do C++: </label>
        {/* łączy input ze stanem Reacta (tzw. Controlled Component) */}
        <input type="text" value={input} onChange={(e) => setInput(e.target.value)} placeholder='np. TajneHasło123'/>
      </div>

        <button onClick={sendToCpp}>Wyślij do silnika C++</button>
      <div className="cpp_response">
        <strong>Odpowiedź z backendu:</strong>
        <p style={{ color: "blue", fontSize: "18px" }}>{result}</p>
      </div>
    </section>
  );
}

export default App;