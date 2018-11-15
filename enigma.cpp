#include<iostream>
#include<fstream>
#include<cstring>
#include<string.h>
#include<cstdlib>
#include"enigma.h"
#include"errors.h"

using namespace std;

//THIS SECTION CONTAINS MEMBER FUNCTIONS SPECIFIC TO THE CLASS ENIGMA.

/*Enigma class constructor. Used to intialise enigma object.
 Function reads information from configuration files and throws an integer (error code)
 exception if an invalid configuration exists. A plugboard and reflector are automatically
created on declaration as data members but this constructor is used to count number of rotors
and create 0-many rotor objects on the heap. */

Enigma::Enigma(char** argv, int argc) : plugboard(argv, argc), reflector(argv, argc)
{
  int starting_pos;
  int index_first_rotor = getFileTypeIndex(argv, argc, ".rot");
  int index_rotor_position = getFileTypeIndex(argv, argc, ".pos");
  ifstream in_stream;
  
  number_of_rotors = getNumberOfRotors(argv, argc); //count number of rotors
  rotors_array = new Rotor*[number_of_rotors]; //create array of Rotor pointers on heap

  //initialise rotor_array pointers to null incase deconstructor is called early in case of exception
  
  for (int i = 0; i < number_of_rotors; i++)
  {
    rotors_array[i] = nullptr;
  }
  
  //Rest of function creates each rotor whilst checking for errors in rotor config files
  
  if (index_rotor_position) //check if .pos file exists
  {
    if (!index_first_rotor) //check if .rot file does not exist
    {
      cerr << "Position files exists but no rotor files." << endl;
      throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
    }
    
    in_stream.open(argv[index_rotor_position]); //open .pos file

    if (in_stream.fail()) //check for failure opening file
    {
      cerr << "Error: file " << argv[index_rotor_position] << " could not be opened." << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    
    for (int i = 0; i < number_of_rotors + 1; i++) //for each rotor set position
    {
      in_stream >> starting_pos;
    
      if (i == number_of_rotors) //if true then all rotor positions should have been set already
      {
	if (!in_stream.eof()) //eof flag has not been set
	{
	  if (in_stream.bad()) //check for badbit
	  {
	    cerr << "Error reading file " << argv[index_rotor_position] << endl;
	    throw(ERROR_OPENING_CONFIGURATION_FILE);
	  }
	  if (in_stream.fail()) //check for failbit
	  {
	    cerr << "Non-numeric character in rotor positions file " << argv[index_rotor_position];
	    cerr << endl;
	    throw(NON_NUMERIC_CHARACTER);
	  }
	  cerr << "Too many parameters in rotor position file " << argv[index_rotor_position];
	  cerr << ", only " << number_of_rotors << " rotors exist." << endl;
	  throw(TOO_MANY_PARAMETERS);
	}
	in_stream.close(); 
	return; //return if all rotor positions set and eof flag has been set
      }
    
      if (in_stream.eof()) //check if eof flag has been set and not all rotors positions have been set
      {
	cerr << "No starting position for rotor " << i << " in rotor position file: ";
	cerr << argv[index_rotor_position] << endl;
	throw(NO_ROTOR_STARTING_POSITION);
      }

      if (in_stream.bad()) //check for badbit
      {
	cerr << "Error reading file " << argv[index_rotor_position] << endl;
	throw(ERROR_OPENING_CONFIGURATION_FILE);
      }
      if (in_stream.fail()) //check for failbit
      {
	cerr << "Non-numeric character in rotor positions file " << argv[index_rotor_position];
	cerr << endl;
	throw(NON_NUMERIC_CHARACTER);
      }
      //if no error from file: create a rotor on stack and pass stating_pos to Rotor constructor
      rotors_array[i] = new Rotor(argv[i + index_first_rotor], starting_pos);
    }
  }
}

/*Enigma class destructor*/

Enigma::~Enigma()
{
  for (int i = 0; i < number_of_rotors; i++)
  {
    delete rotors_array[i]; //delete each rotor
  }
  delete [] rotors_array; //delete array of pointers
}


/*Function that runs the enigma machine.
This function assumes that plugboard, reflector and 0 to many rotors are set up previously.
Function reads in inputs from the standard input stream (only characters between A-Z), 
throws exeptions if the input is invalid, encrypts/decrypts the characters and outputs
them to the standard output stream.*/

void Enigma::runEnigma(int argc)
{
  char letter;

  cin >> ws;
  cin >> letter;
  
  while(!cin.fail()) //while characters remain
  {
    if (letter < 65 || letter > 90) //check for invalid input character
    {
      cerr << letter;
      cerr << " is not a valid input character (input characters must be upper case letters A-Z)!";
      cerr << endl;
      throw(INVALID_INPUT_CHARACTER);
    }

    //turn far right rotor anticlockwise once then cascade turning to the left if any notch aligns
    for (int j = 0; j < number_of_rotors; j++)
    {
      if (!rotors_array[number_of_rotors - 1 - j]->turnRotor())
	break;
    }
    
    getOutput(letter, argc); //encript/decrypt letter
    cout << letter; 
    cin >> letter;
  }
}


/*Function returns the number of rotors present but counting the number of
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

/*Function returns the index of the file where the specified extension appears in the
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

/*Function takes an input character by reference and encripts/decripts it by passing it through
each element of the enigma machine.*/

void Enigma::getOutput(char& letter, int argc)
{
  int number = letter - 65; //convert to number

  plugboard.getOutput(number); //pass letter forward through plugboard
  
  for (int i = 0; i < number_of_rotors; i++)  //pass letter forward through each rotor
  {
    rotors_array[number_of_rotors-1-i]->getOutputForwards(number);
  }

  reflector.getOutput(number); //pass letter through reflector
  
  for (int i = 0; i < number_of_rotors; i++) //pass letter backward through each rotor
  {
    rotors_array[i]->getOutputBackwards(number);
  }

  plugboard.getOutput(number); //pass letter backward through plugboard
  
  letter = number + 65; //convert back to letter
}



//THIS SECTION CONTAINS MEMBER FUNCTIONS SPECIFIC TO THE CLASS PLUGBOARD.

/*Plugboard class constructor. Used to initialise a plugboard object.
Function reads information from the configuration file, throwing an integer (error_code)
exception if an invalid configuration exists. Function sets up data member mapping.*/

Plugboard::Plugboard(char** argv, int argc)
{
  ifstream in_stream;
  int pair[2]; //pair of contacts attemping to connect
  int loop_count = 0;
  bool already_paired[26] = {}; //to keep track of contacts already connected
  int index_plugboard = getFileIndex(argv, argc);
  char* config_file_name = argv[index_plugboard];

  if (!index_plugboard) //check if .pb file does not exist
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }

  for (int i = 0; i < 26; i++) //initialise as one to one mapping
    mapping[i] = i;

  in_stream.open(config_file_name);

  if (in_stream.fail()) //check for failure opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
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
      throw(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //take in second digit
    
    if (in_stream.eof()) //check for odd number of parameters
    {
      cerr << "Incorrect number of parameters in plugboard file " << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for invalid input
    {
      cerr << "Non-numeric character in plugboard file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }
    
    if (pair[0] == pair[1]) //check for attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file ";
      cerr << config_file_name << endl;
      throw(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range parameter
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      throw(INVALID_INDEX);
    }
    if (already_paired[pair[0]] == 1 || already_paired[pair[1]] == 1) //check if previously swapped
    {
      cerr << "Attempted connection of a contact with more than one other contact in file ";
      cerr << config_file_name << endl;
      throw(IMPOSSIBLE_PLUGBOARD_CONFIGURATION);
    }
      
    mapping[pair[0]] = pair[1]; //if no exceptions: pair contacts
    mapping[pair[1]] = pair[0];
    already_paired[pair[0]] = 1; //set contacts to already paired
    already_paired[pair[1]] = 1;
  }
  in_stream.close();
}

/*Function takes an input integer passed by reference, relating to input character's absolute
position, and converts it to an output integer, relating to absolute output position.*/

void Plugboard::getOutput(int& input)
{
  input = mapping[input];
}

/*Function returns the index of where the file with the extension .pb appears in the
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

/*Reflector class constructor. Used to initialise a reflector object.
Function reads information from the configuration file, throwing an integer (error_code)
exception if an invalid configuration exists. Function sets up data member mapping.*/

Reflector::Reflector(char** argv, int argc)
{
  int index_reflector = getFileIndex(argv, argc);
  char* config_file_name = argv[index_reflector];
  ifstream in_stream;
  int pair[2]; //pair of contacts attempting to connect
  int loop_count = 0;
  bool already_paired[26] = {}; //keep track of contacts already paired

  if (!index_reflector) //check if .rf file does not exist
  {
    cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>* rotor-positions)?" << endl;
    throw(INSUFFICIENT_NUMBER_OF_PARAMETERS);
  }
  
  in_stream.open(config_file_name); //open file .rf file
  
  if (in_stream.fail()) //check for error opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
  }
      
  while(true)
  {
    loop_count++;
    
    in_stream >> pair[0]; //take in first integer
    
    if (in_stream.eof()) //check for end of file
    {
      if (loop_count != 14) //check if not enough parameters in file
      {
	cerr << "Insufficient number of mappings in reflector file: " << config_file_name << endl;
	throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
      }
      break; //break loop if finished pairing contacts
    }

    if (loop_count == 14) //check for too many parameters in file
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }
    
    in_stream >> pair[1]; //take in second integer
    
    if (in_stream.eof()) //check for odd number of parameters
    {
      cerr << "Incorrect (odd) number of parameters in reflector file " << config_file_name << endl;
      throw(INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS);
    }
    
    if (in_stream.fail()) //check for non-numeric character
    {
      cerr << "Non-numeric character in reflector file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }
      
    if (pair[0] == pair[1]) //check attempted mapping of contact to itself
    {
      cerr << "Attempted connection of contact to itself in plugboard file ";
      cerr << config_file_name << endl;
      throw(INVALID_REFLECTOR_MAPPING);
    }
    
    if (pair[0] < 0 || pair[0] > 25 || pair[1] < 0 || pair[1] > 25) //check for out of range character
    {
      cerr << "Parameter out of range in plugboard file " << config_file_name << endl;
      throw(INVALID_INDEX);
    }
    
    if (already_paired[pair[0]] == 1 || already_paired[pair[1]] == 1) //check if previously paired
    {
      cerr << "Attempted connection of a contact with more than one other contact in file ";
      cerr << config_file_name << endl;
      throw(INVALID_REFLECTOR_MAPPING);
    }
      
    mapping[pair[0]] = pair[1]; //no exceptions: pair contacts
    mapping[pair[1]] = pair[0];
    already_paired[pair[0]] = 1; //set contacts to already paired
    already_paired[pair[1]] = 1;
  }
  in_stream.close();
}

