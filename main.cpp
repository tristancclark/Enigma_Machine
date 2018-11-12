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
  
  runEnigma(plugboard, reflector, rotors, argv, argc, number_of_rotors);
  
  return 0;
}
