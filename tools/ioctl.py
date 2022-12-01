from fcntl import ioctl
from enum import IntEnum
import os

CMD_ID = 100


class TSL2561IoctlOp(IntEnum):
    WR = 0
    RD = 1


class TSL2561Regs(IntEnum):
    ID = 0x0A


class TSL2561DriverBusHandler:

    def __init__(self, file_dev_name: str) -> None:
        if not os.path.exists(file_dev_name):
            raise SystemError('Device or Kernel Module unavailable.')
        self.dev_name = file_dev_name

    def write_reg(self, reg: TSL2561Regs, val):
        self.__xfer(TSL2561IoctlOp.WR, reg, val)

    def read_reg(self, reg: TSL2561Regs) -> int:
        return self.__xfer(TSL2561IoctlOp.RD, reg)

    def __xfer(self,
               cmd: TSL2561IoctlOp,
               reg: TSL2561Regs,
               val: int = 0) -> int:
        with open(DEV_NAME, 'r') as dev:
            msg = self.__build_message(0, cmd, reg, val)
            res = ioctl(dev, CMD_ID, msg)
        return res

    @staticmethod
    def __build_message(*args: int):
        res = 0x00000000
        for i, val in enumerate(args):
            res |= (val & 0xFF) << (i * 8)
        return int(res)


DEV_NAME = '/dev/tsl2561'

if __name__ == '__main__':
    bus_handler = TSL2561DriverBusHandler(DEV_NAME)
    id = bus_handler.read_reg(TSL2561Regs.ID)
    print(f'TSL ID {id}')
