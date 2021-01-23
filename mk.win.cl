TARGET =cd.exe
FLAG= /WX /utf-8 /MP

SOURCES = \
	lib\argParse.c	\
	lib\stdio.c		\
	lib\gc.c		\
	lib\hmap.c	\
	lib\string.c	\
	src\lexer.c     \
	src\error.c		\
	src\main.c		\
	
all: $(TARGET)

$(TARGET):$(SOURCES)
	cl $(FLAG) /Febin\$(TARGET) $(SOURCES)
	
clean:
		del bin\*.exe >nul 2>&1 	& del obj\*.obj >nul 2>&1 	& del lib\*.obj >nul 2>&1
		del *.exe >nul 2>&1 		& del *.obj >nul 2>&1
		del *.lexer >nul 2>&1 		& del tst\*.lexer >nul 2>&1
		del *.parser >nul 2>&1 		& del tst\*.parser  >nul 2>&1
		del *.output >nul 2>&1 		& del tst\*.output >nul 2>&1
		del *.ast >nul 2>&1 		& del tst\*.ast >nul 2>&1
		del *.node >nul 2>&1 		& del tst\*.node >nul 2>&1
		del *.asm >nul 2>&1 		& del tst\*.asm >nul 2>&1
		del *.vm >nul 2>&1		& del tst\*.vm >nul 2>&1
		del *.symTable >nul 2>&1 	& del tst\*.symTable >nul 2>&1
		del *.out >nul 2>&1 		& del tst\*.out >nul 2>&1

