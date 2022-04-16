/// El programa incluye:
//        *   Control de servomotores por medio del Myo.
//        *   Control de prótesis por medio de FSR de manera analógica.
//        *   Modo de bloqueo de prótesis.
//        *   Cambio de modo.
//        *   LCD indicador de conexion, cambio de modo, bloqueo y posicion.
//        *   Se debe realizar dos veces cada gesto para activar la mano (para evitar falsos positivos).
//        *   Se agrego un pulsador, para prender y apagar el LCD cuando se le plazca.


// Librerías:
 
#include <Servo.h>
#include <MyoBridge.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Se inicializan las constantes, las variables y los objetos:
 
  Servo ServoMenique;           // Meñique
  Servo ServoAnular;            // Anular
  Servo ServoMayor;             // Mayor
  Servo ServoIndice;            // Índice
  Servo ServoPulgar;            // Pulgar
  int Pos=4;                    // Bandera para evitar ir dos veces a la misma posicion
  int PosSeg=4;                 // Bandera para evitar falsos positivos
  int grado=0;
  int gradopulgar = 0;          // Variable para mover pulgar cuando se mide con los FSR
  int gradoindice = 0;          // Variable para mover indice cuando se mide con los FSR
  int gradomayor = 0;           // Variable para mover mayor cuando se mide con los FSR
  int gradoprension = 0;        // Variable para evitar que los motores se pasen de 10° cuando se mide con los FSR
  const int Bloqueo = 8;        // Pin para pulsador de bloqueo.
  const int FSR = 12;           // Pin para FSR.
  const int Modo = 13;          // Pin para pulsador de cambio de modo.
  const int LCD = 7;            // Pin para encendido y apagado de LCD.
  int FSRPulgar = 0;            // Pin analogico de lectura de FSR en dedo pulgar
  int FSRIndice = 0;            // Pin analogico de lectura de FSR en dedo pulgar
  int FSRMayor = 0;             // Pin analogico de lectura de FSR en dedo pulgar
  int BloqueoFlag = 0;
  int ModoFlag = 0;
  int LCDflag = 0;
  int flagpulgar = 0;           // Variable para determinar que el FSR del pulgar esta sensando
  int flagindice = 0;           // Variable para determinar que el FSR del indice esta sensando
  int flagmayor = 0;            // Variable para determinar que el FSR del mayor esta sensando
  int flagFSR = 0;              // Variable para determinar que todos los FSR estan sensando
 

//Utilizamos los pines 2 y 4 como Tx y Rx
SoftwareSerial bridgeSerial(2,4);

//Inicializamos el objeto Myobridge con la conexion software serial
MyoBridge bridge(bridgeSerial);

//Inicializamos la pantalla lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);


