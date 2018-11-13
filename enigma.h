#ifndef ENIGMA_H
#define ENIGMA_H

class Plugboard
{
  int mapping[26];
 public:
  Plugboard(char** argv, int argc);
  void getOutput(int& input);
};

class Reflector
{
  int mapping[26];
 public:
  Reflector(char** argv, int argc);
  void getOutput(int& input);
};

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

class Enigma
{
 private:
  Plugboard plugboard;
  Reflector reflector;
  Rotor** rotors_array;
  int number_of_rotors;
 public:
  Enigma(char** argv, int argc);
  ~Enigma();
  void runEnigma(int argc);
  void getFinalOutput(char& input_letter, int argc);
  // int getFileTypeIndex(char** argv, int argc, const char* extension);
  int getNumberOfRotors(char** argv, int argc);
};

int getFileTypeIndex(char** argv, int argc, const char* extension);

#endif
