// Cristian McGee (Only Member) COP3402 Systems Software Virtual Machine
// Homework Assignment One
#include <stdio.h>

#define ARRAY_SIZE 500 // PAS SIZE

int PC = 10;  // PC Program Counter starts at 10
int BP = 499; // Base Pointer starts at 499
int SP = 500; // Stack Pointer starts at 500

int pas[ARRAY_SIZE]; // PAS program address space

typedef struct InstructionRegister {
  int OP; // Operation Code
  int L;  // Lexicographical Level
  int M;  // Standard M (A Number, Address, or Identity)
} InstructionRegister;

InstructionRegister IR; // The Instruction Register

int base(int BP, int L) {
  // function given by professor to help determine the base
  int arb = BP;
  while (L > 0) {
    arb = pas[arb];
    L--;
  }
  return arb;
}

FILE *ofp; // global variable to help with printing to the output

int numActivationRecords = 0; // number of activation records (other than the
                              // first automatic activation record)

int activationRecordDividers
    [(ARRAY_SIZE - 50) /
     4]; // array to help with printing the activation records, if we assume
         // each activation record is 4 to give us our maximum amount of
         // activation records (considering it can hold our return address,
         // static link, and base, and enough space for one more value to
         // actually do something with a given activation record). Additionally
         // we assume 50 spots just for the text segment before the stack
         // segment.

void printLine(char code[]) {
  // prints the current type of operation, the lexicographical level, the M
  // value, the PC, BP, and SP
  printf("  %s  %d  %d   \t%d\t %d  %d  \t", code, IR.L, IR.M, PC, BP, SP);
  fprintf(ofp, "  %s  %d  %d   \t%d\t %d  %d  \t", code, IR.L, IR.M, PC, BP, SP);

  int j = 0; // value I will use to determine where to print the activation records

  for (int i = ARRAY_SIZE; i > SP; i--) {
    // our stack goes down from the top of the array and we go essentially to
    // where the stack pointer is
    if (j < numActivationRecords) {
      // if we haven't printed all of the differences in activation records, we
      // print the activation record dividers by checking the spot
      if (i == activationRecordDividers[j]) {
        printf(" | ");
        fprintf(ofp, "  |  ");
        j++; // we move on to the next activation record
      }
    }
    printf("%d ", pas[i - 1]); // prints the value at the current spot in the stack
    fprintf(ofp, "%d ", pas[i - 1]);
  }
  printf("\n"); // new line
  fprintf(ofp, "\n");
}

