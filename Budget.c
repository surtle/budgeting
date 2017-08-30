/**
 * Standard libraries 
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "errno.h" 
#include "Category.h" 

/**
 * Strings & Constants 
 */
#define INIT_PROMPT "======================================================" \
                    "\n W.A.Y.S. - What Are You Spending? "\
                    "\n======================================================"
#define USAGE "Usage: ./budget.exe [file_name]" \
              "\n\t file_name: the filename of an existing budget report"
#define PROMPT "Type in a number option to take action:" \
               "\n\t 1) Add spending category" \
               "\n\t 2) Add amount spent to spending category" \
               "\n\t 3) Decrease amount spent to spending category" \
               "\n\t 4) Delete spending category" \
               "\n\t 5) View spending report" \
               "\n\t 6) Export spending report" \
               "\n\t 7) Exit Program" \
               "\n >> " 
#define TOO_MANY_ARGS "Error: too many arguments" 
#define NO_FILE "Error: file does not exist" 
#define NO_OPTION "Error: %s is not a valid option :-(\n\n" 
#define NEW_CATEGORY "Enter name of new category (max 20 characters): "
#define OVER_LIMIT "%s is too long. (%d max character limit)\n"
#define NO_MEM "Error: no more memory\n" 

#define BASE 10
#define NOFILE_ARG 1
#define FILE_ARG 2
#define MAX_CATEGORIES 20
#define MIN_OPTION 1
#define MAX_OPTION 7 

/**
 * Function: usage( int argc ) 
 * Parameters: argc - the number of arguments passed into the program
 * Description: checks if the number & type of arguments are valid 
 * Return: 0 if valid, -1 if not
 * Error Conditions: invalid command line argument, invalid file
 */
int usage( int argc, char* argv[], FILE *budgetAcct ) {
  
  // set errno to 0
  errno = 0;
  
  // if no arguments, prompt user for action
  if( argc == NOFILE_ARG ) {
    return 0;

  } else if ( argc == FILE_ARG ) {
    // open file specified in description 
    budgetAcct = fopen( argv[1], "r" );
    
    // if file does not exist, return -1
    if( budgetAcct == NULL || errno != 0 ) {
      fprintf( stdout, "%s\n", NO_FILE );
      return -1;

    } else {
      // read file and import information
      return 0;

    }
  } else {

    // if too many arguments, return -1
    fprintf( stderr, "%s\n", TOO_MANY_ARGS );
    return -1;
  }
}

/**
 * Function: validate_options( char *input )
 * Parameters: char *input - user input following prompt 
 * Description: makes sure that the input is a valid option
 * Return: int from 1-5 depending on input, -1 if not valid
 * Error Conditions: option is not an integer between 1 & 5 inclusive
 */
int validate_options( const char *strInput ) {
 
  char *endptr; 
  char *newlineChar; 

  // replace newline character with null-terminating character
  newlineChar = memchr( strInput, (int) '\n', BUFSIZ ); 
  *newlineChar = '\0';

  // convert string input into int 
  int input = (int) strtol( strInput, &endptr, BASE ); 

  if( *endptr != '\0' || (input < MIN_OPTION || input > MAX_OPTION)) {
    return -1;
  } else { 
    return input;
  }
}

/**
 * Function: addCategory() 
 * Parameters: numCategories - the number of categories in this spending report
 *             catArray - the array of struct Categories 
 * Description: adds category to array and updates spending report 
 * Return: 0 if successful, -1 if name invalid, -2 if no more memory 
 * Error condition: name of category is 0 or over 20 characters 
 */ 
struct Category *addCategory( int numCategories, struct Category *catArray[] ) {
  char *categoryName = malloc( BUFSIZ );

  // prompt user 
  fprintf( stdout, "%s", NEW_CATEGORY ); 

  // get new category name 
  fgets( categoryName, BUFSIZ, stdin );

  // check length of name
  if( strlen( categoryName ) < 1 || strlen( categoryName ) > MAX_CATEGORIES ) {
    return NULL; 
  }

  // create new category  
  struct Category *newCategory = malloc( sizeof(struct Category)); 

  // return -1 if no more memory 
  if( newCategory == NULL ) { 
    return NULL; 
  }

  // put name information in new struct, then put it in array of structs
  newCategory->name = categoryName; 
  catArray[numCategories] = newCategory; 

  return newCategory; 
}

/** 
 * Function: main( int argc, char* argv[] ) 
 * Parameters: argc - the number of args 
 *             argv - the arguments inputted
 * Description: runs the program
 * Return: 0 if successful, -1 if not
 * Error Conditions: invalid parameters, idk
 */
int main( int argc, char* argv[] ) {

  // for existing spending reports 
  FILE *filePath;
  struct Category *categories[MAX_CATEGORIES];
  int numCategories = 0; 
  char *input; 
  int option;

  // Checks validity of arguments 
  if( usage( argc, argv, filePath ) == -1 ) {
    fprintf( stderr, "%s\n", USAGE );
    return EXIT_FAILURE;
  }  

  // prompt user and get input
  fprintf( stdout, "%s\n", INIT_PROMPT );
  fprintf( stdout, "%s", PROMPT );
  fgets( input, BUFSIZ, stdin );

  // while input is valid, do action and prompt again
  while( input != NULL ) {

    // validates input as an option
    option = validate_options( input ); 

    // print error message if option not valid
    if( option == -1 ) {
      fprintf( stdout, NO_OPTION, input );

    // perform necessary command according to option sensei
    } else {
      switch( option ) {
        case 1: 
          //add spending category
          if( addCategory( numCategories, categories ) == NULL ) {
            fprintf( stdout, OVER_LIMIT, "Category name", MAX_CATEGORIES ); 
            break; 
          }

          numCategories++; 
          break; 
        case 2:
          // add amount to spending category
          printf("option 2");
          break;
        case 3:
          // decrease amount to spending category
          printf("option 3");
          break; 
        case 4:
          // delete spending category
          printf("option 4");
          break;
        case 5:
          // view spending report
          printf("option 5");
          break; 
        case 6:
          // export spending report
          printf("option 6");
          break;
        case 7:
          // return exit success 
          return EXIT_SUCCESS;
      }
    }

    // reprompt
    fprintf( stdout, "%s", PROMPT );
    fgets( input, BUFSIZ, stdin );
  }

  return EXIT_SUCCESS;
}
