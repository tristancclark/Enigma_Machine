#ifndef ENIGMA_H
#define ENIGMA_H

//define Reflector class

class Plugboard
{
  int mapping[26];
 public:
  Plugboard(char** argv, int argc);
  void getOutput(int& input);
  int getFileIndex(char** argv, int argc);
};

//define Reflector class

class Reflector
{
  int mapping[26];
 public:
  Reflector(char** argv, int argc);
  void getOutput(int& input);
  int getFileIndex(char** argv, int argc);
};

//define Rotor class

class Rotor
{
 private:
  int mapping[26] = {};
  bool notches[26] = {};
  int relative_position = 0;
  
 public:
  Rotor(char* config_file_name, int starting_position);
  void getOutputForwards(int& input);
  void getOutputBackwards(int& input);
  bool turnRotor();
};

//define Enigma class

class Enigma
{
 private:
  Plugboard plugboard;
  Reflector reflector;
  Rotor** rotors_array; //dynamic array as don't know number of rotors
  int number_of_rotors;
  void getFinalOutput(char& input_letter, int argc);
  int getFileTypeIndex(char** argv, int argc, const char* extension);
  int getNumberOfRotors(char** argv, int argc);
  
 public:
  Enigma(char** argv, int argc);
  ~Enigma();
  void runEnigma(int argc);
};

#endif
