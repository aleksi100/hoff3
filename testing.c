#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
  if(strcmp(argv[1], argv[2])==0){
    printf("Test passed!! :)\n");
  }else{
    printf("Test failed!! :(\n");
  }

}