using System;
using System.IO.Ports;
using System.Net.Http;
using System.Text;
using System.Text.Json;
using System.Threading;
using System.Threading.Tasks;

class Program
{
    private const string ApiUrl = "http://localhost:3000/api/temperatura";
    private const int BaudRate = 115200;

    // Protocolo proprietário
    private const byte PROTO_START = 0xAA;
    private const byte PROTO_END = 0x55;
    private const byte PROTO_CMD_ADC = 0x01;
    private const byte PROTO_LEN_ADC = 0x02;

    private static readonly HttpClient HttpClient = new HttpClient();

    private static SerialPort? _porta;
    private static volatile bool _executando = true;

    static async Task Main()
    {
        Console.WriteLine("=================================");
        Console.WriteLine(" STM32 -> PROTOCOLO -> C# -> API");
        Console.WriteLine("=================================\n");

        string? nomePorta = ProcurarPortaCom();

        if (nomePorta == null)
        {
            Console.WriteLine("Nenhuma porta COM encontrada.");
            return;
        }

        Console.WriteLine($"Usando porta: {nomePorta}\n");

        _porta = new SerialPort(nomePorta)
        {
            BaudRate = BaudRate,
            Parity = Parity.None,
            DataBits = 8,
            StopBits = StopBits.One,
            ReadTimeout = 2000,
            WriteTimeout = 2000
        };

        try
        {
            _porta.Open();
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Erro ao abrir a porta: {ex.Message}");
            return;
        }

        Console.WriteLine("Porta conectada.");
        Console.WriteLine("Aguardando pacotes do STM32...\n");

        _ = Task.Run(EscutarSaida);

        while (_executando)
        {
            try
            {
                byte[] pacote = LerPacote();

                if (pacote.Length == 7)
                {
                    await ProcessarPacote(pacote);
                }
            }
            catch (TimeoutException)
            {
                continue;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Erro na comunicação: {ex.Message}");
            }
        }

        if (_porta.IsOpen)
            _porta.Close();

        Console.WriteLine("\nPrograma encerrado.");
    }

    // ============================================================
    // PROCURA A PORTA COM
    // ============================================================

    private static string? ProcurarPortaCom()
    {
        string[] portas = SerialPort.GetPortNames();

        if (portas.Length == 0)
            return null;

        if (portas.Length == 1)
            return portas[0];

        Console.WriteLine("Portas COM disponíveis:");

        for (int i = 0; i < portas.Length; i++)
        {
            Console.WriteLine($"[{i}] {portas[i]}");
        }

        while (true)
        {
            Console.Write("\nEscolha a porta: ");

            string? escolha = Console.ReadLine();

            if (int.TryParse(escolha, out int indice) &&
                indice >= 0 &&
                indice < portas.Length)
            {
                return portas[indice];
            }

            Console.WriteLine("Opção inválida.");
        }
    }

    // ============================================================
    // LÊ O PACOTE DO PROTOCOLO
    // ============================================================

    private static byte[] LerPacote()
    {
        if (_porta == null)
            throw new Exception("Porta COM não inicializada.");

        // Procura o byte inicial 0xAA
        while (true)
        {
            int primeiroByte = _porta.ReadByte();

            if (primeiroByte == PROTO_START)
                break;
        }

        // Depois do START, esperamos mais 6 bytes
        byte[] pacote = new byte[7];

        pacote[0] = PROTO_START;

        for (int i = 1; i < 7; i++)
        {
            pacote[i] = (byte)_porta.ReadByte();
        }

        return pacote;
    }

    // ============================================================
    // PROCESSA O PACOTE
    // ============================================================

