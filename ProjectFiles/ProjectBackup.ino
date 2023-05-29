#include <SPI.h>
#include <SD.h>
#include "TMRpcm.h"

File myFile;
TMRpcm tmrpcm;

// C++ code
// 
int yes_button_pin = 3;
int no_button_pin = 2;
int LED_pin = 5;
int message_array[5] = {1,1,1,1,1};
int answer = 100;

bool yes_no_decision = false;

int message_index;

//States:
enum program_states{
	default_state,
    ask_to_read_state,
    read_state,
    ask_to_answer_state,
    answer_state,
    ask_to_send_state  
  };
  program_states state = default_state;


void setup()
{
  pinMode(LED_pin, OUTPUT);
  pinMode(yes_button_pin, INPUT_PULLUP);
  pinMode(no_button_pin, INPUT_PULLUP);
  Serial.begin(9600); 

  
  
  //Initialize SD
  initialize_SD();

  //Initialize Speaker
  tmrpcm.speakerPin=9;
  tmrpcm.setVolume(4);
  tmrpcm.quality(1); // Set the audio quality to 1 (default is 0)
  // Set the PWM frequency to 31.25 kHz on pin 9
  pinMode(9, OUTPUT);
  TCCR1B = (TCCR1B & 0b11111000) | 0x02; // Set timer 1 prescaler to 8
  OCR1A = 44; // Set the output compare register to 44

  //Test play'
  //Serial.print("Is playing:");
  //Serial.println(tmrpcm.isPlaying());
  //tmrpcm.play("test.WAV");
  //Serial.print("Is playing:");
  //Serial.println(tmrpcm.isPlaying());

  
}

void loop()
{
  //Light LED if there are any messages
  if(sizeof(message_array)/sizeof(message_array[0]) > 0){
    digitalWrite(LED_pin, HIGH);}
  else{
    digitalWrite(LED_pin, LOW);}
  
  //Read lightvalue in order to see if its open or closed
  int light_value = analogRead(A0); 
  //Serial.print("Light value is:");
  //Serial.println(light_value);
  
  
  
  //Serial.print("YesButtonPin: ");
  //Serial.println(digitalRead(yes_button_pin));
  //Serial.print("NoButtonPin: ");
  //Serial.println(digitalRead(no_button_pin));
  
  
  //Switch that checks for states
  if(light_value > 50){
      switch (state)
    {
    case default_state:
      Serial.println("I am in default_state");
      message_index = find_message_index();
      Serial.println(message_index);
      if (message_index > -1){
        
        switch(message_index){
          case 0: 
          play_system_message("FiveM.WAV");
          break;
          case 1: 
          play_system_message("FourM.WAV");
          break;
          case 2: 
          play_system_message("ThreeM.WAV");
          break;
          case 3: 
          play_system_message("TwoM.WAV");
          break;
          case 4: 
          play_system_message("OneM.WAV");
          break;
        }

        state = ask_to_read_state;
      }
      else{
        play_system_message("ZeroM.WAV");
      }

      break;

    case ask_to_read_state:
      Serial.println("I am in ask_to_read_state");
          message_index = find_message_index();
          
          play_system_message("Listen.WAV");
          Serial.println("Would you like to have the message read?");
          int answer = decision();
          if(answer == 1){
            state = read_state;
            }
            else{
            state = default_state;
            break;
            }


    case read_state:
      Serial.println("I am in read_state. BLABLABLA");
      delay(1000);
      message_index = find_message_index();
      switch(message_index){
          case 0: 
          play_system_message("Msg1.WAV");
          break;
          case 1: 
          play_system_message("Msg2.WAV");
          break;
          case 2: 
          play_system_message("Msg3.WAV");
          break;
          case 3: 
          play_system_message("Msg4.WAV");
          break;
          case 4: 
          play_system_message("Msg5.WAV");
          break;
        }

      delay(1000);
      state = ask_to_answer_state;

      
    
  
  	case ask_to_answer_state:
	    Serial.println("Would you like to answer the message?");
      play_system_message("Answer.WAV");
      answer = decision();
  	  if (answer == 1){
        state = answer_state;
      }
  	  else{
        state = default_state;
        break;
  	  }
        
    case answer_state:
      delay(1000);
      Serial.println("I am in answer_state. SAY: BLABLABLA");
      play_system_message("Record.WAV");
      delay(5000);
      state = ask_to_send_state;

    
 	  case ask_to_send_state:
  	  Serial.println("Would you like to send the message?");
      play_system_message("Send.WAV");
      answer = decision();
  	  if (answer == 1){
        Serial.println("Message sent");
        play_system_message("Sent.WAV");
        remove_message();
        state = default_state;

      }
  	  else{
        Serial.println("Message not sent");
        play_system_message("Cancel.WAV");
        state = default_state;
  	  }
      }
  }
  else{
    state = default_state;

  }

  delay(1000);
}

void remove_message(){
  for (int i = 0; i < 5; i++) {
  if (message_array[i] == 1) {
    message_array[i] = 0;
    Serial.println("Message removed");
    break;
  }
 }
}

int find_message_index() { //Searches the array and returns the index where a message is
  for (int i = 0; i < 5; i++) {
    if (message_array[i] == 1) {
      //Serial.println("Message in index:");
      //Serial.println(i);
      return i;
    }
  }
  //Serial.println("No message found");
  return -1;
}

int decision(){ //Function that waits for the player to press yes or no and returns whichever they pressed
	yes_no_decision = true;
	while (yes_no_decision == true){
		if((digitalRead(yes_button_pin)) == 1){
			Serial.println("YES");
            yes_no_decision = false;
          	return 1;
            }
        if((digitalRead(no_button_pin)) == 1){
            Serial.println("NO");
            yes_no_decision = false;
          	return 0;
            }
    }
}

void initialize_SD(){
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
}

void play_system_message(char* file_name){
  Serial.println(file_name);
  tmrpcm.play(file_name);
  while (tmrpcm.isPlaying() == 1){

  }
  return;
}
