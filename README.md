# SmartWork – Estação IoT de Bem-Estar para o Futuro do Trabalho  
### Global Solution 2025 – FIAP  
### ESP32 • IoT • MQTT • Sensores • Automação • Futuro do Trabalho

- João Pedro Saborido | RM: 98184 
- Matheus Haruo | RM: 97663
- Pedro Guerra | RM: 99526	

---

## 📘 Visão Geral

O **SmartWork** é uma estação IoT inteligente desenvolvida para monitorar **conforto térmico**, **iluminação**, **tempo de foco**, **alertas de fadiga** e **bem-estar geral** em ambientes de estudo ou trabalho híbridos.  
A solução demonstra como tecnologias IoT podem transformar o **Futuro do Trabalho**, promovendo saúde, autonomia e produtividade.

Este projeto integra:

- ESP32  
- DHT22 (temperatura/umidade)  
- LDR (iluminação)  
- LCD 16x2 (I2C)  
- LEDs indicadores (verde, vermelho, amarelo e azul)  
- Buzzer  
- Botões de controle  
- Envio de dados via **MQTT**  
- Exibição de informações em tempo real  

---

## 🎯 Problema Abordado

No futuro do trabalho — especialmente em home office e ambientes híbridos — problemas como:

- Fadiga acumulada  
- Poucas pausas  
- Temperatura ou umidade inadequadas  
- Iluminação ruim  
- Queda de produtividade  

impactam a saúde física e mental de profissionais e estudantes.

Ambientes sem controle ambiental geram:

- Enxaqueca  
- Cansaço ocular  
- Baixa motivação  
- Má postura  
- Estresse  

---

## 💡 Solução Proposta: SmartWork IoT

O SmartWork monitora automaticamente o ambiente e o comportamento de foco usando:

### ✔ Sensores
- **DHT22:** mede temperatura e umidade  
- **LDR:** detecta nível de iluminação  

### ✔ Indicadores Visuais
- 🟢 **LED Verde:** ambiente confortável  
- 🟡 **LED Amarelo:** atenção (tempo de foco alto ou ambiente irregular)  
- 🔴 **LED Vermelho:** crítico (alerta)  
- 🔵 **LED Azul:** iluminação baixa  

### ✔ Notificações Sonoras
- **Buzzer** alerta estados críticos ou comandos

### ✔ Interface
- **LCD 16x2** mostra temperatura, umidade, tempo de foco e estado

### ✔ Controles
- **Botão Verde:** iniciar/zerar tempo de foco  
- **Botão Azul:** registrar pausa  

### ✔ IoT / MQTT
- Envia dados ambientais e estado para um **broker MQTT**  
- Pode ser consumido por dashboards, sistemas ou automações

---

## 🧱 Hardware Utilizado

| Componente | Função |
|-----------|--------|
| ESP32 | Microcontrolador WiFi |
| DHT22 | Sensor de temperatura/umidade |
| LDR + módulo | Sensor de luminosidade |
| LCD 16x2 I2C | Interface de visualização |
| LED verde | Estado OK |
| LED vermelho | Estado crítico |
| LED amarelo (verde+vermelho) | Estado de atenção |
| LED azul | Iluminação baixa |
| Buzzer | Alerta sonoro |
| Botão verde | Início/zerar foco |
| Botão azul | Registrar pausa |

---

## 🖥 Como Executar no Wokwi

1. Abra o link público do projeto  
2. Clique em **Play**  
3. Aguarde conectar no WiFi  
4. Observe no LCD:
   - Temperatura  
   - Umidade  
   - Tempo de foco  
   - Estado  

5. Tampe o LDR → **LED azul acende**  
6. Aguarde o tempo de foco subir → **estado muda automaticamente**  
7. Aperte:
   - **Botão verde** → reinicia o foco  
   - **Botão azul** → registra pausa concluída  

8. Veja a telemetria MQTT no **console do Wokwi** (JSON publicado a cada 30s)

---

## 🚦 Lógica dos Estados

### 🟢 STATUS OK
- Conforto adequado  
- Iluminação normal  
- Tempo de foco baixo  

### 🟡 STATUS ATENÇÃO
- Foco prolongado  
- Ambiente parcialmente confortável  

### 🔴 STATUS CRÍTICO
- Foco excessivo  
- Ambiente ruim  
- Buzzer dispara  

### 🔵 LED Azul (extra)
- Acende quando **luz < 400**  
- Indica ambiente escuro  

---

## 🧮 Cálculo de Índice de Conforto

O sistema considera valores ideais de conforto:

- Temperatura: **21–24°C**  
- Umidade: **40–60%**  
- Iluminação adequada: **>800**  

Com base nesses parâmetros, o SmartWork gera um **índice entre 0 e 1** para determinar automaticamente o estado:

- **> 0.7** → OK  
- **0.4 a 0.7** → Atenção  
- **< 0.4** → Crítico  

---

## 🎥 Vídeo Demonstrativo

O vídeo deve mostrar:

- O problema abordado  
- Funcionamento completo do hardware  
- LEDs mudando entre OK / Atenção / Crítico  
- Buzzer ativando no estado crítico  
- LCD atualizando em tempo real  
- MQTT funcionando  

---

## 🛰 Aplicações Reais

- Ambientes híbridos de trabalho  
- Home office  
- Estações de estudo  
- Monitoramento de produtividade  
- Prevenção de fadiga  
- Bem-estar organizacional  

