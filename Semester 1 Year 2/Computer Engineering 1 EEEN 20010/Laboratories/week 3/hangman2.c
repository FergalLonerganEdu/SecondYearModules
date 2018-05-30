/* Program to copy a series of integers from a source file
 to a destination file. The copy process terminates when
 EOF is encountered in the source file */

#include <stdio.h>
#include <stdlib.h>

#define READONLY "r"
#define WRITEONLY "w"

typedef int Character;

int main(void)
{
 FILE *ip_file, *op_file;
 char filename[FILENAME_MAX + 1];
 int data, status;

 printf("\nEnter input file name: ");
 scanf("%s", filename);

 /* Open the input file for reading only */
 if( (ip_file = fopen(filename, READONLY)) == NULL){
 printf("\nmain: failed to open file %s.", filename);
 exit(EXIT_FAILURE);
 }

 printf("\nEnter output file name: ");
 scanf("%s", filename);

 /* Open the output file for writing only */
 if( (op_file = fopen(filename, WRITEONLY)) == NULL){
 printf("\nmain: failed to open file %s.", filename);
 exit(EXIT_FAILURE);
 }

 /* fscanf will attempt to assign data read from the input
 file to an integer */
 while((status = fscanf(ip_file, "%d", &data)) != EOF){
 /* If the number of assignments isn't 1, an error has occurred
 */
 if(status != 1){
 printf("\nmain: source file read failure.");
 exit(EXIT_FAILURE);
 }
 /* If fprintf fails, it returns a negative value */
 if(fprintf(op_file, "%d\n", data) < 0 ){
 printf("\nmain: destination file write failure.");
 exit(EXIT_FAILURE);
 }
 }

 fclose(ip_file);
 fclose(op_file);

 return(0);
}
