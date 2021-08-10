import enum

# Use global constants for representing states
INIT = 0
RECV = 1
SEND = 2
END = 3

# Use class as namespace for holding state
class TFTP:
    INIT = 0
    RECV = 1
    SEND = 2
    END = 3

# Class built upon dictionary
TFTP.__dict__

# Nothing prevents you from assigning to a non-existing enumerator
tftp_state = TFTP.INIT
tftp_state = 42
tftp_state

# I can iterate over the enumerators but it brings in other local members
# So I have have to filter those out.
enum_values = [v for k, v in TFTP.__dict__.items() if k.isupper()]
enum_values

# I have to provide a reverse mapping if I want to print the enum names
enum_names = {v : k for k, v in  TFTP.__dict__.items() if k.isupper()}
enum_names[INIT]

# Nothing prevents an enumerator from being defined twice with different values.
class TFTP:
    INIT = 0
    RECV = 1
    SEND = 2
    END = 3
    INIT = 4 # Second definition


class Foo(enum.Enum):
    A = 1
    B = 2

foo

TFTP.INIT
type(TFTP.INIT)
type(Foo.A)
isinstance(Foo.A, Foo)
isinstance(TFTP.INIT, TFTP)

# print
# type
# isinstance
# Constructor(int)
# duplicates

class EnumMeta(type):
    pass
    def __repr__(cls):
        return "<enum %r>" % cls.__name__
    def __new__(cls, name, bases, dct):
        print("meta", dct)
        return type.__new__(cls, name, bases, dct)

class Enum(metaclass=EnumMeta):
    pass
    def __repr__(self):
        return 'TODO'

class TFTP(Enum):
    INIT = 0
    RECV = 1
    SEND = 2
    END = 3

TFTP.FOO
