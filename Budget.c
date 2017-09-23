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
               "\n\t 7) Exit program" \
               "\n >> " 
#define NEW_CATEGORY "Enter name of new category (max 20 characters): "
#define FIND_CATEGORY "Enter name of the category you want to edit: "
#define REM_CATEGORY "Enter name of the category you want to remove: " 
#define NEW_AMOUNT "Enter the amount you want to log into %s: " 
#define REM_AMOUNT "Enter the amount you want to remove from %s: " 
#define NEW_FILENAME "Enter filename to save report under: " 

#define TOO_MANY_ARGS "Error: too many arguments\n\n" 
#define NO_FILE "Error: file does not exist\n\n" 
#define NO_OPTION "Error: %s is not a valid option :-(\n\n" 
#define NO_CATEGORY "Error: category not found\n\n" 
#define NO_LONG "Error: %s is not a valid amount\n\n" 
#define OVER_LIMIT "Error: %s is too long. (%d max character limit)\n\n"
#define NO_PRINT "Error: no data to show\n\n" 
#define NO_MEM "Error: no more memory\n\n" 
#define BAD_FILE "Error: cannot read file\n\n" 

#define FILE_IMPORTED "Success! %s imported\n\n" 

#define FORMAT_SEP "======================================================\n"
#define FORMAT_HEADER "Budget Report for %s"  // Header for report
#define FORMAT_CATEGORY "%-30s$%-16.2f%-4.2f%%\n" // Lists spending category
#define FORMAT_TOTAL "%-30s$%-16.2f\n"              // Lists total spending

#define BASE 10                     // Base conversion for strtol
#define NOFILE_ARG 1                // Flag if there is no file to scan
#define FILE_ARG 2                  // Flag if there is a file to scan 
#define MAX_CATEGORIES 20           // Max number of categories allowed
#define MIN_OPTION 1                // First option given in prompt
#define MAX_OPTION 7                // Last option given in prompt
#define FORMAT_CATEGORY_WIDTH 30    // Format width for category name
#define FORMAT_MONEY_WIDTH 16       // Format width for amount spent
#define FORMAT_PERCENT_WIDTH 4      // Format width for percentage of total

#define FILE_WRITE "w"
#define FILE_READ "r" 

struct Category *findCategory( int numCat, char *categoryName, 
                               struct Category *catArr[] );
/**
 * Function: usage( int argc ) 
 * Parameters: argc - the number of arguments passed into the program
 * Description: checks if the number & type of arguments are valid 
 * Return: 0 if valid, -1 if not
 * Error Conditions: invalid command line argument, invalid file
 */
