// ========== CONFIGURACOES DE VOLTAS POR COMANDO ==========
// VALORES PADRAO - CADA COMANDO REPRESENTA 1 VOLTA
// Altere estes valores para ajustar quantas voltas cada comando representa
#define VOLTAS_POR_F 0.85 // Voltas para comando F (Frente)
#define VOLTAS_POR_B 0.85 // Voltas para comando B (Trás/Back)
#define VOLTAS_POR_L 1.05 // Voltas para comando L (Esquerda/Left) - giro de 90°
#define VOLTAS_POR_R 1.05 // Voltas para comando R (Direita/Right) - giro de 90°
//============================================================================
// Correção para diretar a diferença de velocidade das rodas
#define CORRECAO_VELOCIDADE_DIR 1.0   // Ajuste fino: >1 = mais rápido, <1 = mais lento
#define CORRECAO_VELOCIDADE_ESQ 1.003 // Comece com 1.0 e ajuste
// ========================================================

// Roda Direita
const int pinoMotorDirA = 4;
const int pinoMotorDirB = 3;
const int pinoMotorDirEnable = 5;
const int pinoEncoderDir = A0;

// Roda esquerda
const int pinoMotorEsqA = 7;
const int pinoMotorEsqB = 8;
const int pinoMotorEsqEnable = 6;
const int pinoEncoderEsq = A1;

// ========== CONFIGURAÇÃO DOS LEDS DO SHIELD ESCOLA MAKER ==========
const int pinoLedEsquerdo = A5; // LED_E na placa
const int pinoLedDireito = 2;   // LED_D na placa
// ===============================================================

// ========== CONFIGURAÇÃO DO KEYPAD ==========
const int pinoKeypad = A2; // Pino analógico para o keypad
// ============================================

// ========== CONFIGURAÇÕES AJUSTÁVEIS ==========
// Dimensões do robô (em mm) - ALTERE AQUI!
#define DIAMETRO_RODA 64.0          // Diâmetro da roda
#define DISTANCIA_ENTRE_RODAS 130.0 // Distância entre centros das rodas

// Configuração do encoder - VALORES CALIBRADOS E INDEPENDENTES
#define PULSOS_POR_VOLTA_DIR 43 // Pulsos por volta - Roda DIREITA
#define PULSOS_POR_VOLTA_ESQ 43 // Pulsos por volta - Roda ESQUERDA
#define LIMITE_DETECCAO 300     // Threshold calibrado
                           // ==============================================

// Variáveis calculadas automaticamente
float CIRCUNFERENCIA_RODA;
float DISTANCIA_90GRAUS;
float VOLTAS_90GRAUS;
int PULSOS_90GRAUS_DIR; // Pulsos para 90° - roda direita
int PULSOS_90GRAUS_ESQ; // Pulsos para 90° - roda esquerda

// Variáveis para roda direita
unsigned long contadorPulsosDir = 0;
unsigned long tempoUltimoPulsoDir = 0;
int ultimoValorAnalogicoDir = 0;
bool estavaDetectadoDir = false;

// Variaveis para roda esquerda
unsigned long contadorPulsosEsq = 0;
unsigned long tempoUltimoPulsoEsq = 0;
int ultimoValorAnalogicoEsq = 0;
bool estavaDetectadoEsq = false;

// Controle dos motores
bool estaExecutando = false;
int pulsosAlvoDir = 0;
// Pulsos alvo para roda direita
int pulsosAlvoEsq = 0;
// Pulsos alvo para roda esquerda
int direcaoAtualDir = 0;
int direcaoAtualEsq = 0;

// Sistema de fila de comandos (usando array fixo para economizar memória)
char filaComandos[20] = ""; // Array fixo de caracteres
bool executandoFila = false;
int indiceComandoAtual = 0;
char direcaoFila = ' ';

// Buffer para comandos serial (tamanho reduzido)
char bufferSerial[30] = "";
int indiceBuffer = 0;

// Variaveis para controle do keypad
unsigned long ultimaLeituraKeypad = 0;
const unsigned long INTERVALO_KEYPAD = 300;
// ms entre leituras
bool botaoPressionado = false;

// Funcao para recalcular todas as variaveis geometricas
void recalcularGeometria()
{
  CIRCUNFERENCIA_RODA = PI * DIAMETRO_RODA;
  DISTANCIA_90GRAUS = (PI * DISTANCIA_ENTRE_RODAS) / 4.0;
  VOLTAS_90GRAUS = DISTANCIA_90GRAUS / CIRCUNFERENCIA_RODA;
  PULSOS_90GRAUS_DIR = round(VOLTAS_90GRAUS * PULSOS_POR_VOLTA_DIR);
  PULSOS_90GRAUS_ESQ = round(VOLTAS_90GRAUS * PULSOS_POR_VOLTA_ESQ);
}

