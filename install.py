# installer for the bootstrap skin.
#
# Based on installer for xstats
#
# Configured by morrowwm to install Arduino driver

import os.path
import configobj

import setup
import distutils

def loader():
    return wxArduinoInstaller()

class wxArduinoInstaller(setup.ExtensionInstaller):
    _driver_conf_files = ['wxArduino/bin/weewx/drivers/wxArduio.py']

    def __init__(self):
        super(wxArduinoInstaller, self).__init__(
            version="0.1",
            name='wxArduino',
            description='A driver for Arduino-based weather station',
            author="Bill Morrow",
            author_email="morrowwm@yahoo.ca",
            config={
                'wxArduino': {
                        'port':'/dev/ttyUSB21',
                        'HTML_ROOT':'wxArduino'}}},

            files=[('bin/weewx/drivers',
                    ['bin/weewx/drivers/wxArduino.py'
                     )
                   ]
            )

