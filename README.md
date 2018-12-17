## Erros
Essa implementação da comunicação via modbus pode apresentar quatro erros, são eles:

 - CRC_ERROR: Quando o modulo ESP-01 responde com frame de CRC inválido;
 - REGISTER_ERROR: Quando o módulo ESP-01 responde com frame de registrador inválido;
 - FRAME_ERROR: Quando o frame de resposta do módulo ESP-01 contém um tamanho ou CRC inválido;
 - TIMEOUT_ERROR: Quando o módulo demora para responder.

A visualização dos erros é feita através de três *leds*.

| PORTB5 | PORTB0 | PORTB1 | ERRO |
| -- | -- | -- | -- | 
| 1 | 0 | 0 | CRC_ERROR |
| 1 | 0 | 1 | FRAME_ERROR |
| 1 | 1 | 0 | REGISTER_ERROR|
| 1 | 1 | 1 | TIMEOUT_ERROR |

