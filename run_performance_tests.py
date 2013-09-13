from math import sqrt
import random
import sys
import subprocess
import time

versions = ['deque_NO_RESERVE', 'list_NO_RESERVE', 'vector_NO_RESERVE']

def step(array_size):
	return 1000000

for array_size in ['500', '1000']:
	skip = {}
	for version in versions:
		skip[version] = False
	for iteration in range(step(array_size), 100000000, step(array_size)):
		random.shuffle(versions)
		for version in versions:
			if skip[version] == True:
				continue
			print version + '\t' + array_size + '\t' + str(iteration) + '\t',
			sys.stdout.flush()
			name = './performance_' + version + '_' + array_size
			start = time.time()
			subprocess.call([name, str(iteration)])
			end = time.time()
			if end - start > 100.5:
				skip[version] = True
	print