int main(int argc, char *argv[]) {
  
  if (argc != 3 && argc != 2) {
    fprintf(stderr, "Error, not accurate arguments, needs an input and output file only\n");
    return 1;
  }

  // initializing all of the elements in the process address space to 0
  for (int i = 0; i < ARRAY_SIZE; i++) {
    pas[i] = 0;
  }

  // FILE POINTER to get input and output
  FILE *ifp = fopen(argv[1], "r");
  if (argc == 3){
    ofp = fopen(argv[2], "w");
  } else {
    ofp = fopen("defaultOutput.txt", "w");
  }
  

  // Filling in pas with the instructions before we can start with pc and
  // whatnot
  int s = 10;
  while (fscanf(ifp, "%d", &pas[s]) != EOF) {
    s += 1;
  }

  // we will be using these variables to determine if we are currently fetching,
  // executing, and if the code is over
  int fetch = 1;
  int execute = 0;
  int halt = 0;

  // basic print statements to help sort of colummnize the output
  fprintf(ofp, "                PC   BP   SP   stack\n");
  printf("                PC   BP   SP   stack\n");
  fprintf(ofp, "Initial values: %d   %d  %d\n\n", PC, BP, SP);
  printf("Initial values: %d   %d  %d\n\n", PC, BP, SP);

  // until we are "halted" the program keeps running
  while (!halt) {
    // if we are in the fetch phaset we will fetch the next instruction
    if (fetch) {
      IR.OP = pas[PC];
      IR.L = pas[PC + 1];
      IR.M = pas[PC + 2];
      PC += 3;
      // swtich from fetch to execute, operations based on above and pc also
      // increments
      fetch = 0;
      execute = 1;
    }
    // if we are in the execute phase we will execute the next instruction by
    // using a switch statement
    if (execute) {
      switch (IR.OP) {
      case 1:
        pas[SP - 1] = IR.M;
        SP = SP - 1;
        printLine("LIT");
        // if operation code is one we just load the M value to the top of the
        // stack but we are going downwards so we are subtracting
        break;

      case 2:
        switch (IR.M) {
        case 0:
          SP = BP + 1; // we are going back an activation record so we are
                       // putting the sp behind the bp
          BP = pas[SP - 2]; // we are getting our previous bp and pc values back
                            // as well
          PC = pas[SP - 3];
          numActivationRecords =
              numActivationRecords > 0
                  ? numActivationRecords - 1
                  : 0; // I don't want to go under zero just in case,
                       // essentially we are substracting one from the number of
                       // activation records
          printLine("RTN");
          break;
        case 1:
          SP = SP + 1;
          pas[SP] = pas[SP] + pas[SP - 1];
          // adds the top two values and increments back
          printLine("ADD");
          break;
        case 2:
          SP = SP + 1;
          pas[SP] = pas[SP] - pas[SP - 1];
          // subtracts the top two values and incrememnts back
          printLine("SUB");
          break;
        case 3:
          SP = SP + 1;
          pas[SP] = pas[SP] * pas[SP - 1];
          // mutliplies the top two values and increments back
          printLine("MUL");
          break;
        case 4:
          SP = SP + 1;
          pas[SP] = pas[SP] / pas[SP - 1];
          // divides the top two values and increments back
          printLine("DIV");
          break;
        case 5:
          SP = SP + 1;
          pas[SP] = pas[SP] == pas[SP - 1];
          // compares the top two values and increments back
          printLine("EQL");
          break;
        case 6:
          SP = SP + 1;
          pas[SP] = pas[SP] != pas[SP - 1];
          // compares the top two values and checks if not equal and increments
          // back
          printLine("NEQ");
          break;
        case 7:
          SP = SP + 1;
          pas[SP] = pas[SP] < pas[SP - 1];
          // checks if one is less than the other and increments back
          printLine("LSS");
          break;
        case 8:
          SP = SP + 1;
          pas[SP] = pas[SP] <= pas[SP - 1];
          // checks if one is less than or equal to the other and increments
          // back
          printLine("LEQ");
          break;
        case 9:
          SP = SP + 1;
          pas[SP] = pas[SP] > pas[SP - 1];
          // checks if one is greater than the other and increments back
          printLine("GTR");
          break;
        case 10:
          SP = SP + 1;
          pas[SP] = pas[SP] >= pas[SP - 1];
          // checks if one is greater than or equal to the other and increments
          // back
          printLine("GEQ");
          break;
        }
        break;

      case 3:
        SP = SP - 1;
        pas[SP] = pas[base(BP, IR.L) - IR.M];
        // loads the value at the base pointer minus the M value into the top of
        // the stack
        printLine("LOD");
        break;

      case 4:
        pas[base(BP, IR.L) - IR.M] = pas[SP];
        SP = SP + 1;
        // stores the value at the top of the stack into the base pointer minus
        // the M value
        printLine("STO");
        break;

      case 5:
        pas[SP - 1] = base(BP, IR.L);
        pas[SP - 2] = BP;
        pas[SP - 3] = PC;
        BP = SP - 1;
        PC = IR.M;
        activationRecordDividers[numActivationRecords] = SP;
        numActivationRecords++;
        // creates a new activation record and moves all previous return address
        // and links accordingly
        printLine("CAL");
        break;

      case 6:
        SP -= IR.M;
        printLine("INC");
        // increments the stack pointer by the M value
        break;

      case 7:
        PC = IR.M;
        printLine("JMP");
        // jumps the PC to the M value
        break;

      case 8:
        if (pas[SP] == 0) {
          PC = IR.M;
          // jumps the PC to the M value if the top of the stack is 0
        }
        SP = SP + 1;
        printLine("JPC");
        break;

      case 9:
        switch (IR.M) {
        case 1:
          //Stack decreases by 1 and prints the output
          printf("Output result is: %d\n", pas[SP]);
          fprintf(ofp, "Output result is: %d\n", pas[SP]);
          SP += 1;
          break;
        case 2:
          //Stack Increases by 1 and reads input to the top of the stack
          printf("Please Enter an Integer: ");
          scanf("%d", &pas[SP - 1]);
          fprintf(ofp, "Please Enter an Integer: %d\n", pas[SP]);
          SP -= 1;
          break;
        case 3:
          //halt p machine
          halt = 1;
          break;
        }
        printLine("SYS");
        break;
      }
      execute = 0;
      fetch = 1;
      // transfer back to the fetch phase
    }
  }
  // close the file pointers
  fclose(ifp);
  fclose(ofp);
  return 0;
}