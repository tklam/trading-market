template<typename Put>

void CommitTradedOrder<Put>::commit(const NERV::TradedOrder & o) {
    Put() = o;
}
