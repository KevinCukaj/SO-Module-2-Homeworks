PATTERN = txt

bin/main.out: 1.c
	gcc 1.c -o main.out

run:
	./main.out -P ${PATTERN} . ../

# Target to clean up the object files and executable
clean:
	rm main.out
