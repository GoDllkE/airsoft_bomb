
/////////////////////////////////////
//          BOMBA TALIBA           //
//       por Gustavo Toledo        //
/////////////////////////////////////
/*
 *  Projeto para construcao de uma bomba com timer.
 *  
 *  Quantidade de pinos usado:
 *  
 *  Componentes requeridos:
 *  - Arduino Mega 2560
 *  - Teclado numerico
 *  - Display LCD 16x2
 *  - Buzzer
 *  - 3x leds:
 *    - 1x led vermelha
 *    - 1x led amarela
 *    - 1x led verde
 *  - Jumpers (muitos)
 * 
 * 
*/
#include <Keypad.h>
#include <LiquidCrystal.h>

#define DEBUG false

/*
 ********************************************************
 *      Define dados estaticos referente a senha
 *******************************************************
*/
// Define senha de ativacao
const char senha_real[6] = {'3','3', '4', '1', '4', '8'};

// Define variavel que guardara a senha digitada
char senha[6] = {'*', '*', '*', '*', '*', '*'};

//
int senha_correta = 0;


/*
 ********************************************************
 *      Define dados estaticos referente ao desarme da bomba
 *******************************************************
*/
// 1 = desarmado com sucesso | 0 = bomba nao desarmada
// int defuse = 0;

// Fios
struct Fios {
  const int fio_certo_in;
  const int fio_certo_out;
  const int fio_errado_in[1];
  const int fio_errado_out;
  int defuse;
};

// Instancia os fios
struct Fios fios = {28, 29, 30, 32, 31};

/*
 ********************************************************
 *      Define estrutura de dados referente aos leds
 *******************************************************
*/
struct Led {
  const int vermelho;                   // Pino do led vermelho
  const int amarelo;                    // Pino do led amarelo
  const int verde;                      // Pino do led verde
 };
 
// Define pinos dos leds (vermelho, amarelo, verde)
struct Led led = { A8, A9, A10};

/*
 ********************************************************
 *      Define estruturea de dados referente ao temporizador
 *******************************************************
*/
struct Temporizador {
    int seconds;                        // Segundos
    int minutes;                        // Minutos
    int hours;                          // Horas
    int defuse_time;                    // Nao implementado
    int time_interval;                  // Tempo entre os intervalos (em milisegundos)
};

// Define temporizador
struct Temporizador temporizador;

/*
 ********************************************************
 *          Define teclado numerico
 *******************************************************
*/
const byte ROWS = 4, COLS = 4; //four rows
byte rowPins[ROWS] = {14, 15, 16, 17};
byte colPins[COLS] = {18, 19, 20, 21};
char keys[ROWS][COLS] = {
    {'1','2','A','3'},
    {'4','5','B','6'},
    {'7','8','C','9'},
    {'*','0','D','#'}
};

// Instancia objeto KeyPad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/*
 ********************************************************
 *          Define LCD
 *******************************************************
 O circuito:
 * LCD VSS to ground
 * LCD VDD to +5V
 * LCD V0 to ground
 * LCD RS pin to digital pin 0
 * LCD R/W pin to ground
 * LCD E (Enable) pin to digital pin 1
 * LCD D0,1,2,3 leave with nothing
 * LCD D4 pin to digital pin 2
 * LCD D5 pin to digital pin 3
 * LCD D6 pin to digital pin 4
 * LCD D7 pin to digital pin 5
 * LCD A pin to +5V with an 10k resistor (to lower to 3.3v)
 * LCD K pin to ground
*/
const int rs = A7, en = A6, d4 = A5, d5 = A4, d6 = A3, d7 = A2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/*
 ********************************************************
 *          Define buzzer
 *******************************************************
 */
const int buzzer = A0;

/*
 *********************************************************************************
 *                           Funcoes auxiliares
 *********************************************************************************
*/

/*
 *  Funcao que faz beep
 *  STATUS: finalizado
*/
void beep(int tempo_delay=25, int frequencia=1500) {
  pinMode(buzzer, OUTPUT);
  tone(buzzer, frequencia);
  delay(tempo_delay);
  noTone(buzzer);
  pinMode(buzzer, INPUT);
}

