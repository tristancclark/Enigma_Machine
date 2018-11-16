#ifndef ENIGMA_H
#define ENIGMA_H

//define Plugboard class

class Plugboard
{
 private:
  int mapping[26];  //absolute mapping from input position (index) to output position (mapping[index])
  
 public:
  //Plugboard constuctor. Sets up plugboard if config files are valid
  Plugboard(char** argv, int argc);
  //takes input contact position by reference and converts it to corresponding output contact
  void getOutput(int& input);
  //returns position in command line arguments of file with '.pb' extension
  int getFileIndex(char** argv, int argc);
};

//define Reflector class

class Reflector
{
 private:
  int mapping[26];  //absolute mapping from input position (index) to output position (mapping[index])
  
 public:
  //Reflector constructor. Sets up reflector if config files are valid
  Reflector(char** argv, int argc);
  //takes input contact position by reference and converts it to corresponding output contact
  void getOutput(int& input);
  //returns position in command line arguments of file with '.rf' extension
  int getFileIndex(char** argv, int argc);
};

//define Rotor class

class Rotor
{
 private:
  int mapping[26]; //absolute mapping from input position (index) to output position (mapping[index])
  bool notches[26] = {}; //notches array, true if notch appears at that absolute position
  int relative_position = 0; //relative position to 12 o'clock
  
 public:
  //Rotor contructor. Sets up rotor if config files are valid
  Rotor(char* config_file_name, int starting_position);
  //takes input contact position by reference and converts it to corresponding output contact
  //specific to going 'forwards' through rotor
  void getOutputForwards(int& input);
  //specific to going 'backwards' through rotor
  void getOutputBackwards(int& input);
  //turns rotor anticlockwise once and returns true if a notch then appears at 12 o'clock position
  bool turnRotor();
};

//define Enigma class

class Enigma
{
 private:
  Plugboard plugboard;
  Reflector reflector;
  Rotor** rotors_array; //dynamic memory for rotors
  int number_of_rotors;
  
  //takes letter by reference and encrypts/decrypts it
  void getOutput(char& letter, int argc);
  //returns position in command line arguments of files with specified extension
  int getFileTypeIndex(char** argv, int argc, const char* extension);
  //returns the number of rotors present in command line arguments
  int getNumberOfRotors(char** argv, int argc);
  
 public:
  //Enigma constructor. Sets up entire Enigma machine if config files are valid
  Enigma(char** argv, int argc);
  //Enigma destructor
  ~Enigma();
  //Runs enigma machine. Takes input from standard input stream, encrypts/decrypts it and
  //outputs to standard output stream. As long as valid input (A-Z)
  void runEnigma(int argc);
};

#endif
