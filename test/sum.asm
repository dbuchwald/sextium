       CONST end
       SWAPA
start: READ
       BRANCHZ
       ADD
       SWAPD
       CONST start
       JUMP
end:   SWAPD
       WRITE
       HALT
