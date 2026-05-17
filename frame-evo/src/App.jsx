import { useState } from 'react'
import heroImg from './assets/hero.png'
import './App.css'
import Controle from './Controle'

function App() {
  const [view, setView] = useState('home')

  if (view === 'controle') {
    return (
      <>
        <button className="back-button" onClick={() => setView('home')}>
          ← Voltar ao Início
        </button>
        <Controle />
      </>
    )
  }

  return (
    <>
      <section id="center">
        <div className="hero">
          <img src={heroImg} className="base" width="220" alt="Robô FRAME" />
        </div>
        <div className="text-container">
          <h1>FRAME Evo</h1>
          <p className="subtitle">
            Plataforma Educacional de Robótica
          </p>
          <p className="description">
            Controle e programe seu robô FRAME de forma intuitiva através da Web Serial API.
          </p>
        </div>
        <button
          type="button"
          className="main-button"
          onClick={() => setView('controle')}
        >
          Iniciar Controle do Robô
        </button>
      </section>

      <div className="ticks"></div>

      <section id="info-section">
        <div className="info-card">
           <h3>Conexão Direta</h3>
           <p>Utilize o cabo USB para conectar seu robô diretamente ao navegador.</p>
        </div>
        <div className="info-card">
           <h3>Programação em Fila</h3>
           <p>Adicione comandos de movimento e execute sequências completas.</p>
        </div>
        <div className="info-card">
           <h3>Interface Intuitiva</h3>
           <p>Desenvolvido para facilitar o aprendizado de lógica e robótica.</p>
        </div>
      </section>

      <div className="ticks"></div>
      <section id="footer">
        <p>&copy; 2024 Projeto de Robótica Educacional FRAME</p>
      </section>
    </>
  )
}

export default App
