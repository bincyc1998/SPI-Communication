/* Set TASK to the task you are working on.
 * Each task assumes that the previous tasks are already completed. */ 
#define TASK 4


#define SPI_SCK_PIN  13
#define SPI_MISO_PIN 12
#define SPI_MOSI_PIN 11
#define SPI_CS_PIN   10

#define SPI_SCK_T 100 // Clock period in ms

/*  Be sure that this delay is smaller than the SLAVE's delay */
#define START_DELAY 500 // Delay before loop().
#define SEND_DELAY  500 // PAUSE BETWEEN CONSECUTIVE BYTE SENDS

/* Send data from FIRST_CHAR to LAST_CHAR */
#define FIRST_CHAR 'A'
#define LAST_CHAR  'Z'

/* Data to be sent */
char dataSend = FIRST_CHAR;

/* Clock internal variables */
int clkState = LOW;
unsigned long prevTick = 0.0;

/* Set the Clock pin to low and reset clock internal variables */
void resetClock(void)
{
  prevTick = millis();
  clkState = LOW;

#if TASK >= 3
  /* TODO3: Set the Clock pin to the idle state */
  digitalWrite(SPI_SCK_PIN, LOW);
#endif /* TASK >= 3 */
}

/* Simulate a clock sourse: HIGH, LOW, HIGH, LOW ... 
 * by toggling SPI_SCK_PIN every SPI_SCK_T ms.
 *
 * Return -1       NO transition
 *         0(LOW)  in case it was a transition from HIGH to LOW.
 *         1(HIGH) in case it was a transition from LOW to HIGH.
 */
int getClockTransition(void)
{
  int transition = -1;
  
  if ((millis() - prevTick ) >= SPI_SCK_T)
  {
    clkState = !clkState;
    prevTick = millis();
    transition = clkState;

#if TASK >= 3
    /* TODO3: Toggle the Clock pin by setting it to the clkState variable */
    digitalWrite(SPI_SCK_PIN, clkState);
#endif /* TASK >= 3 */
  }
  
  return transition;
}

/* Send a byte of data to the Slave. 
 * Receive a byte of data from the Slave.
 *
 * param  dataSend - data to be sent to the Slave
 *
 * Return char     - data received from the Slave
 */
char sendRecvByte(char dataSend)
{
  int bit;
  int clkTransition;
  char dataRecv = '\0';
  
  int dataSendBitIndex = 0;
  int dataRecvBitIndex = 0;
  
#if TASK >= 3
  resetClock();
  Serial.println("CLK signal reset");

#if TASK >= 4
  /* TODO4: Extract the bit from the dataSendBitIndex of the dataSend byte of data */
  bit = bitRead(dataSend, dataSendBitIndex);
  /* TODO4: Send the bit using the MOSI pin */
  digitalWrite(SPI_MOSI_PIN, bit);
#endif /* TASK >= 4 */
  dataSendBitIndex++;

  while (1)
  {
    clkTransition = getClockTransition();
    if (clkTransition == HIGH)
    {
      Serial.println("CLK signal set HIGH");
      
#if TASK >= 4
      /* TODO4: Read the incoming bit of data on the MISO pin */
      bit = digitalRead(SPI_MISO_PIN);
      /* TODO4: Store the bit to the dataRecvBitIndex of the dataRecv byte of data */
      bitWrite(dataRecv, dataRecvBitIndex, bit);
#endif /* TASK >= 4 */
      dataRecvBitIndex++;
    }
    else if (clkTransition == LOW)
    {
      Serial.println("CLK signal set LOW");
      
#if TASK >= 4
      /* TODO4: Extract the bit from the dataSendBitIndex of the dataSend byte of data */
      bit = bitRead(dataSend, dataSendBitIndex);
      /* TODO4: Send the bit using the MOSI pin */
      digitalWrite(SPI_MOSI_PIN, bit);
#endif /* TASK >= 4 */
      dataSendBitIndex++;
    }

    if ((dataSendBitIndex == 8) && (dataRecvBitIndex == 8))
    {
      break;
    }
  }
  
  resetClock();
#else
  /* This delay will allow us to se the CS pin change before TASK 3 */
  delay(1000);
#endif /* TASK >= 3 */

  return dataRecv;
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello, I am SPI Master");

#if TASK >= 1
  /* TODO1: Configure SPI pins: INPUT/OUTPUT */
  pinMode(SPI_SCK_PIN,  OUTPUT);
  pinMode(SPI_MISO_PIN, INPUT);
  pinMode(SPI_MOSI_PIN, OUTPUT);
  pinMode(SPI_CS_PIN,   OUTPUT);

  /* TODO1: Set the output SPI pins to the idle values: LOW/HIGH */
  digitalWrite(SPI_SCK_PIN,  LOW);
  digitalWrite(SPI_MOSI_PIN, LOW);
  digitalWrite(SPI_CS_PIN,   HIGH);

  Serial.println("SPI Master pins are initialized");
#endif /* TASK >= 1 */

  delay(START_DELAY);
  Serial.println();
}

void loop(void)
{
  char dataRecv = '?';
  
#if TASK >= 2
  /* TODO2: Select the Slave device */
  digitalWrite(SPI_CS_PIN, LOW);
  Serial.println("Slave device selected");
#endif /* TASK >= 2 */

  /* Communicate over SPI */
  dataRecv = sendRecvByte(dataSend);
  
#if TASK >= 2
  /* TODO2: Unselect the Slave device */
  digitalWrite(SPI_CS_PIN, HIGH);
  Serial.println("Slave device unselected");
#endif /* TASK >= 2 */

#if TASK >= 4
  /* Print data */
  Serial.print("Sent:     ");
  Serial.println(dataSend);
  Serial.print("Recieved: ");
  Serial.println(dataRecv);
  
  /* Choose the new data to be sent */
  dataSend++;
  if (dataSend == (LAST_CHAR + 1))
  {
    dataSend = FIRST_CHAR;
  }
#endif /* TASK >= 4 */
  
#if TASK >= 2
  Serial.println();
#endif /* TASK >= 2 */
  
  delay(SEND_DELAY);
}
