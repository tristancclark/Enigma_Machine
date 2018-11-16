#ifndef ROTOR_H
#define ROTOR_H

//define Rotor class

class Rotor
{
 private:
  int mapping[26]; //absolute mapping from input position (index) to output position (mapping[index])
  bool notches[26] = {}; //notches array, true if notch appears at that absolute position
  int relative_position = 0; //relative position to 12 o'clock
  
 public:
  /*Rotor class constructor. Used to initialise a rotor object.
    Function reads information from the configuration file, throwing an integer (error_code)
    exception if an invalid configuration exists. Argument starting_position relates to the
    starting position specific to that rotor extracted from .pos file previously.
    Function sets up data member mapping.*/
  Rotor(char* config_file_name, int starting_position);
  
  /*Function takes an input integer passed by reference, relating to input character's absolute
    position, and converts it to an output integer, relating to absolute output position.
    Function is specific to going 'forwards' through the rotor.*/
  void getOutputForwards(int& input);
  
  /*Function takes an input integer passed by reference, relating to input character's absolute
    position, and converts it to an output integer, relating to absolute output position.
    Function is specific to going 'backwards' through the rotor.*/
  void getOutputBackwards(int& input);
  
  /*Function that mimics turning the rotor anticlockwise by one increment by adding one to the
    relative position of that rotor.
    Function returns true if, after turning, a notch exists in absolute position 0 (12 o'clock).
    Function returns false otherwise.*/
  bool turnRotor();
};

#endif
