### VARIABLES ###
CC = gcc
BIN_DIR = bin/
CFLAGS = -Wall \
         -ansi \
         -pedantic

EXE_CLT = miniftp_clt
EXE_SERV = miniftp_serv

EXE = miniftp_clt miniftp_serv


OFILES = ${BIN_DIR}tea.o \
		 ${BIN_DIR}diffie_hellman.o \
		 ${BIN_DIR}request.o \
		 ${BIN_DIR}common.o \


but : ${EXE}

# binary code
${BIN_DIR}client.o : client.h client.c
	${CC} -c client.c -o ${BIN_DIR}client.o

${BIN_DIR}common.o : common.h common.c
	${CC} -c common.c -o ${BIN_DIR}common.o

${BIN_DIR}diffie_hellman.o : diffie_hellman.h diffie_hellman.c ${BIN_DIR}common.o
	${CC} -c diffie_hellman.c -o ${BIN_DIR}diffie_hellman.o

${BIN_DIR}request.o : request.h request.c ${BIN_DIR}tea.o ${BIN_DIR}diffie_hellman.o ${BIN_DIR}common.o
	${CC} -c request.c -o ${BIN_DIR}request.o

${BIN_DIR}tea.o : tea.h tea.c ${BIN_DIR}common.o
	${CC} -c tea.c -o ${BIN_DIR}tea.o

${BIN_DIR}miniftp_clt.o : miniftp_clt.c ${BIN_DIR}tea.o ${BIN_DIR}common.o ${BIN_DIR}request.o ${BIN_DIR}client.o
	${CC} -c miniftp_clt.c -o ${BIN_DIR}miniftp_clt.o

${BIN_DIR}miniftp_serv.o : miniftp_serv.c ${BIN_DIR}server.o ${BIN_DIR}common.o
	${CC} -c miniftp_serv.c -o ${BIN_DIR}miniftp_serv.o

${BIN_DIR}server.o : server.h server.c
	${CC} -c server.c -o ${BIN_DIR}server.o


# executables
miniftp_clt : ${OFILES} ${BIN_DIR}miniftp_clt.o ${BIN_DIR}client.o
	$(CC) $(CFLAGS) -o ${EXE_CLT} ${OFILES} ${BIN_DIR}miniftp_clt.o ${BIN_DIR}client.o

miniftp_serv: ${OFILES} ${BIN_DIR}miniftp_serv.o ${BIN_DIR}server.o
	${CC} ${FLAGS} -o ${EXE_SERV} ${OFILES} ${BIN_DIR}miniftp_serv.o ${BIN_DIR}server.o


clean :
	-rm -f ${OFILES}

mrproper : clean but

.PHONY : but clean mrproper