CC = g++
OBJECTS = new_example.o
PROJECT = new_example
CFLAGS = -c -Wall -O2
LFLAGS = -Wall

$(PROJECT): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(PROJECT) $(LFLAGS)

# compile just the new_example.o object
new_example.o: new_example.cpp
	$(CC) new_example.cpp -o new_example.o $(CFLAGS)
clean:
 	@rm $(OBJECTS) $(PROJECT) -f
# @echo "Everything is clean now!"
