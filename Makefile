default : run

run: 
	cd one-one && $(MAKE) --no-print-directory run
	cd many-one && $(MAKE) --no-print-directory run

clean :
	cd one-one && $(MAKE) --no-print-directory clean
	cd many-one && $(MAKE) --no-print-directory clean
