#ifndef ENIGMA_H
#define ENIGMA_H

#include "plugboard.h"
#include "reflector.h"
#include "rotor.h"
//define Enigma class

class Enigma
{
 private:
  Plugboard plugboard;
  Reflector reflector;
  Rotor** rotors_array = nullptr; //dynamic memory for rotors
  int number_of_rotors;

  /*Function takes an input character by reference and encripts/decripts it by passing it through
    each element of the enigma machine.*/
  void getOutput(char& letter, int argc);

  /*Function returns the index of the file where the specified extension appears in the
    command line arguments*/
  int getFileTypeIndex(char** argv, int argc, const char* extension);

  /*Function returns the number of rotors present but counting the number of
    .rot files in the command line arguments*/
  int getNumberOfRotors(char** argv, int argc);

 public:
  /*Enigma class constructor. Used to intialise enigma object.
    Function reads information from configuration files and throws an integer (error code)
    exception if an invalid configuration exists. A plugboard and reflector are automatically
    created on declaration as data members but this constructor is used to count number of rotors
    and create 0-many rotor objects on the heap. */
  Enigma(char** argv, int argc);

  //Enigma class destructor
  ~Enigma();

  /*Function that runs the enigma machine.
    This function assumes that plugboard, reflector and 0 to many rotors are set up previously.
    Function reads in inputs from the standard input stream (only characters between A-Z),
    throws exeptions if the input is invalid, encrypts/decrypts the characters and outputs
    them to the standard output stream.*/
  void runEnigma(int argc);
};

#endif