// DECLARACOES DAS FUNCOES (prototypes)
void pararMotores();
void verificarEncoders();
void processarComandosSerial();
void processarComando(char *cmd);
bool comandoValido(char *cmd);
void executarProximoComandoFila();
void iniciarMovimento(int pulsosAlvoDirVal, int pulsosAlvoEsqVal, int direcaoDir, int direcaoEsq);
void controlarMotores();
void mostrarProgresso();
void reiniciarFila();
void alterarConfiguracoes();
void calibrarEncoders();
void lerKeypad();
void registrarComando(const char *comando);
void executarSequencia();
void piscarLedsFeedback();
int lerADCEstavel(int pino);

void setup()
{
  // Configuração dos pinos da roda direita
  pinMode(pinoMotorDirA, OUTPUT);
  pinMode(pinoMotorDirB, OUTPUT);
  pinMode(pinoMotorDirEnable, OUTPUT);

  // Configuração dos pinos da roda esquerda
  pinMode(pinoMotorEsqA, OUTPUT);
  pinMode(pinoMotorEsqB, OUTPUT);
  pinMode(pinoMotorEsqEnable, OUTPUT);

  // Configuração do pino do keypad
  pinMode(pinoKeypad, INPUT);

  // NOVO: Configura os pinos dos LEDs como saída
  pinMode(pinoLedEsquerdo, OUTPUT);
  pinMode(pinoLedDireito, OUTPUT);

  // Calcular variáveis geométricas
  recalcularGeometria();

  Serial.begin(9600);
  Serial.println(F("=== SISTEMA DE CONTROLE DE MOTORES ==="));
  Serial.println(F("Configuração atual:"));
  Serial.print(F("- Diâmetro roda: "));
  Serial.print(DIAMETRO_RODA);
  Serial.println(F(" mm"));
  Serial.print(F("- Distância rodas: "));
  Serial.print(DISTANCIA_ENTRE_RODAS);
  Serial.println(F(" mm"));
  Serial.print(F("- Pulsos/volta DIR: "));
  Serial.println(PULSOS_POR_VOLTA_DIR);
  Serial.print(F("- Pulsos/volta ESQ: "));
  Serial.println(PULSOS_POR_VOLTA_ESQ);
  Serial.print(F("- Pulsos 90° DIR: "));
  Serial.println(PULSOS_90GRAUS_DIR);
  Serial.print(F("- Pulsos 90° ESQ: "));
  Serial.println(PULSOS_90GRAUS_ESQ);

  // Mostrar configuração de voltas por comando
  Serial.println(F("Voltas por comando:"));
  Serial.print(F("- F (Frente): "));
  Serial.println(VOLTAS_POR_F);
  Serial.print(F("- B (Trás): "));
  Serial.println(VOLTAS_POR_B);
  Serial.print(F("- L (Esquerda): "));
  Serial.println(VOLTAS_POR_L);
  Serial.print(F("- R (Direita): "));
  Serial.println(VOLTAS_POR_R);

  Serial.println(F("Comandos: F, B, R, L, FB, RL"));
  Serial.println(F("C-Calibrar | X-Reset | D-Configurar"));
  Serial.println(F("=== CONTROLE POR BOTÕES ATIVADO ==="));
  Serial.println(F("Botão 1 (Verde): L"));
  Serial.println(F("Botão 2 (Azul): F"));
  Serial.println(F("Botão 3 (Amarelo): B"));
  Serial.println(F("Botão 4 (Vermelho): R"));
  Serial.println(F("Botão 5 (Preto): Executar"));
  Serial.println(F("==================================="));

  pararMotores();
}

void loop()
{
  processarComandosSerial();
  lerKeypad();

  if (executandoFila && !estaExecutando)
  {
    executarProximoComandoFila();
  }

  if (estaExecutando)
  {
    verificarEncoders();
    controlarMotores();

    // Mostra progresso a cada 500ms (reduzido)
    static unsigned long ultimoProgresso = 0;
    if (millis() - ultimoProgresso > 500)
    {
      mostrarProgresso();
      ultimoProgresso = millis();
    }
  }
}

