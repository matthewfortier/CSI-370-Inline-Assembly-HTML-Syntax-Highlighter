; Assignment 8.2 Orbital Velocity
; Author: Matthew Fortier
; Description: Calculates orbital velocity with given distance from the Earth
; Running MASM Assembler on Windows
; Using Visual Studio assemble, run, and debug code
; I certify that this is my own work

_getDouble PROTO						; declare external functions
_printString PROTO
_printDouble PROTO

.data
align 16								; align data on 16-byt boundary
G			REAL8 6.6741E-11			; constants in scientific notation
me			REAL8 5.9722E24
distance    REAL8 6.371E6
mph			REAL8 0.44704
exitNum		REAL8 0.0
radius	    REAL8 0.0
ovelocity	REAL8 0.0
										; display messages
endMessage BYTE "Continue? (1/0): ", 0
radiusMessage BYTE "Enter distance(m) of object from surface of the Earth: ", 0
msMessage BYTE "Orbital Velocity(m/s): ", 0
mphMessage BYTE "Orbital Velocity(mph): ", 0

.code
_asmMain PROC 
	
	push rbp
	sub rsp, 20h
	lea rbp, [rsp + 20h]				; align the stack pointer

	velocity:

		lea rcx, radiusMessage			; load string to be printed
		call _printString				; print string to stdout
		call _getDouble					; use C++ function to get double

		addsd xmm0, distance			; getDouble puts return value in xmm0, add the radius of the earth

		movsd radius, xmm0				; mov the add result to radius variable

		lea rcx, msMessage
		call _printString

		movsd xmm0, G					; load gravitational constant
		mulsd xmm0, me					; multiply it against the mass of the eart

		divsd xmm0, radius				; divide by the calculated radius

		sqrtsd xmm0, xmm0				; square root the result

		movsd ovelocity, xmm0			; mov the result into the appropriate variable

		call _printDouble				; print the results in (m/s)

		lea rcx, mphMessage
		call _printString				

		movsd xmm0, ovelocity			
		divsd xmm0, mph					; divide velocity by mph constant

		call _printDouble

		lea rcx, endMessage
		call _printString
		call _getDouble

		movsd xmm1, xmm0				; mov the given double to xmm1 for comparison
		movsd xmm2, exitNum				; compare against 0 constant
		comisd xmm1, xmm2				; compare the two float registers and set the flags
		je endProgram					; end if equal

		jmp velocity					; repeat

	endProgram:
	
	lea rsp, [rbp]						; clean up stack and end
	pop rbp

	ret
_asmMain ENDP
END