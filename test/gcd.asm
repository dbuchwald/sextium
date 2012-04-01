# Program gcd.asm
# Wczytuje dwie liczby i wypisuje
# ich najwi¦kszy wspólny dzielnik
#
        CONST x # czytaj x
        SWAPA
        READ
        STORE
        CONST y # czytaj y
        SWAPA
        READ
        STORE
dalej:  CONST y # czy y=0?
        SWAPA
        LOAD
        SWAPD
        CONST koniec
        SWAPA
        SWAPD
        BRANCHZ
        SWAPD # z=x/y
        CONST x
        SWAPA
        LOAD
        DIV
        MUL # z=z*y
        SWAPD # z=x-z
        CONST x
        SWAPA
        LOAD
        SUB
        SWAPD
        CONST z
        SWAPA
        SWAPD
        STORE
        CONST y # x=y
        SWAPA
        LOAD
        SWAPD
        CONST x
        SWAPA
        SWAPD
        STORE
        CONST z # y=z
        SWAPA
        LOAD
        SWAPD
        CONST y
        SWAPA
        SWAPD
        STORE
        CONST dalej
        JUMP
koniec: CONST x # pisz x
        SWAPA
        LOAD
        WRITE
        HALT
x:      DATA # zmienne
y:      DATA
z:      DATA
        
