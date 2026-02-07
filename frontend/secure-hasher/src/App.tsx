import { useState } from 'react';

function App() {
  const [result, setResult] = useState("");

  const sendToCpp = async () => {
    try {
      // WAŻNE: Tu nie ma już "http://localhost:18080", jest tylko ścieżka
      const response = await fetch("/api/hash", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ password: "test" }),
      });

      const data = await response.json();
      setResult(data.hash);
    } catch (e) {
      console.error(e);
      setResult("Błąd połączenia (sprawdź czy serwer C++ działa)");
    }
  };

  return (
    <div className="main">
      <h1>React + C++ (Proxy Mode)</h1>
      <button onClick={sendToCpp}>Wyślij do C++</button>
      <p>Odpowiedź: <b>{result}</b></p>
    </div>
  );
}

export default App;