#include "Session.h"
#include <stdlib.h>
#include <unistd.h>

namespace modou
{
    Session::Session() : eof(0)
    {}

    Session::~Session()
    {}

    void Session::clear(Session *s)
    {
        if (s->in_buf) {
            free(s->in_buf);
            s->in_buf = NULL;
        }
        if (s->out_buf) {
            free(s->out_buf);
            s->out_buf = NULL;
        }
        close(s->mSocket);
        if (s) 
            free(s);
    }
}
