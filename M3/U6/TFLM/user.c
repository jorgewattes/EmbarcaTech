/*
Explicação do Código
Inicialização do ADC:

Configuramos o GP28 como entrada analógica para capturar os sinais do microfone.
Convertendo o sinal analógico para o formato correto, considerando o nível médio do microfone (1,65V).
Buffer de Áudio:

Capturamos 1 segundo de áudio em um buffer de 16.000 amostras (taxa de 16 kHz).
Pré-Processamento (Espectrograma):

O áudio capturado é transformado em um espectrograma, que é o formato de entrada do modelo. Neste exemplo, simplificamos a lógica. Na prática, você pode usar uma biblioteca de FFT ou implementar seu próprio algoritmo.
Inferência com TensorFlow Lite:

O espectrograma é alimentado no modelo TensorFlow Lite.
O resultado do modelo é interpretado para determinar se a palavra "Alexa" foi detectada.
Detecção Contínua:

O código roda continuamente, detectando a palavra "Alexa" em loops de 1 segundo.
Compilação e Execução
Instale o Pico SDK e configure o ambiente de desenvolvimento.

Salve o modelo alexa_model.tflite como um array binário com o comando:

bash
Copiar
Editar
xxd -i alexa_model.tflite > alexa_model_data.h
Compile o código e faça o upload para o Raspberry Pi Pico:

bash
Copiar
Editar
cmake ..
make
sudo picotool load nome_do_binario.uf2
Conecte o microfone de eletreto ao GP28 e monitore os resultados via terminal serial.

Próximos Passos
Melhorar o Pré-Processamento: Substituir a função fictícia compute_spectrogram por uma implementação real de FFT para calcular o espectrograma.
Ajustar o Modelo: Certifique-se de que o modelo está otimizado para detecção em tempo real.
Aprimorar a Inferência: Ajuste os limiares de decisão para aumentar a precisão da detecção.
Com essas etapas, você terá um sistema TinyML rodando no Raspberry Pi Pico W, capaz de detectar a palavra-chave "Alexa" de forma eficiente! 😊
*/

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// Importar o modelo treinado como um array
#include "alexa_model_data.h"

// Configurações de áudio
#define AUDIO_BUFFER_SIZE 16000  // Tamanho do buffer de áudio (1 segundo a 16 kHz)
#define SAMPLE_RATE 16000        // Taxa de amostragem
#define ADC_PIN 28               // GP28 (ADC2)
#define ADC_VREF 3.3             // Tensão de referência (3,3V)
#define ADC_LEVEL_OFFSET 1.65    // Offset do nível médio do microfone (1,65V)

// Configurações do TensorFlow Lite
constexpr int kTensorArenaSize = 10 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// Buffers
int16_t audio_buffer[AUDIO_BUFFER_SIZE]; // Buffer de áudio
float input_tensor_data[40 * 40];        // Dados de entrada pré-processados (espectrograma)

// Funções auxiliares
void record_audio();
void compute_spectrogram(const int16_t *audio, float *output);

// Função principal do TensorFlow Lite
void detect_keyword();

int main() {
    stdio_init_all();
    printf("Inicializando sistema de detecção de palavra-chave 'Alexa'...\n");

    // Configurar ADC
    adc_init();
    adc_gpio_init(ADC_PIN);  // Configura o pino GP28 como entrada analógica
    adc_select_input(2);     // Seleciona ADC2 (GP28)

    // Configuração inicial do TensorFlow Lite
    tflite::MicroErrorReporter error_reporter;
    tflite::AllOpsResolver resolver;
    const tflite::Model* model = tflite::GetModel(alexa_model_data);

    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("Modelo incompatível com TensorFlow Lite!\n");
        while (1);
    }

    tflite::MicroInterpreter interpreter(
        model, resolver, tensor_arena, kTensorArenaSize, &error_reporter);
    interpreter.AllocateTensors();

    // Loop principal
    printf("Sistema pronto. Detectando a palavra 'Alexa'...\n");
    while (1) {
        record_audio();  // Grava áudio no buffer
        compute_spectrogram(audio_buffer, input_tensor_data);  // Processa o espectrograma

        // Alimenta o modelo com o espectrograma
        TfLiteTensor* input = interpreter.input(0);
        memcpy(input->data.f, input_tensor_data, sizeof(input_tensor_data));

        // Executa a inferência
        interpreter.Invoke();

        // Lê os resultados
        TfLiteTensor* output = interpreter.output(0);
        int alexa_detected = output->data.f[0] > 0.5;  // Resultado binário (1 para "Alexa", 0 para ruído)

        // Mostra o resultado
        if (alexa_detected) {
            printf("Palavra 'Alexa' detectada!\n");
        } else {
            printf("Ruído detectado...\n");
        }

        sleep_ms(500);  // Espera 500 ms antes da próxima inferência
    }
}

// Função para gravar áudio
void record_audio() {
    for (int i = 0; i < AUDIO_BUFFER_SIZE; i++) {
        // Lê o valor do ADC e converte para o formato correto (-1 a 1 em 16 bits)
        uint16_t raw_adc = adc_read();
        float voltage = raw_adc * (ADC_VREF / 4095.0);  // Converte para tensão (0V a 3.3V)
        audio_buffer[i] = (int16_t)((voltage - ADC_LEVEL_OFFSET) * 32768 / ADC_LEVEL_OFFSET);
        sleep_us(1000000 / SAMPLE_RATE);  // Espera para manter a taxa de amostragem
    }
}

// Função para calcular o espectrograma
void compute_spectrogram(const int16_t *audio, float *output) {
    // Esta função deve implementar um algoritmo para converter o áudio em um espectrograma.
    // Como exemplo, usaremos valores fictícios. Na prática, use uma biblioteca FFT.
    for (int i = 0; i < 40 * 40; i++) {
        output[i] = (float)audio[i % AUDIO_BUFFER_SIZE] / 32768.0f;  // Normaliza o áudio
    }
}

  git config --global user.name "Your Name"
