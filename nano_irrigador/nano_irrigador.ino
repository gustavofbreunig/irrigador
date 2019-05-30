//tempo que a irrigação ficará ligada quando apertar o botao (em minutos)
#define TEMPO_LIGADO_MINUTOS 10

#define PINO_BOTAO_LIGA 2
#define PINO_LUZ_AZUL 4
#define PINO_LUZ_VERMELHA 6
#define PINO_REGADOR 8

volatile bool ligarRegador = false;

void ligaValvulaIrrigacao();
void desligaValvulaIrrigacao();
void atualizaStatusLuzes(uint16_t estadoLuzes);
void efetuaIrrigacao(int32_t segundos);
void digitalInterrupt();
void entraEmSonoProfundo();

void setup() {

#ifdef MSGS_SERIAL
  Serial.begin(9600);
  Serial.println("Iniciando sistema...");
#endif

  //********************************************************************************************************
  //técnicas para diminuição de consumo, técnicas do video https://www.youtube.com/watch?v=urLSDi7SD8M
  //passa todos os outros pinos para OUTPUT
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);

  //desabilita o conversor analógico-digital
  ADCSRA &= ~(1 << 7);
  //********************************************************************************************************

  //define a interrupção pra quando o pino vai pra LOW (falling edge)
  pinMode(PINO_BOTAO_LIGA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PINO_BOTAO_LIGA), digitalInterrupt, FALLING);

  //default values
  digitalWrite(PINO_LUZ_AZUL, LOW);
  digitalWrite(PINO_LUZ_VERMELHA, LOW);
  digitalWrite(PINO_REGADOR, LOW);
  ligarRegador = false;

  //pisca as luzes rapidamente pra dizer que o sistema ligou
  for (int i = 0; i < 20; i++)
  {
    delay(100);
    atualizaStatusLuzes(1);
    delay(100);
    atualizaStatusLuzes(0);
  }

  atualizaStatusLuzes(-1);

#ifdef MSGS_SERIAL
  Serial.println("Sistema iniciado");
#endif

  entraEmSonoProfundo();
}

void loop() {
  if (ligarRegador)
  {
#ifdef MSGS_SERIAL
    Serial.println("Ligando regador.");
#endif
    efetuaIrrigacao(TEMPO_LIGADO_MINUTOS * 60);
    ligarRegador = false;
    delay(500);//espera a valvula desmagnetizar

    entraEmSonoProfundo();
  }
}

void entraEmSonoProfundo()
{
#ifdef MSGS_SERIAL  
  Serial.println("Entrando em deep sleep");
#endif
  delay(500);
  //bota o sistema em sleep até que uma interrupt no pin o acorde
  SMCR |= (1 << SM1); //power down mode
  SMCR |= SE; //enable sleep
  __asm__ __volatile__("sleep");
}

void digitalInterrupt()
{
  ligarRegador = true;
}

void ligaValvulaIrrigacao()
{
  digitalWrite(PINO_REGADOR, HIGH);
}

void desligaValvulaIrrigacao()
{
  digitalWrite(PINO_REGADOR, LOW);
}

void atualizaStatusLuzes(uint16_t estadoLuzes)
{
  //estadoLuzes:
  //-1 = luzes desligadas
  //0 = luz azul ligada
  //1 = luz vermelha ligada

  if (estadoLuzes == -1)
  {
    digitalWrite(PINO_LUZ_AZUL, LOW);
    digitalWrite(PINO_LUZ_VERMELHA, LOW);
  }
  else if (estadoLuzes == 0)
  {
    digitalWrite(PINO_LUZ_AZUL, HIGH);
    digitalWrite(PINO_LUZ_VERMELHA, LOW);
  }
  else if (estadoLuzes == 1)
  {
    digitalWrite(PINO_LUZ_AZUL, LOW);
    digitalWrite(PINO_LUZ_VERMELHA, HIGH);
  }
}

void efetuaIrrigacao(int32_t segundos)
{
  //efetua irrigadao por x segundos
  ligaValvulaIrrigacao();
  while (segundos > 0)
  {
    segundos--;

    atualizaStatusLuzes(0);
    delay(500);
    atualizaStatusLuzes(1);
    delay(500);

  }

  desligaValvulaIrrigacao();

  //termina desligado as luzes
  atualizaStatusLuzes(-1);

}
