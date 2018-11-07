#include<iostream>
#include<fstream>
#include<string.h>
#include"enigma.h"
#include"errors.h"

using namespace std;

//plugboard functions
void runEnigma(Plugboard plugboard, Reflector reflector, Rotor* rotors, char** argv, int argc, int number_of_rotors)
{
  char input[10000];

  cin >> ws;
  cin >> input;
  while(!cin.eof()){
    for (size_t i = 0; i < strlen(input); i++)
    {
      for (int j = 0; j < number_of_rotors; j++) //turn rotors using notches
      {
	if (!rotors[number_of_rotors - 1 - j].turnRotor())
	  break;
      }
      getTotalOutput(input[i], argc, reflector, plugboard, rotors, number_of_rotors);
    }
    cout << input;
    cin >> input;
  }
}

void initialiseEnigma(Plugboard& plugboard, Reflector& reflector, Rotor* rotors, char** argv, int argc, int& number_of_rotors)
{
  int number;
  number_of_rotors = getNumberOfRotors(argv, argc);
  int index_first_rotor = getFileTypeIndex(argv, argc, ".rot");
  int index_plugboard = getFileTypeIndex(argv, argc, ".pb");
  int index_reflector = getFileTypeIndex(argv, argc, ".rf");

  if (!index_plugboard || !index_reflector)
  {
    cerr << "Error: insufficient number of parameters." << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  plugboard.initialisePlugboard(argv[index_plugboard]);
  reflector.initialiseReflector(argv[index_reflector]);

  ifstream in_stream;
  in_stream.open(argv[getFileTypeIndex(argv, argc, ".pos")]);
  for (int i = 0; i < number_of_rotors; i++)
  {
    in_stream >> number;
    rotors[i].initialiseRotor(argv[i + index_first_rotor], number);
  }
}

int getNumberOfRotors(char** argv, int argc)
{
  int count = 0;
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".rot"))
      count++;
  }
  return count;
}

int getFileTypeIndex(char** argv, int argc, const char* extension)
{
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], extension))
      return i;
  }
  return 0;
}

void displayEnigmaSetUp(Plugboard plugboard, Reflector reflector, Rotor* rotors, int argc, int number_of_rotors)
{
  cout << "Plugboard mapping:  ";
  for (int i = 0; i < 26; i++)
    cout << plugboard.mapping[i] << " ";
  cout << endl << endl;

  cout << number_of_rotors << " rotors are present." << endl << endl;

  for (int i = 0; i < argc - 4; i++)
  {
    cout << "Rotor " << i+1 << " mapping:  ";
    for (int j = 0; j < 26; j++)
      cout << rotors[i].mapping[j] << " ";
    cout << endl;
    cout << "Relative shift: " << rotors[i].relative_position << endl;
    cout << "Notches occur at: ";
    for (int j = 0; j < 26; j++)
      if(rotors[i].notches[j] == 1)
	cout << j << " ";
    cout << endl << endl;
  }
  
  cout << "Reflector mapping:  ";
  for (int i = 0; i < 26; i++)
    cout << reflector.mapping[i] << " ";
  cout << endl;
 }  

      
void getTotalOutput(char& input_letter, int argc, Reflector reflector, Plugboard plugboard, Rotor* rotors, int number_of_rotors)
{
  int input = input_letter - 65;
  plugboard.getOutput(input);
  
  
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors[number_of_rotors-1-i].getOutputForwards(input);
  }

  reflector.getOutput(input);
  
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors[i].getOutputBackwards(input);
  }

  plugboard.getOutput(input);
  
  input_letter = input + 65;
}

//PLUGBOARD FUNCTIONS

