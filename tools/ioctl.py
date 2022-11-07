from fcntl import ioctl

DEV_NAME = '/dev/tsl2561'

if __name__ == '__main__':
    with open(DEV_NAME, 'r') as dev:
        ioctl(dev, 100, 101)
