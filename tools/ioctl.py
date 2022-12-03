import os
import time
from dataclasses import dataclass
from enum import IntEnum
from fcntl import ioctl
from typing import Dict, Literal

# **************************************************************************** #
#                               Chip Definitions                               #
# **************************************************************************** #
class TSL2561Regs(IntEnum):
    CONTROL = 0x00,
    TIMING = 0x01,
    THRESHHOLDL_LOW = 0x02,
    THRESHHOLDL_HIGH = 0x03,
    THRESHHOLDH_LOW = 0x04,
    THRESHHOLDH_HIGH = 0x05,
    INTERRUPT = 0x06,
    CRC = 0x08,
    ID = 0x0A,
    CHAN0_LOW = 0x0C,
    CHAN0_HIGH = 0x0D,
    CHAN1_LOW = 0x0E,
    CHAN1_HIGH = 0x0F


TSL2561PartNo = Literal["TSL2560", "TSL2561"]


@dataclass
class TSL2561Version:
    revno: int
    partno: TSL2561PartNo

# **************************************************************************** #
#                           Linux Driver Definitions                           #
# **************************************************************************** #
DEV_NAME = '/dev/tsl2561'
CMD_ID = 100

@dataclass
class TSL2561Command:
    class Mode(IntEnum):
        BYTE = 1
        WORD = 2
    class Operation(IntEnum):
        WR = 0
        RD = 1
    operation: Operation 
    reg: TSL2561Regs
    data: int = 0
    xfer_mode: Mode = Mode.BYTE 

    def serialize(self) -> int:
        res = 0x00000000
        fields = (self.xfer_mode, self.operation, self.reg, self.data)
        for i, val in enumerate(fields):
            res |= (val & 0xFF) << (i * 8)
        return int(res)


# **************************************************************************** #
#                                 Abstractions                                 #
# **************************************************************************** #
class TSL2561DriverBusHandler:

    def __init__(self, file_dev_name: str) -> None:
        if not os.path.exists(file_dev_name):
            raise SystemError('Device or Kernel Module unavailable.')
        self.dev_name = file_dev_name

    def write_reg(self, reg: TSL2561Regs, val):
        self.__xfer(TSL2561Command(TSL2561Command.Operation.WR, reg, val))

    def read_reg(self, reg: TSL2561Regs) -> int:
        return self.__xfer(TSL2561Command(TSL2561Command.Operation.RD, reg))

    def __xfer(self, cmd:TSL2561Command) -> int:
        with open(DEV_NAME, 'r') as dev:
            res = ioctl(dev, CMD_ID, cmd.serialize())
        return res

class TSL2561:

    def __init__(self, bus_handler: TSL2561DriverBusHandler) -> None:
        self.bus = bus_handler
        self.bus.write_reg(TSL2561Regs.CONTROL, 0x3)  # As per datasheet.
        self.bus.write_reg(TSL2561Regs.TIMING, 0b10)  # Gain X1.

    def read_irradiance(self) -> float:
        lsby = self.bus.read_reg(TSL2561Regs.CHAN0_LOW)
        msby = self.bus.read_reg(TSL2561Regs.CHAN0_HIGH)
        return ((msby * 2**8) + lsby) / 0xFFFF

    def read_version(self) -> TSL2561Version:
        raw = bus_handler.read_reg(TSL2561Regs.ID)
        return TSL2561Version(
            partno=f"TSL256{(raw >> 4) & 0b1}",  # type: ignore
            revno=(raw & 0xFF))

    def dump_regs(self) -> Dict[TSL2561Regs, int]:
        return {reg: bus_handler.read_reg(reg) for reg in TSL2561Regs}


# **************************************************************************** #
#                                  Application                                 #
# **************************************************************************** #
if __name__ == '__main__':
    bus_handler = TSL2561DriverBusHandler(DEV_NAME)
    sensor = TSL2561(bus_handler)
    print(sensor.read_version())
    quit = False
    period_s = 1
    try:
        while True:
            msg = ''
            for k, v in sensor.dump_regs().items():
                msg += f'{k.name.ljust(20)} (0x{k.value:02X}): 0x{v:02X} | {v}\n'
            print(msg, flush=True)
            time.sleep(1)
    except KeyboardInterrupt:
        print('Bye!')
