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
  int number_of_rotors;

  initialiseEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  
  displayEnigmaSetUp(plugboard, reflector, rotors, argc, number_of_rotors);
  //uncomment to check enigma set up.
  
  runEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  

  return 0;
}
