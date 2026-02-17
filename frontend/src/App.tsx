import './App.css';
import { useState } from 'react';
import { Hasher } from './components/Hasher/Hasher';
import { Generator } from './components/Generator/Generator';
import { EntropyGuardian } from './components/EntropyGuardian/EntropyGuardian';

const tabs = {
  hasher: <Hasher />,
  generator: <Generator />,
  entropyGuardian: <EntropyGuardian />
};

function App() {

  type TabType = 'hasher' | 'generator' | 'entropyGuardian';
  // Stan kontrolujący, którą zakładkę widzimy
  const [activeTab, setActiveTab] = useState<TabType>('hasher');

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
          <button 
            className={`nav-button ${activeTab === 'entropyGuardian' ? 'active' : ''}`}
            onClick={() => setActiveTab('entropyGuardian')}
          >
            Entropy Guardian
          </button>
        </nav>
      </header>

      {/* Dynamiczne wyświetlanie komponentu */}
      <section className="app-content">
        {tabs[activeTab]}
      </section>

      <footer className="app-footer">
        <p>&copy; 2026 Secure Unicorn Security Tools</p>
      </footer>
    </main>
  );
}

export default App;