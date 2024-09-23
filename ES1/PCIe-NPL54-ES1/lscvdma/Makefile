# Top level Makefile responsible for build Lib,.ko, App demo binary

all:
	$(MAKE) -C lib/
	$(MAKE) -C drvr/
	$(MAKE) -C app/GUI/AVCapture/

driver:
	$(MAKE) -C drvr/
libs:
	$(MAKE) -C lib/

application:
	$(MAKE) -C lib/
	$(MAKE) -C app/GUI/AVCapture/

clean:
	$(MAKE) clean -C lib/
	$(MAKE) clean -C drvr/
	$(MAKE) clean -C app/GUI/AVCapture/
