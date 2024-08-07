IN  = f1.txt
OUT = f2.txt

bin/main.out: 3.c
	gcc 3.c -o main.out

run:
	./main.out ${IN} ${OUT}

check:
	cat ${IN} ${OUT}

# Target to clean up the object files and executable
clean:
	rm main.out
