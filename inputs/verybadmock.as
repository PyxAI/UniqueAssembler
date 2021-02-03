.entry LIST
.extern fn1
MAIN: add r3,LIST
add * r3, LIST
;warning 1
  ; this should be ignored also
        	 	 	 	 	 
add: r3 r2
; last two lines too        
\n 
;warning 2 (wtf is that line)

jsr fn1
LOOP: prn #48
lea STR,r6
inc r6
mov *r6, L3
sub r1, r4
cmp r3,#-6
bne END
add r7, *r6
clr K
sub L3,L3
.entry MAIN
jmp LOOP
END: stop
STR: .string "abcd"
LIST: .data -33333, -9
.data -100
LIST: .string "eff"
K: .data 31
.extern L3
.extern NOGOOD