#ifndef _JOURNAL_ORDER_H_
#define _JOURNAL_ORDER_H_

#include "OrderRing.h"

namespace NERV {

template<typename Get>
struct JournalOrder {
    void save();
};

#include "JournalOrder.hxx"

};

#endif
