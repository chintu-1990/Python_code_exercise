import subprocess
from subprocess import PIPE,Popen
def call_subprocess(command):
    proc = Popen(command.split(" "), stdout=PIPE, stderr=PIPE)
    try:
        output,error=proc.communicate()
        result_op=output.decode("utf-8")
        result_error = error.decode("utf-8")
        return result_op,result_error
    except:
        print("Some error so process getting quit")
        proc.kill()
        quit()