//Declaramos una funcion para manejar datos de gestos
void handlePoseData(MyoPoseData& data) {
  
  //Convertimos datos de gestos a MyoPose
  MyoPose pose;
  pose = (MyoPose)data.pose;

  //Imprimimos el gesto y la pasamos a string
  Serial.println(bridge.poseToString(pose));
  String posa=bridge.poseToString(pose);

  if (ModoFlag==0)
  {
      lcd.setCursor(0,0);
      lcd.print("Armyo modo: Ppal");

 ///////////////////////////////////////////////////////////----------------TIPEO-----------//////////////////////////////////////////

  if(posa.equals("MYO_POSE_WAVE_OUT") && Pos!=0) //Tipeo
  {
    if(PosSeg==0)
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Tipeo");
    if (Pos==1)          // Si la mano esta en posicion de prension fina, solo muevo los dedos necesarios para ir a tipeo.
    {
      ServoMenique.write(10);
      ServoAnular.write(10);
      ServoMayor.write(10);
      ServoIndice.write(180);
    }
    else if (Pos==2)        // Si la mano esta abierta, solo muevo los dedos necesarios para ir a tipeo.
    {
      ServoMenique.write(10);
      ServoAnular.write(10);
      ServoMayor.write(10);
      ServoPulgar.write(10);
    }
    else if (Pos==3)        // Si la mano esta cerrada, solo muevo los dedos necesarios para ir a tipeo.
    {
      ServoIndice.write(180);
    }
    else if (Pos==4)        // Es la posicion de arranque de la mano, posicion neutra.
    {
      ServoMenique.write(10);
      ServoAnular.write(10);
      ServoMayor.write(10);
      ServoIndice.write(180);
      ServoPulgar.write(10);
    }
    Pos=0;  
    }
    else if(PosSeg!=0)
    {
      PosSeg=0;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Tipeo...");
    }
    
  }
  
////////////////////////////////////////////////////////////-------------PRENSION FINA---------///////////////////////////////////////////777
  
  if(posa.equals("MYO_POSE_WAVE_IN")  && Pos!=1) //Prension
  {
    if(PosSeg==1)
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Prension fina");
      
      ServoMenique.write(180);
      ServoAnular.write(180);
      ServoMayor.write(180);
      ServoIndice.write(180);
      ServoPulgar.write(180);
      
      gradopulgar = 180;
      gradoindice = 180;
      gradomayor = 180;
      gradoprension = 180;
      flagFSR = 0;
      
      while (flagFSR<3)
      {
        FSRPulgar= analogRead(0);
        FSRIndice = analogRead(1);
        FSRMayor = analogRead(2);
                
        if(FSRPulgar<300)
        {
          gradopulgar = gradopulgar - 10;
          ServoPulgar.write(gradopulgar);
          flagpulgar = 0;

        }
        else if(FSRPulgar>300)
        {
          flagpulgar=1;
        }
        
        if(FSRIndice<300)
        {
          gradoindice = gradoindice - 10;
          ServoIndice.write(gradoindice);
          flagindice = 0;        
        }
        else if(FSRIndice>300)
        {
          flagindice=1;
        }
        
        if(FSRMayor<300)
        {
          gradomayor = gradomayor - 10;
          ServoMayor.write(gradomayor);
          flagmayor = 0;       
        }
        else if(FSRMayor>300)
        {
          flagmayor = 1;
        }     
          flagFSR = flagpulgar + flagmayor + flagindice; 
          gradoprension = gradoprension - 10;
                   
          if (gradoprension == 20)
          {
            flagFSR=4;               
          }
          delay (100);
       }
      
      bridge.vibrate(1);
      Pos=1;
    }
    else if(PosSeg!=1)
    {
      PosSeg=1;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Prension fina...");      
    }
  }

//////////////////////////////////////////////////////////-----------------MANO ABIERTA--------------------------///////////////////////////////////////////////////////////////////////
  
  if(posa.equals("MYO_POSE_FINGERS_SPREAD")  && Pos!=2)  //Mano Abierta
  {
      if (PosSeg==2)
      {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Mano Abierta"); 
         
    for(grado=10;grado<=180;grado=grado+10)
    {
      ServoMenique.write(grado);
      ServoAnular.write(grado);
      ServoMayor.write(grado);
      ServoIndice.write(grado);
      ServoPulgar.write(grado);
    }
    Pos=2;
      }
    else if(PosSeg!=2)
    {
      PosSeg=2;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Mano Abierta...");
    }
  }

