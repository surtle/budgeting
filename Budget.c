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
#define NEW_CATEGORY "Enter name of new category (max 20 characters): "
#define FIND_CATEGORY "Enter name of the category you want to edit: "
#define REM_CATEGORY "Enter name of the category you want to remove: " 
#define NEW_AMOUNT "Enter the amount you want to log into %s: " 
#define REM_AMOUNT "Enter the amount you want to remove from %s: " 
#define TOO_MANY_ARGS "Error: too many arguments\n\n" 
#define NO_FILE "Error: file does not exist\n\n" 
#define NO_OPTION "Error: %s is not a valid option :-(\n\n" 
#define NO_CATEGORY "Error: category not found\n\n" 
#define NO_LONG "Error: %s is not a valid amount\n\n" 
#define OVER_LIMIT "Error: %s is too long. (%d max character limit)\n\n"
#define NO_MEM "Error: no more memory\n\n" 

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
  char *newlineChar; 

  // prompt user 
  fprintf( stdout, "%s", NEW_CATEGORY ); 

  // get new category name 
  fgets( categoryName, BUFSIZ, stdin );

  // check length of name
  if( strlen( categoryName ) < 1 || strlen( categoryName ) > MAX_CATEGORIES ) {
    fprintf( stdout, OVER_LIMIT, "Category name", MAX_CATEGORIES ); 
    return NULL; 
  }

  // create new category  
  struct Category *newCategory = malloc( sizeof(struct Category)); 

  // return -1 if no more memory 
  if( newCategory == NULL ) { 
    fprintf( stdout, NO_MEM ); 
    return NULL; 
  }

  // replaces newline character in category name with null terminating character
  newlineChar = memchr( categoryName, (int) '\n', BUFSIZ ); 
  *newlineChar = '\0';

  // convert name to all caps
  int i;
  for( i = 0; i < strlen( categoryName ); i++ ) {
    categoryName[i] = toupper( categoryName[i] ); 
  } 

  // put name information in new struct, then put it in array of structs
  newCategory->name = categoryName; 
  newCategory->amount = 0;
  catArray[numCategories] = newCategory; 

  return newCategory; 
}

/**
 * Function: alterAmount( float *total, int dollars, float cents, struct
 * Category *category )
 * Parameters: total - pointer to the running total 
 *             dollars - int amount of dollars to add/subtract
 *             cents - float amount of cents to add/subtract
 *             category - category to add 
 * Description: alters amount to an existing category 
 * Return: void 
 * Error Conditions: none 
 */ 
int alterAmount( float *total, int dollars, float cents, struct Category
    *category ) {
  // add to category amount and running total 
  category->amount = category->amount + (float) dollars + ( cents / 100.00 );  
  *total = *total + category->amount; 

  return 0;
}

/**
 * Function: addAmount( int *total, struct Category *category, int mode ) 
 * Parameters: category - the category to add money into 
 *             total - addy to total spending budget
 *             mode - 0 for add amount, 1 for subtract amount
 * Description: prompts user and adds spending amount to a 
 *              specified spending category 
 * Return: 0 if successful, -1 if not 
 * Error Conditions: if it aint a numba
 */
int askAmount( float *total, struct Category *category, int mode ) { 
  char *endPtr; 
  char *newlineChar;
  char *decimal;
  char *amountStr = malloc( BUFSIZ ); 
  int dollars; 
  float cents = 0; 

  fgets( amountStr, BUFSIZ, stdin ); 

  // remove newline character
  newlineChar = memchr( amountStr, (int) '\n', BUFSIZ ); 
  *newlineChar = '\0';

  // checks if there is a decimal point
  if( memchr( amountStr, (int) '.', BUFSIZ ) != NULL ) {

    // replace decimal with null terminator 
    decimal = memchr( amountStr, (int) '.', BUFSIZ ); 
    *decimal = '\0'; 

    // read dollars, check for error
    dollars = (int) strtol( amountStr, &endPtr, BASE ); 
    if( *endPtr != '\0' ) {
      fprintf( stdout, NO_LONG, amountStr ); 
      return -1;
    }
    
    // read cents, check for error
    endPtr = endPtr + 1; 
    cents = strtol( endPtr, &endPtr, BASE );
    if( *endPtr != '\0' ) {
      fprintf( stdout, NO_LONG, amountStr ); 
      return -1; 
    } 
  }

  // otherwise, just convert dollars 
  dollars = strtol( amountStr, &endPtr, BASE ); 
  if( *endPtr != '\0' ) { 
    fprintf( stdout, NO_LONG, amountStr ); 
    return -1; 
  } 

  // either adds or subtracts amount from category depending on mode 
  if( mode == 0 ) { 
    alterAmount( total, dollars, cents, category ); 
  } else {
    dollars = 0 - dollars; 
    cents = 0 - cents; 
    alterAmount( total, dollars, cents, category ); 
  }

  free( amountStr ); 
  return 0; 
}

