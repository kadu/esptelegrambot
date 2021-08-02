# ESP8266 / ESP32 - Telegram BOT

## Demonstração

Bem vindos ao projeto de demonstração dos microcontroladores da Espressif ESP8266 / ESP32.
Nesse projeto vamos testar as capacidades do ESP8266 (NodeMCU) fazendo um BOT para o Telegram. Esse Bot terá uma fita de LED com 20 leds RGBs endereçaveis e um sensor de temperatura (BMP180) e através de comandos enviados para o BOT será possível mudar as cores de um determinado pixel da fita de led e saber qual é a temperatura do local aonde o microcontrolador está instalado.

Nesse projeto utilizei o plugin do VSCode PlatformIO, para fazer o gerenciamento das bibliotecas e desenvolvimento e instalação dos firmwares dos microcontroladores.

Caso você queira rodar esse exemplo em um ESP32, será necessário apenas alterar alguns cabeçalhos das bibliotecas de conexão.

## Bibliotecas utilizadas
- Adafruit Neopixel
- UniversalTelegramBot

## Arquivo de Segredos (secrets.h)
Existe um arquivo chamado secrets.h contendo os dados secretos do Wifi e o Token do BOT de Telegram

### Conseguindo o seu ID
Algumas funções foram pensadas para ser feitas apenas pelo "dono" do bot, para isso precisamos informar o ChatID do dono do bot, para isso, toda mensagem recebida pelo bot irá mostrar um cabeçalho de debug contendo algumas informações, dentre elas o ChatID, mande uma mensagem e anote esse número no arquivo de segredos e você terá acesso as funções administrativas do bot.

## Reproduzindo o experimento
Você precisará pegar o token para seu bot no telegram, para isso adicione o contato BotFather no seu telegram e siga as instruções para a criação de um novo BOT. Anote seu token e atualize o arquivo de segredos com suas credenciais.
Adicione o bot criado por você nos seus contatos e então você poderá fazer as chamadas das funções para seu bot.

## Ligações (Wiring)

## Algumas telas do BOT funcionando