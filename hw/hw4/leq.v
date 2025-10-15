
myROM[0] = {movi, RXX, Rg0, 4'b0111}; 
myROM[1] = {movi, RXX, Rg1, 4'b0010}; 
myROM[2] = {cmp, Rg0, Rg1, 4'b0000}; 
myROM[3] = {jz,  RXX, RXX, 4'b1000}; 
myROM[4] = {cmpi, RXX, Rg0, 4'b0000};
myROM[5] = {jz,  RXX, RXX, 4'b0110}; 
myROM[6] = {cmpi, RXX, Rg1, 4'b0000};
myROM[7] = {jz, RXX, RXX, 4'b0110};  
myROM[8] = {subi, RXX, Rg0, 4'b0001}; 
myROM[9] = {subi, RXX, Rg1, 4'b0001}; 
myROM[10] = {jmp, RXX, RXX, 4'b1001}; 
myROM[11] = {movi, RXX, Rg2, 4'b0000};
myROM[12] = {jmp, RXX, RXX, 4'b0000}; 
myROM[13] = {movi, RXX, Rg2, 4'b0001};
myROM[14] = {jmp, RXX, RXX, 4'b0000}; 
myROM[15] = {jmp, RXX, RXX, 4'b0000}; 


