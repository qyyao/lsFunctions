all:
	gcc main.c lsFunctions.c -o myls -Wall 

clean:
	rm myls