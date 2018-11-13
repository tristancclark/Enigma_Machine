#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"enigma.h"
#include"errors.h"

using namespace std;

//THIS SECTION CONTAINS GENERAL FUNCTIONS SPECIFIC TO THE ENIGMA MACHINE AS A WHOLE.

/*Function that runs the enigma machine. This function assumes that plugboard, rotors and reflector
are all set up previously. Function reads in inputs from the standard input stream (only characters
between A-Z) and after encrypting/decrypting them outputs them to the standard output stream.*/

void Enigma::runEnigma(int argc)
{
  char input_letter;

  cin >> ws;
  cin >> input_letter;
  
  while(!cin.eof())
  {
    if (input_letter < 65 || input_letter > 90) //check for invalid input character
    {
      cerr << input_letter << " is not a valid input character (input characters must be upper case letters A-Z)!" << endl;
      exit(INVALID_INPUT_CHARACTER);
    }
      
    for (int j = 0; j < number_of_rotors; j++) //turn far right rotor and cascade turning if a notch aligns
    {
      if (!rotors_array[number_of_rotors - 1 - j]->turnRotor())
	break;
    }
    
    getFinalOutput(input_letter, argc);
    cout << input_letter; //output encrypted/decrypted character
    cin >> input_letter;
  }
}

/*Function used to intialise enigma machine. Function reads in information from configuration files
and exits if an invalid configuration exists. Function sets up 'mapping' arrays in each object,
the location of notches in the rotors and the starting positions of each rotor. */

Enigma::Enigma(char** argv, int argc) : plugboard(argv, argc), reflector(argv, argc)
{
  int number;
  int index_first_rotor = getFileTypeIndex(argv, argc, ".rot");
  int index_rotor_position = getFileTypeIndex(argv, argc, ".pos");
  number_of_rotors = getNumberOfRotors(argv, argc);
  rotors_array = new Rotor*[number_of_rotors];
   
  //Rest of function extracts rotor positions, checks for errors and passes them to the Rotor initialise rotor member function
  
  if (index_rotor_position) //check if .pos if exists then initialise rotors
  {
    if (index_first_rotor == 0) //check if .rot file exists
    {
      cerr << "Position files exists but no rotor files." << endl;
      exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
    }
    ifstream in_stream;
    in_stream.open(argv[index_rotor_position]); //open .pos file

    if (in_stream.fail()) //check for failure opening file
    {
      cerr << "Error: file " << argv[index_rotor_position] << " could not be opened." << endl;
      exit(ERROR_OPENING_CONFIGURATION_FILE);
    }
    
    for (int i = 0; i < number_of_rotors + 1; i++)
    {
      in_stream >> number;
    
      if (i == number_of_rotors)
      {
	if (!in_stream.eof()) //check if all rotor positions have been set but eof flag has not been set
	{
	  if (in_stream.bad()) //check for badbit
	  {
	    cerr << "Error reading file " << argv[index_rotor_position] << endl;
	    exit(ERROR_OPENING_CONFIGURATION_FILE);
	  }
	  if (in_stream.fail()) //check for failbit
	  {
	    cerr << "Non-numeric character in rotor positions file " << argv[index_rotor_position] << endl;
	    exit(NON_NUMERIC_CHARACTER);
	  }
	  cerr << "Too many parameters in rotor position file " << argv[index_rotor_position] << ", only " << number_of_rotors << " rotors exist." << endl;
	  exit(TOO_MANY_PARAMETERS);
	}
	in_stream.close(); 
	return; //return if all rotor positions set and eof flag has been set
      }
    
      if (in_stream.eof()) //check if eof flag has been set and not all rotors positions have been set
      {
	cerr << "No starting position for rotor " << i << " in rotor position file: " << argv[index_rotor_position] << endl;
	exit(NO_ROTOR_STARTING_POSITION);
      }

      if (in_stream.bad()) //check for badbit
      {
	cerr << "Error reading file " << argv[index_rotor_position] << endl;
	exit(ERROR_OPENING_CONFIGURATION_FILE);
      }
      if (in_stream.fail()) //check for failbit
      {
	cerr << "Non-numeric character in rotor positions file " << argv[index_rotor_position] << endl;
	exit(NON_NUMERIC_CHARACTER);
      }

      rotors_array[i] = new Rotor(argv[i + index_first_rotor], number);
    }
  }
}

