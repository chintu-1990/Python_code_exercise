from logging import *
#Youtube tutorial logger :https://www.youtube.com/watch?v=aCk-qE_WQJ4
#debug("This is debug")
"""Default loggere set is root and so you will see logging from warning
 to above like error critical etc but will not see debug,info"""
"""warning("This is warning")
error("This is error")
critical("This is critical")"""
#creat a file
"""basicConfig(filename="testlogs.log")
#creat log file in cwd else need to give path for specific directory
warning("This is warning")
error("This is error")
critical("This is critical")"""
#basicConfig(filename="testlogs.log")

#basicConfig(filename="testlogs.log",level=DEBUG,filemode="w",format='%(asctime)s -- %(message)s')

#in log to get tme date format
#log level can set like debug,info,warning etc.
# file mode can set to w mode or  default file mode is append
#creat log file in cwd else need to give path for specific directory
#Log_format='%(asctime)s // %(message)s // %(lineno)d'
#basicConfig(filename="testlogs.log",level=DEBUG,filemode="w",format=Log_format)
#change % { style
"""Log_format='{name}*** {asctime} *** {message} *** {lineno}'
basicConfig(filename="testlogs.log",level=DEBUG,filemode="w",style='{' ,format=Log_format)
debug("This is debug")
info("This is info")
warning("This is warning")
error("This is error")
critical("This is critical")"""
#use get logger from root another self define
"""logger=getLogger("Amit_unit_test")
Log_format='{name}*** {asctime} *** {message} *** {lineno}'
basicConfig(filename="testlogs.log",level=DEBUG,filemode="w",style='{' ,format=Log_format)
logger.debug("This is debug")
logger.info("This is info")
logger.warning("This is warning")
logger.error("This is error")"""
#try in your unit test by defining a function
logger = getLogger("Amit_unit_test")
Log_format = '{name}*** {asctime} *** {message} *** {lineno}'
basicConfig(filename="testlogs.log", level=DEBUG, filemode="w", style='{', format=Log_format)
logger.debug("This is debug")
logger.info("This is info")
logger.warning("This is warning")
logger.error("This is error")


