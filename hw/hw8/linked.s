movi $7, head       #Holds address of curr node
lw $3, 0($7)        #Holds data of curr node
lw $4, 1($7)        #Holds next of curr node

loop:
    add $1, $1, $3  #Add curr node data to sum of nodes
    jeq $4, $0, end #Check if end of Linked list
    lw $3, 0($4)    #Set data of next node
    movi $7, 0      #Reset address
    add $7, $7, $4  #Set address
    lw $4, 1($7)    #Set next curr node
    j loop          #Loop again


end:
    halt


head:
    .fill 34
    .fill chain1
    .fill 82
    .fill 10
chain1:
    .fill 23
    .fill chain2
chain2:
    .fill 0
    .fill chain3
chain3:
    .fill 4
    .fill chain4
    .fill 229
    .fill 449
chain4:
    .fill 100
    .fill chain5
    .fill 300
chain5:
    .fill 12
    .fill 0
    .fill 9999
chain6:
    .fill 99
    .fill 59
chain7:
    .fill 16384
    .fill 909
    .fill 0