void lerKeypad()
{
  unsigned long tempoAtual = millis();

  // Verifica se é hora de ler o keypad novamente
  if (tempoAtual - ultimaLeituraKeypad < INTERVALO_KEYPAD)
  {
    return;
  }

  ultimaLeituraKeypad = tempoAtual;

  // int valorADC = analogRead(pinoKeypad);
  int valorADC = lerADCEstavel(pinoKeypad);

  // Detecta qual botão foi pressionado
  if (valorADC < 10)
  {
    registrarComando("R"); // Botão 1 (Verde)
  }
  else if (valorADC < 60)
  {
    registrarComando("F"); // Botão 2 (Amarelo)
  }
  else if (valorADC < 155)
  {
    registrarComando("B"); // Botão 3 (Azul)
  }
  else if (valorADC < 310)
  {
    registrarComando("L"); // Botão 4 (Vermelho)
  }
  else if (valorADC < 1000)
  {
    executarSequencia(); // Botão 5 (Preto)
  }
  else
  {
    botaoPressionado = false;
  }
}

void registrarComando(const char *comando)
{
  // Evita múltiplos registros do mesmo botão
  if (botaoPressionado)
  {
    return;
  }
  piscarLedsFeedback();
  botaoPressionado = true;

  // Adiciona o comando à fila (se houver espaço)
  int tamanhoAtual = strlen(filaComandos);
  if (tamanhoAtual < sizeof(filaComandos) - 1)
  {
    strcat(filaComandos, comando);
    Serial.print(F(">>> COMANDO ADICIONADO: "));
    Serial.println(comando);
    Serial.print(F(">>> SEQUÊNCIA ATUAL: "));
    Serial.println(filaComandos);
  }
  else
  {
    Serial.println(F(">>> FILA CHEIA! Pressione Botão 5 para executar ou X para limpar"));
  }
}

void executarSequencia()
{
  if (botaoPressionado)
  {
    return;
  }
  piscarLedsFeedback();
  botaoPressionado = true;

  if (strlen(filaComandos) == 0)
  {
    Serial.println(F(">>> NENHUM COMANDO NA FILA!"));
    return;
  }

  if (executandoFila)
  {
    Serial.println(F(">>> SISTEMA OCUPADO. Aguarde..."));
    return;
  }

  Serial.print(F(">>> EXECUTANDO SEQUÊNCIA: "));
  Serial.println(filaComandos);

  indiceComandoAtual = 0;
  executandoFila = true;
  executarProximoComandoFila();
}

void verificarEncoders()
{
  // Verifica encoder da roda direita
  int valorAnalogicoDir = analogRead(pinoEncoderDir);
  int diferencaDir = abs(valorAnalogicoDir - ultimoValorAnalogicoDir);

  if (diferencaDir > LIMITE_DETECCAO && !estavaDetectadoDir)
  {
    contadorPulsosDir++;
    tempoUltimoPulsoDir = millis();
    estavaDetectadoDir = true;
  }
  else if (diferencaDir <= LIMITE_DETECCAO)
  {
    estavaDetectadoDir = false;
  }

  ultimoValorAnalogicoDir = valorAnalogicoDir;

  // Verifica encoder da roda esquerda
  int valorAnalogicoEsq = analogRead(pinoEncoderEsq);
  int diferencaEsq = abs(valorAnalogicoEsq - ultimoValorAnalogicoEsq);

  if (diferencaEsq > LIMITE_DETECCAO && !estavaDetectadoEsq)
  {
    contadorPulsosEsq++;
    tempoUltimoPulsoEsq = millis();
    estavaDetectadoEsq = true;
  }
  else if (diferencaEsq <= LIMITE_DETECCAO)
  {
    estavaDetectadoEsq = false;
  }

  ultimoValorAnalogicoEsq = valorAnalogicoEsq;

  delay(2);
}

void processarComandosSerial()
{
  while (Serial.available() > 0)
  {
    char c = Serial.read();

    if (c == '\n' || c == '\r')
    {
      if (indiceBuffer > 0)
      {
        bufferSerial[indiceBuffer] = '\0';
        processarComando(bufferSerial);
        indiceBuffer = 0;
      }
    }
    else if (indiceBuffer < sizeof(bufferSerial) - 1)
    {
      bufferSerial[indiceBuffer++] = c;
    }
  }
}

