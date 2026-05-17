import React, { useState, useRef, useEffect } from 'react';
import './Controle.css';

const Controle = () => {
    const [status, setStatus] = useState('Desconectado');
    const [conectado, setConectado] = useState(false);
    const [filaComandos, setFilaComandos] = useState('');
    const [logs, setLogs] = useState([{ prefix: '>', msg: 'Aguardando conexão...' }]);
    
    const portRef = useRef(null);
    const writerRef = useRef(null);
    const logTerminalRef = useRef(null);

    const addLog = (msg, isComando = false) => {
        setLogs(prev => [...prev, { prefix: isComando ? '🤖' : '>', msg }]);
    };

    useEffect(() => {
        if (logTerminalRef.current) {
            logTerminalRef.current.scrollTop = logTerminalRef.current.scrollHeight;
        }
    }, [logs]);

    const conectar = async () => {
        if ('serial' in navigator) {
            try {
                // Pede pro usuário selecionar a porta COM
                const port = await navigator.serial.requestPort();
                
                // Abre a porta com o mesmo BaudRate do Arduino (9600)
                await port.open({ baudRate: 9600 });
                
                // Prepara para escrever dados
                const textEncoder = new TextEncoderStream();
                textEncoder.readable.pipeTo(port.writable);
                writerRef.current = textEncoder.writable.getWriter();
                portRef.current = port;

                setStatus('Conectado com Sucesso!');
                setConectado(true);
                addLog('Porta serial aberta. Pronto para comandar!');

            } catch (error) {
                addLog('Erro ao conectar: ' + error);
            }
        } else {
            alert('Seu navegador não suporta a Web Serial API. Use o Google Chrome no computador.');
        }
    };

    const adicionarAFila = (comando) => {
        setFilaComandos(prev => prev + comando);
        
        let nomeComando = "";
        if(comando === 'F') nomeComando = "Frente";
        if(comando === 'B') nomeComando = "Trás";
        if(comando === 'L') nomeComando = "Esquerda";
        if(comando === 'R') nomeComando = "Direita";
        
        addLog("Adicionado: " + nomeComando);
        addLog("Fila atual: " + (filaComandos + comando), true);
    };

    const limparFilaLocal = async () => {
        setFilaComandos('');
        addLog("Fila limpa localmente.");
        
        if (writerRef.current) {
            try {
                await writerRef.current.write("X\n");
                addLog("Comando de RESET enviado ao robô.", true);
            } catch (e) {
                addLog("Erro ao resetar robô: " + e);
            }
        }
    };

    const enviarFilaParaRobo = async () => {
        if (!writerRef.current) {
            alert("Por favor, conecte o robô primeiro clicando em 'Conectar'.");
            return;
        }

        if (filaComandos.length === 0) {
            alert("Adicione movimentos à fila primeiro!");
            return;
        }

        try {
            // Envia a string (ex: "FFLRB") que o Arduino processará como uma sequência
            await writerRef.current.write(filaComandos + "\n");
            addLog("Enviando sequência: " + filaComandos, true);
            
            // Limpa a fila após enviar para nova programação
            setFilaComandos('');
        } catch (error) {
            addLog("Erro ao enviar: " + error);
        }
    };

    return (
        <div className="controle-container">
            <div className="container-inner">
                <h1>Controle Frame</h1>
                <div className={`status ${conectado ? 'conectado' : ''}`}>{status}</div>
                
                {!conectado && (
                    <button id="btnConectar" onClick={conectar}>🔌 Conectar ao Robô</button>
                )}

                <div className="controle-grid">
                    <button className="btn-dir btn-up" onClick={() => adicionarAFila('F')}>⬆️</button>
                    <button className="btn-dir btn-left" onClick={() => adicionarAFila('L')}>⬅️</button>
                    <button className="btn-dir btn-down" onClick={() => adicionarAFila('B')}>⬇️</button>
                    <button className="btn-dir btn-right" onClick={() => adicionarAFila('R')}>➡️</button>
                </div>

                <div className="acoes">
                    <button className="btn-acao btn-limpar" onClick={limparFilaLocal}>🗑️ Limpar</button>
                    <button className="btn-acao btn-play" onClick={enviarFilaParaRobo}>▶️ Executar</button>
                </div>

                <div className="terminal-display" ref={logTerminalRef}>
                    {logs.map((log, i) => (
                        <div key={i}>
                            <span className="log-prefix">{log.prefix}</span> {log.msg}
                        </div>
                    ))}
                </div>
            </div>
        </div>
    );
};

export default Controle;
