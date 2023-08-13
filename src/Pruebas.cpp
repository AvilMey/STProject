#include "MarkovManager.h"
#include <iostream>

int main() {

	MarkovManager markovModel{};

    std::string out{};
    std::string ins[] = { "A", "A", "C", "B", "A"};
    for (auto i = 0; i < 5; ++i) { //Warning! dont put more iteration tan events in the model
        markovModel.putEvent(ins[i]);
        state_single out = markovModel.getEvent();
        int order = markovModel.getOrderOfLastEvent();
        //if (out == ins[i]) {
        //    std::cout << "Same " << ins[i] << ":" << out << std::endl;
        //}
        std::cout << "Next state " << out << " order " << order << std::endl;

    }
    //std::cout << markovModel.getModelAsString() << std::endl;

}