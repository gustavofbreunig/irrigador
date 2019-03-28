//tempo que a irrigação ficará ligada quando apertar o botao (em minutos)
#define TEMPO_LIGADO_MINUTOS 20

#define PINO_BOTAO_LIGA 2
#define PINO_LUZ_AZUL 4
#define PINO_LUZ_VERMELHA 6
#define PINO_REGADOR 8

void ligaValvulaIrrigacao();
void desligaValvulaIrrigacao();
void atualizaStatusLuzes(uint16_t estadoLuzes);
void efetuaIrrigacao(int32_t segundos);

void setup() {
  pinMode(PINO_BOTAO_LIGA, INPUT_PULLUP); //mantem ligado ao vcc pelo pull up
  pinMode(PINO_LUZ_AZUL, OUTPUT);
  pinMode(PINO_LUZ_VERMELHA, OUTPUT);
  pinMode(PINO_REGADOR, OUTPUT);

  digitalWrite(PINO_LUZ_AZUL, LOW);
  digitalWrite(PINO_LUZ_VERMELHA, LOW);
  digitalWrite(PINO_REGADOR, LOW);
}

void loop() {
  //TODO: deep sleep para economizar bateria  
  delay(1000);

  if (digitalRead(PINO_BOTAO_LIGA) == LOW)
  {
    int32_t segundos_irrigacao = 60 * TEMPO_LIGADO_MINUTOS;
    efetuaIrrigacao(segundos_irrigacao);
  }
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
