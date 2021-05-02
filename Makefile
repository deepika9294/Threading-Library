default : run

run: 
	cd one-one && $(MAKE) run
	cd many-one && $(MAKE) run

clean :
	cd one-one && $(MAKE) clean
	cd many-one && $(MAKE) clean

.PHONY : run clean