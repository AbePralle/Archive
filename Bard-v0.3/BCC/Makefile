all: compile

compile:
	mkdir -p Program
	iced --compile --runtime inline --output Program Source/*.iced
	node Program/BCC.js
	@echo cat output.c
	@echo -------------------------------------------------------------------------------
	@cat output.c
	@echo -------------------------------------------------------------------------------

run: compile
	gcc output.c
	./a.out


clean:
	rm Program/*.js
