# Sistema IoT de baixo custo para monitoramento contínuo de CO₂, temperatura e umidade relativa em ambientes internos, utilizando Arduino Uno, sensor SCD40 e ESP8266.
Monitoramento contínuo de CO₂ com sistema IoT de baixo custo

Repositório de códigos-fonte de um sistema IoT de baixo custo desenvolvido para o monitoramento contínuo da concentração de dióxido de carbono (CO₂), temperatura e umidade relativa em ambientes internos.

O projeto foi desenvolvido no contexto de um Trabalho de Conclusão de Curso (TCC) da Licenciatura em Química da Universidade Federal de Alagoas (UFAL), Campus Arapiraca.

## 1. Apresentação

O sistema utiliza um sensor Sensirion SCD40 para realizar medições ambientais, um Arduino Uno para aquisição e processamento dos dados e um módulo ESP-01 (ESP8266) para comunicação Wi-Fi e transmissão das informações à plataforma Adafruit IO.

A proposta busca oferecer uma alternativa acessível para o acompanhamento de parâmetros ambientais em espaços internos, especialmente em contextos educacionais.

Os códigos disponibilizados foram adaptados para compartilhamento público, permitindo que outros usuários configurem suas próprias redes Wi-Fi e credenciais de acesso.

## 2. Componentes utilizados

| Componente | Função |
|---|---|
| Arduino Uno | Controle do sistema e aquisição dos dados |
| Sensirion SCD40 | Medição de CO₂, temperatura e umidade relativa |
| ESP-01 (ESP8266) | Comunicação Wi-Fi |
| Conversor de nível lógico | Adequação dos níveis elétricos entre Arduino e ESP-01 |
| Display LCD 16 × 2 com interface I²C | Visualização local das medições e do estado da conexão |
| LEDs | Sinalização visual |
| Buzzer | Sinalização sonora |
| Fonte de alimentação | Fornecimento de energia ao sistema |

## 3. Funcionamento do sistema

O Arduino Uno realiza a leitura dos parâmetros ambientais fornecidos pelo sensor SCD40 e controla a apresentação das informações no display LCD.

A comunicação com a ESP-01 ocorre por interface serial, utilizando uma taxa de 9600 baud. As medições são transmitidas no seguinte formato:

`CO2,TEMPERATURA,UMIDADE`

Exemplo:

`650,27.35,65.20`

A ESP-01 é responsável pela conexão à rede Wi-Fi e pelo encaminhamento dos dados à plataforma Adafruit IO.

O código disponibilizado para o Arduino estabelece um intervalo de 15 segundos entre as verificações de leitura do sensor.

## 4. Bibliotecas utilizadas

### Arduino Uno

- `Wire.h`
- `SensirionI2cScd4x.h`
- `SoftwareSerial.h`
- `LiquidCrystal_I2C.h`

### ESP-01

- `ESP8266WiFi.h`
- `AdafruitIO_WiFi.h`

É necessário instalar as bibliotecas correspondentes e configurar o ambiente de desenvolvimento para as placas utilizadas.

## 5. Configuração da ESP-01

Antes de carregar o programa na ESP-01, o usuário deverá configurar suas credenciais do Adafruit IO e as redes Wi-Fi disponíveis.

No código, localize:

```cpp
#define IO_USERNAME "SEU_USUARIO_ADAFRUIT"
#define IO_KEY "SUA_CHAVE_ADAFRUIT_IO"
```

Substitua o nome de usuário e a chave pelos dados de sua própria conta no Adafruit IO.

Em seguida, configure os nomes e as senhas das redes Wi-Fi nos vetores `ssids[]` e `senhas[]`.

O código contém cinco posições para redes Wi-Fi, permitindo cadastrar diferentes pontos de acesso.

**Atenção:** não publique senhas pessoais ou chaves privadas em repositórios públicos.

## 6. Configuração do Adafruit IO

Crie uma conta na plataforma [Adafruit IO](https://io.adafruit.com/) e configure os seguintes feeds:

| Feed | Variável |
|---|---|
| `co2` | Concentração de CO₂ |
| `temperatura` | Temperatura |
| `umidade` | Umidade relativa |

Os identificadores dos feeds devem corresponder aos utilizados no código da ESP-01.

## 7. Códigos-fonte

O repositório contém dois programas:

- `1.CodeArduino.ino`: leitura do sensor, controle do LCD, sinalização e comunicação serial.
- `2.CodeESP01.ino`: gerenciamento das redes Wi-Fi, recepção das medições e transmissão ao Adafruit IO.

Os arquivos são disponibilizados para fins de estudo, adaptação e desenvolvimento de aplicações semelhantes.

**Estado da versão disponibilizada:** os programas ainda requerem verificação integrada do protocolo de comunicação serial e do gerenciamento de múltiplas redes Wi-Fi antes de serem considerados uma versão validada para reprodução.

## 8. Considerações sobre a utilização

A reprodução do sistema exige atenção à alimentação elétrica dos componentes, à compatibilidade dos níveis lógicos, às conexões físicas e à configuração das bibliotecas.

As medições obtidas devem ser interpretadas considerando as condições de instalação, as características do sensor e as limitações do sistema.

O protótipo não deve ser considerado, sem validação específica, um instrumento certificado para avaliações regulamentares de qualidade do ar.

## 9. Contexto acadêmico

Este repositório está associado ao Trabalho de Conclusão de Curso intitulado:

**MONITORAMENTO CONTÍNUO DE CO₂ EM AMBIENTES INTERNOS: DESENVOLVIMENTO DE UM SISTEMA IoT DE BAIXO CUSTO E DE UM GUIA DIDÁTICO BASEADO NA ABORDAGEM STEAM
.**

Instituição: Universidade Federal de Alagoas (UFAL) — Campus Arapiraca.

Curso: Licenciatura em Química.

Orientação: Prof. Dr. Sérgio Modesto Vechi.

## 10. Repositório

Código-fonte disponível em:

https://github.com/handedsoiler/monitoramento-co2-iot
