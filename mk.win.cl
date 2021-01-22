TARGET =cd.exe
FLAG= /WX /utf-8 /MP

SOURCES = \
	lib\argParse.c	\
	lib\stdio.c		\
	lib\gc.c		\
	lib\hmap.c 		\
	lib\string.c	\
	src\lexer.c     \	
	src\error.c		\
	src\main.c		\
	
all: $(TARGET)

$(TARGET):$(SOURCES)
	cl $(FLAG) /Febin\$(TARGET) $(SOURCES)
	
clean:
		del bin\*.exe & del obj\*.obj & del lib\*.obj
		del *.exe & del *.obj     
		del *.lexer & del tst\*.lexer
		del *.parser & del tst\*.parser        
		del *.output & del tst\*.output
		del *.ast & del tst\*.ast
		del *.node & del tst\*.node    
		del *.asm & del tst\*.asm        
		del *.vm & del tst\*.vm 
 		  
