package boss.vm;

public class BossOpcode
{
  final static public int
    NOP             = 0,
    RETURN_NIL      = 1,
    RETURN_VALUE    = 2,
    RETURN_COMPOUND = 3,
    PUSH_INT32      = 4,
    PRINT_INT32     = 5,
    PRINTLN         = 6;
}