void processarComando(char *cmd)
{
  // Converter para maiúsculas
  for (int i = 0; cmd[i] != '\0'; i++)
  {
    cmd[i] = toupper(cmd[i]);
  }

  // Comandos especiais do sistema
  if (strcmp(cmd, "C") == 0)
  {
    calibrarEncoders();
    return;
  }

  if (strcmp(cmd, "X") == 0)
  {
    reiniciarFila();
    return;
  }

  if (strcmp(cmd, "D") == 0)
  {
    alterarConfiguracoes();
    return;
  }

  if (strcmp(cmd, "E") == 0)
  {
    executarSequencia();
    return;
  }

  // Verifica se é um comando válido (F, B, R, L)
  if (comandoValido(cmd))
  {
    if (executandoFila)
    {
      Serial.println(F(">>> Sistema ocupado. Comando na fila."));
    }

    strcpy(filaComandos, cmd);
    indiceComandoAtual = 0;
    executandoFila = true;

    Serial.print(F(">>> SEQUÊNCIA: "));
    Serial.println(cmd);
    Serial.print(F(">>> MOVIMENTOS: "));
    Serial.println(strlen(cmd));
    Serial.println(F("--- INICIANDO ---"));

    executarProximoComandoFila();
  }
  else
  {
    Serial.println(F(">>> Comando inválido! Use F, B, R, L"));
    Serial.println(F(">>> Ex: F, B, R, L, FB, RL"));
  }
}

bool comandoValido(char *cmd)
{
  for (int i = 0; cmd[i] != '\0'; i++)
  {
    if (cmd[i] != 'F' && cmd[i] != 'B' && cmd[i] != 'R' && cmd[i] != 'L')
    {
      return false;
    }
  }
  return strlen(cmd) > 0;
}

void executarProximoComandoFila()
{
  if (indiceComandoAtual >= strlen(filaComandos))
  {
    Serial.println(F("==================================="));
    Serial.println(F(">>> SEQUÊNCIA CONCLUÍDA!"));
    Serial.println(F("==================================="));
    executandoFila = false;
    filaComandos[0] = '\0';
    return;
  }

  // Pega o próximo comando da sequência
  char proximoComando = filaComandos[indiceComandoAtual];
  direcaoFila = proximoComando;
  indiceComandoAtual++;

  Serial.print(F(">>> EXECUTANDO: "));
  Serial.print(indiceComandoAtual);
  Serial.print(F("/"));
  Serial.print(strlen(filaComandos));
  Serial.print(F(" - "));
  Serial.print(proximoComando);

  // Determina as direções e pulsos alvo para CADA RODA
  int direcaoDir = 0;
  int direcaoEsq = 0;
  int pulsosAlvoDirComando = 0;
  int pulsosAlvoEsqComando = 0;

  switch (proximoComando)
  {
  case 'F': // Frente: ambos motores horário
    direcaoDir = 1;
    direcaoEsq = 1;
    pulsosAlvoDirComando = round(VOLTAS_POR_F * PULSOS_POR_VOLTA_DIR);
    pulsosAlvoEsqComando = round(VOLTAS_POR_F * PULSOS_POR_VOLTA_ESQ);
    Serial.print(F(" (FRENTE - "));
    Serial.print(VOLTAS_POR_F);
    Serial.print(F(" volta(s))"));
    break;
  case 'B': // Trás: ambos motores anti-horário
    direcaoDir = -1;
    direcaoEsq = -1;
    pulsosAlvoDirComando = round(VOLTAS_POR_B * PULSOS_POR_VOLTA_DIR);
    pulsosAlvoEsqComando = round(VOLTAS_POR_B * PULSOS_POR_VOLTA_ESQ);
    Serial.print(F(" (TRÁS - "));
    Serial.print(VOLTAS_POR_B);
    Serial.print(F(" volta(s))"));
    break;
  case 'R': // Direita: roda esq horário, roda dir anti-horário
    direcaoDir = -1;
    direcaoEsq = 1;
    pulsosAlvoDirComando = round(VOLTAS_POR_R * PULSOS_90GRAUS_DIR);
    pulsosAlvoEsqComando = round(VOLTAS_POR_R * PULSOS_90GRAUS_ESQ);
    Serial.print(F(" (DIREITA - "));
    Serial.print(VOLTAS_POR_R);
    Serial.print(F(" volta(s) 90°))"));
    break;
  case 'L': // Esquerda: roda dir horário, roda esq anti-horário
    direcaoDir = 1;
    direcaoEsq = -1;
    pulsosAlvoDirComando = round(VOLTAS_POR_L * PULSOS_90GRAUS_DIR);
    pulsosAlvoEsqComando = round(VOLTAS_POR_L * PULSOS_90GRAUS_ESQ);
    Serial.print(F(" (ESQUERDA - "));
    Serial.print(VOLTAS_POR_L);
    Serial.print(F(" volta(s) 90°))"));
    break;
  }

  Serial.println();
  iniciarMovimento(pulsosAlvoDirComando, pulsosAlvoEsqComando, direcaoDir, direcaoEsq);
}

