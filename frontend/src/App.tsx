import './App.css';
import { useEffect, useState } from 'react';
import { Hasher } from './components/Hasher/Hasher';
import { Generator } from './components/Generator/Generator';
import { EntropyAnalysis } from './components/EntropyGuardian/EntropyAnalysis/EntropyAnalysis';

const tabs = {
  hasher: <Hasher />,
  generator: <Generator />,
  entropyAnalysis: <EntropyAnalysis />
};

function App() {

  type TabType = keyof typeof tabs;

  // stan kontrolujący, którą zakładkę widzimy
  const [activeTab, setActiveTab] = useState<TabType>(() => {
    const savetTab = localStorage.getItem('activeTab');
    return (savetTab as TabType) || 'hasher';
  });

  // zapisuje każdą zmiane - można to wpiąć w funkcję onClick przycisku zmiany tabów
  useEffect(() => {
    localStorage.setItem('activeTab', activeTab);
  }, [activeTab]);
 
  return (
    <main className="app-container">
      {/* nagłówek wspólny dla całej aplikacji */}
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
            className={`nav-button ${activeTab === 'entropyAnalysis' ? 'active' : ''}`}
            onClick={() => setActiveTab('entropyAnalysis')}
          >
            Entropy Guardian
          </button>
        </nav>
      </header>

      {/* dynamiczne wyświetlanie komponentu */}
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