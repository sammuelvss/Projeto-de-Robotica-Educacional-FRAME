import React, { useState, useRef, useEffect } from 'react';
import './Controle.css';

const Controle = () => {
    const [status, setStatus] = useState('Pronto para começar?');
    const [conectado, setConectado] = useState(false);
    const [filaComandos, setFilaComandos] = useState('');
    const [logs, setLogs] = useState([{ prefix: '🤖', msg: 'Olá! Vamos programar meu caminho?' }]);
    
    const portRef = useRef(null);
    const writerRef = useRef(null);
    const logTerminalRef = useRef(null);

    const addLog = (msg, isComando = false) => {
        setLogs(prev => [...prev, { prefix: isComando ? '⚡' : '🤖', msg }]);
    };

    useEffect(() => {
        if (logTerminalRef.current) {
            logTerminalRef.current.scrollTop = logTerminalRef.current.scrollHeight;
        }
    }, [logs]);

    const conectar = async () => {
        if ('serial' in navigator) {
            try {
                const port = await navigator.serial.requestPort();
                await port.open({ baudRate: 9600 });
                const textEncoder = new TextEncoderStream();
                textEncoder.readable.pipeTo(port.writable);
                writerRef.current = textEncoder.writable.getWriter();
                portRef.current = port;

                setStatus('Uhuu! Estou conectado!');
                setConectado(true);
                addLog('Conectado! Escolha as setas para me mover.');

            } catch (error) {
                addLog('Ops! Tente conectar de novo.');
            }
        } else {
            alert('Use o Google Chrome no computador para brincar!');
        }
    };

    const adicionarAFila = (comando) => {
        setFilaComandos(prev => prev + comando);
        
        let emoji = "";
        if(comando === 'F') emoji = "⬆️";
        if(comando === 'B') emoji = "⬇️";
        if(comando === 'L') emoji = "⬅️";
        if(comando === 'R') emoji = "➡️";
        
        addLog("Adicionei: " + emoji);
    };

    const limparFilaLocal = async () => {
        setFilaComandos('');
        addLog("Fila limpa! Vamos criar uma nova?");
        
        if (writerRef.current) {
            try {
                await writerRef.current.write("X\n");
            } catch (e) {
                console.error(e);
            }
        }
    };

    const enviarFilaParaRobo = async () => {
        if (!writerRef.current) {
            alert("Clique em 'Conectar' primeiro!");
            return;
        }

        if (filaComandos.length === 0) {
            alert("Me diga para onde ir primeiro!");
            return;
        }

        try {
            await writerRef.current.write(filaComandos + "\n");
            addLog("Partiu! Executando meus movimentos...", true);
            setFilaComandos('');
        } catch (error) {
            addLog("Tive um probleminha ao enviar.");
        }
    };

    const renderFilaPreview = () => {
        return filaComandos.split('').map((cmd, i) => {
            let icon = "";
            if(cmd === 'F') icon = "⬆️";
            if(cmd === 'B') icon = "⬇️";
            if(cmd === 'L') icon = "⬅️";
            if(cmd === 'R') icon = "➡️";
            return <span key={i} className="fila-icon">{icon}</span>;
        });
    };

    return (
        <div className="controle-page">
            <div className="main-card">
                <header className="header-ludico">
                    <h1>Controle FRAME</h1>
                    <div className={`status-pill ${conectado ? 'on' : 'off'}`}>
                        {status}
                    </div>
                </header>

                <main className="game-board">
                    {!conectado ? (
                        <div className="welcome-area">
                            <p>Conecte o cabo para começarmos a aventura!</p>
                            <button className="btn-conectar-ludico" onClick={conectar}>
                                🔌 Ligar Robô
                            </button>
                        </div>
                    ) : (
                        <>
                            <div className="remote-control">
                                <div className="d-pad">
                                    <button className="dir-btn up" onClick={() => adicionarAFila('F')}>⬆️</button>
                                    <button className="dir-btn left" onClick={() => adicionarAFila('L')}>⬅️</button>
                                    <button className="dir-btn down" onClick={() => adicionarAFila('B')}>⬇️</button>
                                    <button className="dir-btn right" onClick={() => adicionarAFila('R')}>➡️</button>
                                    <div className="center-dot"></div>
                                </div>
                            </div>

                            <div className="programming-zone">
                                <h3>Minha Sequência:</h3>
                                <div className="preview-area">
                                    {filaComandos.length > 0 ? renderFilaPreview() : <span className="placeholder">Aguardando comandos...</span>}
                                </div>
                                <div className="action-buttons">
                                    <button className="action-btn clear" onClick={limparFilaLocal}>🗑️ Limpar</button>
                                    <button className="action-btn go" onClick={enviarFilaParaRobo}>▶️ Iniciar!</button>
                                </div>
                            </div>
                        </>
                    )}
                </main>

                <footer className="terminal-ludico" ref={logTerminalRef}>
                    {logs.map((log, i) => (
                        <div key={i} className="log-line">
                            <span className="emoji">{log.prefix}</span> {log.msg}
                        </div>
                    ))}
                </footer>
            </div>
        </div>
    );
};

export default Controle;