void iniciarMovimento(int pulsosAlvoDirVal, int pulsosAlvoEsqVal, int direcaoDir, int direcaoEsq)
{
  if (estaExecutando)
  {
    pararMotores();
    delay(200);
  }

  // pulsosAlvoDir = pulsosAlvoDirVal;
  // pulsosAlvoEsq = pulsosAlvoEsqVal;
  //===========================================================
  // Sugetao para corrigor diferenca entre as rodas
  pulsosAlvoDir = pulsosAlvoDirVal * CORRECAO_VELOCIDADE_DIR;
  pulsosAlvoEsq = pulsosAlvoEsqVal * CORRECAO_VELOCIDADE_ESQ;
  //========================================================
  contadorPulsosDir = 0;
  contadorPulsosEsq = 0;
  direcaoAtualDir = direcaoDir;
  direcaoAtualEsq = direcaoEsq;
  estaExecutando = true;

  // Configura motor direito
  digitalWrite(pinoMotorDirA, direcaoDir == 1 ? HIGH : LOW);
  digitalWrite(pinoMotorDirB, direcaoDir == 1 ? LOW : HIGH);
  digitalWrite(pinoMotorDirEnable, HIGH);

  // Configura motor esquerdo
  digitalWrite(pinoMotorEsqA, direcaoEsq == 1 ? HIGH : LOW);
  digitalWrite(pinoMotorEsqB, direcaoEsq == 1 ? LOW : HIGH);
  digitalWrite(pinoMotorEsqEnable, HIGH);

  Serial.print(F("Meta DIR: "));
  Serial.print(pulsosAlvoDir);
  Serial.print(F(" pulsos | ESQ: "));
  Serial.print(pulsosAlvoEsq);
  Serial.println(F(" pulsos"));
}

void controlarMotores()
{
  bool dirCompleto = (contadorPulsosDir >= pulsosAlvoDir);
  bool esqCompleto = (contadorPulsosEsq >= pulsosAlvoEsq);

  // Para cada motor individualmente quando atingir seu alvo
  if (dirCompleto)
  {
    digitalWrite(pinoMotorDirEnable, LOW);
  }
  if (esqCompleto)
  {
    digitalWrite(pinoMotorEsqEnable, LOW);
  }

  // Ambos motores completaram o movimento
  if (dirCompleto && esqCompleto)
  {
    pararMotores();
    Serial.print(F(">>> Movimento "));
    Serial.print(direcaoFila);
    Serial.println(F(" concluído!"));
    delay(300);
  }

  // Timeout de segurança
  if ((millis() - tempoUltimoPulsoDir > 3000 && contadorPulsosDir > 0) ||
      (millis() - tempoUltimoPulsoEsq > 3000 && contadorPulsosEsq > 0))
  {
    pararMotores();
    Serial.println(F(">>> ALERTA: Timeout"));
    executandoFila = false;
  }
}

void mostrarProgresso()
{
  if (pulsosAlvoDir > 0 && pulsosAlvoEsq > 0)
  {
    int progressoDir = (contadorPulsosDir * 100) / pulsosAlvoDir;
    int progressoEsq = (contadorPulsosEsq * 100) / pulsosAlvoEsq;

    Serial.print(F("Dir: "));
    Serial.print(contadorPulsosDir);
    Serial.print(F("/"));
    Serial.print(pulsosAlvoDir);
    Serial.print(F(" ("));
    Serial.print(progressoDir);
    Serial.print(F("%)"));

    Serial.print(F(" | Esq: "));
    Serial.print(contadorPulsosEsq);
    Serial.print(F("/"));
    Serial.print(pulsosAlvoEsq);
    Serial.print(F(" ("));
    Serial.print(progressoEsq);
    Serial.println(F("%)"));
  }
}

void reiniciarFila()
{
  pararMotores();
  executandoFila = false;
  filaComandos[0] = '\0';
  botaoPressionado = false;
  Serial.println(F(">>> FILA RESETADA"));
}

