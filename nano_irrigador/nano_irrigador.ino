//tempo que a irrigação ficará ligada quando apertar o botao (em minutos)
#define TEMPO_LIGADO_MINUTOS 20

#define PINO_BOTAO_LIGA 2
#define PINO_LUZ_AZUL 4
#define PINO_LUZ_VERMELHA 6
#define PINO_REGADOR 8
#define PINO_CARREGADOR 9 //pino que liga o carregador em LOW, e desliga em HIGH


volatile bool ligarRegador = false;


void ligaValvulaIrrigacao();
void desligaValvulaIrrigacao();
void atualizaStatusLuzes(uint16_t estadoLuzes);
void efetuaIrrigacao(int32_t segundos);
void digitalInterrupt();
void entraEmSonoProfundo();

void setup() {

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


  Serial.begin(9600);
  Serial.println("Iniciando sistema...");

  //default values
  digitalWrite(PINO_LUZ_AZUL, LOW);
  digitalWrite(PINO_LUZ_VERMELHA, LOW);
  digitalWrite(PINO_REGADOR, LOW);
  digitalWrite(PINO_CARREGADOR, LOW);
  ligarRegador = false;

  //pisca as luzes rapidamente pra dizer que o sistema ligou
  for (int i = 0; i < 50; i++)
  {
    delay(100);
    atualizaStatusLuzes(1);
    delay(100);
    atualizaStatusLuzes(0);
  }

  atualizaStatusLuzes(-1);

  Serial.println("Sistema iniciado");

  entraEmSonoProfundo();
}

void loop() {
  if (ligarRegador)
  {    
    Serial.println("Desligando carregador.");
    digitalWrite(PINO_CARREGADOR, HIGH);
    delay(500); //espera o relay magnetizar e desligar o carregador
    
    Serial.println("Ligando regador.");
    efetuaIrrigacao(TEMPO_LIGADO_MINUTOS * 60);
    delay(500);//espera a valvula desmagnetizar

    
    Serial.println("Ligando carregador.");
    digitalWrite(PINO_CARREGADOR, LOW);
    delay(500); //espera o relay desmagnetizar e ligar o carregador
    ligarRegador = false;
    
    entraEmSonoProfundo();
  }
}

void entraEmSonoProfundo()
{
  Serial.println("Entrando em deep sleep");

  delay(1000);
    //bota o sistema em sleep até que uma interrupt no pin o acorde
    SMCR |= (1 << 2); //power down mode
    SMCR |= 1; //enable sleep
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
  while (segundos > 0)
  {
    segundos--;

    //liga o regador por x segundos e desliga por y
    //assim o fluxo de agua alterna entre perto e longe
    for (int i = 0; i < 10; i++)
    {
      ligaValvulaIrrigacao();
      atualizaStatusLuzes(0);
      delay(500);
      atualizaStatusLuzes(1);
      delay(500);

      segundos--;
    }

    for (int i = 0; i < 10; i++)
    {
      desligaValvulaIrrigacao();
      atualizaStatusLuzes(0);
      delay(500);
      atualizaStatusLuzes(1);
      delay(500);

      segundos--;
    }
  }

  //termina desligado as luzes
  atualizaStatusLuzes(-1);

}
