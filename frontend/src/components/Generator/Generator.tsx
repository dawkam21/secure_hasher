import { useState } from "react"

export function Generator() { 

    const [passLength, setPassLength] = useState<number>(12);
    const [wantSpecial, setWantSpecial] = useState<boolean>(false);
    const [wantNumbers, setWantNumbers] = useState<boolean>(false);

    // const response = await fetch()

    // const data = await response.json();
    //   setWantSpecial(data.wantSpecial); // wyświetla to, co c++ odeśle
    //   setWantNumbers(data.wantNumbers);
    //   setPassLength(data.passLength);

    return <section className="container">
        <h2>Secure Unicorn Generator 🦄</h2>

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

        <button className="generator-button">Generuj bezpieczne hasło</button>
    </section> 
}