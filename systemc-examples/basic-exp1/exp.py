#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from subprocess import *

#==============================#
# main :					   #
#  entry point				   #
#==============================#
def main(argv=None):
	
	be = "run"
	file = "main.linked.bc"
	pinavm = "../../toplevel/pinavm"

	nb = 100
	sum = 0
   	for i in range(0, nb):
   		p =  Popen([pinavm,"-b",be,file,"-dis-opt-msg", "-dis-dbg-msg"], stdout=PIPE)
		while True:
			o = p.stdout.readline()
   			if o == '' and p.poll() != None: break
   			sum = sum + int(o)
   	result = sum/nb
	print result

if __name__ == "__main__":
	sys.exit(main())
	
