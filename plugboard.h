#ifndef PLUGBOARD_H
#define PLUGBOARD_H

//define Plugboard class

class Plugboard
{
 private:
  int mapping[26];  //absolute mapping from input position (index) to output position (mapping[index])

 public:
  /*Plugboard class constructor. Used to initialise a plugboard object.
    Function reads information from the configuration file, throwing an integer (error_code)
    exception if an invalid configuration exists. Function sets up data member mapping.*/
  Plugboard(char** argv, int argc, int index_plugboard);

  /*Function takes an input integer passed by reference, relating to input character's absolute
    position, and converts it to an output integer, relating to absolute output position.*/
  void getOutput(int& input);
};

#endif
