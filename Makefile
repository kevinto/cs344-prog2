#########################################################
# File: Makefile
# # Description: A makefile for CS344.
# #########################################################

CXX = gcc

SRC1 = toke.adventure.c
SRCS = ${SRC1}

PROG1 = a 
PROGS = ${PROG1}

default:
	${CXX} ${SRCS} -g -Wall -o ${PROG1}

clean:
	rm -rf ${PROGS} *.o *~ *rooms*