int usage( int argc, char* argv[], FILE **budgetAcct ) {
  
  // set errno to 0
  errno = 0;
  
  // if no file is specified, assign null to address and continue
  if( argc == NOFILE_ARG ) {
    *budgetAcct = NULL;
    return 0;

  } else if ( argc == FILE_ARG ) {
    // open file specified in description 
    *budgetAcct = fopen( argv[1], "r" );
    
    // if file does not exist, return -1
    if( *budgetAcct == NULL || errno != 0 ) {
      fprintf( stdout, "%s\n", NO_FILE );
      return -1;

    } else {
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

  // return -1 if entry contains non-digits or is outside limits
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
  struct Category *newCategory = malloc( sizeof(struct Category) ); 

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
 * Function: convertCents( float cents ) 
 * Parameters: cents - float larger than 1 representing cents
 * Description: helper function for alterAmount that converts cents from number
 * larger than 1 to number less than 1
 * Return: converted cents as a float
 * Error Conditions: none
 */ 
float convertCents( float cents ) {
  while( cents > 1 ) { 
    cents = cents / 10.0;
  }

  return cents;
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
  category->amount = category->amount + (float) dollars + convertCents( cents );
  *total = *total + (float) dollars + convertCents( cents ); 

  return 0;
}

/**
 * Function: askAmount( int *total, struct Category *category, int mode ) 
 * Parameters: category - the category to add money into 
 *             total - addy to total spending budget
 *             mode - 0 for add amount, 1 for subtract amount
 * Description: prompts user and adds spending amount to a 
 *              specified spending category 
 * Return: 0 if successful, -1 if not 
 * Error Conditions: if the amount entered is not a number
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

    // turn into negative versions of dollars and cents
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

  // don't search if there are 0 categories
  if( numCat == 0 ) { 
    return NULL;
  }

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
 *             total - total amount of money spent
 * Description: removes category from array of categories
 * Return: void
 * Error Conditions: none
 */
void removeCategory( struct Category *remCategory, struct Category *catArr[],
                     int *numCategories, float *total ) { 
  // subtract amount recorded in category from recorded total 
  *total = *total - remCategory->amount; 
  
  // replace category to remove with last category
  remCategory->name = catArr[*numCategories - 1]->name; 
  remCategory->amount = catArr[*numCategories - 1]->amount; 

  // decrease num of categories
  (*numCategories)--; 
}

/** 
 * Function: printData( int numCategories, int total, struct Category 
 *                      *catArr[], FILE *stream ) 
 * Parameters: numCategories - total number of categories recorded 
 *             total - total amount of money spent
 *             catArr - the array of categories
 *             stream - where the report should be outputted
 * Description: prints out the category data in a legible manner to the stream
 *              specified
 * Return: void
 * Error Conditions: none
 */ 
void printData( int numCategories, float total, struct Category *catArr[], 
                FILE *stream ) {
  int i;

  // beginning separator
  fprintf( stream, "%s%s", "\n", FORMAT_SEP ); 

  // prints each category and its respective statistics
  for( i = 0; i < numCategories; i++ ) {
    fprintf( stream, FORMAT_CATEGORY, catArr[i]->name,
        catArr[i]->amount, ((catArr[i]->amount/total) * 100)); 
  }

  // newline buffer between categories and total
  fprintf( stream, "%s", "\n" ); 

  // print total spent
  fprintf( stream, FORMAT_TOTAL, "TOTAL", total ); 

  // end separator
  fprintf( stream, "%s%s", FORMAT_SEP, "\n" ); 
}

/**
 * Function: readFile( float *runningTotal, FILE *exisFile, struct Category
 * *catArr[] ) 
 * Parameters: exisFile - existing file that needs to be read 
 *             runningTotal - pointer to the running total  
 *             catArr - array of Categories to record information
 * Description: reads and records data into categories
 * Return: 0 if successful, 1 if not 
 * Error Conditions: if file is unable to be read, not in correct format
 */
int readFile( int *categoryCount, float *runningTotal, FILE *exisFile, 
              struct Category *catArr[] ) { 
  char *newLine; 
  char *separator; 
  char *category; 

  // grab newline from report 
  newLine = malloc( BUFSIZ ); 
  fgets( newLine, BUFSIZ, exisFile ); 
  if( strncmp( newLine, "\n", BUFSIZ ) != 0 ) {

    free( newLine );

    return -1;
  }

  // grab separator
  separator = malloc( BUFSIZ ); 
  fgets( separator, BUFSIZ, exisFile ); 
  if( strncmp( separator, FORMAT_SEP, BUFSIZ ) != 0 ) {

    free( newLine ); 
    free( separator ); 

    return -1; 
  }

  // free newline and separator 
  free( newLine );
  free( separator ); 

  // loop through categories specified and record them into array
  category = malloc( BUFSIZ ); 
  fgets( category, BUFSIZ, exisFile ); 
  while( strncmp( category, "\n", BUFSIZ ) != 0 ) { 
    
    // keep track of temporary information 
    struct Category *newCategory; 
    char *catName = malloc( BUFSIZ ); 
    strncpy( catName, strtok(category, " "), BUFSIZ ); 
    //catName = strtok( category, " " ); 
    char *amountStr = malloc( BUFSIZ );
    amountStr = strtok( NULL, " " ); 

    char *decimal; 
    char *endPtr; 
    int dollars = 0; 
    float cents = 0; 

    // allocate space for new category and record category name
    if( catName != NULL ) {
      newCategory = malloc( sizeof(struct Category) ); 
      newCategory->name = catName; 
      catArr[*categoryCount] = newCategory; 

    } else { 
      free( catName ); 
      free( amountStr ); 
      
      return -1; 
    }

    // remove dollar sign and convert to dollars and cents
    if( amountStr != NULL ) {
      amountStr++; 
      
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

      // record amount into array
      alterAmount( runningTotal, dollars, cents, catArr[*categoryCount] );

      // increment category count
      (*categoryCount)++; 

    } else {
      free( catName ); 
      free( amountStr ); 
      free( newCategory ); 

      return -1;
    }
    
    // save next line in string category 
    fgets( category, BUFSIZ, exisFile ); 
    free( amountStr ); 
  }

  free( category ); 

  return 0;
}

/**
 * Function: freeMemory( int categoryCount, struct Category *catArray[] ) 
 * Parameters: categoryCount - the number of categories recorded
 *             catArray - the array of categories
 * Description: frees all memory allocated to category array starting with
 * category names and then the struct categories themselves 
 * Return: void
 * Error Conditions: none
 */
void freeMemory( int categoryCount, struct Category *catArray[] ) {
  int i;
  
  for( i = 0; i < categoryCount; i++ ) { 
    free( catArray[i]->name );
    free( catArray[i] ); 
  }
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

  // checks validity of arguments 
  if( usage( argc, argv, &filePath ) == -1 ) {
    fprintf( stderr, "%s\n", USAGE );
    return EXIT_FAILURE;
  }  

  // import information from existing file, print error message and exit
  // otherwise
  if( filePath != NULL ) { 
    if( readFile( &numCategories, &runningTotal, filePath, categories ) != 0 ) {
      fprintf( stderr, BAD_FILE ); 
      return EXIT_FAILURE;
    } else {
      fprintf( stdout, FILE_IMPORTED, argv[1] );  
    }
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
      int i;
      struct Category *newCat; 
      struct Category *exisCat; 
      char *input; 
      char *newLine; 
      FILE *newFile;

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
            removeCategory( exisCat, categories, &numCategories, 
                            &runningTotal ); 
          } else {
            fprintf( stdout, NO_CATEGORY );
          }

          free( input ); 
          break;

        case 5: // view spending report
          
          // error message if there are no categories to print
          if( numCategories == 0 ) { 
            fprintf( stdout, NO_PRINT ); 

          // print out data
          } else { 
            printData( numCategories, runningTotal, categories, stdout ); 
          }
          break; 

        case 6: // export spending report
          input = malloc( BUFSIZ ); 

          // prompt user to enter a filename 
          fprintf( stdout, NEW_FILENAME ); 
          fgets( input, BUFSIZ, stdin ); 

          // remove newline character from filename 
          newLine = memchr( input, '\n', BUFSIZ ); 
          *newLine = '\0'; 

          // create new file and write report to it
          newFile = fopen( input, FILE_WRITE );  
          printData( numCategories, runningTotal, categories, newFile ); 

          free( input );
          break;
          
        case 7: // free all allocated memory and return EXIT_SUCCESS
          freeMemory( numCategories, categories );
          return EXIT_SUCCESS;
      }
    }

    // reprompt
    fprintf( stdout, "%s", PROMPT );
    fgets( input, BUFSIZ, stdin );
  }

  return EXIT_SUCCESS;
}
