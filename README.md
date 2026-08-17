# 🌐 Projeto Integrado - Sistema de Monitoramento IoT

![STM32](https://img.shields.io/badge/STM32-Microcontroller-blue)
![C#](https://img.shields.io/badge/C%23-Application-purple)
![Node.js](https://img.shields.io/badge/Node.js-API-green?logo=node.js)
![JavaScript](https://img.shields.io/badge/JavaScript-Web-yellow?logo=javascript)
![Git](https://img.shields.io/badge/Git-Version%20Control-red?logo=git)
![GitHub](https://img.shields.io/badge/GitHub-Repository-black?logo=github)

Repositório do Projeto Integrado desenvolvido para simular um sistema IoT de aquisição, processamento e visualização de dados utilizando STM32, C#, API REST, Inteligência Artificial e interface Web.

## 📋 Descrição do Projeto

O sistema realiza a leitura de um sensor analógico através de um microcontrolador STM32, envia os dados para uma aplicação em C#, que encaminha as informações para uma API REST responsável por processar as medições utilizando um modelo de Inteligência Artificial. Os resultados são exibidos em uma interface Web em tempo real.

---

## ⚙️ Módulos do Sistema

### 🔹 Aquisição de Dados (STM32)
**Funcionalidades:**
- Leitura de sensor analógico
- Comunicação via USB CDC (Porta COM)
- Envio periódico das medições
- Pré-processamento opcional por GPIO

---

### 🔹 Aplicação Desktop (C#)
**Funcionalidades:**
- Comunicação com a Porta COM
- Recebimento contínuo das leituras
- Conversão dos dados para JSON
- Envio das informações para a API

---

### 🔹 API REST
**Funcionalidades:**
- Recebimento das medições
- Processamento das informações
- Comunicação com o modelo de IA
- Retorno da classificação
- Disponibilização dos dados para o frontend

---

### 🔹 Inteligência Artificial
**Funcionalidades:**
- Classificação automática das leituras
- Modelo de Machine Learning treinado
- Classificação em múltiplas categorias

---

### 🔹 Interface Web
**Funcionalidades:**
- Exibição da leitura atual
- Classificação em tempo real
- Histórico das últimas medições
- Atualização automática da página
- Indicação visual do estado do sistema

---
## ⚙️ Módulo de Funcionamento

O funcionamento completo do sistema ocorre através do fluxo de dados integrado entre os módulos:

1. **STM32:**
   - Realiza a leitura do trimpot/sensor analógico utilizando ADC.
   - Monta o pacote de dados seguindo o protocolo proprietário desenvolvido no projeto.
   - Envia os dados encapsulados para o computador através da interface USB CDC (Porta COM virtual).

2. **C#:**
   - Conecta-se à porta COM e recebe os pacotes de bytes do STM32.
   - Interpreta o protocolo proprietário e realiza a validação completa (START, CMD, LEN, CHECKSUM e END).
   - Reconstrói o valor numérico do ADC a partir dos bytes recebidos.
   - Converte a leitura do ADC no valor correspondente de temperatura.
   - Formata a medição em JSON e a envia via requisição HTTP POST para a API REST Node.js.

3. **Server Node.js:**
   - Deve ser iniciado manualmente antes da transmissão dos dados.
   - Expõe a API e recebe as medições de temperatura através do endpoint `/api/temperatura`.
   - Dispara e executa automaticamente o script Python (`classificador.py`) repassando a temperatura recebida.
   - Armazena em memória a leitura atual, a classificação retornada e o histórico de medições.
   - Disponibiliza os endpoints REST para consulta de dados pela interface Web.

4. **IA Python:**
   - Precisa ser iniciada a primeira vez manualmente pelo usuário.
   - Depois disso é chamada e executada automaticamente em segundo plano pelo Server Node.js a cada nova leitura.
   - Utiliza um modelo K-NN (K-Nearest Neighbors) previamente treinado para classificar a faixa de temperatura.
   - Retorna o resultado da classificação em tempo de execução para o servidor Node.js.

5. **Interface Web:**
   - Consome os dados fornecidos pela API do Server Node.js.
   - Exibe em tempo real a temperatura atual medida.
   - Exibe a classificação retornada pela IA (ex: Baixa, Normal, Alta).
   - Apresenta o histórico com as últimas medições realizadas.

### ▶️ Inicialização do Sistema

Para colocar o sistema em pleno funcionamento, o usuário precisa iniciar **MANUALMENTE** apenas os seguintes serviços:

1. **Server Node.js:** Iniciar o servidor da API REST e backend web.
2. **Aplicação C#:** Executar o programa desktop para abrir a porta COM e iniciar a leitura do STM32.
3. **IA:**Executar a IA uma única vez para inicia-la.

---

## 🛠️ Tecnologias Utilizadas

- STM32
- Linguagem C
- C#
- Node.js
- JavaScript
- HTML
- CSS
- Machine Learning
- Git
- GitHub

---

## 📂 Estrutura do Projeto

```
📦 Projeto
 ├── STM32
 ├── Aplicação C#
 ├── API REST
 ├── Inteligência Artificial
 ├── Frontend Web
 └── README.md
```

---

## 👨‍🏫 Informações

| Campo | Informação |
|-------|------------|
| **Professor(es)** | Ana Leticia G. Gonçalves, Daniel Albino Mosca e José Andery Carneiro |
| **Curso** | Desenvolvimento de Sistemas |
| **Ano** | 2026 |
