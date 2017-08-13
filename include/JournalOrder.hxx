#include <iostream>

template<typename Get>
void JournalOrder<Get>::save() {
    //TODO just for proof-of-concept
    size_t i=0;
    bool isLoop = true;
    while (isLoop) {
        for(Order & o: Get())
        {
            std::cout << "[JOURNAL] " << o.str() << std::endl;
            /*
            if ((++i)==1000000) {
                isLoop=false;
                break;
            }
            */
        }
    }
    std::cout << "JournalOrder END" << std::endl;
}
