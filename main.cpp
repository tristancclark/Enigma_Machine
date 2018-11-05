#include<iostream>
#include<fstream>
#include<cstring>
#include"enigma.h"
#include"errors.h"

using namespace std;

int main(int argc, char** argv) {

  Rotor rotors[argc-3];
  Plugboard plugboard;
  Reflector reflector;
  char input[100];

  if (argc < 3) //check if number of parameters is sufficients
  {
    cerr << "Error: insufficient number of parameters." << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }
 
   initialiseEnigma(plugboard, reflector, rotors, argv, argc);

   showEnigmaSetUp(plugboard, reflector, rotors, argc);
   
   // int in = 4;
   // rotors[0].getOutputBackwards(in);
   // cout << in << endl;

     cout << "Please enter your encrypted message: " << endl;
  while(true){
    cin.getline(input, 100);
    // cout << input << " is your input with length " << strlen(input) <<  endl;
    cout << "The encrypted message reads: ";
    for (size_t i = 0; i < strlen(input); i++)
    {
      for (int j = 0; j < argc - 4; j++) //turn rotors usuing notches
      {
	if (!rotors[j].turnRotor())
	  break;
      }
      getTotalOutput(input[i], argc, reflector, plugboard, rotors);
    }
    cout << input;
    cout << endl;
  }
   
  


  
  
  /*  for (int j = 0; j < 4; j++){
    cout << "rotor " << j << " has mapping: " << endl;
    for (int i = 0; i < 26; i++)
      cout << rotors[j].mapping[i] << " ";
    cout << endl;
    }*/
  
  /*
   int number = 8;
  
  plugboard.getOutput(number);
  cout << "output is " << number << endl;
  */
  

  /*for (int i = 0; i < 26; i++)
    cout << rotor1.mapping[i] << " ";
  cout <<  endl;

  for (int i = 0; i < 26; i++)
    cout << rotor1.notches[i] << " ";
  cout <<  endl;

   number = 0;
  rotor1.getOutputForwards(number);
  cout << "output is " << number << endl;
  rotor1.turnRotor();
  rotor1.turnRotor();
  rotor1.turnRotor();
  number = 0;
  rotor1.getOutputForwards(number);
  cout << "output is " << number << endl;*/

  
  


  
  
  return 0;
}
