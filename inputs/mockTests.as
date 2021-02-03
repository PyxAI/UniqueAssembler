; line starts with this
just a line without a ;
	 	 	 	
cmp 4 #14
cmp 4, #14
cmp #4, #14
tests: jsr r3 
jmp NUM
illegal,string: "Hello"
illegal,label: .string "Hello"
LABEL: .extern test
longlabellonglabellonglabellonglabellonglabellonglabellonglabel: add r1 r2
Main: .string "abc"
Main: .data  5
.entry Main
lea r2 , r5
string: mov 5 , r8
r1: add r1, r1
stop operand
rts operand
rts r1
EMPTY:
LABEL: .data 2  , 1, 1 
TESTS: .data , 2,   -12 ,  2
TESTS: .data 1 ,, -22 , 45
LAST: .data 1,
isdata: .DATA 41
add Main#5
sub r3,   #4
sub r2, # -1
.string "bad string
.string badstring"
.string "good string"
clr r11
r12: dec *r6
toomany: add #1,r1 r2
toomany: sub #1,r1, r2
14badlabel: add 4, r1
label1: label2: label3: add r1 r1
jmp123: jmp r2 , *r6
jmp: red r7
prn 15
.extern Main