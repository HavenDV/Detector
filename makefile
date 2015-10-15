all:	main.cpp
	gcc -std=c++1y main.cpp -o detect -ggdb `pkg-config --cflags --libs opencv` -L/usr/lib64 -lstdc++ -lm -lboost_filesystem -lboost_system
install: 
	install ./detect /usr/local/bin

