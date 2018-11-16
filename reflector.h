#ifndef REFLECTOR_H
#define REFLECTOR_H

//define Reflector class

class Reflector
{
 private:
  int mapping[26];  //absolute mapping from input position (index) to output position (mapping[index])

 public:
  /*Reflector class constructor. Used to initialise a reflector object.
    Function reads information from the configuration file, throwing an integer (error_code)
    exception if an invalid configuration exists. Function sets up data member mapping.*/
  Reflector(char** argv, int argc, int index_reflector);

  /*Function takes an input integer passed by reference, relating to input character's absolute
    position, and converts it to an output integer, relating to absolute output position.*/
  void getOutput(int& input);

  /*Function returns the index of where the file with the extension .rf appears in the
    command line arguments*/
  int getFileIndex(char** argv, int argc);
};

#endif
