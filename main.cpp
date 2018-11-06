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
  char input[10000];
  int number_of_rotors;

  if (argc < 3) //check if number of parameters is sufficient
  {
    cerr << "Error: insufficient number of parameters." << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  initialiseEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  //displayEnigmaSetUp(plugboard, reflector, rotors, argc, number_of_rotors); //uncomment to check enigma set up.
  
  cin >> input;
  cin >> ws;
  while(!cin.eof()){
    for (size_t i = 0; i < strlen(input); i++)
    {
      for (int j = 0; j < argc - 4; j++) //turn rotors usuing notches
      {
	if (!rotors[argc - 5 - j].turnRotor())
	  break;
      }
      getTotalOutput(input[i], argc, reflector, plugboard, rotors, number_of_rotors);
    }
    cout << input;
    cin >> input;
  }
  cout << endl;
  return 0;
}
