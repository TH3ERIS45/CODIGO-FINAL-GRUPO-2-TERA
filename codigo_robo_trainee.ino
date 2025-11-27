const int MOTOR_ESQ_AIN1 = 5; // Pino PWM
const int MOTOR_ESQ_AIN2 = 6; // Pino PWM

const int MOTOR_DIR_BIN1 = 9; // Pino PWM
const int MOTOR_DIR_BIN2 = 10;// Pino PWM

const int PIN_SENSOR_BORDA_ESQ = A0; 
const int PIN_SENSOR_BORDA_DIR = A1; 

const int PIN_SENSOR_TRIG = 4;
const int PIN_SENSOR_ECHO = 7; 

const int LIMITE_DISTANCIA_ATAQUE_CM = 30; 

const int LIMIAR_BORDA = 500; 

const int VEL_MAXIMA_ATAQUE = 255;  
const int VEL_FUGA_TRAS = 255; 
const int VEL_FUGA_GIRO = 220;
const int VEL_BUSCA_GIRO = 180;    

bool bordaEsquerdaDetectada = false;
bool bordaDireitaDetectada = false;
long distanciaCentroCm = 0;

unsigned long ultimoTempoRadar = 0;
bool virandoEsquerda = true;
const int TEMPO_RADAR = 600; 

void moverFrente(int velocidade);
void moverTras(int velocidade);
void girarEsquerda(int velocidade);
void girarDireita(int velocidade);
void pararMotores();
long lerDistanciaUltrassonico(int pinoTrig, int pinoEcho);
bool lerSensorBordaEsquerda();
bool lerSensorBordaDireita();
void buscaRadar(); 

void setup() 
{
  pinMode(MOTOR_ESQ_AIN1, OUTPUT); 
  pinMode(MOTOR_ESQ_AIN2, OUTPUT);

  pinMode(MOTOR_DIR_BIN1, OUTPUT);
  pinMode(MOTOR_DIR_BIN2, OUTPUT);

  pinMode(PIN_SENSOR_BORDA_ESQ, INPUT);
  pinMode(PIN_SENSOR_BORDA_DIR, INPUT);

  pinMode(PIN_SENSOR_TRIG, OUTPUT);
  pinMode(PIN_SENSOR_ECHO, INPUT);

  Serial.begin(9600);
  Serial.println("<<<Robo Iniciado>>>");
  delay(5000); 
  Serial.println("<<<Robo incia o combate>>>");
  ultimoTempoRadar = millis(); 
}

void loop() 
{
  bordaEsquerdaDetectada = lerSensorBordaEsquerda();
  bordaDireitaDetectada = lerSensorBordaDireita();
  distanciaCentroCm = lerDistanciaUltrassonico(PIN_SENSOR_TRIG, PIN_SENSOR_ECHO);

  // --- 1. Nao cair da borda
  if (bordaEsquerdaDetectada || bordaDireitaDetectada)
  {
    Serial.println("Prioridade 1: Borda PRETA!");
    pararMotores(); 
    delay(50); 
    
    moverTras(VEL_FUGA_TRAS);
    delay(400);
    
    if (bordaEsquerdaDetectada) 
    {
      girarDireita(VEL_FUGA_GIRO);
    } 
    else 
    {
      girarEsquerda(VEL_FUGA_GIRO);
    }
    delay(400);
    
    ultimoTempoRadar = millis(); // Reseta o radar
    return; 
  }
  
  // --- 2. ATAQUE ---
  else if (distanciaCentroCm > 0 && distanciaCentroCm < LIMITE_DISTANCIA_ATAQUE_CM)
  {
    Serial.println("P2: Ataque"); 
    moverFrente(VEL_MAXIMA_ATAQUE); 
    ultimoTempoRadar = millis(); 
  }
  
  // --- 3. BUSCA (ZIG-ZAG AVANTE) ---
  else 
  {
    buscaRadar();
  }
}

void buscaRadar() 
{
  // Controla o tempo de troca de lado
  if (millis() - ultimoTempoRadar > TEMPO_RADAR) 
  {
    virandoEsquerda = !virandoEsquerda; 
    ultimoTempoRadar = millis();
    pararMotores(); 
    delay(50); 
  }
  if (virandoEsquerda) 
  {
    analogWrite(MOTOR_ESQ_AIN1, 0);
    digitalWrite(MOTOR_ESQ_AIN2, LOW);

    analogWrite(MOTOR_DIR_BIN1, VEL_BUSCA_GIRO);
    digitalWrite(MOTOR_DIR_BIN2, LOW);
  }
  else 
  {
    analogWrite(MOTOR_ESQ_AIN1, VEL_BUSCA_GIRO);
    digitalWrite(MOTOR_ESQ_AIN2, LOW);

    analogWrite(MOTOR_DIR_BIN1, 0);
    digitalWrite(MOTOR_DIR_BIN2, LOW);
  }
}

void moverFrente(int velocidade) 
{
  analogWrite(MOTOR_ESQ_AIN1, velocidade); 
  digitalWrite(MOTOR_ESQ_AIN2, LOW);

  analogWrite(MOTOR_DIR_BIN1, velocidade); 
  digitalWrite(MOTOR_DIR_BIN2, LOW);
}

void moverTras(int velocidade) 
{
  digitalWrite(MOTOR_ESQ_AIN1, LOW); 
  analogWrite(MOTOR_ESQ_AIN2, velocidade);

  digitalWrite(MOTOR_DIR_BIN1, LOW); 
  analogWrite(MOTOR_DIR_BIN2, velocidade);
}

void girarEsquerda(int velocidade) 
{
  digitalWrite(MOTOR_ESQ_AIN1, LOW); 
  analogWrite(MOTOR_ESQ_AIN2, velocidade);

  analogWrite(MOTOR_DIR_BIN1, velocidade); 
  digitalWrite(MOTOR_DIR_BIN2, LOW);
}

void girarDireita(int velocidade) 
{
  analogWrite(MOTOR_ESQ_AIN1, velocidade); 
  digitalWrite(MOTOR_ESQ_AIN2, LOW);

  digitalWrite(MOTOR_DIR_BIN1, LOW); 
  analogWrite(MOTOR_DIR_BIN2, velocidade);
}

void pararMotores() 
{
  digitalWrite(MOTOR_ESQ_AIN1, HIGH); 
  digitalWrite(MOTOR_ESQ_AIN2, HIGH);

  digitalWrite(MOTOR_DIR_BIN1, HIGH); 
  digitalWrite(MOTOR_DIR_BIN2, HIGH);
}

long lerDistanciaUltrassonico(int pinoTrig, int pinoEcho) 
{
  digitalWrite(pinoTrig, LOW); 
  delayMicroseconds(2);
  digitalWrite(pinoTrig, HIGH); 
  delayMicroseconds(10);
  digitalWrite(pinoTrig, LOW);
  
  long duracao = pulseIn(pinoEcho, HIGH, 5000); 
  
  if (duracao == 0) 
  {
    return 100;
  }
  
  return duracao * 0.034 / 2;
}

bool lerSensorBordaEsquerda() 
{
  return (analogRead(PIN_SENSOR_BORDA_ESQ) > LIMIAR_BORDA);
}

bool lerSensorBordaDireita() 
{
  return (analogRead(PIN_SENSOR_BORDA_DIR) > LIMIAR_BORDA);
}