Enigma::~Enigma()
{
  for (int i = 0; i < number_of_rotors; i++)
  {
    delete rotors_array[i];
  }
  delete [] rotors_array;
}



/*Function returns the number of rotors present in the enigma set up but counting the number of
.rot files in the command line arguments*/

int Enigma::getNumberOfRotors(char** argv, int argc)
{
  int count = 0;
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".rot"))
      count++;
  }
  return count;
}

/*Function returns the index of the file with the specified extension appears in the
command line arguments*/

int Enigma::getFileTypeIndex(char** argv, int argc, const char* extension)
{
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], extension))
      return i;
  }
  return 0;
}

/*Function that takes an input character by reference and passes it through the entire enigma machine and changes
it to the encripted/decripted character*/

void Enigma::getFinalOutput(char& input_letter, int argc)
{
  int input_number = input_letter - 65;

  plugboard.getOutput(input_number); //pass forward through plugboard
  
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors_array[number_of_rotors-1-i]->getOutputForwards(input_number); //pass forward through each rotor
  }

  reflector.getOutput(input_number); //pass through reflector
  
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors_array[i]->getOutputBackwards(input_number); //pass backward through each rotor
  }

  plugboard.getOutput(input_number); //pass backward through plugboard
  input_letter = input_number + 65;
}

//THIS SECTION CONTAINS MEMBER FUNCTIONS SPECIFIC TO THE CLASS PLUGBOARD.

/*This function initialises the plugboard by reading the configuration file and setting up a 'mapping' array
 which maps each index to a number. Function checks for errors in configuration file*/

Plugboard::Plugboard(char** argv, int argc)
{
  ifstream in_stream;
  int pair[2], loop_count = 0;
  bool already_swapped[26] = {};
  int index_plugboard = getFileIndex(argv, argc);
  char* config_file_name = argv[index_plugboard];

  if (!index_plugboard) //check whether .pb and .rf files are present
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }


  for (int i = 0; i < 26; i++) //initialise as one to one mapping
    mapping[i] = i;

  in_stream.open(config_file_name);

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

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      exit(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
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

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      exit(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for invalid input
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    if (pair[0] == pair[1]) //check for attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file " << config_file_name << endl;
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range parameter
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      exit(INVALID_INDEX);
    }
    if (already_swapped[pair[0]] == 1 || already_swapped[pair[1]] == 1) //check if contact was previously swapped
    {
      cerr << "Attempted connection of a contact with more than one other contact in file " << config_file_name << endl;
      exit(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
      
    mapping[pair[0]] = pair[1]; //swap pairs
    mapping[pair[1]] = pair[0];
    already_swapped[pair[0]] = 1; //set contacts to already swapped
    already_swapped[pair[1]] = 1;
  }
  in_stream.close();
}

/*Function takes an input integer passed by reference, relating to input character absolute position, 
and converts it to an output integer, relating to absolute output position.*/

void Plugboard::getOutput(int& input)
{
  input = mapping[input];
}

/*Function returns the index of the file with the specified extension appears in the
command line arguments*/

int Plugboard::getFileIndex(char** argv, int argc)
{
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".pb"))
      return i;
  }
  return 0;
}

//THIS SECTION CONTAINS MEMBER FUNCTIONS SPECIFIC TO THE CLASS REFLECTOR.

/*This function initialises a reflector by reading the configuration file and setting up a 'mapping' array which maps each index to a number.
 Function checks for errors in configuration file*/

