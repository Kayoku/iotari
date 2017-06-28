import sys
import glob
import serial
import struct


def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def read_message(serial):
    ba = bytearray()

    while True:
        read_byte = serial.read()
        if not read_byte:
            break
        ba.append(ord(read_byte))
    if len(ba) == 0:
        return

    print(ba)

    id_ = ba[0]
    if id_ == 1:
        print("id 1!")
        # h : arduino int / i : arduino long
        format_ = "=bhlh" # char :id -- int : a -- long: b -- int: c (1+2+4+2)
    if id_ == 2:
        print("id 2!")
        # h : arduino int / i : arduino long
        format_ = "=bhhh" # char :id -- int : a -- long: b -- int: c (1+2+4+2)

    print(len(ba))
    print(format_)
    data = struct.unpack(format_, ba)
    print(data)

    print("===========================")


if __name__ == '__main__':
    print(serial_ports())
    for ser_port in serial_ports():
        ser = serial.Serial(ser_port, timeout=3)
        for _ in range(3):
            read_message(ser)
