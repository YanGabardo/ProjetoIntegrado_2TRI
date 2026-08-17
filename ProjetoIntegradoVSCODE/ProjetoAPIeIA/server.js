const express = require("express");
const path = require("path");
const { spawn } = require("child_process");

const app = express();

app.use(express.json());

// ============================================================
// CONFIGURAÇÕES
// ============================================================

const PORTA = 3000;

// Guarda a última temperatura recebida
let temperaturaAtual = 0;

// Guarda a última classificação
let classificacaoAtual = "Aguardando...";

// Histórico das medições
let historico = [];

// ============================================================
// PAINEL HTML
// ============================================================

app.get("/painel", (req, res) => {
    res.sendFile(path.join(__dirname, "index.html"));
});

// ============================================================
// POST - C# ENVIA A TEMPERATURA
// ============================================================

app.post("/api/temperatura", (req, res) => {

    const temperatura = Number(req.body.temperatura);

    if (isNaN(temperatura)) {
        return res.status(400).json({
            erro: "Temperatura inválida"
        });
    }

    temperaturaAtual = temperatura;

    console.log(
        `Temperatura recebida do C#: ${temperatura} °C`
    );

    executarIA(temperatura)
        .then((classificacao) => {

            classificacaoAtual = classificacao;

            historico.unshift({
                temperatura: temperatura,
                classificacao: classificacao,
                horario: new Date().toLocaleTimeString("pt-BR")
            });

            if (historico.length > 20) {
                historico.pop();
            }

            console.log(
                `IA: ${temperatura} °C -> ${classificacao}`
            );

        })
        .catch((erro) => {
            console.log("Erro ao executar IA:", erro.message);
        });

    res.status(200).json({
        mensagem: "Temperatura recebida",
        temperatura: temperatura
    });
});

// ============================================================
// POST - PYTHON ENVIA A CLASSIFICAÇÃO
// ============================================================

app.post("/api/classificacao", (req, res) => {

    const temperatura = Number(req.body.temperatura);
    const classificacao = req.body.classificacao;

    if (isNaN(temperatura) || !classificacao) {
        return res.status(400).json({
            erro: "Dados inválidos"
        });
    }

    temperaturaAtual = temperatura;
    classificacaoAtual = classificacao;

    // Adiciona ao histórico
    historico.unshift({
        temperatura: temperatura,
        classificacao: classificacao,
        horario: new Date().toLocaleTimeString("pt-BR")
    });

    // Mantém somente as últimas 20 medições
    if (historico.length > 20) {
        historico.pop();
    }

    console.log(
        `Classificação recebida: ${temperatura} °C -> ${classificacao}`
    );

    res.status(200).json({
        mensagem: "Classificação recebida",
        temperatura: temperatura,
        classificacao: classificacao
    });
});

// ============================================================
// GET - HTML CONSULTA OS DADOS
// ============================================================

app.get("/api/medicoes", (req, res) => {

    res.json({
        temperatura: temperaturaAtual,
        classificacao: classificacaoAtual,
        historico: historico
    });

});

// ============================================================
// EXECUTA A IA AUTOMATICAMENTE
// ============================================================

function executarIA(temperatura) {

    return new Promise((resolve, reject) => {

        const python = spawn("python", [
            "classificador.py",
            temperatura.toString()
        ]);

        let resultado = "";
        let erro = "";

        python.stdout.on("data", (data) => {
            resultado += data.toString();
        });

        python.stderr.on("data", (data) => {
            erro += data.toString();
        });

        python.on("close", (codigo) => {

            if (codigo !== 0) {
                reject(new Error(erro));
                return;
            }

            resolve(resultado.trim());
        });
    });
}

// ============================================================
// INICIA A API
// ============================================================

app.listen(PORTA, () => {

    console.log("======================================");
    console.log("       API DE MONITORAMENTO");
    console.log("======================================");
    console.log(`API rodando em http://localhost:${PORTA}`);
    console.log(`Endpoint: http://localhost:${PORTA}/api/medicoes`);
    console.log(`Painel: http://localhost:${PORTA}/painel`);
    console.log("======================================");

});