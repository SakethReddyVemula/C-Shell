SOURCE_FILES = ./main/main.c ./main/prompt.c ./main/processInput.c ./main/hop.c ./main/reveal.c ./main/log.c ./main/proclore.c ./main/seek.c ./main/sysCmnds.c ./main/bgHandler.c ./main/fgHandler.c ./main/myshrc.c ./main/signals.c ./networking/iMan.c

all:
	gcc ${SOURCE_FILES}

clean:
	rm -f ./a.out .log *.txt