void alterarConfiguracoes()
{
  Serial.println(F("=== CONFIGURAÇÕES ==="));
  Serial.println(F("Para ajustar, edite as constantes no código:"));
  Serial.println(F("- PULSOS_POR_VOLTA_DIR"));
  Serial.println(F("- PULSOS_POR_VOLTA_ESQ"));
  Serial.println(F("- VOLTAS_POR_F, VOLTAS_POR_B, etc."));
  Serial.println(F("Valores atuais:"));
  Serial.print(F("- Pulsos/volta DIR: "));
  Serial.println(PULSOS_POR_VOLTA_DIR);
  Serial.print(F("- Pulsos/volta ESQ: "));
  Serial.println(PULSOS_POR_VOLTA_ESQ);
}

void calibrarEncoders()
{
  Serial.println(F("=== CALIBRAÇÃO INDIVIDUAL ==="));
  Serial.println(F("Gire a RODA DIREITA UMA volta completa"));
  Serial.println(F("Pressione 'D' quando terminar"));

  contadorPulsosDir = 0;
  bool calibrandoDir = true;

  while (calibrandoDir)
  {
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      if (c == 'D' || c == 'd')
      {
        calibrandoDir = false;
      }
    }

    int valorAnalogicoDir = analogRead(pinoEncoderDir);
    int diferencaDir = abs(valorAnalogicoDir - ultimoValorAnalogicoDir);

    if (diferencaDir > 100 && !estavaDetectadoDir)
    {
      contadorPulsosDir++;
      estavaDetectadoDir = true;
    }
    else if (diferencaDir <= 100)
    {
      estavaDetectadoDir = false;
    }
    ultimoValorAnalogicoDir = valorAnalogicoDir;

    delay(10);
  }

  Serial.print(F("Pulsos RODA DIREITA: "));
  Serial.println(contadorPulsosDir);

  Serial.println(F("Agora gire a RODA ESQUERDA UMA volta completa"));
  Serial.println(F("Pressione 'E' quando terminar"));

  contadorPulsosEsq = 0;
  bool calibrandoEsq = true;

  while (calibrandoEsq)
  {
    if (Serial.available() > 0)
    {
      char c = Serial.read();
      if (c == 'E' || c == 'e')
      {
        calibrandoEsq = false;
      }
    }

    int valorAnalogicoEsq = analogRead(pinoEncoderEsq);
    int diferencaEsq = abs(valorAnalogicoEsq - ultimoValorAnalogicoEsq);

    if (diferencaEsq > 100 && !estavaDetectadoEsq)
    {
      contadorPulsosEsq++;
      estavaDetectadoEsq = true;
    }
    else if (diferencaEsq <= 100)
    {
      estavaDetectadoEsq = false;
    }
    ultimoValorAnalogicoEsq = valorAnalogicoEsq;

    delay(10);
  }

  Serial.print(F("Pulsos RODA ESQUERDA: "));
  Serial.println(contadorPulsosEsq);
  Serial.println(F("=== CALIBRAÇÃO CONCLUÍDA ==="));
  Serial.println(F("Atualize PULSOS_POR_VOLTA_DIR e PULSOS_POR_VOLTA_ESQ no código"));
}

void pararMotores()
{
  digitalWrite(pinoMotorDirEnable, LOW);
  digitalWrite(pinoMotorDirA, LOW);
  digitalWrite(pinoMotorDirB, LOW);

  digitalWrite(pinoMotorEsqEnable, LOW);
  digitalWrite(pinoMotorEsqA, LOW);
  digitalWrite(pinoMotorEsqB, LOW);

  estaExecutando = false;
}
// =================================================================
// >>> NOVA FUNÇÃO PARA O FEEDBACK VISUAL DOS BOTÕES <<<
// =================================================================
void piscarLedsFeedback()
{
  digitalWrite(pinoLedEsquerdo, HIGH); // Liga o LED esquerdo
  digitalWrite(pinoLedDireito, HIGH);  // Liga o LED direito
  delay(100);                          // Espera por 100 milissegundos (duração da piscada)
  digitalWrite(pinoLedEsquerdo, LOW);  // Desliga o LED esquerdo
  digitalWrite(pinoLedDireito, LOW);   // Desliga o LED direito
}
// =================================================================
// >>> NOVA FUNCAO PARA LER O ADC DE FORMA ESTAVEL <<<
// =================================================================
int lerADCEstavel(int pino)
{
  int total = 0;
  int numLeituras = 10; // Faz 10 leituras para tirar a média

  for (int i = 0; i < numLeituras; i++)
  {
    total += analogRead(pino);
    delay(1); // Pequena pausa entre leituras
  }

  return total / numLeituras; // Retorna a média
}