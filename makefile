preprocess:
	g++ preprocess.cpp -o preprocess.exe

clean:
	del *.exe
	del *.obj
	del *.o