void Plugboard::initialisePlugboard(char* config_file_name)
{
  ifstream in_stream;
  int pair[2], loop_count = 0;
  bool swapped[26] = {};
  
  for (int i = 0; i < 26; i++) //initialise as one to one mapping
    mapping[i] = i;

  in_stream.open(config_file_name);//open file

  if (in_stream.fail()) //check for failure opening file
  {
    cerr << "Error: file could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }

  while(true)
  {
    loop_count++;
    
    in_stream >> pair[0]; //take in first integer

    if (in_stream.eof())
      break;

    if (loop_count == 13) //check for too many parameters
    {
      cout << "Incorrect number of parameters in plugboard file " << config_file_name;
      exit(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }
      
    if (in_stream.fail()) //check for valid input
    {
      cout << "Non-numeric character in plugboard file " << config_file_name;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //take in second digit
    
    if (in_stream.eof()) //check for odd number of parameters
    {
      cout << "Incorrect number of parameters in plugboard file " << config_file_name;
      exit(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }
      
    if (in_stream.fail()) //check for invalid input
    {
      cout << "Non-numeric character in plugboard file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    if (pair[0] == pair[1])
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION); 
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25)
      exit(INVALID_INDEX);
    if (swapped[pair[0]] == 1 || swapped[pair[1]] == 1)
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION); 
      
    mapping[pair[0]] = pair[1]; //swap pairs
    mapping[pair[1]] = pair[0];
    swapped[pair[0]] = 1; //set numbers to swapped
    swapped[pair[1]] = 1;
  }
  in_stream.close();
}

void Plugboard::getOutput(int& input)
{
  input = mapping[input];
}

//Reflector functions

void Reflector::initialiseReflector(char* config_file_name)
{
  ifstream in_stream;
  int pair[2];
  bool swapped[26] = {};
  
  in_stream.open(config_file_name); //open file
  if (in_stream.fail()) 
  {
    cerr << "Error: file could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }
      
  for (int i = 0; i < 14; i++)
  {
    in_stream >> pair[0]; //import first digit and check
    if (i == 13) //check for too many inputs
    {
      if (in_stream.eof())
	break;
      else
	exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if ((in_stream.eof())) {  //check first digit is valid
      cout << "fail at loop " << i;
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);}
        if (in_stream.fail()){
      cout << "fail 1 at loop " << i;
      exit(NON_NUMERIC_CHARACTER);}
    
    in_stream >> pair[1]; //import second digit and check
    if (in_stream.eof())
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);   
    if (in_stream.fail()){
      cout << "fail 2 at loop " << i;
      exit(NON_NUMERIC_CHARACTER);}
      
    
    if (pair[0] == pair[1]) //check mapping digit to itself
      exit(INVALID_REFLECTOR_MAPPING);
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range
      exit(INVALID_INDEX);
    
    if (swapped[pair[0]] == 1 || swapped[pair[1]] == 1) //check mapping same number to more than one other
      exit(INVALID_REFLECTOR_MAPPING); 
      
    mapping[pair[0]] = pair[1]; //swap pairs
    mapping[pair[1]] = pair[0];
    swapped[pair[0]] = 1; //set numbers to swapped
    swapped[pair[1]] = 1;
  }
  in_stream.close();
}

void Reflector::getOutput(int& input)
{
  input = mapping[input];
  return;
}

//rotor functions

void Rotor::initialiseRotor(char* config_file_name, int starting_position)
{
  ifstream in_stream;
  int number;
  
  relative_position = starting_position;

  in_stream.open(config_file_name); //open file
  in_stream >> number;

  //get mappings
  for (int i = 0; i < 26; i++)
  {
    mapping[i] = number;
    in_stream >> number;
  }

  //get notch positions
  while (!in_stream.fail())
  {
    notches[number] = 1;
    in_stream >> number;
  }
}


void Rotor::getOutputForwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  if (relative_input > 25)
    relative_input -= 26;
  relative_output = mapping[relative_input];
  relative_output -= relative_position; //for anticlockwise
  if (relative_output < 0)
    relative_output += 26;
  input = relative_output;
}

void Rotor::getOutputForwardsWithComments(int& input)
{
  cout << "relative position:  " << relative_position << endl;
  cout << input << "(input) > ";
  int relative_output, relative_input = input + relative_position;
  cout << relative_input << "(relative input) > ";
  if (relative_input > 25)
    relative_input -= 26;
  cout << relative_input << "(relative input checked) > ";
  relative_output = mapping[relative_input];
  cout << relative_output << "(relative output) > ";
  relative_output -= relative_position; //for anticlockwise
  cout << relative_output << "(output) > ";
  if (relative_output < 0)
    relative_output += 26;
  cout << relative_output << "(output checked) > ";
  input = relative_output;
  cout << input << endl; 
}

void Rotor::getOutputBackwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  if (relative_input > 25)
    relative_input -= 26;
  for (int i = 0; i < 26; i++)
  {
    if (mapping[i] == relative_input)
    {
      relative_output = i - relative_position;
      if (relative_output < 0)
	relative_output += 26;
      input = relative_output;
      return;
    }
  }
}

bool Rotor::turnRotor()
{
  relative_position++;
  if (relative_position == 26)
    relative_position = 0;
  if (notches[relative_position] == 1)
    return true;
  return false;
}
