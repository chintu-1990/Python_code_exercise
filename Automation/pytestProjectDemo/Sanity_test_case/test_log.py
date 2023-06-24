#import logging
import logging
from logging import *
logging.basicConfig(
    filename='test.log',
    filemode='w',
    level=logging.INFO,
    format = '%(asctime)s - %(levelname)s: %(message)s'
)


def test_logging():
    for i in range(10**2):
        print('aaa')
        logging.critical('bbb')
        logging.debug('ccc')
        logging.error("errr")
    assert True