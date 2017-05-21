from timeit import default_timer as timer
# from subprocess import call
import subprocess
import os



original_cmd = "/SIP3/coolprogram32"
original_devnull_cmd = "/SIP3/coolprogram32_2"
patched_cmd = "/SIP3/coolprogram32_instrumented_patched"
patched_devnull_cmd = "/SIP3/coolprogram32_instrumented_patched2"

cmds = [original_cmd, original_devnull_cmd, patched_cmd, patched_devnull_cmd]

run_counts = [1, 10, 100, 1000, 10000, 100000, 1000000]
# run_counts = [1, 10, 100]

orig_averages = [0]*7
orig_devnull_averages = [0]*7
patched_averages = [0]*7
patch_devnull_averages = [0]*7

devnull = open(os.devnull, 'w')

# start = timer()
for cmd in cmds:
	print("Running " + cmd)
	for count in run_counts:
		count_sum = 0
		for i in range(count):	
			if(cmd == patched_devnull_cmd or cmd == original_devnull_cmd):
				out = devnull
				err = subprocess.STDOUT
			else:
				out = None
				err = None
						
			start = timer()
			subprocess.call(cmd, stdout=out, stderr=err)			
			end = timer()
			
			t = end-start
			count_sum += t

		count_average = count_sum / count
		
		if(cmd == original_cmd):
			orig_averages[run_counts.index(count)] = count_average
		elif(cmd == original_devnull_cmd):
			orig_devnull_averages[run_counts.index(count)] = count_average
		elif(cmd == patched_cmd):
			patched_averages[run_counts.index(count)] = count_average
		elif(cmd == patched_devnull_cmd):
			patch_devnull_averages[run_counts.index(count)] = count_average
		else:
			print("Something weird happened")			
			exit()
		
print(orig_averages)
print(orig_devnull_averages)
print(patched_averages)
print(patch_devnull_averages)
