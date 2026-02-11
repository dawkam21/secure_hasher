import { useState } from 'react';
import { Hasher } from './components/Hasher/Hasher';
import { Generator } from './components/Generator/Generator';
import './App.css';

// Definiujemy dostępne widoki dla TypeScripta
type View = 'hasher' | 'generator';

function App() {
  // Stan kontrolujący, którą zakładkę widzimy
  const [activeTab, setActiveTab] = useState<View>('hasher');

  return (
    <main className="app-container">
      {/* Nagłówek wspólny dla całej aplikacji */}
      <header className="app-header">
        <h1>Secure Unicorn Toolkit 🦄</h1>
        <nav className="app-nav">
          <button 
            className={`nav-button ${activeTab === 'hasher' ? 'active' : ''}`}
            onClick={() => setActiveTab('hasher')}
          >
            Password hasher
          </button>
          <button 
            className={`nav-button ${activeTab === 'generator' ? 'active' : ''}`}
            onClick={() => setActiveTab('generator')}
          >
            Generator
          </button>
        </nav>
      </header>

      {/* Dynamiczne wyświetlanie komponentu */}
      <section className="app-content">
        {activeTab === 'hasher' ? (
          <Hasher />
        ) : (
          <Generator />
        )}
      </section>

      <footer className="app-footer">
        <p>&copy; 2026 Secure Unicorn Security Tools</p>
      </footer>
    </main>
  );
}

export default App;