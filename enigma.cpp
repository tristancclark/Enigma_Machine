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
      if (input[i] < 65 || input[i] > 90)
      {
	cerr << input[i] << " is not a valid input character (input characters must be upper case letters A-Z)!" << endl;
	exit(INVALID_INPUT_CHARACTER);
      }
      
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
  int index_rotor_position = getFileTypeIndex(argv, argc, ".pos");

  if (!index_plugboard || !index_reflector)
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  plugboard.initialisePlugboard(argv[index_plugboard]);
  reflector.initialiseReflector(argv[index_reflector]);

  ifstream in_stream;
  in_stream.open(argv[index_rotor_position]);
  for (int i = 0; i < number_of_rotors++; i++)
  {
    in_stream >> number;
    
    if (i == number_of_rotors)
    {
      if (!in_stream.eof())
      {
	cerr << "Too many parameters in rotor position file " << argv[index_rotor_position] << ", only " << number_of_rotors << " rotors exist." << endl;
	exit(TOO_MANY_PARAMETERS);
      }
      in_stream.close();
      return;
    }
    
    if (in_stream.eof())
    {
      cerr << "No starting position for rotor " << i << " in rotor position file " << argv[index_rotor_position] << endl;
      exit(NO_ROTOR_STARTING_POSITION);
    }
    if (in_stream.fail())
    {
      cerr << "Non-numerical character in rotor position file " << argv[index_rotor_position] << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
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
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }

  while(true)
  {
    loop_count++;
    
    in_stream >> pair[0]; //take in first integer

    if (in_stream.eof())
      break;

    if (loop_count == 14) //check for too many parameters
    {
      cerr << "Incorrect number of parameters in plugboard file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }
      
    if (in_stream.fail()) //check for valid input
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //take in second digit
    
    if (in_stream.eof()) //check for odd number of parameters
    {
      cerr << "Incorrect number of parameters in plugboard file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }
      
    if (in_stream.fail()) //check for invalid input
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    if (pair[0] == pair[1])
    {
      cerr << "Attempted connection of contact to itself in plugboard file " << config_file_name << endl;
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25)
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      exit(INVALID_INDEX);
    }
    if (swapped[pair[0]] == 1 || swapped[pair[1]] == 1)
    {
      cerr << "Attempted connection of a contact with more than one other contact in file " << config_file_name << endl;
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
      
    mapping[pair[0]] = pair[1]; //swap pairs
    mapping[pair[1]] = pair[0];
    swapped[pair[0]] = 1; //set contacts to already swapped
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
  int pair[2], loop_count = 0;
  bool swapped[26] = {};
  
  in_stream.open(config_file_name); //open file
  if (in_stream.fail()) 
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }
      
  while(true)
  {
    loop_count++;
    in_stream >> pair[0]; //import first digit and check
    
    if (in_stream.eof())
    {
      if (loop_count != 14)
      {
	cerr << "Insufficient number of mappings in reflector file " << config_file_name << endl;
	exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
      }
      in_stream.close();
      return;
    }

    if (loop_count == 14)
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail())
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //import second digit and check
    
    if (in_stream.eof())
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail())
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
      
    if (pair[0] == pair[1]) //check mapping digit to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file " << config_file_name << endl;
      exit(INVALID_REFLECTOR_MAPPING);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      exit(INVALID_INDEX);
    }
    
    if (swapped[pair[0]] == 1 || swapped[pair[1]] == 1) //check mapping same number to more than one other
    {
      cerr << "Attempted connection of a contact with more than one other contact in file " << config_file_name << endl;
      exit(INVALID_REFLECTOR_MAPPING);
    }
      
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
  int number, loop_count = 0;
  bool already_mapped[26] = {};
  
  relative_position = starting_position;

  in_stream.open(config_file_name); //open file

  if (in_stream.fail())
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }
  
  in_stream >> number;

  //get mappings
  for (int i = 0; i < 26; i++)
  {
    if (in_stream.eof())
    {
      cerr << "Not all inputs mapped in rotor file: " << config_file_name << endl;
      exit(INVALID_ROTOR_MAPPING);
    }
    
    if (in_stream.fail())
    {
      cerr << "Non-numeric character for mapping in rotor file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }

    if (number > 25 || number < 0)
    {
      cerr << "Parameter out of range in rotor file " << config_file_name << endl;
      exit(INVALID_INDEX);
    }

    if (already_mapped[number] == 1)
    {
      cerr << "Invalid mapping of input " << i << "to output " << number << " (output " << number << " is already mapped to from input " << endl;
      exit(INVALID_ROTOR_MAPPING);
    }
    for (int j = 0; j < 26; j++)
    {
      if (mapping[j] == number)
	cerr << j << ") in rotor file " << config_file_name << endl;
    }
    
    mapping[i] = number;
    already_mapped[number] = 1;
    in_stream >> number;
  }

  //get notch positions
  while (!in_stream.eof())
  {
    if (in_stream.fail())
    {
      cerr << "Non-numeric character for mapping in rotor file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }

    if (loop_count > 26)
    {
      cerr << "Too many input parameters for rotor file " << config_file_name << endl;
      exit(TOO_MANY_PARAMETERS);
    }
    
    notches[number] = 1;

    in_stream >> number;
  }
  
  in_stream.close();
}


void Rotor::getOutputForwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  if (relative_input > 25)
    relative_input -= 26;
  relative_output = mapping[relative_input];
  relative_output -= relative_position; 
  if (relative_output < 0)
    relative_output += 26;
  input = relative_output;
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
