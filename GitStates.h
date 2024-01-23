#ifndef GITSTATES_H
#define GITSTATES_H

enum GitStates
{
    None   = 0,
    Init   = 1,
    Add    = 1 << 1,
    Commit = 1 << 2,
    Remote = 1 << 3,
    Push   = 1 << 4
};

#endif // GITSTATES_H
