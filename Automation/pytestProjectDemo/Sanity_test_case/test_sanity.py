#This test script is for sanity run on different math calculation
from module_script.calculation_module import addition,subtraction,division,multiplication
from module_script.sub_process_call import call_subprocess
from module_script.logger_use import self_log
from logging import *
Log_format = '{name}*** {asctime} *** {message} *** {lineno}'
basicConfig(filename="testlogs.log", level=DEBUG, filemode="w", style='{', format=Log_format)
#logging.basicConfig(__name__)
Logger=getLogger("chintu")
inp1=3
inp2=5
cmd="ping google.com"
def test_add():
    output=addition(inp1,inp2)
    Logger.debug(output)
    assert True
    #print("My o/p is",output)
def test_sub():
    output=subtraction(inp1,inp2)
    Logger.debug(output)
    assert True
    #print("My o/p is",output)
def test_div():
    output=division(inp1,inp2)
    Logger.debug(output)
    assert True
    #print("My o/p is",output)
def test_mul():
    output=multiplication(inp1,inp2)
    Logger.info(output)
    assert True
    #print("My o/p is",output)
def test_pingcmd():
    output=call_subprocess(cmd)
    Logger.info(output)
    #print("Output:",output)
    assert True


