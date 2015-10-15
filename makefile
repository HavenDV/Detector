all:	main.cpp
	gcc -std=c++1y main.cpp detect.cpp -o detect -ggdb `pkg-config --cflags --libs opencv` -L/usr/lib64 -lstdc++ -lm -lboost_filesystem -lboost_system -lboost_program_options
install: 
	install ./detect /usr/bin

