#ifndef ENIGMA_H
#define ENIGMA_H

class Plugboard
{
 public:
  int mapping[26];
  
  void initialisePlugboard(char* config_file_name);
  void getOutput(int& input);
};

class Reflector
{
 public:
  int mapping[26];
  
  void initialiseReflector(char* config_file_name);
  void getOutput(int& input);
};

class Rotor
{
 public:
  int mapping[26];
  bool notches[26] = {0};
  int relative_position = 0;
  
  void initialiseRotor(char* config_file_name, int starting_position);
  void getOutputForwards(int& input);
  void getOutputForwardsWithComments(int& input);
  void getOutputBackwards(int& input);
  bool turnRotor();
};

void getTotalOutput(char& input_letter, int argc, Reflector reflector, Plugboard plugboard, Rotor* rotors, int number_of_rotors);
void getTotalOutputWithComments(char& input_letter, int argc, char** argv, Reflector reflector, Plugboard plugboard, Rotor* rotors);
void initialiseEnigma(Plugboard& plugboard, Reflector& reflector, Rotor* rotors, char** argv, int argc, int& number_of_rotors);
void displayEnigmaSetUp(Plugboard plugboard, Reflector reflector, Rotor* rotors, int argc, int number_of_rotors);
int getFileTypeIndex(char** argv, int argc, const char* extension);
int getNumberOfRotors(char** argv, int argc);
void runEnigma(Plugboard plugboard, Reflector reflector, Rotor* rotors, char** argv, int argc, int number_of_rotors);

#endif
