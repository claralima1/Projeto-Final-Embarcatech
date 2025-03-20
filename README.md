# Berço Inteligente com Pulseira de Alerta

Este projeto é o protótipo de um sistema que simula o monitoramento de berço inteligente que utiliza sensores de pressão para detectar movimentos do bebê e alertar os pais ou cuidadores por meio de uma pulseira conectada. O sistema é composto por uma Raspberry Pi Pico W, sensores de pressão, um display OLED, LEDs RGB e um buzzer para alertas visuais e sonoros.

## Funcionalidades

- **Monitoramento em Tempo Real:** O sistema detecta movimentos do bebê por meio de um sensor de pressão simulado.
- **Alertas Visuais e Sonoros:** Quando o movimento é detectado, o sistema aciona um LED vermelho e um buzzer para alertar os responsáveis.
- **Display OLED:** Exibe mensagens de status, como "Bebe Movimentando!" e "Bebe Dormindo!", para fornecer informações visuais.
- **Simulação de Sensor de Pressão:** O valor do sensor de pressão é gerado aleatoriamente a cada 5 segundos para simular a detecção de movimento.

## Componentes Utilizados

- **Raspberry Pi Pico W:** Microcontrolador principal.
- **Sensor de Pressão (Simulado):** Detecta movimentos do bebê.
- **Display OLED:** Exibe informações sobre o estado do bebê.
- **LED RGB:** Fornece alertas visuais.
- **Buzzer:** Emite alertas sonoros.
- **Pulseira de Alerta:** Conectada ao sistema para notificar os pais.

## Como Funciona

1. **Detecção de Movimento:** O sensor de pressão simulado gera valores aleatórios a cada 5 segundos. Se o valor exceder um limiar pré-definido, o sistema detecta movimento.
2. **Alertas:** Quando o movimento é detectado, o LED vermelho pisca e o buzzer emite um som. O display OLED exibe a mensagem "Bebe Movimentando!".
3. **Sem Movimento:** Se nenhum movimento for detectado, o display OLED exibe "Bebe Dormindo!" e os alertas são desativados.
