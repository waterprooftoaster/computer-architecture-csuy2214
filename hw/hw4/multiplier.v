myROM[0] = {movi, RXX, Rg0, 4'b0111}; //store value 7 into Rg0
myROM[1] = {movi, RXX, Rg1, 4'b0010}; //store value 2 into Rg1
myROM[2] = {movi, RXX, Rg2, 4'b0000}; //store value 0 into Rg2
myROM[3] = {cmpi, RXX, Rg0, 4'b0000}; //check if Rg0 is 0
myROM[4] = {jz,  RXX, RXX, 4'b1000};  //jump to set Rg2 to 0 
myROM[5] = {cmpi, RXX, Rg1, 4'b0000}; //check if Rg1 is 0
myROM[6] = {jz,  RXX, RXX, 4'b0110};  //jump to set Rg2 to 0  
myROM[7] = {cmpi, RXX, Rg0, 4'b0000}; //CHECK if Rg0 == 0
myROM[8] = {jz, RXX, RXX, 4'b0101};   //JUMP if TRUE to 12 to end
myROM[9] = {subi, RXX, Rg0, 4'b0001}; //SUBTRACT 1 from Rg0
myROM[10] = {add, Rg1, Rg2, 4'b0001};  //ADD value of Rg1 to Rg2, and store sum into Rg2
myROM[11] = {jmp, RXX, RXX, 4'b1100}; //JUMP to 
myROM[12] = {movi, RXX, Rg2, 4'b0000};//SET Rg2 to 0
myROM[13] = {jmp, RXX, RXX, 4'b0000}; //HALT
myROM[14] = {jmp, RXX, RXX, 4'b0000}; //HALT
myROM[15] = {jmp, RXX, RXX, 4'b0000}; //HALT