/*
 *  Funcao que valida a senha digitada no comeco do programa
 *  STATUS: finalizado
*/
int valida_senha() {
  // Controladores
  int valido = 0;
  int invalido = 0;

  // Valida cara entrada na lista
  for (int i=0; i < 6; i++) {
    //
    if (DEBUG) {
      Serial.print("DEBUG => ");
      Serial.print("Index: ");
      Serial.print(i);
      Serial.print(" | Senha real: ");
      Serial.print(senha_real[i]);
      Serial.print(" | senha: ");
      Serial.print(senha[i]);
      Serial.print(" | ");
    }

    // Atualiza os controladores
    if (senha_real[i] == senha[i]) { 
      if (DEBUG) { Serial.print("Valido"); }
      valido++; 
    } else {
      if (DEBUG) { Serial.print("Invalido"); }
      invalido++; 
    }
    
    //
    if (DEBUG) { Serial.println(); }    
  }

  // Valida pela quantidade de vetores validos
  // Retorna 1 se algum index invalido foi encontrado
  if (invalido != 0) { 
    senha_correta = 0;
    return 1; 
  } 
  
  // Validado
  senha_correta = 1;
  return 0;
}


/*
 *  Funcao que retorna o tamanho da senha digitada
 *  STATUS: finalizado
*/
int tamanho_senha_digitada() {
  int tamanho = 0;

  for (int i=0; i<6; i++) {
    if (senha[i] != '*') {
      tamanho +=1;      
    }
  }

  return tamanho;
}


/*
 *  Funcao que exibe mensagem de inicializacao no display
 *  STATUS: finalizado
*/
void inicializando_display() {
  // Limpa a tela antes de inicializar
  lcd.clear();
  lcd.home();
  lcd.print("Initializing...");
  delay(100);

  // Faz a barra de progresso na segunda linha
  lcd.setCursor(0, 1);
  lcd.print("[");
  
  for (int i=0; i<14; i++) {
    lcd.print("=");
    delay(200);
  }
  lcd.print("]");
  delay(200);

  // Exibe mensagem de inicializacao concluida
  lcd.clear();
  lcd.home();
  lcd.print("Initialized!");
}

/*
 *  Funcao que faz o led piscar
 *  STATUS: finalizado
*/
void pisca_led(int led, int tempo_delay=25) {
  pinMode(led, OUTPUT);
  delay(tempo_delay);
  pinMode(led, INPUT);
}

/*
 *  Funcao responsavel por alertar que uma bomba explodiu
 *  STATUS: finalizado
*/
void bomba_explodiu() {
  // Exibe mensagem no display
  lcd.clear();
  lcd.home();
  lcd.print("Bomb exploded!");
  lcd.setCursor(0, 1);
  lcd.print("Game over"); 
  
  // Faz alerta de bomba explodida (som e luz)
  while (true) {
    pinMode(led.vermelho, OUTPUT);
    pinMode(led.amarelo, OUTPUT);
    beep();
  }

  // Finaliza programa | bloqueia quaisquer acoes futuras
  exit(0);
}

/*
 *  Funcao que faz o checkup se a bomba foi desarmada
 *  STATUS: em andamento
*/
void defuse() {
  // Configura fios de desarme certos
  pinMode(fios.fio_certo_out, OUTPUT);
  pinMode(fios.fio_certo_in, INPUT);

  // Configura fios de desarme errados
  pinMode(fios.fio_errado_out, OUTPUT);
  pinMode(fios.fio_errado_in[0], INPUT);  
  pinMode(fios.fio_errado_in[1], INPUT);

  // Faz leitura do fios errados
  digitalWrite(fios.fio_errado_out, 1);
  int wrong_wires[2] = { digitalRead(fios.fio_errado_in[0]), digitalRead(fios.fio_errado_in[1]) };

  // Faz leitura do fio certo
  digitalWrite(fios.fio_certo_out, 1);
  int defuse_wire = digitalRead(fios.fio_certo_in);

  // Se o fio errado nao estiver comunicando...
  // entao a bomba foi desarmada incorretamente / explodiu
  if ((wrong_wires[0] != 1) || (wrong_wires[1] != 1)) {
     bomba_explodiu();
  }

  // Se fio certo nao estiver comunicando...
  // entao a bomba foi desarmada com sucesso
  if ((defuse_wire != 1) && ((wrong_wires[0] == 1) || (wrong_wires[1] == 1))) {
    lcd.setCursor(0, 1);
    lcd.print("Defused");
    exit(0);
  }
  
}