/**
 * Function: findCategory( int numCat, char *categoryName,
 *                         struct Category *catArr[] ) 
 * Parameters: numCat - the number of category entries in the array  
 *             categoryName - the name of the category to find
 *             catArr - the array of categories 
 * Description: returns the category found from the category array
 * Return: pointer to Category if found, NULL if not found
 * Error Conditions: None
 */ 
struct Category *findCategory( int numCat, char *categoryName, 
                               struct Category *catArr[] ) {
  int i;
  char *newlineChar; 

  // replace newline character with null terminating character 
  newlineChar = memchr( categoryName, (int) '\n', BUFSIZ ); 
  *newlineChar = '\0'; 

  // convert input to all caps
  for( i = 0; i < strlen(categoryName); i++ ) {
    categoryName[i] = toupper( categoryName[i] ); 
  } 

  // search for equals 
  for( i = 0; i < numCat; i++ ) {
    if( strncmp( catArr[i]->name, categoryName, MAX_CATEGORIES ) == 0 ) {
      return catArr[i];
    }
  }

  return NULL; 
}

/**
 * Function: removeCategory( struct Category *remCategory, struct Category
 * *catArr[], int numCategories ) 
 * Parameters: remCategory - the category to remove from array
 *             catArr - array of categories 
 *             numCategories - number of total categories 
 * Description: removes category from array of categories
 * Return: void
 * Error Conditions: none
 */
void removeCategory( struct Category *remCategory, struct Category *catArr[],
                     int *numCategories ) { 
  
  // replace category to remove with last category
  remCategory->name = catArr[*numCategories - 1]->name; 
  remCategory->amount = catArr[*numCategories - 1]->amount; 

  // decrease num of categories
  (*numCategories)--; 
}

/** 
 * Function: main( int argc, char* argv[] ) 
 * Parameters: argc - the number of args 
 *             argv - the arguments inputted
 * Description: runs the program
 * Return: EXIT_SUCCESS if successful, EXIT_FAILURE if not
 * Error Conditions: invalid parameters, idk
 */
int main( int argc, char* argv[] ) {

  // for existing spending reports 
  FILE *filePath;
  struct Category *categories[MAX_CATEGORIES];
  int numCategories = 0; 
  float runningTotal = 0; 
  char *input = malloc( BUFSIZ ); 
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
      struct Category *newCat; 
      struct Category *exisCat; 
      char *input; 

      switch( option ) {
        case 1: // add spending category 

          newCat = addCategory( numCategories, categories ); 
          if( newCat == NULL ) {
            break; 
          }

          // asks user to input spending amount to new category 
          fprintf( stdout, NEW_AMOUNT, newCat->name ); 
          askAmount( &runningTotal, newCat, 0 ); 

          // increase number of Categories
          numCategories++; 
          break; 

        case 2: // add amount to spending category 
          input = malloc( BUFSIZ ); 

          // prompt user 
          fprintf( stdout, FIND_CATEGORY ); 
          fgets( input, BUFSIZ, stdin ); 

          exisCat = findCategory( numCategories, input, categories ); 
          if( exisCat != NULL ) { 

            // prompt user to enter an amount 
            fprintf( stdout, NEW_AMOUNT, exisCat->name ); 
            askAmount( &runningTotal, exisCat, 0 ); 
          } else { 
            fprintf( stdout, NO_CATEGORY ); 
          } 

          free( input ); 
          break;

        case 3: // decrease amount to spending category 
          input = malloc( BUFSIZ ); 

          // prompt user 
          fprintf( stdout, FIND_CATEGORY ); 
          fgets( input, BUFSIZ, stdin ); 

          exisCat = findCategory( numCategories, input, categories ); 
          if( exisCat != NULL ) {
              
            // prompt user to enter an amount 
            fprintf( stdout, REM_AMOUNT, exisCat->name ); 
            askAmount( &runningTotal, exisCat, 1 ); 
          } else {
            fprintf( stdout, NO_CATEGORY ); 
          }

          free( input ); 
          break; 

        case 4: // delete spending category 
          input = malloc( BUFSIZ ); 

          // prompt user 
          fprintf( stdout, REM_CATEGORY ); 
          fgets( input, BUFSIZ, stdin ); 

          exisCat = findCategory( numCategories, input, categories ); 
          if( exisCat != NULL ) {
            removeCategory( exisCat, categories, &numCategories ); 
          } else {
            fprintf( stdout, NO_CATEGORY );
          }

          free( input ); 
          break;

        case 5:
          // view spending report
          printf( "option 5\n" );

          int i;
          for( i = 0; i < numCategories; i++ ) {
            fprintf( stdout, "%s\t\t%G\n", categories[i]->name,
                categories[i]->amount ); 
          }
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
