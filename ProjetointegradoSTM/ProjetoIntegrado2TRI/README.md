# ProjetoIntegrado_2TRI
Sistema IoT com STM32, C#, API REST, IA e interface Web para monitoramento de sensor analógico, classificação automática das leituras e visualização em tempo real.

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

## 🛠️ Tecnologias Utilizadas

- STM32
- Linguagem C
- C#
- Node.js
- JavaScript
- HTML
- CSS
- Machine Learning


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
| **Projeto** | Projeto Integrado |
| **Professor(es)** | Ana Leticia G. Gonçalves, Daniel Albino Mosca e José Andery Carneiro |
| **Ano** | 2026 |