/*
 *   Funcao que faz o timer da bomba
 *   STATUS: finalizado
*/
void timer() {
    //
    if (temporizador.hours <= 0) {
        if (temporizador.minutes <= 0) {
            if (temporizador.seconds == 0) {
              bomba_explodiu();
            }
        }
    }

    lcd.setCursor(0, 0);
    lcd.print("Timer: ");

    // Padrao do temporizador 00:00:00
    // As proximas condicionais servem para formatar esse padrao

    // Formata as horas
    if (temporizador.hours >= 10) {
        lcd.setCursor(7, 0);
        lcd.print(temporizador.hours);
    } else { // Incrementa um '0' antes da hora
        lcd.setCursor(7, 0);
        lcd.write("0");
        lcd.setCursor(8, 0);
        lcd.print(temporizador.hours);
    }

    //
    lcd.print(":");

    // Formata os minutos
    if (temporizador.minutes >= 10) {
        lcd.setCursor(10, 0);
        lcd.print(temporizador.minutes);
    } else { // Incrementa um '0' antes do minuto
        lcd.setCursor(10, 0);
        lcd.write("0");
        lcd.setCursor(11, 0);
        lcd.print(temporizador.minutes);
    }

    //
    lcd.print(":");

    // Formata os segundos
    if (temporizador.seconds >= 10) {
        lcd.setCursor(13, 0);
        lcd.print(temporizador.seconds);
    } else { // Incrementa um '0' antes do segundo
        lcd.setCursor(13, 0);
        lcd.write("0");
        lcd.setCursor(14, 0);
        lcd.print(temporizador.seconds);
    }

    // Readequa os tempos
    if (temporizador.hours < 0) { temporizador.hours = 0; }

    if (temporizador.minutes < 0) {
        temporizador.hours--;        // Diminui -1 das horas
        temporizador.minutes = 59;   // Reseta timer dos minutos
    }

    if (temporizador.seconds < 1) {
        temporizador.minutes--;     // Diminui -1 dos minutos
        temporizador.seconds = 60;  // Reseta timer dos segundos
    }

    if (temporizador.seconds > 0) {
        // Espera intervalo de 1 segundo / 1000 milisegundos
        // delay(temporizador.time_interval);
        
        // Diminui -1 segundo do contador
        temporizador.seconds--;
        // beep();

        // SE estiver nos segundos finais, pisque/beep igual louco
        if ((temporizador.hours == 0) && (temporizador.minutes == 0)) {  
          if (temporizador.seconds < 10) { // Ultimos 10 segundos
            for (int i=0; i <10; i++) {
              pisca_led(led.amarelo);
              beep();
              delay(100); // Delay de 1s
            }        
          } else if (temporizador.seconds < 20) { // Ultimos 20 segundos
            for (int i=0; i <7; i++) {
              pisca_led(led.amarelo);
              beep();
              delay(150); // Delay de 1s
            }     
           } else if (temporizador.seconds < 30) { // Ultimos 30 segundos
            for (int i=0; i <5; i++) {
              pisca_led(led.amarelo);
              beep();
              delay(200); // Delay de 1s
            }     
          } else { // Ultimos 40 segundos
            for (int i=0; i <4; i++) {
              pisca_led(led.amarelo);
              beep();
              delay(250); // Delay de 1s
            }
          }
          
        } else { // Se nao, pisque apenas uma vez
            pisca_led(led.amarelo);
            beep();

            // Delay de 1s - 25 milisegundos do beep e 25 milisegundos do led
            delay(temporizador.time_interval-(25+25));
        }        
    }
} // Fim do 'timer'