Reflector::Reflector(char** argv, int argc)
{
  int index_reflector = getFileIndex(argv, argc);
  char* config_file_name = argv[index_reflector];
  ifstream in_stream;
  int pair[2], loop_count = 0;
  bool already_swapped[26] = {};

  if (!index_reflector) //check whether .pb and .rf files are present
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    exit(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }
  
  in_stream.open(config_file_name); //open file
  if (in_stream.fail()) //check for error opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    exit(ERROR_OPENING_CONFIGURATION_FILE);
  }
      
  while(true)
  {
    loop_count++;
    in_stream >> pair[0]; //import FIRST digit
    
    if (in_stream.eof()) //check for end of file
    {
      if (loop_count != 14) //check if not enough parameters in file
      {
	cerr << "Insufficient number of mappings in reflector file: " << config_file_name << endl;
	exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
      }
      in_stream.close();
      return;
    }

    if (loop_count == 14) //check for too many parameters in file
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //import SECOND digit
    
    if (in_stream.eof()) //check for end of file
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      exit(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      exit(NON_NUMERIC_CHARACTER);
    }
      
    if (pair[0] == pair[1]) //check attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file " << config_file_name << endl;
      exit(INVALID_REFLECTOR_MAPPING);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range input character
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      exit(INVALID_INDEX);
    }
    
    if (already_swapped[pair[0]] == 1 || already_swapped[pair[1]] == 1) //check if input character has already been mapped
    {
      cerr << "Attempted connection of a contact with more than one other contact in file " << config_file_name << endl;
      exit(INVALID_REFLECTOR_MAPPING);
    }
      
    mapping[pair[0]] = pair[1]; //swap pairs
    mapping[pair[1]] = pair[0];
    already_swapped[pair[0]] = 1; //set character to already swapped
    already_swapped[pair[1]] = 1;
  }
  in_stream.close();
}

/*Function takes an input integer passed by reference, relating to input character absolute position, 
and converts it to output integer, relating to absolute output position.*/

void Reflector::getOutput(int& input)
{
  input = mapping[input];
  return;
}

/*Function returns the index of the file with the specified extension appears in the
command line arguments*/

int Reflector::getFileIndex(char** argv, int argc)
{
  for (int i = 0; i < argc; i++)
  {
    if (strstr(argv[i], ".rf"))
      return i;
  }
  return 0;
}

//THIS SECTION CONTAINS MEMBER FUNCTIONS SPECIFIC TO THE CLASS ROTOR.

/*This function initialises a rotor by reading the configuration file and setting up a 'mapping' array which maps each
 index to a number, a notches boolean array (true if a notch exists in that position) and sets data
member relative_position to rotor starting position. Function checks for errors in configuration file*/

Rotor::Rotor(char* config_file_name, int starting_position)
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

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      exit(ERROR_OPENING_CONFIGURATION_FILE);
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
      cerr << "Invalid mapping of input " << i << " to output " << number << " (output " << number << " is already mapped to from input " << endl;
 
      for (int j = 0; j < 26; j++)
      {
	if (mapping[j] == number)
	{
	  cerr << j << ") in rotor file " << config_file_name << endl;
	  exit(INVALID_ROTOR_MAPPING);
	}
      }
    }
    
    mapping[i] = number;
    already_mapped[number] = 1;
    in_stream >> number;
  }

  //get notch positions
  while (!in_stream.eof())
  {
    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      exit(ERROR_OPENING_CONFIGURATION_FILE);
    }
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

/*Function takes an input integer passed by reference, relating to input character absolute position, 
and converts it to output integer, relating to absolute output position.
Function is specific to going forwards through the rotor.*/

void Rotor::getOutputForwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against overflow
  relative_output = mapping[relative_input];
  relative_output -= relative_position;
  relative_output = (relative_output + 26)%26; //guard against underflow
  input = relative_output;
}

/*Function takes an input integer passed by reference, relating to input character absolute position, 
and converts it to output integer, relating to absolute output position.
Function is specific to going backwards through the rotor.*/

void Rotor::getOutputBackwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against overflow
  for (int i = 0; i < 26; i++)
  {
    if (mapping[i] == relative_input)
    {
      relative_output = i - relative_position;
      relative_output = (26 + relative_output)%26; //guard against underflow
      input = relative_output;
      return;
    }
  }
}

/*Function that mimics turning the rotor anticlockwise by one increment by adding one to the
relative position of that rotor.
Function returns true if, after turning, a notch exists in absolute position 0.
Function returns false if, after turning, a notch does not exist in absolute position 0.*/

bool Rotor::turnRotor()
{
  relative_position++;
  relative_position = relative_position%26; //guard against overflow
  if (notches[relative_position] == 1)
    return true;
  return false;
}