/*Function takes an input integer passed by reference, relating to input character's absolute
position, and converts it to an output integer, relating to absolute output position.*/

void Reflector::getOutput(int& input)
{
  input = mapping[input];
  return;
}

/*Function returns the index of where the file with the extension .rf appears in the
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

/*Rotor class constructor. Used to initialise a rotor object.
Function reads information from the configuration file, throwing an integer (error_code)
exception if an invalid configuration exists. Argument starting_position relates to the
starting position specific to that rotor extracted from .pos file previously.
Function sets up data member mapping.*/


Rotor::Rotor(char* config_file_name, int starting_position)
{
  ifstream in_stream;
  int number, loop_count = 0;
  bool already_mapped[26] = {};
  
  relative_position = starting_position; //set data member relative position to starting position

  in_stream.open(config_file_name);

  if (in_stream.fail()) //check for failure opening file
  {
    cerr << "Error: file " << config_file_name << " could not be opened." << endl;
    throw(ERROR_OPENING_CONFIGURATION_FILE);
  }
  
  in_stream >> number; //take in first integer

  for (int i = 0; i < 26; i++) //loop to construct data member mapping array
  {
    if (in_stream.eof()) //check if not enough parameters
    {
      cerr << "Not all inputs mapped in rotor file: " << config_file_name << endl;
      throw(INVALID_ROTOR_MAPPING);
    }

    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character for mapping in rotor file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (number > 25 || number < 0) //check for out of range parameter
    {
      cerr << "Parameter out of range in rotor file " << config_file_name << endl;
      throw(INVALID_INDEX);
    }

    if (already_mapped[number] == 1) //check if contact already mapped
    {
      cerr << "Invalid mapping of input " << i << " to output " << number << " (output ";
      cerr << number << " is already mapped to from input " << endl;
 
      for (int j = 0; j < 26; j++) //find contact already mapped to
      {
	if (mapping[j] == number)
	{
	  cerr << j << ") in rotor file " << config_file_name << endl;
	  throw(INVALID_ROTOR_MAPPING);
	}
      }
    }

    mapping[i] = number; //no exceptions: set contact mapping
    already_mapped[number] = 1; //set contact to already mapped
    in_stream >> number;
  }

  
  while (!in_stream.eof()) //loop to set data member notches array
  {
    if (in_stream.bad()) //check for badbit
    {
      cerr << "Error reading file " << config_file_name << endl;
      throw(ERROR_OPENING_CONFIGURATION_FILE);
    }
    if (in_stream.fail()) //check for failbit
    {
      cerr << "Non-numeric character for mapping in rotor file " << config_file_name << endl;
      throw(NON_NUMERIC_CHARACTER);
    }

    if (loop_count > 26) //check for too many parameters
    {
      cerr << "Too many input parameters for rotor file " << config_file_name << endl;
      throw(TOO_MANY_PARAMETERS);
    }
    
    notches[number] = 1; //set notch position to true

    in_stream >> number;
  }
  
  in_stream.close();
}