/*
 *********************************************************************************
 *                           Funcoes principais
 *********************************************************************************
*/
void setup() {
  //
  lcd.begin(16, 2);
  Serial.begin(9600);

  // Configura os pinos dos leds

  // Inicializa o display com animacao
  inicializando_display();

  // Dispara 10 beeps na inicializacao
  for (int i=0; i<10; i++) {
    pisca_led(led.verde, 25);
    beep();
    pisca_led(led.amarelo, 25);
    delay(50);
    pisca_led(led.vermelho, 25);
    delay(50);
  }

  // Pede codigo de ativacao
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Safety protocol");
  lcd.setCursor(0, 1);
  lcd.print("Code: ");

  // Configura senha na inicializacao
  //
  char key = keypad.getKey();
  int index = tamanho_senha_digitada();

  //
  if (DEBUG) {
    Serial.print("DEBUG => ");
    Serial.print("Tamanho da senha real: ");
    Serial.print(sizeof(senha_real));
    Serial.print(" | Tamanho da senha: ");
    Serial.print(tamanho_senha_digitada());
    Serial.print(" | Index: ");
    Serial.print(index);
    Serial.println(); 
  }

  while (index < sizeof(senha_real)) {
    // Controladores
    index = tamanho_senha_digitada();
    key = keypad.getKey();
    
    // Se uma tecla for pressionada
    if (key) {
      // Emite um beep de tecla precionada
      beep();

      //
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Safety protocol.");

      // Depuracao serial
      if (DEBUG) {
        Serial.print("DEBUG => ");
        Serial.print("Chave digitada: ");
        Serial.println(key); 
        Serial.print(" | Tamanho da senha: ");
        Serial.print(tamanho_senha_digitada());
        Serial.print(" | Index: ");
        Serial.print(index);
        Serial.println(); 
      }

      // Insere chave a lista de caracteres referente a senha final
      senha[index] = key;

      // Exibe no display
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Safety protocol.");
      lcd.setCursor(0, 1);
      lcd.print("Code: ");
      lcd.print(senha);

      // Depuracao serial
      if (DEBUG) {
        Serial.print("DEBUG => ");
        Serial.print("Senha real: ");
        Serial.print(senha_real);
        Serial.print(" | Senha digitada: ");
        Serial.print(senha);
        Serial.println();
      }
    }
  }

  // Valida a senha digitada com a senha real
  if (sizeof(senha_real) == index) {
    if (valida_senha() == 1) {
      if (DEBUG) {
        Serial.println("Senha invalida");
        Serial.println("Reinicie a bomba e tente novamente"); 
      }

      // Emite tres bipes longos para sinalizar falha de ativacao
      for (int i=0; i<3; i++) {
        beep(600, 2000);
        delay(100);
      }
    } else { // Beep longo para sinalizar que a bomba foi ativada
      if (DEBUG) { Serial.println("Senha valida!"); }
      beep(1000, 2000);
    }
  }

  //
  lcd.clear();
  lcd.home();
  if (senha_correta == 1) {
    lcd.print("Activated!");
  } else {
    lcd.print("Not activared!");
  }

  /* PS: Aqui define o tempo de duracao da bomba */
  temporizador.seconds = 0;
  temporizador.minutes = 1;
  temporizador.hours = 0;
  temporizador.time_interval = 1000;
  
}


/* ********************************************************************************** */

void loop() {
  //
  // Chama o timer aqui
  if(senha_correta ==1) {
    timer();
    defuse();
  } else {
    pinMode(led.vermelho, OUTPUT);
    lcd.setCursor(0, 1);
    lcd.print("Restart the bomb");
    delay(500);    
  }
}

/* ********************************************************************************** */
