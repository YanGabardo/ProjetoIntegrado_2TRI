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

O sistema é composto por três partes principais:

- **STM32:** realiza a leitura do sensor/trimpot e envia os dados utilizando o protocolo proprietário através da USB.
- **C#:** recebe os dados enviados pelo STM32, interpreta o protocolo, converte o valor do ADC para temperatura e envia a temperatura para a API.
- **IA (Python):** realiza a classificação da temperatura utilizando o modelo K-NN.
- **Server (Node.js):** recebe as temperaturas enviadas pelo C#, executa a IA e disponibiliza os dados para o sistema/painel.

### ▶️ Ordem de inicialização

Para que o sistema funcione corretamente, os três programas precisam ser iniciados **manualmente**.

#### 1. Iniciar o Server

Entre na pasta do projeto da API e execute:

```bash
node server.js

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
