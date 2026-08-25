import requests
import time
from sklearn.neighbors import KNeighborsClassifier

API_GET = "http://localhost:3000/api/medicoes"
API_POST = "http://localhost:3000/api/classificacao"

# Guarda a última temperatura processada
ultima_temperatura = None



X = [
    [5.0],
    [10.0],
    [12.0],

    [20.0],
    [22.0],
    [25.0],

    [32.0],
    [35.0],
    [38.0]
]

y = [
    "Ruim",
    "Ruim",
    "Ruim",

    "Bom",
    "Bom",
    "Bom",

    "Ruim",
    "Ruim",
    "Ruim"
]



modelo = KNeighborsClassifier(n_neighbors=3)

modelo.fit(X, y)

print("Aguardando novas temperaturas...\n")




while True:

    try:

        

        resposta = requests.get(API_GET)
        resposta.raise_for_status()

        dados = resposta.json()

        temperatura = float(dados["temperatura"])

      

        if temperatura != ultima_temperatura:

            print(
                f"Nova temperatura recebida: "
                f"{temperatura:.2f} °C"
            )

            # Guarda a temperatura atual
            ultima_temperatura = temperatura

        

            classificacao = modelo.predict(
                [[temperatura]]
            )[0]

            print(
                f"Classificação da IA: {classificacao}"
            )

           

            dados_classificacao = {
                "temperatura": temperatura,
                "classificacao": classificacao
            }

            resposta = requests.post(
                API_POST,
                json=dados_classificacao
            )

            resposta.raise_for_status()

            print(
                "Classificação enviada para a API!"
            )

            print(
                f"Resposta: {resposta.json()}\n"
            )

    except requests.exceptions.RequestException as erro:

        print(f"Erro ao acessar a API: {erro}")

    except (KeyError, ValueError):

        print(
            "Erro: a API não retornou "
            "uma temperatura válida."
        )

   
    time.sleep(1)