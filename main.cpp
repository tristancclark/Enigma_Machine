#include<iostream>
#include<fstream>
#include<cstring>
#include"enigma.h"
#include"errors.h"

using namespace std;

int main(int argc, char** argv) {
  

  int number_of_rotors = getNumberOfRotors(argv, argc);
  Rotor rotors[number_of_rotors];
  Plugboard plugboard;
  Reflector reflector;

  initialiseEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  
  //displayEnigmaSetUp(plugboard, reflector, rotors, argc, number_of_rotors);
  //uncomment to check enigma set up.
  
  runEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  

  return 0;
}
