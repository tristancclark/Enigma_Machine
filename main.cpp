#include<iostream>
#include<fstream>
#include<cstring>
#include"enigma.h"
#include"errors.h"

using namespace std;

int main(int argc, char** argv) {
  
  //  int number_of_rotors = getNumberOfRotors(argv, argc);
  //Rotor rotors[number_of_rotors];
  //Plugboard plugboard;
  //Reflector reflector;

  Enigma enigma(argv, argc);
    
  // initialiseEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  
  enigma.runEnigma(argc);
  
  return 0;
}