/*Function takes an input integer passed by reference, relating to input character's absolute
position, and converts it to an output integer, relating to absolute output position.
Function is specific to going 'forwards' through the rotor.*/

void Rotor::getOutputForwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against rotor overflow
  relative_output = mapping[relative_input];
  relative_output -= relative_position;
  relative_output = (relative_output + 26)%26; //guard against rotor underflow
  input = relative_output;
}

/*Function takes an input integer passed by reference, relating to input character's absolute
position, and converts it to an output integer, relating to absolute output position.
Function is specific to going 'backwards' through the rotor.*/

void Rotor::getOutputBackwards(int& input)
{
  int relative_output, relative_input = input + relative_position;
  relative_input = relative_input%26; //guard against rotor overflow
  for (int i = 0; i < 26; i++)
  {
    if (mapping[i] == relative_input)
    {
      relative_output = i - relative_position;
      relative_output = (26 + relative_output)%26; //guard against rotor underflow
      input = relative_output;
      return;
    }
  }
}

/*Function that mimics turning the rotor anticlockwise by one increment by adding one to the
relative position of that rotor.
Function returns true if, after turning, a notch exists in absolute position 0 (12 o'clock).
Function returns false otherwise.*/

bool Rotor::turnRotor()
{
  relative_position++;
  relative_position = relative_position%26; //guard against rotor overflow
  if (notches[relative_position] == 1)
    return true;
  return false;
}