    private static async Task ProcessarPacote(byte[] pacote)
    {
        // --------------------------------------------------------
        // Verifica START
        // --------------------------------------------------------

        if (pacote[0] != PROTO_START)
        {
            Console.WriteLine("Erro: START inválido.");
            return;
        }

        // --------------------------------------------------------
        // Verifica CMD
        // --------------------------------------------------------

        if (pacote[1] != PROTO_CMD_ADC)
        {
            Console.WriteLine($"Erro: comando inválido: 0x{pacote[1]:X2}");
            return;
        }

        // --------------------------------------------------------
        // Verifica LEN
        // --------------------------------------------------------

        if (pacote[2] != PROTO_LEN_ADC)
        {
            Console.WriteLine($"Erro: tamanho inválido: {pacote[2]}");
            return;
        }

        // --------------------------------------------------------
        // Verifica END
        // --------------------------------------------------------

        if (pacote[6] != PROTO_END)
        {
            Console.WriteLine("Erro: END inválido.");
            return;
        }

        // --------------------------------------------------------
        // Calcula checksum
        // --------------------------------------------------------

        byte checksumCalculado =
            (byte)(
                pacote[1] ^
                pacote[2] ^
                pacote[3] ^
                pacote[4]
            );

        // --------------------------------------------------------
        // Compara checksum recebido
        // --------------------------------------------------------

        if (pacote[5] != checksumCalculado)
        {
            Console.WriteLine(
                $"Erro: checksum inválido. " +
                $"Recebido: 0x{pacote[5]:X2} | " +
                $"Calculado: 0x{checksumCalculado:X2}"
            );

            return;
        }

        // --------------------------------------------------------
        // Reconstrói o ADC
        // --------------------------------------------------------

        ushort valorADC =
            (ushort)(
                (pacote[3] << 8) |
                pacote[4]
            );

        // --------------------------------------------------------
        // Verifica limite do ADC
        // --------------------------------------------------------

        if (valorADC > 4095)
        {
            Console.WriteLine($"ADC inválido: {valorADC}");
            return;
        }

        // --------------------------------------------------------
        // Converte ADC para temperatura
        // --------------------------------------------------------

        double temperatura =
            valorADC * 50.0 / 4095.0;

        // --------------------------------------------------------
        // Mostra no terminal
        // --------------------------------------------------------

        Console.WriteLine("---------------------------------");

        Console.WriteLine(
            $"Pacote recebido: " +
            $"{pacote[0]:X2} " +
            $"{pacote[1]:X2} " +
            $"{pacote[2]:X2} " +
            $"{pacote[3]:X2} " +
            $"{pacote[4]:X2} " +
            $"{pacote[5]:X2} " +
            $"{pacote[6]:X2}"
        );

        Console.WriteLine($"ADC: {valorADC}");

        Console.WriteLine(
            $"Temperatura: {temperatura:F2} °C"
        );

        // --------------------------------------------------------
        // Envia para API
        // --------------------------------------------------------

        await EnviarParaAPI(temperatura);
    }

    // ============================================================
    // ENVIA TEMPERATURA PARA NODE.JS
    // ============================================================

    private static async Task EnviarParaAPI(double temperatura)
    {
        var dados = new
        {
            temperatura = temperatura
        };

        string json =
            JsonSerializer.Serialize(dados);

        Console.WriteLine($"JSON: {json}");

        try
        {
            using StringContent content =
                new StringContent(
                    json,
                    Encoding.UTF8,
                    "application/json"
                );

            HttpResponseMessage resposta =
                await HttpClient.PostAsync(
                    ApiUrl,
                    content
                );

            Console.WriteLine(
                $"Status da API: " +
                $"{(int)resposta.StatusCode} " +
                $"{resposta.StatusCode}"
            );

            string respostaApi =
                await resposta.Content.ReadAsStringAsync();

            Console.WriteLine(
                $"Resposta da API: {respostaApi}"
            );

            Console.WriteLine();
        }
        catch (HttpRequestException ex)
        {
            Console.WriteLine(
                $"Erro ao conectar com a API: {ex.Message}"
            );
        }
    }

    // ============================================================
    // COMANDO SAIR
    // ============================================================

    private static void EscutarSaida()
    {
        while (_executando)
        {
            string? entrada = Console.ReadLine();

            if (entrada != null &&
                entrada.Equals(
                    "sair",
                    StringComparison.OrdinalIgnoreCase))
            {
                _executando = false;
            }
        }
    }
}