/////////////////////////////////////////////////////7/------------------MANO CERRADA-------------------/////////////////////////////////////////////////////7
  
  if(posa.equals("MYO_POSE_FIST") && Pos!=3)  //Mano Cerrada      BAJAMOS GRADUALMENTE PARA EL SENSADO
  {  
    if(PosSeg==3)
    {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Prension");
          
      ServoMenique.write(180);        // Abro completamente la mano previo a cerrarla.
      ServoAnular.write(180);         // Esto lo hago ya que para agarrar algo, debo partir desde la mano abierta.
      ServoMayor.write(180);
      ServoIndice.write(180);
      ServoPulgar.write(180);

      gradopulgar = 180;
      gradoindice = 180;
      gradomayor = 180;
      gradoprension = 180;
      flagFSR = 0;
      
      while (flagFSR<3)
      {
        FSRPulgar= analogRead(0);
        FSRIndice = analogRead(1);
        FSRMayor = analogRead(2);
                
        if(FSRPulgar<300)
        {
          gradopulgar = gradopulgar - 10;
          ServoPulgar.write(gradopulgar);
          flagpulgar = 0;

        }
        else if(FSRPulgar>300)
        {
          flagpulgar=1;
        }
        
        if(FSRIndice<300)
        {
          gradoindice = gradoindice - 10;
          ServoIndice.write(gradoindice);
          flagindice = 0;        
        }
        else if(FSRIndice>300)
        {
          flagindice=1;
        }
        
        if(FSRMayor<300)
        {
          gradomayor = gradomayor - 10;
          ServoMayor.write(gradomayor);
          ServoAnular.write(gradomayor);
          ServoMenique.write(gradomayor); 
          flagmayor = 0;       
        }
        else if(FSRMayor>300)
        {
          flagmayor = 1;
        }     
          flagFSR = flagpulgar + flagmayor + flagindice; 
          gradoprension = gradoprension - 10;
                   
          if (gradoprension == 20)
          {
            flagFSR=4;
            ServoMenique.write(10);              
          }
          delay(100);
       }
    Pos=3;
    }
    else if(PosSeg!=3)
    {
      PosSeg=3;  
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Prension...");    
    }
  }
}

  else if (ModoFlag==1)
  {
      lcd.setCursor(0,0);
      lcd.print("Armyo modo: Sign");
/////////////////////////7-----------------------------------Cuernitos---------------------------------////////////////////////////7

    if(posa.equals("MYO_POSE_WAVE_OUT") && Pos!=0) //Cuernitos
    {
      if(PosSeg==0)
      {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Cuernitos");
      
      ServoMenique.write(180);
      ServoAnular.write(10);
      ServoMayor.write(10);
      ServoIndice.write(180);
      ServoPulgar.write(10);
      Pos=0;
      }
      else if(PosSeg!=0)
      {
        PosSeg=0;
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Cuernitos...");
      }
    }

///////////////////////////////////////////////////////---------------------Fuck You--------------------------------///////////////////////////////////7

    if(posa.equals("MYO_POSE_WAVE_IN")  && Pos!=1) //Fuck You
    {
      if(PosSeg==1)
      {
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Fuck you!");
            
      ServoMenique.write(10);
      ServoAnular.write(10);
      ServoMayor.write(180);
      ServoIndice.write(10);
      ServoPulgar.write(10);
      bridge.vibrate(1);
      Pos=1;
    }
    
    else if(PosSeg!=1)
    {
      PosSeg=1;
      lcd.setCursor(0,1);
      lcd.print("                ");
      lcd.setCursor(0,1);
      lcd.print("Fuck you!...");
    }
  }
}
}
void setup() {
    
   lcd.begin(); // initialize the LCD
   lcd.backlight(); // Turn on the blacklight and print a message.
   
   ServoMenique.attach(10);     //Agregamos los servos al arduino como salidas
   ServoAnular.attach(9);
   ServoMayor.attach(6);
   ServoIndice.attach(5);
   ServoPulgar.attach(3);
   pinMode(Bloqueo,INPUT);
   pinMode(Modo,INPUT);
   pinMode(FSR,INPUT);
   pinMode(LCD,INPUT);
  //Inicializamos las dos conexiones serial
  Serial.begin(115200);
  bridgeSerial.begin(115200);

  //Esperamos hasta que MyoBridge encuentre al Myo y se conecte
  lcd.print("Buscando a Myo...");
  bridge.begin();
  lcd.setCursor(0,1);
  lcd.print("Conectado!");

  //Seteamos la funcion que maneja los eventos de gestos
  bridge.setPoseEventCallBack(handlePoseData);
  //Le decimos al Myo que queremos datos de gestos
  bridge.enablePoseData();
  //Nos aseguramos que el Myo se encuentre desbloqueado
  bridge.unlockMyo();
  //Es necesario realizar el gesto de sincronizacion para obtener datos de gestos
  
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print("Armyo esperando");
  bridge.vibrate(2);
}

void loop() {
  if(digitalRead(LCD)==HIGH) //Pulsador bloqueo
  {
    delay(200);
    if (LCDflag==0) //Se utiliza una bandera y un led para saber si esta "bloqueado" o no
    {
      LCDflag=1;
      lcd.noBacklight();
      }
    else if (LCDflag==1)
    {
      LCDflag=0;
      lcd.backlight();
      }
  }
  
  if(digitalRead(Bloqueo)==HIGH) //Pulsador bloqueo
  {
    delay(200);
    if (BloqueoFlag==0) //Se utiliza una bandera y un led para saber si esta "bloqueado" o no
    {
      BloqueoFlag=1;
      lcd.clear();
      lcd.print("ArMyo Bloqueado");
      }
    else if (BloqueoFlag==1)
    {
      BloqueoFlag=0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ArMyo listo!");
      }
  }
  
  if(digitalRead(Modo)==HIGH) //Pulsador de cambio de modo
  {
    delay(200);
    if (ModoFlag==0) //Se utiliza una bandera y un led para saber si esta "bloqueado" o no
    {
      ModoFlag=1;
      lcd.clear();
      lcd.print("ArMyo Modo: Sign");
      }
    else if (ModoFlag==1)
    {
      ModoFlag=0;
      lcd.clear();
      lcd.print("ArMyo Modo: Ppal");
      }
   }

  if (BloqueoFlag==0) //El "bloqueo" consiste en que la conexion BT no se actualice, el Myo sigue sensando
  {
  //Actualizamos la conexion con el Myo
  bridge.update();
  }
  }
