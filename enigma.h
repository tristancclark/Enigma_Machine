#ifndef ENIGMA_H
#define ENIGMA_H

class Plugboard
{
  int mapping[26];
 public:
  void initialisePlugboard(char* config_file_name);
  void getOutput(int& input);
};

class Reflector
{
  int mapping[26];
 public:
  void initialiseReflector(char* config_file_name);
  void getOutput(int& input);
};

class Rotor
{
  int mapping[26] = {};
  bool notches[26] = {};
  int relative_position = 0;
 public:
  void initialiseRotor(char* config_file_name, int starting_position);
  void getOutputForwards(int& input);
  void getOutputBackwards(int& input);
  bool turnRotor();
};

void getFinalOutput(char& input_letter, int argc, Reflector reflector, Plugboard plugboard, Rotor* rotors, int number_of_rotors);
void initialiseEnigma(Plugboard& plugboard, Reflector& reflector, Rotor* rotors, char** argv, int argc, int number_of_rotors);
void displayEnigmaSetUp(Plugboard plugboard, Reflector reflector, Rotor* rotors, int argc, int number_of_rotors);
int getFileTypeIndex(char** argv, int argc, const char* extension);
int getNumberOfRotors(char** argv, int argc);
void runEnigma(Plugboard plugboard, Reflector reflector, Rotor* rotors, char** argv, int argc, int number_of_rotors);

#endif
